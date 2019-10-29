#include <ftk/filters/track_critical_points_2d_regular_serial.hh>
#include <hypermesh/synthetic.hh>
#include <hypermesh/grad.hh>

const int DW = 256, DH = 256, DT = 10;

int main(int argc, char **argv)
{
  auto scalar = hypermesh::synthetic_woven_2Dt<double>(DW, DH, DT);
  
  ftk::track_critical_points_2d_regular_serial tracker;
  tracker.set_input_scalar_field(scalar);
  tracker.execute();

#if 0 // FTK_HAVE_VTK
  auto polydata = extractor.get_results_vtk();
  ftk::write_vtp("asdf.vtp", polydata);
#endif

  return 0;
}
