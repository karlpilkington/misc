// jquery
$(document).ready(startup);

function update_slidogram(u) {
  var max = d3.max(u, function(s){return s.n;});
  var y = d3.scale.linear()
          .domain([0,max])
          .rangeRound([0,200]);
  var data = d3.select("#slidogram").selectAll("rect")
    .data(u, function(d) {return u.ts;});
  data.transition()
   .attr("y", function(d,i) {return 200-y(d.n);})
   .attr("height", function(d,i) {return y(d.n);})
   .attr("x", function(d,i) {return i*30 - 0.5;})
   .duration(1000);
  data.enter().append("svg:rect")
   .attr("y", function(d,i) {return 200-y(d.n);})
   .attr("height", function(d,i) {return y(d.n);})
   .attr("x", function(d,i) {return i*30 - 0.5;})
   .attr("width",30);
  data.exit().remove();
}

var u = [];
var now = 0;
function update() {
  u.push( {"ts":now, "n": Math.floor(Math.random() * 100)});
  if (u.length > 6) { u.shift(); }
  update_slidogram(u);
  now++;
}

function startup() {
  setInterval(update, 2000);
}
