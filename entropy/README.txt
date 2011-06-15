Measuring Entropy in a Byte Stream

Entropy is a measure of the unpredictability of an information stream. A
perfectly consistent stream of bits (all zeroes, or all ones) is totally
predictable (has no entropy). A stream of completely unpredictable bits
has maximum entropy. The idea of entropy of information is credited to 
Claude Shannon who gave a formula to express it.

Compression of information without loss (lossless compression) is bounded
by how entropic the information is. Totally unpredictable streams of bits
are not compressible. A totally consistent stream of bits is completely
compressible. We say that such a stream has little information content.
So information content and entropy are used casually as synonyms. A 
compression algorithm is bounded mathematically by Shannon's formula which
tells us, based on probabilities of the symbols being compressed, what is
the maximum amount of compression that can be acheived. (It does not tell
us how to actually implement that level of compression however. But using it
we can gauge how well a particular compression algorithm does versus the most
optimal possible compression).

While entropy is mathematically described in generic terms, we will describe 
here how to apply the formula to a stream of bytes to measure its entropy.
The quantity of entropy we will use is "bits per byte".

Let's explain this unit more carefully. We know that (at least in modern
computers) all bytes have 8 bits. We're not talking about measuring that. This
measure tells us how many bits are _necessary_ to encode each byte.

Suppose that we have a stream of bytes where every byte has only of two values
(so two of the possible 256 values are used). We can tell immediately that
every byte really only needs one bit to represent it. The entropy measure will
confirm that this stream has "2 bits per byte" of information content.

Recall
~~~~~~
First recall that the base2-logarithm of a number n (we'll call it ln(n)) just
tells us "how many bits are needed to distinguish n states". A byte has 256
values, so it requires ln(256)=8 bits to distinguish them. (2^8 = 256).

Also recall negative exponents are a way of writing an inverse to a power; in
other words 2^-3 is the same as 1/(2^3).

Formula
~~~~~~~
The theoretical entropy of a stream of bytes is,

  E = -SUM[v in 0..255]( p(v) * ln(p(v)) )

Where p(v) is the probability of the byte value v. We've used SUM to
mean the sigma notation.  Notice the entropy does not depend on the length
of the byte stream (the length is factored in by virtue of the probabilities
which are individually a count of a certain byte value v over the total count
of bytes). So the entropy definition can be used over any length of byte
stream.

Example 1
~~~~~~~~~
Let's apply the formula to the example mentioned where a stream of bytes only
takes on two distinct values with equal probabilities. Since 254 of the 256 
values of a byte never occur, their probability is zero, so they do not
contribute to the entropy measure. The two remaining byte values occur with
p(v)=1/2. The base-2 logarithm of 1/2 is -1 (because 2^-1 = 1/2). Summing 
both of those terms (since they're the same, just multiply the term by two),

2 * (1/2 * -1)  == -1

Finally the entropy formula has a leading negation (whose purpose is now
apparent) so the resulting entropy is 1. That is, 1 bit per byte, q.e.d.

Example 2
~~~~~~~~~
As a quick confirmation that we're doing the math right, if we had four
equally likely bytes, the formula gives us an entropy of,

-1 * 4 * (1/4 * -2) == 2 bits per byte.

Example 3
~~~~~~~~~
Suppose we had a stream with only two byte values that ever occur, but they
occur with probability 2/3 and 1/3. The formula tells us the entropy is,

-1 * ( (2/3 * ln(2/3)) + (1/3 * ln(1/3)) ) == .913 bits per byte

So we see that entropy can be fractional. To put it concrete terms this
suggests that if we had 1000 of these bytes we could encode them with 913 bits
(115 bytes).

Example 4
~~~~~~~~~
Let's try one last example with an even more predictable probability 
of 9/10 and 1/10. This should have lower entropy than the last example.

-1 * ( (9/10 * ln(9/10)) + (1/10 * ln(1/10)) ) = .467

This is consistent with our expectation. About 467 bits (58 bytes) would be
needed to encode a stream of 1000 bytes with this probability distribution.

Reality check
^^^^^^^^^^^^^
Let's try making a file with 1000 bytes where every byte is either 'y' or 'n'
(the ASCII value for those characters) with probability 9/10 and 1/10.

  perl -e 'my $y; $y .= int(rand(100))>90 ? "y" : "n" for (0..999); print $y;'

Run that and put its output into a temporary file /tmp/y.

Let's run the popular compression tool 'bzip2' on it, and see how it does
compared to the approximate 58 byte ideal final size:

  % ls -l /tmp/y
  -rw-r--r-- 1 thanson thanson 1000 2011-06-15 18:56 /tmp/y
  % bzip2 /tmp/y
  % ls -l /tmp/y.bz2 
  -rw-r--r-- 1 thanson thanson 118 2011-06-15 18:56 /tmp/y.bz2

So it's reduced the file from size 1000 bytes to 118 bytes. That's still twice
as large we think the best compression can acheive. Let's see if increasing
the stream size allows bzip2 to more closely approach the entropic limit:

Change the Perl program to generate 1,000,000 y/n bytes, run, and compare:

  % ls -l /tmp/y
  -rw-r--r-- 1 thanson thanson 1000000 2011-06-15 18:58 /tmp/y
  % bzip2 /tmp/y 
  % ls -l /tmp/y.bz2 
  -rw-r--r-- 1 thanson thanson 62957 2011-06-15 18:58 /tmp/y.bz2

So, the compressed file is about 6% of its original size, which is close
to the entropic limit of (58/1000) or about 5.8% of original size. Good job,
bzip2.
