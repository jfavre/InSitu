
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# ParaView 5.4.1 64 bits

#--------------------------------------------------------------
# Global screenshot output options
imageFileNamePadding=5
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
      renderView1.ViewSize = [1083, 823]
      renderView1.AnnotationColor = [0.0, 0.0, 0.0]
      renderView1.InteractionMode = '2D'
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.OrientationAxesLabelColor = [0.0, 0.0, 0.0]
      renderView1.OrientationAxesOutlineColor = [0.0, 0.0, 0.0]
      renderView1.CenterOfRotation = [100.5, 100.5, 0.0]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [100.5, 100.5, 10000.0]
      renderView1.CameraFocalPoint = [100.5, 100.5, 0.0]
      renderView1.CameraParallelScale = 142.12846301849606
      renderView1.Background = [1.0, 1.0, 1.0]

      # init the 'GridAxes3DActor' selected for 'AxesGrid'
      renderView1.AxesGrid.XTitleColor = [0.0, 0.0, 0.0]
      renderView1.AxesGrid.YTitleColor = [0.0, 0.0, 0.0]
      renderView1.AxesGrid.ZTitleColor = [0.0, 0.0, 0.0]
      renderView1.AxesGrid.GridColor = [0.0, 0.0, 0.0]
      renderView1.AxesGrid.XLabelColor = [0.0, 0.0, 0.0]
      renderView1.AxesGrid.YLabelColor = [0.0, 0.0, 0.0]
      renderView1.AxesGrid.ZLabelColor = [0.0, 0.0, 0.0]

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=0, magnification=1, width=1083, height=823, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'XDMF Reader'
      # create a producer from a simulation input
      jacobixmf = coprocessor.CreateProducer(datadescription, 'input')

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # get color transfer function/color map for 'temperature'
      temperatureLUT = GetColorTransferFunction('temperature')
      temperatureLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 0.4999847318844663, 0.865003, 0.865003, 0.865003, 0.9999694637689326, 0.705882, 0.0156863, 0.14902]
      temperatureLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'temperature'
      temperaturePWF = GetOpacityTransferFunction('temperature')
      temperaturePWF.Points = [0.0, 0.0, 0.5, 0.0, 0.9999694637689326, 1.0, 0.5, 0.0]
      temperaturePWF.ScalarRangeInitialized = 1

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from jacobixmf
      jacobixmfDisplay = Show(jacobixmf, renderView1)
      # trace defaults for the display properties.
      jacobixmfDisplay.Representation = 'Slice'
      jacobixmfDisplay.AmbientColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.ColorArrayName = ['POINTS', 'temperature']
      jacobixmfDisplay.LookupTable = temperatureLUT
      jacobixmfDisplay.OSPRayScaleArray = 'temperature'
      jacobixmfDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
      jacobixmfDisplay.SelectOrientationVectors = 'None'
      jacobixmfDisplay.ScaleFactor = 20.1
      jacobixmfDisplay.SelectScaleArray = 'temperature'
      jacobixmfDisplay.GlyphType = 'Arrow'
      jacobixmfDisplay.GlyphTableIndexArray = 'temperature'
      jacobixmfDisplay.DataAxesGrid = 'GridAxesRepresentation'
      jacobixmfDisplay.PolarAxes = 'PolarAxesRepresentation'
      jacobixmfDisplay.ScalarOpacityUnitDistance = 8.284132126156585
      jacobixmfDisplay.ScalarOpacityFunction = temperaturePWF

      # init the 'GridAxesRepresentation' selected for 'DataAxesGrid'
      jacobixmfDisplay.DataAxesGrid.XTitleColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.DataAxesGrid.YTitleColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.DataAxesGrid.ZTitleColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.DataAxesGrid.GridColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.DataAxesGrid.XLabelColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.DataAxesGrid.YLabelColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.DataAxesGrid.ZLabelColor = [0.0, 0.0, 0.0]

      # init the 'PolarAxesRepresentation' selected for 'PolarAxes'
      jacobixmfDisplay.PolarAxes.PolarAxisTitleColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.PolarAxes.PolarAxisLabelColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.PolarAxes.LastRadialAxisTextColor = [0.0, 0.0, 0.0]
      jacobixmfDisplay.PolarAxes.SecondaryRadialAxesTextColor = [0.0, 0.0, 0.0]

      # show color legend
      jacobixmfDisplay.SetScalarBarVisibility(renderView1, True)

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for temperatureLUT in view renderView1
      temperatureLUTColorBar = GetScalarBar(temperatureLUT, renderView1)
      temperatureLUTColorBar.Title = 'temperature'
      temperatureLUTColorBar.ComponentTitle = ''
      temperatureLUTColorBar.TitleColor = [0.0, 0.0, 0.0]
      temperatureLUTColorBar.LabelColor = [0.0, 0.0, 0.0]

      ResetCamera()
      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(jacobixmf)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [1]}
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
