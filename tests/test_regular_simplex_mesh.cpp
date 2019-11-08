#include <ftk/hypermesh/regular_simplex_mesh.hh>

int main(int argc, char **argv)
{
  ftk::regular_simplex_mesh m(3);
  m.set_lb_ub({0, 0, 0}, {3, 3, 3});

  // m.element_for(2, [=](ftk::regular_simplex_mesh_element e) {
  //     std::cerr << e << std::endl;
  //   }, 1);
 
#if 0
  m.element_for_ordinal(2, 1, [=](ftk::regular_simplex_mesh_element e) {
      std::cerr << e << std::endl;
    }, 1);
#endif
  
  m.element_for_interval(2, 1, 2, [=](ftk::regular_simplex_mesh_element e) {
      std::cerr << e << std::endl;
    }, 1);


#if 0
  ftk::lattice my_lattice({1, 1, 1}, {4, 4, 1});
  my_lattice.print(std::cerr);
  
  std::vector<int> idx = {2, 2, 1};
  
  auto id = my_lattice.to_integer(idx);
  fprintf(stderr, "idx={%d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], id);

  idx = my_lattice.from_integer(id);
  fprintf(stderr, "idx={%d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], id);

  id = my_lattice.to_integer(idx);
  fprintf(stderr, "idx={%d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], id);

  idx = my_lattice.from_integer(id);
  fprintf(stderr, "idx={%d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], id);
#endif

#if 0
  // ftk::lattice my_lattice({4, 4, 4, 4}, {128, 128, 128, 56});
  ftk::lattice my_lattice({0, 0, 0, 0}, {128, 128, 128, 56});
  std::vector<int> idx = {13, 53, 7, 34};
  
  auto id = my_lattice.to_integer(idx);
  fprintf(stderr, "idx={%d, %d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], idx[3], id);

  idx = my_lattice.from_integer(id);
  fprintf(stderr, "idx={%d, %d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], idx[3], id);
  
  id = my_lattice.to_integer(idx);
  fprintf(stderr, "idx={%d, %d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], idx[3], id);
  
  idx = my_lattice.from_integer(id);
  fprintf(stderr, "idx={%d, %d, %d, %d}, id=%d\n", idx[0], idx[1], idx[2], idx[3], id);

  fprintf(stderr, "n=%d\n", my_lattice.n());
  fprintf(stderr, "prod=%d, %d, %d, %d\n", my_lattice.prod_[0], my_lattice.prod_[1], my_lattice.prod_[2], my_lattice.prod_[3]);
#endif

#if 0
  const int nd = 4;
  ftk::regular_simplex_mesh m(nd);

  for (int i = 0; i < nd+1; i ++)
    fprintf(stderr, "%d, %d, %d\n", 
        m.ntypes(i), 
        m.ntypes(i, ftk::ELEMENT_SCOPE_ORDINAL), 
        m.ntypes(i, ftk::ELEMENT_SCOPE_INTERVAL));

#endif
  return 0;
}