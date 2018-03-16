# Demonstration script for paraview version 5.4
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Mon Jul 10 08:12:22 CEST 2017
# Compatible with Python3
# This script generates a rectilinear grid. It lets ParaView automatically
# distributes the grid among multiple MPI tasks.
#
#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# Create a new 'Render View'
viewRG = GetRenderView()

Script1 = """
import numpy as np
from vtk.numpy_interface import algorithms as algs
from vtk.numpy_interface import dataset_adapter as dsa
executive = self.GetExecutive()
outInfo = executive.GetOutputInformation(0)
pid = outInfo.Get(executive.UPDATE_PIECE_NUMBER())
exts = [executive.UPDATE_EXTENT().Get(outInfo, i) for i in range(6)]
whole = [executive.WHOLE_EXTENT().Get(outInfo, i) for i in range(6)]
dims = [exts[1]-exts[0]+1, exts[3]-exts[2]+1, exts[5]-exts[4]+1]
global_dims = [whole[1]-whole[0]+1, whole[3]-whole[2]+1, whole[5]-whole[4]+1]
output.SetExtent(exts)
xaxis = np.linspace(0., 1., global_dims[0])[exts[0]:exts[1]+1]
yaxis = np.linspace(0.,1., global_dims[1])[exts[2]:exts[3]+1]
zaxis = np.linspace(0., 1., global_dims[2])[exts[4]:exts[5]+1]

xaxis = xaxis**2
yaxis = np.sqrt(yaxis)
zaxis = zaxis*np.sqrt(zaxis)

output.SetXCoordinates(dsa.numpyTovtkDataArray( xaxis , "X"))
output.SetYCoordinates(dsa.numpyTovtkDataArray( yaxis , "Y"))
output.SetZCoordinates(dsa.numpyTovtkDataArray( zaxis , "Z"))
xc, yc, zc = np.meshgrid(zaxis, yaxis, xaxis, indexing="ij")
data = np.sqrt(xc**2 + yc**2 + zc**2)
output.PointData.append(data.ravel(), "scalar")
"""
Script2 = """
dims = [10,10,10]
executive = self.GetExecutive()
outInfo = executive.GetOutputInformation(0)
outInfo.Set(executive.WHOLE_EXTENT(), 0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1)
outInfo.Set(vtk.vtkAlgorithm.CAN_PRODUCE_SUB_EXTENT(), 1)
"""

# create a new 'Programmable Source'
programmableSource1 = ProgrammableSource()
programmableSource1.OutputDataSetType = 'vtkRectilinearGrid'
programmableSource1.Script = Script1
programmableSource1.ScriptRequestInformation = Script2
programmableSource1.PythonPath = ''

rep1 = Show()
rep1.Representation = 'Surface With Edges'

Render()
viewRG.ResetCamera()
Render()
