#include "gmodel.hpp"

int main()
{
    using namespace gmod;
    default_size = 0.20;
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

    auto l0 = new_line2(p0, p9);
    auto l1 = new_arc2(p9, p8, p7);
    auto l2 = new_line2(p7, p6);
    auto l3 = new_line2(p6, p5);
    auto l4 = new_line2(p5, p4);
    auto l5 = new_arc2(p4, p3, p2);  
    auto l6 = new_line2(p2, p1);
    auto l7 = new_line2(p1, p0);

    auto loop = new_loop();
    add_use(loop, FORWARD, l0);
    add_use(loop, FORWARD, l1);
    add_use(loop, FORWARD, l2);
    add_use(loop, FORWARD, l3);
    add_use(loop, FORWARD, l4);
    add_use(loop, FORWARD, l5);
    add_use(loop, FORWARD, l6);
    add_use(loop, FORWARD, l7);

    // Create a 2D plane from the loop
    auto f = new_plane2(loop);

    // Define thickness for extrusion
    double thickness = 0.1;

    // Extrude the 2D geometry to create a 3D volume
    auto volume = extrude(f, thickness);

    // Embed point for further operations if needed
    embed(volume, p10);

    // Write output files
    write_closure_to_geo(volume, "notch3D_Asym.geo");
    write_closure_to_dmg(volume, "notch3D_Asym.dmg");

    return 0;
}

