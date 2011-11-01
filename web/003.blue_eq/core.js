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
   .attr("width", 25);

  data.transition()
   .transition(1000)
   .style("opacity", function(d) {return d.n ? 1.0 : 0.5;});

  data.exit().remove();
}

// synthesize data for a 7x24 map
// each cell has a unix timestamp ts (from which its x/y is derived)
// and a datum n (from which the map's cell is colored)
var u = [];
var day,hour;
var start;
function eq_data_refresh() {
  u.shift();
  hour += 1; if (hour == 24) { day += 1; hour = 0;}
  u.push( {"ts": start + (day*24*60*60) + (hour*60*60), 
            "n": ((Math.random()*100 < 70)?0:1)} );
  eq_update(u);
}

function make_initial_data() {
  for(day=0; day < 7; day++) {
    for(hour=0; hour < 24; hour++) {
      u.push( {"ts": start + (day*24*60*60) + (hour*60*60), 
                "n": ((Math.random()*100 < 70)?0:1)} );
    }
  }
  day += 1; hour = 0;
}

function startup() {
  start = Math.floor(new Date().getTime() / 1000);  // unixtime
  make_initial_data();
  eq_update(u);
  //setInterval(eq_data_refresh, 1000);
}
