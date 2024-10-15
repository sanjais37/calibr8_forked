#include "gmodel.hpp"

int main()
{
    using namespace gmod;

    // Initialize parameters
    default_size = 0.020;

    // Define points
    auto p0 = new_point2(Vector{0, 0, 0});
    auto p1 = new_point2(Vector{0.84, 0, 0});
    auto p2 = new_point2(Vector{0.84, 0.2, 0});
    auto p3 = new_point2(Vector{0.84, 0.36, 0});
    auto p4 = new_point2(Vector{0.84, 0.52, 0});
    auto p5 = new_point2(Vector{0.84, 1.0, 0});
    auto p6 = new_point2(Vector{0, 1.0, 0});
    auto p7 = new_point2(Vector{0, 0.80, 0});
    auto p8 = new_point2(Vector{0, 0.64, 0});
    auto p9 = new_point2(Vector{0, 0.48, 0});
    auto p10 = new_point2(Vector{0.42, 0.50, 0});

    // Define lines and arcs
    auto l0 = new_line2(p0, p9);
    auto l1 = new_arc2(p9, p8, p7);
    auto l2 = new_line2(p7, p6);
    auto l3 = new_line2(p6, p5);
    auto l4 = new_line2(p5, p4);
    auto l5 = new_arc2(p4, p3, p2);  
    auto l6 = new_line2(p2, p1);
    auto l7 = new_line2(p1, p0);

    // Create a loop and surface
    auto loop = new_loop();
    add_use(loop, FORWARD, l0);
    add_use(loop, FORWARD, l1);
    add_use(loop, FORWARD, l2);
    add_use(loop, FORWARD, l3);
    add_use(loop, FORWARD, l4);
    add_use(loop, FORWARD, l5);
    add_use(loop, FORWARD, l6);
    add_use(loop, FORWARD, l7);
    
    // Create the 2D surface
    auto f = new_plane2(loop);

    embed(f, p10);

    // Write the 2D geometry to files
    //write_closure_to_geo(f, "notch2D_Asym.geo");
    //write_closure_to_dmg(f, "notch2D_Asym.dmg");

    // Extrude the surface to create a 3D volume
    //double thickness = 0.05; 
    //std::vector<std::pair<int, int>> ov; 
    //auto f_3D = gmsh::model::geo::extrude({{2, 1}}, 0, 0, thickness, ov, {}, {}); 
   
    auto r = gmod::extrude_face(f, gmod::Vector{0,0,0.10}).middle;

    // Optionally, write the 3D mesh to a file
    write_closure_to_geo(r, "notch3D_Asym.geo");
    write_closure_to_dmg(r, "notch3D_Asym.dmg");

    return 0;
}
