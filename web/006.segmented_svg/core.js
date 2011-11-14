// using jquery
$(document).ready(startup);
function startup() {
  setInterval(blink, 500);
}

function blink() {
  var d = document.getElementById("segments").contentDocument;
  var seg1 = d.getElementById("seg1");
  var seg2 = d.getElementById("seg2");
  var seg3 = d.getElementById("seg3");

  var r = Math.floor(Math.random() * 10);
  seg1.style.visibility = (r > 3) ? "hidden" : "visible";
  seg2.style.visibility = (r > 6) ? "hidden" : "visible";
  seg3.style.visibility = (r > 8) ? "hidden" : "visible";
}
