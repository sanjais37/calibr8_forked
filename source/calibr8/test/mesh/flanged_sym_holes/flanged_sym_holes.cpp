#include "gmodel.hpp"

int main()
{
    using namespace gmod;

    // Initialize parameters
    default_size = 0.020;
    
    double a = 0.30;
    double b = 0.20;
    double c = 0.30;
    double d = 0.20;
    double e = 0.50;
    double f = 0.10;
    double h= b+d+e+f;
   

    // Define points based on the geometry image
    auto p0 = new_point2(Vector{0, 0, 0});
    auto p1 = new_point2(Vector{-a, 0, 0});
    auto p2 = new_point2(Vector{-a, b, 0});
   
    auto p5 = new_point2(Vector{-a-c, b+e, 0});
    auto p6 = new_point2(Vector{-a-c, b+e+d, 0});
    auto p7 = new_point2(Vector{-a, b+e+d, 0});
    
    auto p10 = new_point2(Vector{-a,b+e+d+f, 0});
    auto p11 = new_point2(Vector{0, b+e+d+f, 0});
   
    auto p14 = new_point2(Vector{0, b+e+d, 0});
    auto p15 = new_point2(Vector{c, b+d+e, 0});
    auto p16 = new_point2(Vector{c, b+e, 0});
    
    auto p19 = new_point2(Vector{0, b, 0});
    
    auto p20 = new_point2(Vector{-a/2, h/2, 0});
    

    // Create the outer edges using arcs and lines
    auto l0 = new_line2(p0, p1);
    auto l1 = new_line2(p1, p2);
    auto l2 = new_line2(p2, p5);
    auto l3 = new_line2(p5, p6);
    auto l4 = new_line2(p6, p7);
 
    auto l6 = new_line2(p7, p10);
    auto l7 = new_line2(p10, p11);
    auto l8 = new_line2(p11, p14);
  
    auto l10 = new_line2(p14, p15);
    auto l11 = new_line2(p15, p16);
    auto l12 = new_line2(p16, p19);
    auto l13 = new_line2(p19, p0);
    

    // Define the loop for the boundary
    auto outer_loop = new_loop();
    add_use(outer_loop, FORWARD, l0);
    add_use(outer_loop, FORWARD, l1);
    add_use(outer_loop, FORWARD, l2);
    add_use(outer_loop, FORWARD, l3);
    add_use(outer_loop, FORWARD, l4);
   
    add_use(outer_loop, FORWARD, l6);
    add_use(outer_loop, FORWARD, l7);
    add_use(outer_loop, FORWARD, l8);
  
    add_use(outer_loop, FORWARD, l10);
    add_use(outer_loop, FORWARD, l11);
    add_use(outer_loop, FORWARD, l12);
    add_use(outer_loop, FORWARD, l13);
   

    // Define the circular holes in the geometry (Radius = 10)
    auto h1 = new_circle(Vector{-a/2, b+d/4, 0}, Vector{0, 0, 1}, Vector{0.05, 0, 0});
    auto h2 = new_circle(Vector{-a/2, h-f-d/2, 0}, Vector{0, 0, 1}, Vector{0.05, 0, 0});
    auto h3 = new_circle(Vector{-a-c/2, b+e, 0}, Vector{0, 0, 1}, Vector{0.05, 0, 0});
    auto h4 = new_circle(Vector{c/2, b+e, 0}, Vector{0, 0, 1}, Vector{0.05, 0, 0});
    
    
    // Create 2D surfaces
   auto face = new_plane2(outer_loop);
   
   embed(face, p20);
    
    
   add_hole_to_face(face, h1);
   add_hole_to_face(face, h2);
   add_hole_to_face(face, h3);
   add_hole_to_face(face, h4);
    
     // Write geometry to files
   //write_closure_to_geo(face, "flanged_sym_holes.geo");
   //write_closure_to_dmg(face, "flanged_sym_holes.dmg");

    // Add holes to the face
    //add_hole_to_face(face, center_hole);
    //add_hole_to_face(face, top_hole);
    //add_hole_to_face(face, bottom_hole);

    // Extrude the 2D surface to create a 3D volume (thickness = 0.10)
   auto volume = extrude_face(face, Vector{0, 0, 0.10}).middle;

    // Write geometry to files
   write_closure_to_geo(volume, "flanged_sym_holes.geo");
   write_closure_to_dmg(volume, "flanged_sym_holes.dmg");

    return 0;
}

