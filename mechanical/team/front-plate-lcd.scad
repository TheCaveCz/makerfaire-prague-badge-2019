//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//

//
// Front plate for 2004 LCD
//

$fn=100;


module frame(h=9,d=2.3) {
    translate([-98/2,-60/2,-9-1.6]) {
    cube([98,60,1.6]);
    
    translate([0.6,10.3,1.6]) cube([96.8,39.3,9]);
    translate([11.5,18,1.6+9]) cube([75,24,5]);
    
    translate([2.5, 2.5,1.6]) cylinder(d=d,h=h);
    translate([98-2.5,2.5,1.6]) cylinder(d=d,h=h);
    translate([98-2.5,60-2.5,1.6]) cylinder(d=d,h=h);
    translate([2.5,60-2.5,1.6]) cylinder(d=d,h=h);
    }
}

module frameStands(h=10,d=3) {
    translate([-98/2,-60/2,0]) {
    translate([2.5,2.5,0]) cylinder(d=d,h=h);
    translate([98-2.5,2.5,0]) cylinder(d=d,h=h);
    translate([98-2.5,60-2.5,0]) cylinder(d=d,h=h);
    translate([2.5,60-2.5,0]) cylinder(d=d,h=h);
    }
}

module roundCube(x,y,z,r=5) {
    translate([r,0,0]) cube([x-r*2,y,z]);
    translate([0,r,0]) cube([x,y-r*2,z]);
    translate([r,r,0]) cylinder(r=r,h=z);
    translate([x-r,r,0]) cylinder(r=r,h=z);
    translate([x-r,y-r,0]) cylinder(r=r,h=z);
    translate([r,y-r,0]) cylinder(r=r,h=z);
}

module roundCubeStrip(x,y,z,r=5,t=2) {
    difference() {
        roundCube(x,y,z,r);
        translate([t,t,-1]) roundCube(x-t*2,y-t*2,z*2,r);
    }
}


difference() {
union() {
    translate([-110/2,-35,0]) {
        roundCube(110,70,0.8,10);
        translate([5,5,0.8]) roundCubeStrip(100,60,2,5,1.2);
    }
    
    translate([0,0,9.8]) rotate([0,180,0]) frameStands(9,5);
    
}

translate([0,0,0.8]) rotate([0,180,0]) #frame();
}