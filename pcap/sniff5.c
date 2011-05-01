#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pcap.h>

/* NOTES 
 *
 * to test this program's correctness it was compared with the output of
 *
 *  tcpdump -ne (to show link level headers and inhibit name resolution)
 * 
 * another useful program is tcprewrite (part of tcpreplay package) which
 * can be used, for example, to insert vlan tags into a pcap, like 
 *
 *  tcprewrite -i some.pcap -o vlan.pcap --enet-vlan=add --enet-vlan-tag=2210 \
 *              --enet-vlan-pri=0 --enet-vlan-cfi=0
 *
 */

char err[PCAP_ERRBUF_SIZE];
const int maxsz = 65535;
int verbose;

void usage(char *prog) {
  fprintf(stderr,"usage: %s [-v] -i <eth> | -r <file>\n", prog);
  exit(-1);
}

char m[18];
char *macf(const uint8_t *mac) {
  snprintf(m,sizeof(m),"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", (unsigned)mac[0], 
       (unsigned)mac[1], (unsigned)mac[2], (unsigned)mac[3], 
       (unsigned)mac[4], (unsigned)mac[5]);
  return m;
}

/*******************************************************************************
 * ethernet frame: | 6 byte dst MAC | 6 byte src MAC | 2 byte type | data
 * IP datagram: | 1 byte v/len | 1 byte TOS | 2 byte len | 16 more bytes | data
 * TCP segment: | 2 byte src port | 2 byte dst port | 4 byte seq | 4 byte ack | 
 *                2 byte flags | 2 byte window | 2 byte sum | 2 byte urg | data
 ******************************************************************************/
void cb(u_char *unused, const struct pcap_pkthdr *hdr, const u_char *pkt) {
  /* data link: ethernet frame */
  enum {other,arp,rarp,ip,vlan,ipv6} etype=other;
  char *etypes[] = {"other","arp","rarp","ip","vlan","ipv6"};
  const uint8_t *dst_mac=pkt, *src_mac=pkt+6, *typep=pkt+12;
  const uint8_t *data = pkt+14, *tci_p;
  uint16_t type,tci,vid;
  if (hdr->caplen < 12) return;
  printf("dst_mac: %s ", macf(dst_mac));
  printf("src_mac: %s ", macf(src_mac));
 again:
  if (hdr->caplen < ((typep - pkt) + 2)) return;
  memcpy(&type, typep, sizeof(uint16_t)); 
  type = ntohs(type); 
  switch(type) {
    case 0x0800: etype = ip; break;
    case 0x8100: etype = vlan; break;
    case 0x0806: etype = arp; break;
    case 0x8035: etype = rarp; break;
    case 0x86dd: etype = ipv6; break;
    default: etype = other; break;
  }
  printf("type: 0x%x (%s) ", (unsigned)type, etypes[etype]);

  /* vlan tags are 'interjected' before the ethertype; they are known by their
   * own ethertype (0x8100) followed by a TCI, after which the real etherype 
   * (or another double VLAN tag) occurs. Each VLAN tag adds two bytes to the
   * frame size. */
  if (etype==vlan) { 
    tci_p = typep + 2; // tci: tag control identifier, has vlan id in low 12 bits
    if (hdr->caplen < ((tci_p - pkt)+2)) return;
    memcpy(&tci, tci_p, sizeof(uint16_t));  
    tci = ntohs(tci);
    vid = tci & 0xfff; // low 12 bits 
    printf("vlan %d ", vid);
    typep = tci_p + 2;
    goto again; 
  }
  data = typep + 2;
  printf("\n"); /* end of frame level stuff */
}

int main(int argc, char *argv[]) {
  char *dev=NULL,*file=NULL;
  int opt,rc=-1;
  pcap_t *p=NULL;

  while ( (opt=getopt(argc,argv,"vr:i:h")) != -1) {
    switch(opt) {
      case 'v': verbose++; break;
      case 'r': file=strdup(optarg); break;
      case 'i': dev=strdup(optarg); break;
      case 'h': default: usage(argv[0]); break;
    }
  }

  if (file) p = pcap_open_offline(file, err);
  else if (dev) p = pcap_open_live(dev,maxsz,1,0,err);
  else usage(argv[0]);

  if (p == NULL) {
    fprintf(stderr, "can't open %s: %s\n", dev, err);
    goto done;
  }

  rc = pcap_loop(p, 0, cb, NULL);

 done:
  return rc;
}

