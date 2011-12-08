// using jquery
$(document).ready(startup);

function startup() {
  window.alert("here");
  setInterval(roll, 1000);
}

var ts = 0;
var alpha_degrees = 30.0;
var beta_degrees = 30.0;
var cos_alpha = Math.cos(alpha_degrees/360 * 2*Math.PI);
var sin_alpha = Math.sin(alpha_degrees/360 * 2*Math.PI);
var cos_beta = Math.cos(beta_degrees/360 * 2*Math.PI);
var sin_beta = Math.sin(beta_degrees/360 * 2*Math.PI);
var rows = [];

function iso(d,t) {
  // given an ordinal (y-pos), a series (x from 1 to 10) and heights (series[x])
  // we generate a polyline string using an isometric projection
  var y = t; // 0 <= y <= 9
  var x;
  var xs=10;
  var ys=10;
  var zs=1;
  var height = 10*xs + 10*ys + 10*zs;
  var polyline = "";
  for(x=0; x<10; x++) {
    var z = d.v[x];
    var u = x*xs*cos_alpha - y*ys*cos_beta;
    var v = x*xs*sin_alpha + y*ys*sin_beta + z*zs;
    v = height - v; // flip v for SVG (origin at upper-left)
    // append to polyline
    polyline = polyline + (polyline.length ? "," : "") + u + "," + v;
  }
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

  var viz = d3.select("#viz").selectAll("svg:polyline")
   .data(rows, function(d) {return d.ts});

  viz.enter().append("svg:polyline")
   .points(iso);

  viz.transition()
   .points(iso);

  viz.exit().remove();
}
