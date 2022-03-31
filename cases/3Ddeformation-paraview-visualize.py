# trace generated using paraview version 5.10.0
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# create a new 'XML MultiBlock Data Reader'
import sys
import os

case_dir = sys.argv[1]
file_name = os.path.join(os.curdir, case_dir, "VTK", case_dir + ".vtm.series")

# a3Ddeformation_00000_vtmseries = XMLMultiBlockDataReader(registrationName='3Ddeformation_00000_.vtm.series', FileName=['/home/tm/Projects/research/openfoam/leia-lecture-code/cases/3Ddeformation_00000_/VTK/3Ddeformation_00000_.vtm.series'])
a3Ddeformation_00001_vtmseries = XMLMultiBlockDataReader(registrationName=case_dir + ".vtm.series", 
                                                         FileName=[file_name])
a3Ddeformation_00001_vtmseries.CellArrayStatus = ['alpha', 'psi', 'U']
a3Ddeformation_00001_vtmseries.PointArrayStatus = ['alpha', 'psi', 'U']

# get animation scene
animationScene1 = GetAnimationScene()

# update animation scene based on data timesteps
animationScene1.UpdateAnimationUsingDataTimeSteps()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

# show data in view
a3Ddeformation_00001_vtmseriesDisplay = Show(a3Ddeformation_00001_vtmseries, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
a3Ddeformation_00001_vtmseriesDisplay.Representation = 'Surface'
a3Ddeformation_00001_vtmseriesDisplay.ColorArrayName = [None, '']
a3Ddeformation_00001_vtmseriesDisplay.SelectTCoordArray = 'None'
a3Ddeformation_00001_vtmseriesDisplay.SelectNormalArray = 'None'
a3Ddeformation_00001_vtmseriesDisplay.SelectTangentArray = 'None'
a3Ddeformation_00001_vtmseriesDisplay.OSPRayScaleArray = 'U'
a3Ddeformation_00001_vtmseriesDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
a3Ddeformation_00001_vtmseriesDisplay.SelectOrientationVectors = 'None'
a3Ddeformation_00001_vtmseriesDisplay.ScaleFactor = 0.1
a3Ddeformation_00001_vtmseriesDisplay.SelectScaleArray = 'None'
a3Ddeformation_00001_vtmseriesDisplay.GlyphType = 'Arrow'
a3Ddeformation_00001_vtmseriesDisplay.GlyphTableIndexArray = 'None'
a3Ddeformation_00001_vtmseriesDisplay.GaussianRadius = 0.005
a3Ddeformation_00001_vtmseriesDisplay.SetScaleArray = ['POINTS', 'U']
a3Ddeformation_00001_vtmseriesDisplay.ScaleTransferFunction = 'PiecewiseFunction'
a3Ddeformation_00001_vtmseriesDisplay.OpacityArray = ['POINTS', 'U']
a3Ddeformation_00001_vtmseriesDisplay.OpacityTransferFunction = 'PiecewiseFunction'
a3Ddeformation_00001_vtmseriesDisplay.DataAxesGrid = 'GridAxesRepresentation'
a3Ddeformation_00001_vtmseriesDisplay.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
a3Ddeformation_00001_vtmseriesDisplay.ScaleTransferFunction.Points = [-1.9984945058822632, 0.0, 0.5, 0.0, 1.9984945058822632, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
a3Ddeformation_00001_vtmseriesDisplay.OpacityTransferFunction.Points = [-1.9984945058822632, 0.0, 0.5, 0.0, 1.9984945058822632, 1.0, 0.5, 0.0]

# reset view to fit data
renderView1.ResetCamera(False)

# get the material library
materialLibrary1 = GetMaterialLibrary()

# update the view to ensure updated data information
renderView1.Update()

# set scalar coloring
ColorBy(a3Ddeformation_00001_vtmseriesDisplay, ('FIELD', 'vtkBlockColors'))

# show color bar/color legend
a3Ddeformation_00001_vtmseriesDisplay.SetScalarBarVisibility(renderView1, True)

# get color transfer function/color map for 'vtkBlockColors'
vtkBlockColorsLUT = GetColorTransferFunction('vtkBlockColors')

# get opacity transfer function/opacity map for 'vtkBlockColors'
vtkBlockColorsPWF = GetOpacityTransferFunction('vtkBlockColors')

# create a new 'Contour'
contour1 = Contour(registrationName='Contour1', Input=a3Ddeformation_00001_vtmseries)
contour1.ContourBy = ['POINTS', 'alpha']
contour1.Isosurfaces = [0.5]
contour1.PointMergeMethod = 'Uniform Binning'

# Properties modified on contour1
contour1.ContourBy = ['POINTS', 'psi']
contour1.Isosurfaces = [0.0]

# show data in view
contour1Display = Show(contour1, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'psi'
psiLUT = GetColorTransferFunction('psi')

# trace defaults for the display properties.
contour1Display.Representation = 'Surface'
contour1Display.ColorArrayName = ['POINTS', 'psi']
contour1Display.LookupTable = psiLUT
contour1Display.SelectTCoordArray = 'None'
contour1Display.SelectNormalArray = 'Normals'
contour1Display.SelectTangentArray = 'None'
contour1Display.OSPRayScaleArray = 'psi'
contour1Display.OSPRayScaleFunction = 'PiecewiseFunction'
contour1Display.SelectOrientationVectors = 'None'
contour1Display.ScaleFactor = 0.029976366460323336
contour1Display.SelectScaleArray = 'psi'
contour1Display.GlyphType = 'Arrow'
contour1Display.GlyphTableIndexArray = 'psi'
contour1Display.GaussianRadius = 0.0014988183230161666
contour1Display.SetScaleArray = ['POINTS', 'psi']
contour1Display.ScaleTransferFunction = 'PiecewiseFunction'
contour1Display.OpacityArray = ['POINTS', 'psi']
contour1Display.OpacityTransferFunction = 'PiecewiseFunction'
contour1Display.DataAxesGrid = 'GridAxesRepresentation'
contour1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
contour1Display.ScaleTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 1.1757813367477812e-38, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
contour1Display.OpacityTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 1.1757813367477812e-38, 1.0, 0.5, 0.0]

# hide data in view
Hide(a3Ddeformation_00001_vtmseries, renderView1)

# show color bar/color legend
contour1Display.SetScalarBarVisibility(renderView1, True)

# update the view to ensure updated data information
renderView1.Update()

# get opacity transfer function/opacity map for 'psi'
psiPWF = GetOpacityTransferFunction('psi')

# turn off scalar coloring
ColorBy(contour1Display, None)

# Hide the scalar bar for this color map if no visible data is colored by it.
HideScalarBarIfNotNeeded(psiLUT, renderView1)

# Properties modified on contour1
contour1.GenerateTriangles = 0

# update the view to ensure updated data information
renderView1.Update()

# Properties modified on animationScene1
animationScene1.AnimationTime = 1.484375

# get the time-keeper
timeKeeper1 = GetTimeKeeper()

# change representation type
contour1Display.SetRepresentationType('Surface With Edges')

# Hide orientation axes
renderView1.OrientationAxesVisibility = 0

# reset view to fit data
renderView1.ResetCamera(False)

# Properties modified on animationScene1
animationScene1.AnimationTime = 3.0

# Properties modified on animationScene1
animationScene1.AnimationTime = 1.484375

# Properties modified on animationScene1
animationScene1.AnimationTime = 0.0

# Properties modified on animationScene1
animationScene1.AnimationTime = 1.484375

animationScene1.GoToFirst()

# Properties modified on animationScene1
animationScene1.AnimationTime = 1.484375

animationScene1.GoToFirst()

# get layout
layout1 = GetLayout()

# layout/tab size in pixels
layout1.SetSize(1053, 780)

# current camera placement for renderView1
renderView1.CameraPosition = [0.7926662792966029, -0.4910740995731696, -0.6022642975751397]
renderView1.CameraFocalPoint = [0.40202237014603026, 0.8619500895126759, 0.8380579582700254]
renderView1.CameraViewUp = [-0.9809541663562158, -0.14096465036109512, -0.13363341968593032]
renderView1.CameraParallelScale = 0.5213649534447191

# save animation
SaveAnimation(os.path.join(os.curdir, case_dir, "animation.png"), renderView1, ImageResolution=[1053, 780],
    TransparentBackground=1,
    FrameWindow=[0, 11], 
    # PNG options
    CompressionLevel='0')

# layout/tab size in pixels
layout1.SetSize(1053, 780)

# current camera placement for renderView1
renderView1.CameraPosition = [0.7926662792966029, -0.4910740995731696, -0.6022642975751397]
renderView1.CameraFocalPoint = [0.40202237014603026, 0.8619500895126759, 0.8380579582700254]
renderView1.CameraViewUp = [-0.9809541663562158, -0.14096465036109512, -0.13363341968593032]
renderView1.CameraParallelScale = 0.5213649534447191

# save animation
SaveAnimation(os.path.join(os.curdir, case_dir, "animation.ogv"), renderView1, ImageResolution=[1053, 780],
    TransparentBackground=1,
    FrameWindow=[0, 11])

#================================================================
# addendum: following script captures some of the application
# state to faithfully reproduce the visualization during playback
#================================================================

#--------------------------------
# saving layout sizes for layouts

# layout/tab size in pixels
layout1.SetSize(1053, 780)

#-----------------------------------
# saving camera placements for views

# current camera placement for renderView1
renderView1.CameraPosition = [0.7926662792966029, -0.4910740995731696, -0.6022642975751397]
renderView1.CameraFocalPoint = [0.40202237014603026, 0.8619500895126759, 0.8380579582700254]
renderView1.CameraViewUp = [-0.9809541663562158, -0.14096465036109512, -0.13363341968593032]
renderView1.CameraParallelScale = 0.5213649534447191

#--------------------------------------------
# uncomment the following to render all views
# RenderAllViews()
# alternatively, if you want to write images, you can use SaveScreenshot(...).
