#ifndef __vtkSpiralWoven2DSource_h
#define __vtkSpiralWoven2DSource_h

#include "vtkImageAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"

class vtkDataSet;

class vtkSpiralWoven2DSource : public vtkImageAlgorithm
{
public:
  static vtkSpiralWoven2DSource *New();
  vtkTypeMacro(vtkSpiralWoven2DSource, vtkImageAlgorithm);

  vtkSetMacro(DW, int);
  vtkSetMacro(DH, int);
  vtkSetMacro(DT, int);
  vtkSetMacro(ScalingFactor, double);
  vtkSetMacro(StartTime, double);
  vtkSetMacro(TimeScale, double);

protected:
  vtkSpiralWoven2DSource();
  ~vtkSpiralWoven2DSource();

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;

private:
  vtkSpiralWoven2DSource(const vtkSpiralWoven2DSource&);
  void operator=(const vtkSpiralWoven2DSource&);

private:
  int DW, DH, DT;
  double ScalingFactor;
  double StartTime, TimeScale;
};

#endif
