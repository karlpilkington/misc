// using jquery
$(document).ready(startup);
function startup() {
  setInterval(blink, 1000);
}

var r = 0;
function blink() {
  var d = document.getElementById("pentagon_object").contentDocument;
  var p = d.getElementById("pentagon");

  r += 20;

  d3.select(p)
    .data(r, function(d){return d;})
    .enter()
    .select(p)
    .attr("transform", function(d){ return "rotate (" + r + ", 16, 16)"});

  d3.select(p)
    .transition()
    .duration(1000)
    .attr("transform", function(d){ return "rotate (" + r + ", 16, 16)"});

}
