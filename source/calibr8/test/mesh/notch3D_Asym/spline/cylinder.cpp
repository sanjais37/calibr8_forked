#include <gmodel.hpp>

int main()
{
  auto d = gmod::new_disk(gmod::Vector{0,0,0},
                          gmod::Vector{0,0,1},
                          gmod::Vector{1,0,0});
  auto r = gmod::extrude_face(d, gmod::Vector{0,0,1}).middle;
  //prevent_regression(c, "cylinder");
  
  write_closure_to_geo(r, "cylinder.geo");
  write_closure_to_dmg(r, "cylinder.dmg");
}
