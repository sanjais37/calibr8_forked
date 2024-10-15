// GMSH Script: notch3D_Asym.geo

SetFactory("OpenCASCADE");

// Define points
Point(1) = {0, 0, 0, 0.020};
Point(2) = {0.84, 0, 0, 0.020};
Point(3) = {0.84, 0.2, 0, 0.020};
Point(4) = {0.84, 0.36, 0, 0.020};
Point(5) = {0.84, 0.52, 0, 0.020};
Point(6) = {0.84, 1.0, 0, 0.020};
Point(7) = {0, 1.0, 0, 0.020};
Point(8) = {0, 0.80, 0, 0.020};
Point(9) = {0, 0.64, 0, 0.020};
Point(10) = {0, 0.48, 0, 0.020};
Point(11) = {0.42, 0.50, 0, 0.020};

// Define lines and arcs
Line(1) = {1, 10};
Line(2) = {10, 9};
Line(3) = {9, 8};
Line(4) = {8, 7};
Line(5) = {7, 6};
Line(6) = {6, 5};
Line(7) = {5, 4};
Line(8) = {4, 3};
Line(9) = {3, 2};
Line(10) = {2, 1};

// Create a loop and surface
Line Loop(1) = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
Plane Surface(1) = {1};

// Extrude the surface to create a 3D volume
Extrude {0, 0, 0.1} {
    Surface{1}; // The surface to extrude
}

