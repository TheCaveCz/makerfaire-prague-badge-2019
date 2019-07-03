//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//

//
// Front plate for 1602 LCD
//

$fn=100;


module frame(h=7.4,d=2.3) {
    translate([-80/2,-36/2,-7.4-1.6]) {
    cube([80,36,1.6]);
    
    translate([4,5.5,1.6]) cube([72,25,7.4]);
    translate([6.5,9,1.6+7.4]) cube([67,18,5]);
    
    translate([2, 2.5,1.6]) cylinder(d=d,h=h);
    translate([80-2,2.5,1.6]) cylinder(d=d,h=h);
    translate([80-2,36-2,1.6]) cylinder(d=d,h=h);
    translate([2,36-2,1.6]) cylinder(d=d,h=h);
    }
}

module frameStands(h=10,d=3) {
    translate([-80/2,-36/2,0]) {
    translate([2,2.5,0]) cylinder(d=d,h=h);
    translate([80-2,2.5,0]) cylinder(d=d,h=h);
    translate([80-2,36-2,0]) cylinder(d=d,h=h);
    translate([2,36-2,0]) cylinder(d=d,h=h);
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
    translate([-90/2,-46/2,0]) {
        roundCube(90,46,0.8,10);
        translate([5,5,0.8]) roundCubeStrip(80,36,2,5,1.2);
    }
    
    translate([0,0,8.2]) rotate([0,180,0]) frameStands(7.4,5);
    
}

translate([0,0,0.8]) rotate([0,180,0]) #frame();
}