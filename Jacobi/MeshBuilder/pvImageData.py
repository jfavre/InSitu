# Demonstration script for paraview version 5.4
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Mon Jul 10 08:12:22 CEST 2017
# compatible with Python 3

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# Create a new 'Render View'
viewID = GetRenderView()

RequestData = """
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa

executive = self.GetExecutive()
outInfo = executive.GetOutputInformation(0)
exts = [executive.UPDATE_EXTENT().Get(outInfo, i) for i in range(6)]
whole = [executive.WHOLE_EXTENT().Get(outInfo, i) for i in range(6)]
ts = executive.UPDATE_TIME_STEP().Get(outInfo)
dims = [exts[1]-exts[0]+1, exts[3]-exts[2]+1, exts[5]-exts[4]+1]
global_dims = [whole[1]-whole[0]+1, whole[3]-whole[2]+1, whole[5]-whole[4]+1]
output.SetExtent(exts)

#pid = outInfo.Get(executive.UPDATE_PIECE_NUMBER())
#gl = outInfo.Get(executive.UPDATE_NUMBER_OF_GHOST_LEVELS())
#f = open(format("/tmp/n.%d.txt" % pid), "w")
#f.write(format("pid = %d: " % pid))
#f.write(format("exts = (%d,%d,%d,%d,%d,%d)\\n" % (exts[0], exts[1],exts[2], exts[3],exts[4], exts[5])))
#f.write(format("whole = (%d,%d,%d,%d,%d,%d)\\n" % (whole[0], whole[1],whole[2], whole[3],whole[4], whole[5])))
#f.write(format("dims = %d,%d,%d\\n" % (dims[0], dims[1],dims[2])))
xaxis = np.linspace(-.5, 1., global_dims[0])[exts[0]:exts[1]+1]
yaxis = np.linspace(-1.,1., global_dims[1])[exts[2]:exts[3]+1]
zaxis = np.linspace(-1., .5, global_dims[2])[exts[4]:exts[5]+1]
[xc,yc,zc] = np.meshgrid(zaxis,yaxis,xaxis, indexing="ij")
data = np.sqrt(xc**2 + yc**2 + zc**2, dtype='f')
#ids = np.reshape(range(np.prod(global_dims)), global_dims)[exts[4]:exts[5]+1,exts[2]:exts[3]+1,exts[0]:exts[1]+1]
output.PointData.append(data.ravel(), "scalar")
#output.PointData.append(ids.ravel(), "GlobalNodeIds")
#f.close()
"""

RequestInfo = """
executive = self.GetExecutive ()
outInfo = executive.GetOutputInformation(0)

dims = [30,30,30]
outInfo.Set(executive.WHOLE_EXTENT(), 0, dims[0]-1 , 0, dims[1]-1 , 0, dims[2]-1)
outInfo.Set(vtk.vtkDataObject.SPACING(), 1, 1, 1)
outInfo.Set(vtk.vtkDataObject.ORIGIN(), 0,0,0)
outInfo.Set(vtk.vtkAlgorithm.CAN_PRODUCE_SUB_EXTENT(), 1)
#timesteps = (0.,1.,2.)
#outInfo.Remove(executive.TIME_STEPS())
#for timestep in timesteps:
#  outInfo.Append(executive.TIME_STEPS(), timestep)
#outInfo.Remove(executive.TIME_RANGE())
#outInfo.Append(executive.TIME_RANGE(), timesteps[0])
#outInfo.Append(executive.TIME_RANGE(), timesteps[-1])
"""

# create a new 'Programmable Source'
programmableSource1 = ProgrammableSource()
programmableSource1.OutputDataSetType = 'vtkImageData'
programmableSource1.Script = RequestData
programmableSource1.ScriptRequestInformation = RequestInfo
programmableSource1.UpdatePipelineInformation()

rep0 = Show()
rep0.Representation = 'Surface'

Render()
viewID.ResetCamera()
Render()
