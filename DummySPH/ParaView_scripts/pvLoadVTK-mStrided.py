# state file generated using paraview version 5.13.2
import paraview
paraview.compatibility.major = 5
paraview.compatibility.minor = 13

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

materialLibrary1 = GetMaterialLibrary()

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [385, 384]
renderView1.AxesGrid = 'Grid Axes 3D Actor'
renderView1.OrientationAxesVisibility = 0
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [-4327.320330839928, 22.14594729631721, 4728.504820498893]
renderView1.CameraFocalPoint = [-2038.728756438428, 10.74958489424287, 2224.482167117698]
renderView1.CameraViewUp = [0.6927017829289328, -0.3425850285093491, 0.6346650598294554]
renderView1.CameraViewAngle = 2.757527733755943
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 4134.706882841471
renderView1.LegendGrid = 'Legend Grid Actor'
renderView1.PolarGrid = 'Polar Grid Actor'
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

# Create a new 'Render View'
renderView2 = CreateView('RenderView')
renderView2.ViewSize = [384, 384]
renderView2.AxesGrid = 'Grid Axes 3D Actor'
renderView2.OrientationAxesVisibility = 0
renderView2.StereoType = 'Crystal Eyes'
renderView2.CameraPosition = [-4327.320330839928, 22.14594729631721, 4728.504820498893]
renderView2.CameraFocalPoint = [-2038.728756438428, 10.74958489424287, 2224.482167117698]
renderView2.CameraViewUp = [0.6927017829289328, -0.3425850285093491, 0.6346650598294554]
renderView2.CameraViewAngle = 2.757527733755943
renderView2.CameraFocalDisk = 1.0
renderView2.CameraParallelScale = 4134.706882841471
renderView2.LegendGrid = 'Legend Grid Actor'
renderView2.PolarGrid = 'Polar Grid Actor'
renderView2.BackEnd = 'OSPRay raycaster'
renderView2.OSPRayMaterialLibrary = materialLibrary1

SetActiveView(None)

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.SplitHorizontal(0, 0.500000)
layout1.AssignView(1, renderView1)
layout1.AssignView(2, renderView2)
layout1.SetSize(770, 384)

SetActiveView(renderView2)

thresholdPoints0001vtk = LegacyVTKReader(registrationName='thresholdPoints.00.0001.vtk', FileNames=['/dev/shm/thresholdPoints.00.0001.vtk'])

# create a new 'Tipsy Series Reader'
hr8799_bol_bd1017300 = TipsySeriesReader(registrationName='hr8799_bol_bd1.017300', FileNames=['/local/data/Tipsy/hr8799_bol_bd1.017300'])
hr8799_bol_bd1017300.PointArrayStatus = ['rho']

# create a new 'Legacy VTK Reader'
histsample0001vtk = LegacyVTKReader(registrationName='histsample.00.0001.vtk', FileNames=['/dev/shm/histsample.00.0001.vtk'])

hr8799_bol_bd1017300Display = Show(hr8799_bol_bd1017300, renderView1, 'GeometryRepresentation')

# get 2D transfer function for 'rho'
rhoTF2D = GetTransferFunction2D('rho')

# get color transfer function/color map for 'rho'
rhoLUT = GetColorTransferFunction('rho')
rhoLUT.TransferFunction2D = rhoTF2D
rhoLUT.RGBPoints = [1.1557343695789259e-16, 0.301961, 0.047059, 0.090196, 2.086319976829955e-16, 0.396078431372549, 0.0392156862745098, 0.058823529411764705, 3.7662036885738515e-16, 0.49411764705882355, 0.054901960784313725, 0.03529411764705882, 6.79871274845368e-16, 0.5882352941176471, 0.11372549019607843, 0.023529411764705882, 1.2272967385226442e-15, 0.6627450980392157, 0.16862745098039217, 0.01568627450980392, 2.215503640348546e-15, 0.7411764705882353, 0.22745098039215686, 0.00392156862745098, 3.999404729377968e-15, 0.788235294117647, 0.2901960784313726, 0.0, 7.219684905078528e-15, 0.8627450980392157, 0.3803921568627451, 0.011764705882352941, 1.3032902058083493e-14, 0.9019607843137255, 0.4588235294117647, 0.027450980392156862, 2.3526862777088108e-14, 0.9176470588235294, 0.5215686274509804, 0.047058823529411764, 4.2470454367346713e-14, 0.9254901960784314, 0.5803921568627451, 0.0784313725490196, 7.666723401496057e-14, 0.9372549019607843, 0.6431372549019608, 0.12156862745098039, 1.3839891423492477e-13, 0.9450980392156862, 0.7098039215686275, 0.1843137254901961, 2.498363180504001e-13, 0.9529411764705882, 0.7686274509803922, 0.24705882352941178, 4.5100199060109227e-13, 0.9647058823529412, 0.8274509803921568, 0.3254901960784314, 8.141442249605692e-13, 0.9686274509803922, 0.8784313725490196, 0.4235294117647059, 1.4696849079385106e-12, 0.9725490196078431, 0.9176470588235294, 0.5137254901960784, 2.6530603084813883e-12, 0.9803921568627451, 0.9490196078431372, 0.596078431372549, 4.789277594414713e-12, 0.9803921568627451, 0.9725490196078431, 0.6705882352941176, 8.64555539994186e-12, 0.9882352941176471, 0.9882352941176471, 0.7568627450980392, 1.5242453205533332e-11, 0.984313725490196, 0.9882352941176471, 0.8549019607843137, 1.5606869031904376e-11, 0.9882352941176471, 0.9882352941176471, 0.8588235294117647, 1.5612494392622515e-11, 0.9529411764705882, 0.9529411764705882, 0.8941176470588236, 1.5612494392622515e-11, 0.9529411764705882, 0.9529411764705882, 0.8941176470588236, 2.883125754281772e-11, 0.8901960784313725, 0.8901960784313725, 0.807843137254902, 5.3242063093586947e-11, 0.8274509803921568, 0.8235294117647058, 0.7372549019607844, 9.832097258510535e-11, 0.7764705882352941, 0.7647058823529411, 0.6784313725490196, 1.8156722501697046e-10, 0.7254901960784313, 0.7137254901960784, 0.6274509803921569, 3.352962886105268e-10, 0.6784313725490196, 0.6627450980392157, 0.5803921568627451, 6.191844433679414e-10, 0.6313725490196078, 0.6078431372549019, 0.5333333333333333, 1.1434345918281292e-09, 0.5803921568627451, 0.5568627450980392, 0.48627450980392156, 2.1115560634526977e-09, 0.5372549019607843, 0.5058823529411764, 0.44313725490196076, 3.8993651591170685e-09, 0.4980392156862745, 0.4588235294117647, 0.40784313725490196, 7.200873757182543e-09, 0.4627450980392157, 0.4196078431372549, 0.37254901960784315, 1.3297698664010362e-08, 0.43137254901960786, 0.38823529411764707, 0.34509803921568627, 2.4556574066091045e-08, 0.403921568627451, 0.3568627450980392, 0.3176470588235294, 4.534809707302771e-08, 0.37254901960784315, 0.3215686274509804, 0.29411764705882354, 8.374335534794302e-08, 0.34509803921568627, 0.29411764705882354, 0.26666666666666666, 1.5464705285512563e-07, 0.3176470588235294, 0.2627450980392157, 0.23921568627450981, 2.8558338577919624e-07, 0.28627450980392155, 0.23137254901960785, 0.21176470588235294, 5.273806951207373e-07, 0.2549019607843137, 0.2, 0.1843137254901961, 9.73902584799076e-07, 0.23137254901960785, 0.17254901960784313, 0.16470588235294117, 1.7984849530018793e-06, 0.2, 0.1450980392156863, 0.13725490196078433, 3.3224205803790797e-06, 0.14902, 0.196078, 0.278431, 1.0982209793687157e-05, 0.2, 0.2549019607843137, 0.34509803921568627, 3.6301524456243865e-05, 0.24705882352941178, 0.3176470588235294, 0.41568627450980394, 0.0001199941271022485, 0.3058823529411765, 0.38823529411764707, 0.49411764705882355, 0.00039663872949429623, 0.37254901960784315, 0.4588235294117647, 0.5686274509803921, 0.001311083179935908, 0.44313725490196076, 0.5333333333333333, 0.6431372549019608, 0.0043337651542562335, 0.5176470588235295, 0.615686274509804, 0.7254901960784313, 0.014325193625902337, 0.6, 0.6980392156862745, 0.8, 0.04735170576053281, 0.6862745098039216, 0.7843137254901961, 0.8705882352941177, 0.15652033033451368, 0.7607843137254902, 0.8588235294117647, 0.9294117647058824, 0.28456947952426676, 0.807843137254902, 0.9019607843137255, 0.9607843137254902, 0.5173755288124076, 0.8901960784313725, 0.9568627450980393, 0.984313725490196]
rhoLUT.UseLogScale = 1
rhoLUT.ColorSpace = 'Lab'
rhoLUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
hr8799_bol_bd1017300Display.Representation = 'Points'
hr8799_bol_bd1017300Display.ColorArrayName = ['POINTS', 'rho']
hr8799_bol_bd1017300Display.LookupTable = rhoLUT
hr8799_bol_bd1017300Display.OSPRayScaleArray = 'rho'
hr8799_bol_bd1017300Display.OSPRayScaleFunction = 'Piecewise Function'
hr8799_bol_bd1017300Display.Assembly = 'Hierarchy'
hr8799_bol_bd1017300Display.SelectedBlockSelectors = ['']
hr8799_bol_bd1017300Display.SelectOrientationVectors = 'None'
hr8799_bol_bd1017300Display.ScaleFactor = 576.2730224609376
hr8799_bol_bd1017300Display.SelectScaleArray = 'None'
hr8799_bol_bd1017300Display.GlyphType = 'Arrow'
hr8799_bol_bd1017300Display.GlyphTableIndexArray = 'None'
hr8799_bol_bd1017300Display.GaussianRadius = 28.813651123046874
hr8799_bol_bd1017300Display.SetScaleArray = ['POINTS', 'rho']
hr8799_bol_bd1017300Display.ScaleTransferFunction = 'Piecewise Function'
hr8799_bol_bd1017300Display.OpacityArray = ['POINTS', 'rho']
hr8799_bol_bd1017300Display.OpacityTransferFunction = 'Piecewise Function'
hr8799_bol_bd1017300Display.DataAxesGrid = 'Grid Axes Representation'
hr8799_bol_bd1017300Display.PolarAxes = 'Polar Axes Representation'

# init the 'Piecewise Function' selected for 'ScaleTransferFunction'
hr8799_bol_bd1017300Display.ScaleTransferFunction.Points = [1.155734369578926e-16, 0.0, 0.5, 0.0, 0.5173755288124084, 1.0, 0.5, 0.0]

# init the 'Piecewise Function' selected for 'OpacityTransferFunction'
hr8799_bol_bd1017300Display.OpacityTransferFunction.Points = [1.155734369578926e-16, 0.0, 0.5, 0.0, 0.5173755288124084, 1.0, 0.5, 0.0]


histsample0001vtkDisplay = Show(histsample0001vtk, renderView2, 'UnstructuredGridRepresentation')

# get opacity transfer function/opacity map for 'rho'
rhoPWF = GetOpacityTransferFunction('rho')
rhoPWF.Points = [1.155734369578926e-16, 0.0, 0.5, 0.0, 0.5173755288124084, 1.0, 0.5, 0.0]
rhoPWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
histsample0001vtkDisplay.Representation = 'Surface'
histsample0001vtkDisplay.ColorArrayName = ['POINTS', 'rho']
histsample0001vtkDisplay.DiffuseColor = [0.0, 0.0, 0.0]
histsample0001vtkDisplay.LookupTable = rhoLUT
histsample0001vtkDisplay.OSPRayScaleArray = 'rho'
histsample0001vtkDisplay.OSPRayScaleFunction = 'Piecewise Function'
histsample0001vtkDisplay.Assembly = ''
histsample0001vtkDisplay.SelectedBlockSelectors = ['']
histsample0001vtkDisplay.SelectOrientationVectors = 'None'
histsample0001vtkDisplay.ScaleFactor = 422.6898193359375
histsample0001vtkDisplay.SelectScaleArray = 'rho'
histsample0001vtkDisplay.GlyphType = 'Arrow'
histsample0001vtkDisplay.GlyphTableIndexArray = 'rho'
histsample0001vtkDisplay.GaussianRadius = 21.134490966796875
histsample0001vtkDisplay.SetScaleArray = ['POINTS', 'rho']
histsample0001vtkDisplay.ScaleTransferFunction = 'Piecewise Function'
histsample0001vtkDisplay.OpacityArray = ['POINTS', 'rho']
histsample0001vtkDisplay.OpacityTransferFunction = 'Piecewise Function'
histsample0001vtkDisplay.DataAxesGrid = 'Grid Axes Representation'
histsample0001vtkDisplay.PolarAxes = 'Polar Axes Representation'
histsample0001vtkDisplay.ScalarOpacityFunction = rhoPWF
histsample0001vtkDisplay.ScalarOpacityUnitDistance = 79.64982058120201
histsample0001vtkDisplay.OpacityArrayName = ['POINTS', 'rho']

# init the 'Piecewise Function' selected for 'ScaleTransferFunction'
histsample0001vtkDisplay.ScaleTransferFunction.Points = [1.2267063602568575e-15, 0.0, 0.5, 0.0, 0.48016229271888733, 1.0, 0.5, 0.0]

# init the 'Piecewise Function' selected for 'OpacityTransferFunction'
histsample0001vtkDisplay.OpacityTransferFunction.Points = [1.2267063602568575e-15, 0.0, 0.5, 0.0, 0.48016229271888733, 1.0, 0.5, 0.0]

