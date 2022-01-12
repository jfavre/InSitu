# script-version: 2.0
# Catalyst state generated using paraview version 5.10.0

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

materialLibrary1 = GetMaterialLibrary()

# Create a new 'Render View'
renderView1 = GetRenderView()
renderView1.ViewSize = [1024,1024]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [0.001128894224791066, -0.0020283135665234986, 0.5]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [1.7845801111775557, -2.260516154412631, 3.863955224443597]
renderView1.CameraFocalPoint = [-0.6493301399169188, 0.7854990371566732, -0.33923807171894244]
renderView1.CameraViewUp = [-0.1279227782157158, 0.7664098080910214, 0.6294853206195175]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 1.4838473389620004
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

reader = TrivialProducer(registrationName="grid")
reader.UpdatePipeline()
print("CATALYST2: ", reader.PointData["rho"].GetRange(0))

# show data
readerDisplay = Show(reader, renderView1, 'GeometryRepresentation')
readerDisplay.Representation = 'Points'
ColorBy(readerDisplay, ['POINTS', 'rho'])
readerDisplay.PointSize = 2.0
readerDisplay.GaussianRadius = 0.008953551352024079

rhoLUT = GetColorTransferFunction('rho')
rhoLUT.AutomaticRescaleRangeMode = "Grow and update every timestep"

rhoLUTColorBar = GetScalarBar(rhoLUT, renderView1)
rhoLUTColorBar.Title = 'rho'
rhoLUTColorBar.ComponentTitle = ''

# set color bar visibility
rhoLUTColorBar.Visibility = 1

# show color legend
readerDisplay.SetScalarBarVisibility(renderView1, True)

# create extractor
vTU1 = CreateExtractor('VTPD', reader, registrationName='VTU1')
vTU1.Trigger = 'TimeStep'
vTU1.Trigger.Frequency = 50
vTU1.Writer.FileName = 'particles_{timestep:06d}.vtpd'

# create extractor
pNG1 = CreateExtractor('PNG', renderView1, registrationName='PNG1')
pNG1.Trigger = 'TimeStep'
pNG1.Trigger.Frequency = 10
pNG1.Writer.FileName = 'RenderView1_{timestep:06d}{camera}.png'
pNG1.Writer.ImageResolution = [1024,1024]
pNG1.Writer.Format = 'PNG'

SetActiveSource(reader)

# ------------------------------------------------------------------------------
# Catalyst options
from paraview import catalyst
options = catalyst.Options()
options.GlobalTrigger = 'TimeStep'
options.EnableCatalystLive = 1
options.CatalystLiveTrigger = 'TimeStep'

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    from paraview.simple import SaveExtractsUsingCatalystOptions
    # Code for non in-situ environments; if executing in post-processing
    # i.e. non-Catalyst mode, let's generate extracts using Catalyst options
    SaveExtractsUsingCatalystOptions(options)
