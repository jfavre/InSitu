# Demonstration script for ParaView 5.4
# written by Jean M. Favre, Swiss National Supercomputing Centre
# tested Fri Mar 16 15:51:53 CET 2018
#
# Creates a multi-block made of 4 vtkImageData joining each other and
# filling up the whole domain


# state file generated using paraview version 5.4.1

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# Create a new 'Render View'
renderView1 = GetRenderView()
renderView1.AnnotationColor = [0.0, 0.0, 0.0]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.OrientationAxesLabelColor = [0.0, 0.0, 0.0]
renderView1.OrientationAxesOutlineColor = [0.0, 0.0, 0.0]
renderView1.CenterOfRotation = [5.0, 5.0, 5.0]
renderView1.StereoType = 0
renderView1.CameraPosition = [0.10, 23.16, 32.67]
renderView1.CameraFocalPoint = [5., 5., 5.0]
renderView1.CameraViewUp = [0.80, 0.55, -0.21]
renderView1.CameraParallelScale = 1.732
renderView1.Background = [1.0, 1.0, 1.0]

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

Script0 = """
from numpy import ones
N = 5
CompositeBlockDims = [2*N+1, 2*N+1, 2*N+1]
for i in range(4):
  block = vtk.vtkImageData()
  imin = 0
  imax = CompositeBlockDims[0] - 1
  jmin = (i%2) * (CompositeBlockDims[1] // 2)
  jmax = ((i%2)+1) * (CompositeBlockDims[1] // 2)
  kmin = (i//2) * (CompositeBlockDims[2] // 2)
  kmax = ((i//2)+1) * (CompositeBlockDims[2] // 2)
  #print("block extents ", i , " = " , imin, imax, jmin, jmax, kmin, kmax)
  block.SetExtent(imin, imax, jmin, jmax, kmin , kmax)
  block.SetOrigin(0, (i%2)-1.0*(i%2), 0)
  block.SetSpacing(1, 1, 1)
  output.SetBlock(i, block)

"""

# create a new 'Programmable Source'
programmableSource1 = ProgrammableSource()
programmableSource1.OutputDataSetType = 'vtkMultiblockDataSet'
programmableSource1.Script = Script0
programmableSource1.ScriptRequestInformation = ''
programmableSource1.PythonPath = ''

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get color transfer function/color map for 'vtkCompositeIndex'
vtkCompositeIndexLUT = GetColorTransferFunction('vtkCompositeIndex')
vtkCompositeIndexLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 2.0, 0.865003, 0.865003, 0.865003, 4.0, 0.705882, 0.0156863, 0.14902]
vtkCompositeIndexLUT.ScalarRangeInitialized = 1.0

# get opacity transfer function/opacity map for 'vtkCompositeIndex'
vtkCompositeIndexPWF = GetOpacityTransferFunction('vtkCompositeIndex')
vtkCompositeIndexPWF.Points = [0.0, 0.0, 0.5, 0.0, 4.0, 1.0, 0.5, 0.0]
vtkCompositeIndexPWF.ScalarRangeInitialized = 1

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from programmableSource1
programmableSource1Display = Show(programmableSource1, renderView1)
# trace defaults for the display properties.
programmableSource1Display.Representation = 'Surface With Edges'
programmableSource1Display.AmbientColor = [0.0, 0.0, 0.0]
programmableSource1Display.ColorArrayName = ['CELLS', 'vtkCompositeIndex']
programmableSource1Display.LookupTable = vtkCompositeIndexLUT

# show color legend
programmableSource1Display.SetScalarBarVisibility(renderView1, True)

# setup the color legend parameters for each legend in this view

# get color legend/bar for vtkCompositeIndexLUT in view renderView1
vtkCompositeIndexLUTColorBar = GetScalarBar(vtkCompositeIndexLUT, renderView1)
vtkCompositeIndexLUTColorBar.Title = 'vtkCompositeIndex'
vtkCompositeIndexLUTColorBar.ComponentTitle = ''
vtkCompositeIndexLUTColorBar.TitleColor = [0.0, 0.0, 0.0]
vtkCompositeIndexLUTColorBar.LabelColor = [0.0, 0.0, 0.0]

# ----------------------------------------------------------------
# finally, restore active source
SetActiveSource(programmableSource1)
# ----------------------------------------------------------------
