# Demonstration script for vtkpython v8
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Dec  1 10:08:05 CET 2017

import vtk
from vtk import VTK_TETRA, VTK_WEDGE, VTK_PYRAMID
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util.vtkAlgorithm import VTKPythonAlgorithmBase

class UnstructuredGridSource(VTKPythonAlgorithmBase):
    def __init__(self):
        VTKPythonAlgorithmBase.__init__(self,
            nInputPorts=0, nOutputPorts=1, outputType='vtkUnstructuredGrid')

    def RequestData(self, request, inInfo, outInfo):
        output = dsa.WrapDataObject(vtk.vtkUnstructuredGrid.GetData(outInfo))
        XYZ = np.array([0., 0., 0., 1., 0., 0., 2., 0., 0., 0., 1., 0., 1., 1., 0., 2., 1.,
                        0., 0., 2., 0., 1., 2., 0., 2., 2., 0., 0., 0., 1., 1., 0., 1., 2.,
                        0., 1., 0., 1., 1., 1., 1., 1., 2., 1., 1., 0., 2., 1., 1., 2., 1.,
                        2., 2., 1., 0., 0., 2., 1., 0., 2., 2., 0., 2., 0., 2., 2.])
        nnodes = XYZ.shape[0]//3
        if nnodes != 22:
          print('Should have 22 vertices')
        CONNECTIVITY = np.array([4, 10, 11, 14, 20,
                                 5, 0, 2, 8, 6, 13,
                                 6, 9, 10, 18, 15, 16, 21])
        nelts = 3
        CELL_TYPES = np.array([VTK_TETRA, VTK_PYRAMID, VTK_WEDGE], np.ubyte)
        CELL_OFFSETS = np.array([0, 5, 11])
        output.SetCells(CELL_TYPES, CELL_OFFSETS, CONNECTIVITY)
        output.Points = XYZ.reshape((nnodes,3))
        output.PointData.append(XYZ[0::3], "X")
        return 1

uG = UnstructuredGridSource()

geom = vtk.vtkGeometryFilter()
geom.SetInputConnection(uG.GetOutputPort())

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


