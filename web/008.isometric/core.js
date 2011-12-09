// using jquery
$(document).ready(startup);

function startup() {
  setInterval(roll, 1000);
}

var ts = 0;
var alpha_degrees = 30.0;
var beta_degrees = 30.0;
var xs=10;
var ys=10;
var zs=1;
var cos_alpha = xs * Math.cos(alpha_degrees/360 * 2*Math.PI);
var sin_alpha = xs * Math.sin(alpha_degrees/360 * 2*Math.PI);
var cos_beta =  ys * Math.cos(beta_degrees/360 * 2*Math.PI);
var sin_beta =  ys * Math.sin(beta_degrees/360 * 2*Math.PI);
var rows = [];

function iso(d,t) {
  // given an ordinal (y-pos), a series (x from 1 to 10) and heights (series[x])
  // we generate a polyline string using an isometric projection
  var y = t; // 0 <= y <= 9
  var x;
  var height = 10*xs + 10*ys + 10*zs;
  var polyline = "";
  for(x=0; x<10; x++) {
    var z = d.v[x];
    var u = Math.floor(x*cos_alpha - y*cos_beta);
    var v = Math.floor(x*sin_alpha + y*sin_beta + z*zs);
    u += 100;
    v += 100;
    v = height - v; // flip v for SVG (origin at upper-left)
    // append to polyline
    polyline = polyline + (polyline.length ? " " : "") + u + "," + v;
  }
  return polyline;
}

function roll() {
  ts = ts + 1;
  var i; var vals = [];
  for(i=0;i<10;i++) {
    var r = Math.floor(Math.random() * 10 );
    vals.push(r);
  }
  r = {'ts': ts, 'v': vals};
  if (rows.length == 10) rows.shift(); // delete oldest row
  rows.push(r); // add new row

  var viz = d3.select("#viz").selectAll("polyline")
   .data(rows, function(d) {return d.ts});

  viz.enter().append("svg:polyline")
   .attr("points",iso)
   .style("stroke", "blue")
   .style("stroke-width", 3);

  viz.transition()
   .duration(1000)
   .attr("points",iso);

  viz.exit().remove();
}
