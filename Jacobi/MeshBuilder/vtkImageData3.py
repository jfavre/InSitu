# Demonstration script for vtkpython v8
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Dec  1 09:01:26 CET 2017
#
# take previous example in vtkImageData2.py and enable streaming/subsetting

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
#enable streaming/subsetting
        info.Set(vtk.vtkAlgorithm.CAN_PRODUCE_SUB_EXTENT(), 1)
        return 1

    def RequestData(self, request, inInfo, outInfo):
        output = dsa.WrapDataObject(vtk.vtkImageData.GetData(outInfo))
        info = outInfo.GetInformationObject(0)
# VTK's pipeline is designed such that algorithms can ask a data producer for a subset of its whole extent.
# This is done using the UPDATE_EXTENT key
# enable streaming by getting both the full resolution request and the actual "local" request
        exts = info.Get(vtk.vtkStreamingDemandDrivenPipeline.UPDATE_EXTENT())
        whole = info.Get(vtk.vtkStreamingDemandDrivenPipeline.WHOLE_EXTENT())
        print("Whole extents  = ", whole[0], whole[1], whole[2], whole[3], whole[4], whole[5])
        print("update extents = ", exts[0], exts[1], exts[2], exts[3], exts[4], exts[5])
        dims = [exts[1]-exts[0]+1, exts[3]-exts[2]+1, exts[5]-exts[4]+1]
        global_dims = [whole[1]-whole[0]+1, whole[3]-whole[2]+1, whole[5]-whole[4]+1]
        output.SetExtent(exts)
#enable streaming by only computing the "slice" of the data that this processor owns
        xaxis = np.linspace(-.5, 1., global_dims[0])[exts[0]:exts[1]+1]
        yaxis = np.linspace(-1.,1., global_dims[1])[exts[2]:exts[3]+1]
        zaxis = np.linspace(-1., .5, global_dims[2])[exts[4]:exts[5]+1]
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

lut = vtk.vtkLookupTable()
lut.SetNumberOfTableValues(6)
lut.SetHueRange(0.6667,0)
lut.SetSaturationRange(1,1)
lut.SetValueRange(1,1)
lut.SetTableRange(0.0, np.sqrt(3.))
lut.Build()

m = vtk.vtkPolyDataMapper()
m.SetInputConnection(geom.GetOutputPort())
m.UseLookupTableScalarRangeOn()
m.SetLookupTable(lut)
m.SelectColorArray("scalar")
m.SetColorModeToMapScalars()
m.ScalarVisibilityOn()
m.SetScalarModeToUsePointFieldData()
#enable streaming with 4 pieces
m.SetNumberOfPieces(4)
m.SetPiece(0)
 
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


# the following code would write the ImageData grid on disk, in a distributed fashion
'''
piw = vtk.vtkXMLPImageDataWriter()
piw.SetInputConnection(iD.GetOutputPort())
piw.SetFileName("/tmp/file.pvti")
piw.SetWriteSummaryFile(1)
piw.SetNumberOfPieces(4)
piw.SetStartPiece(0)
piw.SetEndPiece(3)
piw.Write()
'''
