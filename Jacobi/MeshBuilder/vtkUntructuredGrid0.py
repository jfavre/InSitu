# Demonstration script for vtkpython v8
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Dec  1 10:08:05 CET 2017

import vtk
from vtk import VTK_TETRA
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util.vtkAlgorithm import VTKPythonAlgorithmBase
from vtk.numpy_interface import algorithms as algs

class UnstructuredGridSource(VTKPythonAlgorithmBase):
    def __init__(self):
        VTKPythonAlgorithmBase.__init__(self,
            nInputPorts=0, nOutputPorts=1, outputType='vtkUnstructuredGrid')
        self.__FileName = ""

    def RequestData(self, request, inInfo, outInfo):
        output = dsa.WrapDataObject(vtk.vtkUnstructuredGrid.GetData(outInfo))
        XYZ = np.array([0., 0., 0., 1., 0., 0., 2., 0., 0., 0., 1., 0., 1., 1., 0., 2., 1.,
                        0., 0., 2., 0., 1., 2., 0., 2., 2., 0., 0., 0., 1., 1., 0., 1., 2.,
                        0., 1., 0., 1., 1., 1., 1., 1., 2., 1., 1., 0., 2., 1., 1., 2., 1.,
                        2., 2., 1., 0., 0., 2., 1., 0., 2., 2., 0., 2., 0., 1., 2., 1., 1.,
                        2., 2., 1., 2., 0., 2., 2., 1., 2., 2., 2., 2., 2.])
        nnodes = XYZ.shape[0]//3
        if nnodes != 27:
          print('Should have 27 vertices')
        CONNECTIVITY = np.array([4, 4, 1, 10, 0, 4, 0, 4, 3, 12, 4, 4, 10, 13, 12, 4, 0, 12,
                                 9, 10, 4, 0, 4, 12, 10, 4, 1, 2, 4, 10, 4, 2, 10, 11, 14, 4,
                                 2, 10, 14, 4, 4, 2, 4, 14, 5, 4, 4, 14, 13, 10, 4, 3, 4, 6, 12,
                                 4, 4, 12, 13, 16, 4, 4, 12, 16, 6, 4, 4, 6, 16, 7, 4, 6, 16, 15,
                                 12, 4, 8, 5, 14, 4, 4, 4, 8, 7, 16, 4, 8, 14, 17, 16, 4, 4, 16,
                                 13, 14, 4, 4, 8, 16, 14, 4, 9, 10, 12, 18, 4, 10, 18, 19, 22, 4,
                                 10, 18, 22, 12, 4, 10, 12, 22, 13, 4, 12, 22, 21, 18, 4, 14, 11,
                                 20, 10, 4, 10, 14, 13, 22, 4, 14, 20, 23, 22, 4, 10, 22, 19, 20,
                                 4, 10, 14, 22, 20, 4, 16, 13, 22, 12, 4, 12, 16, 15, 24, 4, 16,
                                 22, 25, 24, 4, 12, 24, 21, 22, 4, 12, 16, 24, 22, 4, 13, 14, 16,
                                 22, 4, 14, 22, 23, 26, 4, 14, 22, 26, 16, 4, 14, 16, 26, 17, 4,
                                 16, 26, 25, 22])
        nelts = CONNECTIVITY.shape[0]//5
        if nelts != 40:
          print('Should have 40 cells')
        CELL_TYPES = np.full((nelts), VTK_TETRA, np.ubyte)
        CELL_OFFSETS = np.arange(nelts)
        CELL_OFFSETS = 5 * CELL_OFFSETS
        output.SetCells(CELL_TYPES, CELL_OFFSETS, CONNECTIVITY)
        output.Points = XYZ.reshape((nnodes,3))
        output.PointData.append(XYZ[0::3], "X")
        return 1

    def SetFileName(self, fname):
        if fname != self.__FileName:
            self.Modified()
            self.__FileName = fname

    def GetFileName(self):
        return self.__FileName

sG = UnstructuredGridSource()

shrink = vtk.vtkShrinkFilter()
shrink.SetInputConnection(sG.GetOutputPort())

geom = vtk.vtkGeometryFilter()
geom.SetInputConnection(shrink.GetOutputPort())

lut = vtk.vtkLookupTable()
lut.SetNumberOfTableValues(6)
lut.SetHueRange(0.6667,0)
lut.SetSaturationRange(1,1)
lut.SetValueRange(1,1)
lut.SetTableRange(0., 2.)
lut.Build()

m = vtk.vtkPolyDataMapper()
m.SetInputConnection(geom.GetOutputPort())
m.UseLookupTableScalarRangeOn()
m.SetLookupTable(lut)
m.SelectColorArray("X")
m.SetColorModeToMapScalars()
m.ScalarVisibilityOn()
m.SetScalarModeToUsePointFieldData()

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


