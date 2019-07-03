//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//

//
// Holder for backup battery pack
//

$dist = 32.5;
$fn=50;

difference() {
    union() {
        cylinder(d=5,h=8);
        translate([0,-2.5,0]) cube([$dist,5,1.2]);
        translate([$dist,0,0]) cylinder(d=5,h=8);
        translate([0,-1,0]) cube([$dist,2,3]);
        translate([$dist/2,0,0]) cylinder(d=6,h=3);
    }
    
    translate([0,0,1]) cylinder(d=2.2,h=8);
    translate([$dist,0,1]) cylinder(d=2.2,h=8);
    translate([$dist/2,0,-1]) cylinder(d=3.2,h=5);
}