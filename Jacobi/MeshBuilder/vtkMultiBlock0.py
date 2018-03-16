# Demonstration script for vtkpython v8
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Mar 16 15:51:53 CET 2018
#
# Creates a multi-block made of 4 vtkImageData joining each other and
# filling up the whole domain

import vtk
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util.vtkAlgorithm import VTKPythonAlgorithmBase

class MultiBlockDataSetSource(VTKPythonAlgorithmBase):
    def __init__(self):
        VTKPythonAlgorithmBase.__init__(self,
            nInputPorts=0, nOutputPorts=1, outputType='vtkMultiBlockDataSet')
        # one should only change N. This ensures an even number for dims
        self.__N = 5
        self.__CompositeBlockDims = [2*self.__N+1, 2*self.__N+1, 2*self.__N+1]
        self.__NumberOfBlocks = 4

    def RequestInformation(self, request, inInfo, outInfo):
        return 1

    def RequestData(self, request, inInfo, outInfo):
        output = dsa.WrapDataObject(vtk.vtkMultiBlockDataSet.GetData(outInfo))
        for i in range(self.__NumberOfBlocks):
          block = vtk.vtkImageData()
          imin = 0
          imax = self.__CompositeBlockDims[0] - 1
          jmin = (i%2) * (self.__CompositeBlockDims[1] // 2)
          jmax = ((i%2)+1) * (self.__CompositeBlockDims[1] // 2)
          kmin = (i//2) * (self.__CompositeBlockDims[2] // 2)
          kmax = ((i//2)+1) * (self.__CompositeBlockDims[2] // 2)
          print("block extents ", i , " = " , imin, imax, jmin, jmax, kmin, kmax)
          block.SetExtent(imin, imax, jmin, jmax, kmin , kmax)
          nbOfCells = (imax-imin)*(jmax-jmin)*(kmax-kmin)
          # the value of 'i' will be the block color
          blockcolor = np.ones(nbOfCells).astype('ushort')*i
          vtkBlockColors = dsa.numpyTovtkDataArray(blockcolor)
          vtkBlockColors.SetName("vtkCompositeIndex")
          block.GetCellData().SetScalars(vtkBlockColors)
          block.SetOrigin(0, (i%2)-1.0*(i%2), 0)
          block.SetSpacing(1, 1, 1)
          output.SetBlock(i, block)
        return 1

MB = MultiBlockDataSetSource()
MB.Update()

geom = vtk.vtkGeometryFilter()
geom.SetInputConnection(MB.GetOutputPort())

lut = vtk.vtkLookupTable()
lut.SetNumberOfTableValues(6)
lut.SetHueRange(0.6667,0)
lut.SetSaturationRange(1,1)
lut.SetValueRange(1,1)
lut.SetTableRange(0, 3)
lut.Build()

m = vtk.vtkCompositePolyDataMapper()
m.SetInputConnection(geom.GetOutputPort())
m.UseLookupTableScalarRangeOn()
m.SetLookupTable(lut)
m.SelectColorArray("vtkCompositeIndex")
m.SetColorModeToMapScalars()
m.ScalarVisibilityOn()
m.SetScalarModeToUseCellFieldData()

a = vtk.vtkActor()
a.SetMapper(m)
a.GetProperty().EdgeVisibilityOn()

ren = vtk.vtkRenderer()
ren.AddActor(a)

renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)
renWin.SetSize(600, 600)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
iren.Initialize()
renWin.Render()
iren.Start()

