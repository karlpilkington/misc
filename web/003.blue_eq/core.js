// jquery
$(document).ready(startup);

function eq_update(u) {
  var data=d3.select("#eq").selectAll("rect")
   .data(u, function(d) {return d.ts});

  function dow(ts) { var dt = new Date(ts*1000); return dt.getDay();}
  function hod(ts) { var dt = new Date(ts*1000); return dt.getHours();}
  var x = d3.scale.linear().domain([0,6]).range([0,275]);
  var y = d3.scale.linear().domain([0,23]).range([0,190]);
  data.enter().append("svg:rect")
   .attr("x", function(d) {return x(dow(d.ts));})
   .attr("y", function(d) {return 200-y(hod(d.ts));})
   .attr("height", 5)
   .attr("width", 25)
   .style("opacity", function(d) {return d.n ? 1.0 : 0.5;});

  data.transition()
   .style("opacity", function(d) {return d.n ? 1.0 : 0.5;});

  data.exit().remove();
}

// synthesize data for a 7x24 map
// each cell has a unix timestamp ts (from which its x/y is derived)
// and a datum n (from which the map's cell is colored)
var u = [];
var hour;
var start;
function eq_data_refresh() {
  var i;
  for(i=hour; i < hour+24; i++) {
    u.shift();
    u.push( {"ts": start + (i*60*60), 
              "n": ((Math.random()*100 < 70)?0:1)} );
  }
  eq_update(u);
  hour = i;
}

function make_initial_data() {
  for(hour=0; hour < 168; hour++) {
    u.push( {"ts": start + (hour*60*60), 
              "n": ((Math.random()*100 < 70)?0:1)} );
  }
}

function startup() {
  start = 0;
  make_initial_data();
  eq_update(u);
  setInterval(eq_data_refresh, 1000);
}
