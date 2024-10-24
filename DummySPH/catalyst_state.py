# script-version: 2.0
# Catalyst state generated using paraview version 5.10.0

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

materialLibrary1 = GetMaterialLibrary()

reader = TrivialProducer(registrationName="grid")

# Create a new 'Render View'
renderView1 = GetRenderView()
renderView1.ViewSize = [1024, 1024]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [1.7854218337487884, 1.6179981242178436, 0.5]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [2.700054437669351, -5.66312674799613, 5.671826340627948]
renderView1.CameraFocalPoint = [1.5056973030635852, 3.844803979388578, -1.081713454906686]
renderView1.CameraViewUp = [-0.06090498968028826, 0.5729163265620065, 0.8173478237511497]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 3.0342328780921157
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

outline1 = Outline(registrationName='Outline1', Input=reader)
outline1Display = Show(outline1)

"""
readerDisplay = Show(reader, renderView1, 'GeometryRepresentation')
readerDisplay.Representation = 'Point Gaussian'
ColorBy(readerDisplay, ['POINTS', 'Density'])
readerDisplay.PointSize = 2.0
readerDisplay.GaussianRadius = 0.009

DensityLUT = GetColorTransferFunction('Density')
DensityLUT.ApplyPreset('Inferno (matplotlib)', True)
DensityLUT.AutomaticRescaleRangeMode = "Grow and update every timestep"
readerDisplay.PointSize = 2.0

DensityLUTColorBar = GetScalarBar(DensityLUT, renderView1)
DensityLUTColorBar.Title = 'Density'
DensityLUTColorBar.ComponentTitle = ''
DensityLUTColorBar.Visibility = 1
readerDisplay.SetScalarBarVisibility(renderView1, True)
"""

convertToPointCloud1 = ConvertToPointCloud(registrationName='ConvertToPointCloud1', Input=reader)
convertToPointCloud1.CellGenerationMode = 'Polyvertex cell'

processIds1 = ProcessIds(registrationName='ProcessIds1', Input=convertToPointCloud1)

processIds1Display = Show(processIds1, renderView1, 'GeometryRepresentation')
ColorBy(processIds1Display, ['POINTS', 'PointProcessIds'])
processIdLUT = GetColorTransferFunction('PointProcessIds')
processIdLUT.InterpretValuesAsCategories = 1
processIdLUT.AnnotationsInitialized = 1
processIdLUT.AutomaticRescaleRangeMode = 'Never'
processIdLUT.RescaleTransferFunction(0, 3)
#convertToPointCloud1Display = Show(convertToPointCloud1, renderView1, 'GeometryRepresentation')
#convertToPointCloud1Display.Representation = 'Surface'
pNG1 = CreateExtractor('PNG', renderView1, registrationName='PNG1')
pNG1.Trigger = 'TimeStep'
pNG1.Trigger.Frequency = 1
pNG1.Writer.FileName = 'RenderView1_{timestep:06d}{camera}.png'
pNG1.Writer.ImageResolution = [1024, 1024]
pNG1.Writer.Format = 'PNG'

vTP1 = CreateExtractor('VTPD', convertToPointCloud1, registrationName='VTPD1')
vTP1.Trigger = 'TimeStep'
vTP1.Trigger.Frequency = 10
vTP1.Writer.FileName = 'dataset_{timestep:06d}.vtpd'

# Catalyst options
from paraview import catalyst
options = catalyst.Options()
options.GlobalTrigger = 'TimeStep'
options.EnableCatalystLive = 1
options.CatalystLiveTrigger = 'TimeStep'
