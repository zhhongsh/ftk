#ifndef __vtkSpiralWoven2DScalarFieldSource_h
#define __vtkSpiralWoven2DScalarFieldSource_h

#include "vtkImageAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"

class vtkDataSet;

class vtkSpiralWoven2DScalarFieldSource : public vtkImageAlgorithm
{
public:
  static vtkSpiralWoven2DScalarFieldSource *New();
  vtkTypeMacro(vtkSpiralWoven2DScalarFieldSource, vtkImageAlgorithm);

  void setW(int w) {DW = w;}
  void setH(int h) {DH = h;}
  void setT(int t) {DT = t;}
  void setScalingFactor(double s) {ScalingFactor = s;}
  void setStartTime(double st) {StartTime = st;}
  void setTimeScale(double ts) {TimeScale = ts;}

protected:
  vtkSpiralWoven2DScalarFieldSource();
  ~vtkSpiralWoven2DScalarFieldSource();

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;

private:
  vtkSpiralWoven2DScalarFieldSource(const vtkSpiralWoven2DScalarFieldSource&);
  void operator=(const vtkSpiralWoven2DScalarFieldSource&);

private:
  int DW, DH, DT;
  double ScalingFactor, StartTime, TimeScale;
};

#endif
