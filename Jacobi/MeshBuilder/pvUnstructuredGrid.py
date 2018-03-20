# Demonstration script for paraview version 5.4
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Mon Jul 10 08:42:52 CEST 2017

# creates a cube made of 40 tetrahedra

# can be run in parallel, and the mesh is split
# The nodes' coordinates are repeated in full among all tasks
# The offset, cell_types and connectivity arrays are instead split
# cell-based data would be split accordingly

from paraview.simple import *

paraview.simple._DisableFirstRenderCameraReset()

# Create a new 'Render View'
renderView2 = GetRenderView()

ReqDataScript = """
import vtk
from vtk import VTK_TETRA
import numpy as np
from vtk.numpy_interface import dataset_adapter as dsa

executive = self.GetExecutive()
outInfo = executive.GetOutputInformation(0)
piece = outInfo.Get(executive.UPDATE_PIECE_NUMBER())
#f = open(format("/tmp/n.%d.txt" % piece), "w")
#f.write(format("pid = %d\\n" % piece))
numPieces = outInfo.Get(executive.UPDATE_NUMBER_OF_PIECES())
print("pid = ", piece, "numPieces = ", numPieces)
XYZ = np.array([0., 0., 0., 1., 0., 0., 2., 0., 0., 0., 1., 0., 1., 1., 0., 2., 1., 0., 0., 2., 0., 1., 2., 0., 2., 2., 0., 0., 0., 1., 1., 0., 1., 2., 0., 1., 0., 1., 1., 1., 1., 1., 2., 1., 1., 0., 2., 1., 1., 2., 1.,
2., 2., 1., 0., 0., 2., 1., 0., 2., 2., 0., 2., 0., 1., 2., 1., 1., 2., 2., 1., 2., 0., 2., 2., 1., 2., 2., 2., 2., 2.])
nnodes = XYZ.shape[0]//3

CONNECTIVITY = np.array([4, 4, 1, 10, 0, 4, 0, 4, 3, 12, 4, 4, 10, 13, 12, 4, 0, 12, 9, 10, 4, 0, 4, 12, 10, 4, 1, 2, 4, 10, 4, 2, 10, 11, 14, 4, 2, 10, 14, 4, 4, 2, 4, 14, 5, 4, 4, 14, 13, 10, 4, 3, 4, 6, 12, 4, 4, 12, 13, 16, 4, 4, 12, 16, 6, 4, 4, 6, 16, 7, 4, 6, 16, 15, 12, 4, 8, 5, 14, 4, 4, 4, 8, 7, 16, 4, 8, 14, 17, 16, 4, 4, 16, 13, 14, 4, 4, 8, 16, 14,
4, 9, 10, 12, 18, 4, 10, 18, 19, 22, 4, 10, 18, 22, 12, 4, 10, 12, 22, 13, 4, 12, 22, 21, 18, 4, 14, 11, 20, 10, 4, 10, 14, 13, 22, 4, 14, 20, 23, 22, 4, 10, 22, 19, 20, 4, 10, 14, 22, 20, 4, 16, 13, 22, 12, 4, 12, 16, 15, 24, 4, 16, 22, 25, 24, 4, 12, 24, 21, 22, 4, 12, 16, 24, 22, 4, 13, 14, 16, 22, 4, 14, 22, 23, 26, 4, 14, 22, 26, 16, 4, 14, 16, 26, 17, 4, 16, 26, 25, 22])
nelts = CONNECTIVITY.shape[0]//5

CELL_OFFSETS = np.arange(nelts)
CELL_OFFSETS = 5 * CELL_OFFSETS

if numPieces == 1:
  load = MyNumber_of_Cells = nelts
else:
  load = nelts // numPieces
  if piece < (numPieces-1):
    MyNumber_of_Cells = load
  else:
    MyNumber_of_Cells = nelts - (numPieces-1) * load
# begin & end are indices in the cell arrays
begin = piece*load
end = begin + MyNumber_of_Cells
# each partition has (end-begin) cells
#f.write(format("begin = %d; end = %d\\n" % (begin, end)))

minId = np.min(CONNECTIVITY[5*begin:5*end].reshape(end-begin, 5)[:,1:5])
maxId = np.max(CONNECTIVITY[5*begin:5*end].reshape(end-begin, 5)[:,1:5])
#f.write(format("minId = %d; maxId = %d\\n" % (minId, maxId)))

#substract minId since nodes indices will start at 0
CONNECTIVITY[5*begin:5*end].reshape(end-begin, 5)[:,1:5] -= minId

# the list of offsets will start at 0
MY_OFFSET = CELL_OFFSETS[begin:end] - CELL_OFFSETS[begin]

conn_begin = CELL_OFFSETS[begin]
conn_end = CELL_OFFSETS[end-1] + CONNECTIVITY[CELL_OFFSETS[end-1]]
#f.write(format("conn_begin = %d; conn_end = %d\\n" % (conn_begin, conn_end)))

CELL_TYPES = np.full((end-begin), VTK_TETRA, np.ubyte)

output.SetCells(CELL_TYPES, MY_OFFSET, CONNECTIVITY[conn_begin:conn_end+1])

lnnodes = maxId - minId + 1
output.Points = XYZ[3*minId:3*(maxId+1)].reshape((lnnodes,3))

ids = vtk.vtkIdTypeArray()
ids.SetName("GlobalID")
ids.SetNumberOfTuples(lnnodes)
for i in range(lnnodes):
  ids.SetValue(i, i+minId)
output.PointData.SetGlobalIds(ids)

#f.close()
"""

# create a new 'Programmable Source'
programmableSource1 = ProgrammableSource()
programmableSource1.OutputDataSetType = 'vtkUnstructuredGrid'
programmableSource1.Script = ReqDataScript
programmableSource1.ScriptRequestInformation = ''
programmableSource1.PythonPath = ''

# get color transfer function/color map for 'vtkProcessId'
vtkProcessIdLUT = GetColorTransferFunction('vtkProcessId')
vtkProcessIdLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 1.5, 0.865003, 0.865003, 0.865003, 3.0, 0.705882, 0.0156863, 0.14902]
vtkProcessIdLUT.ScalarRangeInitialized = 1.0

# get opacity transfer function/opacity map for 'vtkProcessId'
vtkProcessIdPWF = GetOpacityTransferFunction('vtkProcessId')
vtkProcessIdPWF.Points = [0.0, 0.0, 0.5, 0.0, 3.0, 1.0, 0.5, 0.0]
vtkProcessIdPWF.ScalarRangeInitialized = 1


# show data from programmableSource1
programmableSource1Display = Show(programmableSource1, renderView2)
# trace defaults for the display properties.
programmableSource1Display.ColorArrayName = ['POINTS', 'vtkProcessId']
programmableSource1Display.LookupTable = vtkProcessIdLUT
programmableSource1Display.ScalarOpacityUnitDistance = 0.003259877535608031

ResetCamera()
Render()
