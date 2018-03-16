# Demonstration script for vtkpython v8
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Dec  1 10:08:05 CET 2017

import vtk
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util.vtkAlgorithm import VTKPythonAlgorithmBase

class RectilinearGridSource(VTKPythonAlgorithmBase):
    def __init__(self):
        VTKPythonAlgorithmBase.__init__(self,
            nInputPorts=0, nOutputPorts=1, outputType='vtkRectilinearGrid')
        self.__FileName = ""

    def RequestInformation(self, request, inInfo, outInfo):
        dims = [31,31,31]
        info = outInfo.GetInformationObject(0)
        info.Set(vtk.vtkStreamingDemandDrivenPipeline.WHOLE_EXTENT(),
            (0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1), 6)
        return 1

    def RequestData(self, request, inInfo, outInfo):
        output = dsa.WrapDataObject(vtk.vtkRectilinearGrid.GetData(outInfo))
        info = outInfo.GetInformationObject(0)
        exts = info.Get(vtk.vtkStreamingDemandDrivenPipeline.WHOLE_EXTENT())
        dims = [exts[1]-exts[0]+1, exts[3]-exts[2]+1, exts[5]-exts[4]+1]
        output.SetExtent(exts)
        xaxis = np.linspace(0., 1., dims[0])
        yaxis = np.linspace(0., 1., dims[1])
        zaxis = np.linspace(0., 1., dims[2])
        xaxis = xaxis**2
        yaxis = np.sqrt(yaxis)
        zaxis = zaxis*np.sqrt(zaxis)
        output.SetXCoordinates(dsa.numpyTovtkDataArray( xaxis , "X"))
        output.SetYCoordinates(dsa.numpyTovtkDataArray( yaxis , "Y"))
        output.SetZCoordinates(dsa.numpyTovtkDataArray( zaxis , "Z"))
        return 1

    def SetFileName(self, fname):
        if fname != self.__FileName:
            self.Modified()
            self.__FileName = fname

    def GetFileName(self):
        return self.__FileName

rG = RectilinearGridSource()

geom = vtk.vtkGeometryFilter()
geom.SetInputConnection(rG.GetOutputPort())

m = vtk.vtkPolyDataMapper()
m.SetInputConnection(geom.GetOutputPort())

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


