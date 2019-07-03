//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//

//
// Front plate for 7seg diplays and RX/TX diodes
//

$fn=100;


module frame(h=10,d=3) {
    translate([-6.4-50.4/2,-4.3-19.2/2,-1.6-8]) {
    cube([66,27.3,1.6]);
    
    translate([6.2,4.1,1.6]) cube([50.8,19.6,8]);
    
    translate([6.4- 4.7+1.4, 4.3- 2.9+1.4,0]) cylinder(d=d,h=h);
    translate([6.4- 4.7+1.4, 4.3+19.2+ 2.2-1.4,0]) cylinder(d=d,h=h);
    translate([6.4+50.4 + 7.6-1.4, 4.3+19.2+ 2.2-1.4,0]) cylinder(d=d,h=h);
    translate([6.4+50.4 + 7.6-1.4, 4.3- 2.9+1.4,0]) cylinder(d=d,h=h);
    }
}

module frameStands(h=10,d=3) {
    translate([-6.4-50.4/2,-4.3-19.2/2,0]) {
    translate([6.4- 4.7+1.4, 4.3- 2.9+1.4,0]) cylinder(d=d,h=h);
    translate([6.4- 4.7+1.4, 4.3+19.2+ 2.2-1.4,0]) cylinder(d=d,h=h);
    translate([6.4+50.4 + 7.6-1.4, 4.3+19.2+ 2.2-1.4,0]) cylinder(d=d,h=h);
    translate([6.4+50.4 + 7.6-1.4, 4.3- 2.9+1.4,0]) cylinder(d=d,h=h);
    }
}


module ring(d1,d2,h) {
    difference() {
        cylinder(d=d1,h=h);
        translate([0,0,-1]) cylinder(d=d2,h=h+2);
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
    translate([-80/2,-55,0]) {
        roundCube(80,120,0.8,10);
        translate([5,5,0.8]) roundCubeStrip(70,110,2,5,1.2);
    }
    
    translate([0,-30,8]) rotate([0,180,0]) frameStands(8,6);
    translate([0,0,8]) rotate([0,180,0]) frameStands(8,6);
    translate([0,30,8]) rotate([0,180,0]) frameStands(8,6);
    
    translate([4, 23+30,0]) cylinder(d=8,h=4);
}

translate([0,-30,0]) rotate([0,180,0]) #frame(8,2.8);
translate([0,0,0]) rotate([0,180,0]) #frame(8,2.8);
translate([0,30,0]) rotate([0,180,0]) #frame(8,2.8);

translate([4, 23+30,-1]) #cylinder(d=5.5,h=10);

translate([-9, 17+30, -1]) #cube([7,10, 5]);
}