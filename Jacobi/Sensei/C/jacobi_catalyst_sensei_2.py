
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# ParaView 5.4.1 64 bits

#--------------------------------------------------------------
# Global screenshot output options
imageFileNamePadding=6
rescale_lookuptable=False


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.4.1

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [480, 480]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [21.0, 20.5, 0.4996331036090851]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [107.209823076326, 76.01813159980837, 49.53211207115985]
      renderView1.CameraFocalPoint = [20.02734100934228, 17.100689289013072, 7.240370324312188]
      renderView1.CameraViewUp = [-0.31111906761384134, -0.20565082359763878, 0.9278537947978156]
      renderView1.CameraParallelScale = 29.351313995087544
      renderView1.Background = [0.31999694819562063, 0.3400015259021897, 0.4299992370489052]

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=0, magnification=1, width=480, height=480, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'PVD Reader'
      # create a producer from a simulation input
      meshpvd = coprocessor.CreateProducer(datadescription, 'mesh')

      # create a new 'Warp By Scalar'
      warpByScalar2 = WarpByScalar(Input=meshpvd)
      warpByScalar2.Scalars = ['POINTS', 'temperature']
      warpByScalar2.ScaleFactor = 31.8

      # create a new 'Warp By Scalar'
      warpByScalar1 = WarpByScalar(Input=meshpvd)
      warpByScalar1.Scalars = ['POINTS', 'temperature']
      warpByScalar1.ScaleFactor = 32.0
      warpByScalar1.UseNormal = 1

      # create a new 'Contour'
      contour1 = Contour(Input=warpByScalar1)
      contour1.ContourBy = ['POINTS', 'temperature']
      contour1.ComputeScalars = 1
      contour1.Isosurfaces = [0.0, 0.04344634782608695, 0.0868926956521739, 0.13033904347826086, 0.1737853913043478, 0.21723173913043478, 0.2606780869565217, 0.3041244347826087, 0.3475707826086956, 0.39101713043478264, 0.43446347826086956, 0.47790982608695654, 0.5213561739130435, 0.5648025217391304, 0.6082488695652174, 0.6516952173913043, 0.6951415652173912, 0.7385879130434782, 0.7820342608695653, 0.8254806086956522, 0.8689269565217391, 0.912373304347826, 0.9558196521739131, 0.999266]
      contour1.PointMergeMethod = 'Uniform Binning'

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # get color transfer function/color map for 'temperature'
      temperatureLUT = GetColorTransferFunction('temperature')
      temperatureLUT.RGBPoints = [0.0, 0.278431372549, 0.278431372549, 0.858823529412, 0.14289506389026582, 0.0, 0.0, 0.360784313725, 0.2847908615994808, 0.0, 1.0, 1.0, 0.4286851916707975, 0.0, 0.501960784314, 0.0, 0.5705809893800124, 1.0, 1.0, 0.0, 0.7134760532702783, 1.0, 0.380392156863, 0.0, 0.8563711171605441, 0.419607843137, 0.0, 0.0, 0.99926618105081, 0.878431372549, 0.301960784314, 0.301960784314]
      temperatureLUT.ColorSpace = 'RGB'
      temperatureLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'temperature'
      temperaturePWF = GetOpacityTransferFunction('temperature')
      temperaturePWF.Points = [0.0, 0.0, 0.5, 0.0, 0.99926618105081, 1.0, 0.5, 0.0]
      temperaturePWF.ScalarRangeInitialized = 1

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from contour1
      contour1Display = Show(contour1, renderView1)
      # trace defaults for the display properties.
      contour1Display.Representation = 'Surface'
      contour1Display.ColorArrayName = ['POINTS', 'temperature']
      contour1Display.LookupTable = temperatureLUT
      contour1Display.Opacity = 1.0
      contour1Display.LineWidth = 3.0
      contour1Display.OSPRayScaleArray = 'temperature'
      contour1Display.OSPRayScaleFunction = 'PiecewiseFunction'
      contour1Display.SelectOrientationVectors = 'None'
      contour1Display.ScaleFactor = 4.0923206329345705
      contour1Display.SelectScaleArray = 'temperature'
      contour1Display.GlyphType = 'Arrow'
      contour1Display.GlyphTableIndexArray = 'temperature'
      contour1Display.DataAxesGrid = 'GridAxesRepresentation'
      contour1Display.PolarAxes = 'PolarAxesRepresentation'
      contour1Display.GaussianRadius = 2.0461603164672852
      contour1Display.SetScaleArray = ['POINTS', 'temperature']
      contour1Display.ScaleTransferFunction = 'PiecewiseFunction'
      contour1Display.OpacityArray = ['POINTS', 'temperature']
      contour1Display.OpacityTransferFunction = 'PiecewiseFunction'

      # show color legend
      contour1Display.SetScalarBarVisibility(renderView1, True)

      # show data from warpByScalar2
      warpByScalar2Display = Show(warpByScalar2, renderView1)
      # trace defaults for the display properties.
      warpByScalar2Display.Representation = 'Surface'
      warpByScalar2Display.ColorArrayName = ['POINTS', 'temperature']
      warpByScalar2Display.LookupTable = temperatureLUT
      warpByScalar2Display.OSPRayScaleArray = 'temperature'
      warpByScalar2Display.OSPRayScaleFunction = 'PiecewiseFunction'
      warpByScalar2Display.SelectOrientationVectors = 'None'
      warpByScalar2Display.ScaleFactor = 4.2
      warpByScalar2Display.SelectScaleArray = 'temperature'
      warpByScalar2Display.GlyphType = 'Arrow'
      warpByScalar2Display.GlyphTableIndexArray = 'temperature'
      warpByScalar2Display.DataAxesGrid = 'GridAxesRepresentation'
      warpByScalar2Display.PolarAxes = 'PolarAxesRepresentation'
      warpByScalar2Display.ScalarOpacityFunction = temperaturePWF
      warpByScalar2Display.ScalarOpacityUnitDistance = 5.537006490599973

      # show color legend
      warpByScalar2Display.SetScalarBarVisibility(renderView1, True)

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for temperatureLUT in view renderView1
      temperatureLUTColorBar = GetScalarBar(temperatureLUT, renderView1)
      temperatureLUTColorBar.WindowLocation = 'AnyLocation'
      temperatureLUTColorBar.Position = [0.7354166666666666, 0.5895833333333333]
      temperatureLUTColorBar.Title = 'temperature'
      temperatureLUTColorBar.ComponentTitle = ''
      temperatureLUTColorBar.ScalarBarLength = 0.33000000000000007

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(warpByScalar2)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'mesh': [1, 1, 1, 1]}
  coprocessor.SetUpdateFrequencies(freqs)
  return coprocessor


#--------------------------------------------------------------
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
coprocessor.EnableLiveVisualization(False, 1)

# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    global coprocessor
    if datadescription.GetForceOutput() == True:
        # We are just going to request all fields and meshes from the simulation
        # code/adaptor.
        for i in range(datadescription.GetNumberOfInputDescriptions()):
            datadescription.GetInputDescription(i).AllFieldsOn()
            datadescription.GetInputDescription(i).GenerateMeshOn()
        return

    # setup requests for all inputs based on the requirements of the
    # pipeline.
    coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    global coprocessor

    # Update the coprocessor by providing it the newly generated simulation data.
    # If the pipeline hasn't been setup yet, this will setup the pipeline.
    coprocessor.UpdateProducers(datadescription)

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
