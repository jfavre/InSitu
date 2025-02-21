# script-version: 2.0

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
renderView1.CameraPosition = [0.0, 0.0, 6.69]
renderView1.CameraFocalPoint = [0.0, 0.0, 0.0]
renderView1.CameraViewUp = [0.0, 1.0, 0.0]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 1.73205
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

outline1 = Outline(registrationName='Outline1', Input=reader)
outline1Display = Show(outline1)

clip1 = Clip(registrationName='Clip1', Input=reader)
clip1.ClipType = 'Plane'
clip1.ClipType.Normal = [0.0, 0.0, 1.0]

readerDisplay = Show(clip1, renderView1, 'GeometryRepresentation')
readerDisplay.Representation = 'Point Gaussian'
ColorBy(readerDisplay, ['POINTS', 'rho'])
readerDisplay.PointSize = 1.0
readerDisplay.GaussianRadius = 0.02

DensityLUT = GetColorTransferFunction('rho')
DensityLUT.ApplyPreset('Inferno (matplotlib)', True)
#DensityLUT.AutomaticRescaleRangeMode = "Grow and update every timestep"

DensityLUTColorBar = GetScalarBar(DensityLUT, renderView1)
DensityLUTColorBar.Title = 'rho'
DensityLUTColorBar.ComponentTitle = ''
DensityLUTColorBar.Visibility = 1
readerDisplay.SetScalarBarVisibility(renderView1, True)
readerDisplay.RescaleTransferFunctionToDataRange(False, True)

convertToPointCloud1 = ConvertToPointCloud(registrationName='ConvertToPointCloud1', Input=reader)
convertToPointCloud1.CellGenerationMode = 'Polyvertex cell'
"""
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
"""
ResetCamera()
GetActiveCamera().Azimuth(30)
GetActiveCamera().Elevation(30)
Render()

pNG1 = CreateExtractor('PNG', renderView1, registrationName='PNG1')
pNG1.Trigger = 'TimeStep'
pNG1.Trigger.Frequency = 5
pNG1.Writer.FileName = 'RenderView1_{timestep:06d}{camera}.png'
pNG1.Writer.ImageResolution = [1024, 1024]
pNG1.Writer.Format = 'PNG'

vTP1 = CreateExtractor('VTPD', convertToPointCloud1, registrationName='VTPD1')
vTP1.Trigger = 'TimeStep'
vTP1.Trigger.Frequency = 50
vTP1.Writer.FileName = 'dataset_{timestep:06d}.vtpd'

# Catalyst options
from paraview import catalyst
options = catalyst.Options()
options.GlobalTrigger = 'TimeStep'
options.EnableCatalystLive = 1
options.CatalystLiveTrigger = 'TimeStep'
