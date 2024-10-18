#include "gmodel.hpp"

int main()
{
    using namespace gmod;

    // Initialize parameters
    default_size = 0.020;
    double arcR = 3;
    double a = 30;
    double b = 20;
    double c = 30;
    double d = 20;
    double e = 40;
    double f = 4;
    double hypo = sqrt(e*e + c*c);
    double x_ratio = c/hypo;
    double y_ratio = e/hypo;
   

    // Define points based on the geometry image
    auto p0 = new_point2(Vector{0, 0, 0});
    auto p1 = new_point2(Vector{-a, 0, 0});
    auto p2 = new_point2(Vector{-a, b, 0});
    auto p3 = new_point2(Vector{-(a+x_ratio*arcR), (b+y_ratio*arcR), 0});
    auto p4 = new_point2(Vector{-(a+x_ratio*2*arcR), (b+y_ratio*2*arcR), 0});
    auto p5 = new_point2(Vector{-a-c, b+e, 0});
    auto p6 = new_point2(Vector{-a-c, b+e+d, 0});
    auto p7 = new_point2(Vector{-a-x_ratio*2*arcR, b+e+d, 0});
    auto p8 = new_point2(Vector{-a-x_ratio*arcR, b+e+d+y_ratio*arcR , 0});
    auto p9 = new_point2(Vector{-a, b+e+d+y_ratio*2*arcR, 0});
    auto p10 = new_point2(Vector{-a,b+e+d+y_ratio*2*arcR+f, 0});
    auto p11 = new_point2(Vector{0, b+e+d+y_ratio*2*arcR+f, 0});
    auto p12 = new_point2(Vector{0, b+e+d+y_ratio*2*arcR, 0});
    auto p13 = new_point2(Vector{x_ratio*arcR, b+e+d+y_ratio*arcR, 0});
    auto p14 = new_point2(Vector{x_ratio*2*arcR, b+e+d, 0});
    auto p15 = new_point2(Vector{c, b+d+e, 0});
    auto p16 = new_point2(Vector{c, b+e, 0});
    auto p17 = new_point2(Vector{x_ratio*2*arcR, b+y_ratio*2*arcR, 0});
    auto p18 = new_point2(Vector{x_ratio*arcR, b+y_ratio*arcR, 0});
    auto p19 = new_point2(Vector{0, b, 0});
    

    // Create the outer edges using arcs and lines
    auto l0 = new_line2(p0, p1);
    auto l1 = new_line2(p1, p2);
    auto l2 = new_line2(p0, p1);
    auto l3 = new_arc2(p2, p3, p4);
    auto l4 = new_line2(p4, p5);
    auto l5 = new_line2(p5, p6);
    auto l6 = new_line2(p6, p7);
    auto l7 = new_arc2(p7, p8, p9);
    auto l8 = new_line2(p9, p10);
    auto l9 = new_line2(p10, p11);
    auto l10 = new_line2(p11, p12);
    auto l11 = new_arc2(p12, p13, p14);
    auto l12 = new_line2(p14, p15);
    auto l13 = new_line2(p15, p16);
    auto l14 = new_line2(p16, p17);
    auto l15 = new_arc2(p17, p18, p19);
    auto l16 = new_line2(p19, p0);
    

    // Define the loop for the boundary
    auto outer_loop = new_loop();
    add_use(outer_loop, FORWARD, l0);
    add_use(outer_loop, FORWARD, l1);
    add_use(outer_loop, FORWARD, l2);
    add_use(outer_loop, FORWARD, l3);
    add_use(outer_loop, FORWARD, l4);
    add_use(outer_loop, FORWARD, l5);
    add_use(outer_loop, FORWARD, l6);
    add_use(outer_loop, FORWARD, l7);
    add_use(outer_loop, FORWARD, l8);
    add_use(outer_loop, FORWARD, l9);
    add_use(outer_loop, FORWARD, l10);
    add_use(outer_loop, FORWARD, l11);
    add_use(outer_loop, FORWARD, l12);
    add_use(outer_loop, FORWARD, l13);
    add_use(outer_loop, FORWARD, l14);
    add_use(outer_loop, FORWARD, l15);
    add_use(outer_loop, FORWARD, l16);

    // Define the circular holes in the geometry (Radius = 10)
    //auto center_hole = new_circle(Vector{15, 40, 0}, Vector{0, 0, 1}, Vector{10, 0, 0});
    //auto top_hole = new_circle(Vector{15, 10, 0}, Vector{0, 0, 1}, Vector{10, 0, 0});
    //auto bottom_hole = new_circle(Vector{15, 70, 0}, Vector{0, 0, 1}, Vector{10, 0, 0});
    
    // Create 2D surfaces
    auto face = new_plane2(outer_loop);
    
     // Write geometry to files
    write_closure_to_geo(face, "2Dflanged_sym_holes.geo");
    write_closure_to_dmg(face, "2Dflanged_sym_holes.dmg");

    // Add holes to the face
    //add_hole_to_face(face, center_hole);
    //add_hole_to_face(face, top_hole);
    //add_hole_to_face(face, bottom_hole);

    // Extrude the 2D surface to create a 3D volume (thickness = 0.10)
    //auto volume = extrude_face(face, Vector{0, 0, 0.10}).middle;

    // Write geometry to files
    //write_closure_to_geo(volume, "flanged_sym_holes.geo");
   // write_closure_to_dmg(volume, "flanged_sym_holes.dmg");

    return 0;
}

