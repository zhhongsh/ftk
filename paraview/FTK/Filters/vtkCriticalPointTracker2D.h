#ifndef __vtkCriticalPointTracker2D_h
#define __vtkCriticalPointTracker2D_h

#include "vtkImageAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"
#include <ftk/filters/critical_point_tracker_2d_regular.hh>

class vtkDataSet;

class vtkCriticalPointTracker2D : public vtkImageAlgorithm
{
public:
  static vtkCriticalPointTracker2D *New();
  vtkTypeMacro(vtkCriticalPointTracker2D, vtkImageAlgorithm);

  vtkSetMacro(UseGPU, bool);
  vtkGetMacro(UseGPU, bool);

  vtkSetMacro(GaussianKernelSize, double);
  vtkGetMacro(GaussianKernelSize, double);

  vtkSetMacro(InputVariable, std::string);
  vtkGetMacro(InputVariable, std::string);

protected:
  vtkCriticalPointTracker2D();
  ~vtkCriticalPointTracker2D();

  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;

private:
  vtkCriticalPointTracker2D(const vtkCriticalPointTracker2D&);
  void operator=(const vtkCriticalPointTracker2D&);

private:
  bool UseGPU;
  double GaussianKernelSize;
  std::string InputVariable;

  int currentTimestep;
  int inputDataComponents;
  
  ftk::critical_point_tracker_2d_regular tracker; 
};

#endif
