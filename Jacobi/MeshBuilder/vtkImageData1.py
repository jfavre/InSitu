# Demonstration script for vtkpython v8
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Dec  1 09:01:26 CET 2017
#
# take previous example in vtkImageData0.py and add a scalar field called "scalar"

import vtk
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util.vtkAlgorithm import VTKPythonAlgorithmBase

class ImageDataSource(VTKPythonAlgorithmBase):
    def __init__(self):
        VTKPythonAlgorithmBase.__init__(self,
            nInputPorts=0, nOutputPorts=1, outputType='vtkImageData')
        self.__FileName = ""

    def RequestInformation(self, request, inInfo, outInfo):
        dims = [31,31,31]
        info = outInfo.GetInformationObject(0)
        info.Set(vtk.vtkStreamingDemandDrivenPipeline.WHOLE_EXTENT(),
            (0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1), 6)
        info.Set(vtk.vtkDataObject.SPACING(), 1, 1, 1)
        info.Set(vtk.vtkDataObject.ORIGIN(), 0, 0, 0)
        return 1

    def RequestData(self, request, inInfo, outInfo):
        output = dsa.WrapDataObject(vtk.vtkImageData.GetData(outInfo))
        info = outInfo.GetInformationObject(0)
        exts = info.Get(vtk.vtkStreamingDemandDrivenPipeline.UPDATE_EXTENT())
        dims = [exts[1]-exts[0]+1, exts[3]-exts[2]+1, exts[5]-exts[4]+1]
        output.SetExtent(exts)
        xaxis = np.linspace(-.5, 1., dims[0])
        yaxis = np.linspace(-1.,1., dims[1])
        zaxis = np.linspace(-1., .5, dims[2])
        [xc,yc,zc] = np.meshgrid(zaxis,yaxis,xaxis, indexing="ij")
        data = np.sqrt(xc**2 + yc**2 + zc**2)
        output.PointData.append(data.ravel(), "scalar")
        return 1

    def SetFileName(self, fname):
        if fname != self.__FileName:
            self.Modified()
            self.__FileName = fname

    def GetFileName(self):
        return self.__FileName

iD = ImageDataSource()

geom = vtk.vtkGeometryFilter()
geom.SetInputConnection(iD.GetOutputPort())

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


