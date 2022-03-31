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
a3Dshear_00000_vtmseries = XMLMultiBlockDataReader(registrationName=case_dir + ".vtm.series", FileName=[file_name])
a3Dshear_00000_vtmseries.CellArrayStatus = ['alpha', 'psi', 'U']
a3Dshear_00000_vtmseries.PointArrayStatus = ['alpha', 'psi', 'U']

# get animation scene
animationScene1 = GetAnimationScene()

# update animation scene based on data timesteps
animationScene1.UpdateAnimationUsingDataTimeSteps()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

# show data in view
a3Dshear_00000_vtmseriesDisplay = Show(a3Dshear_00000_vtmseries, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
a3Dshear_00000_vtmseriesDisplay.Representation = 'Surface'
a3Dshear_00000_vtmseriesDisplay.ColorArrayName = [None, '']
a3Dshear_00000_vtmseriesDisplay.SelectTCoordArray = 'None'
a3Dshear_00000_vtmseriesDisplay.SelectNormalArray = 'None'
a3Dshear_00000_vtmseriesDisplay.SelectTangentArray = 'None'
a3Dshear_00000_vtmseriesDisplay.OSPRayScaleArray = 'U'
a3Dshear_00000_vtmseriesDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
a3Dshear_00000_vtmseriesDisplay.SelectOrientationVectors = 'None'
a3Dshear_00000_vtmseriesDisplay.ScaleFactor = 0.2
a3Dshear_00000_vtmseriesDisplay.SelectScaleArray = 'None'
a3Dshear_00000_vtmseriesDisplay.GlyphType = 'Arrow'
a3Dshear_00000_vtmseriesDisplay.GlyphTableIndexArray = 'None'
a3Dshear_00000_vtmseriesDisplay.GaussianRadius = 0.01
a3Dshear_00000_vtmseriesDisplay.SetScaleArray = ['POINTS', 'U']
a3Dshear_00000_vtmseriesDisplay.ScaleTransferFunction = 'PiecewiseFunction'
a3Dshear_00000_vtmseriesDisplay.OpacityArray = ['POINTS', 'U']
a3Dshear_00000_vtmseriesDisplay.OpacityTransferFunction = 'PiecewiseFunction'
a3Dshear_00000_vtmseriesDisplay.DataAxesGrid = 'GridAxesRepresentation'
a3Dshear_00000_vtmseriesDisplay.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
a3Dshear_00000_vtmseriesDisplay.ScaleTransferFunction.Points = [-0.9927886724472046, 0.0, 0.5, 0.0, 0.9927886724472046, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
a3Dshear_00000_vtmseriesDisplay.OpacityTransferFunction.Points = [-0.9927886724472046, 0.0, 0.5, 0.0, 0.9927886724472046, 1.0, 0.5, 0.0]

# reset view to fit data
renderView1.ResetCamera(False)

# get the material library
materialLibrary1 = GetMaterialLibrary()

# update the view to ensure updated data information
renderView1.Update()

# set scalar coloring
ColorBy(a3Dshear_00000_vtmseriesDisplay, ('FIELD', 'vtkBlockColors'))

# show color bar/color legend
a3Dshear_00000_vtmseriesDisplay.SetScalarBarVisibility(renderView1, True)

# get color transfer function/color map for 'vtkBlockColors'
vtkBlockColorsLUT = GetColorTransferFunction('vtkBlockColors')

# get opacity transfer function/opacity map for 'vtkBlockColors'
vtkBlockColorsPWF = GetOpacityTransferFunction('vtkBlockColors')

# create a new 'Contour'
contour1 = Contour(registrationName='Contour1', Input=a3Dshear_00000_vtmseries)
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
contour1Display.ScaleFactor = 0.029666659235954285
contour1Display.SelectScaleArray = 'psi'
contour1Display.GlyphType = 'Arrow'
contour1Display.GlyphTableIndexArray = 'psi'
contour1Display.GaussianRadius = 0.0014833329617977144
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
Hide(a3Dshear_00000_vtmseries, renderView1)

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

# change representation type
contour1Display.SetRepresentationType('Surface With Edges')

# Hide orientation axes
renderView1.OrientationAxesVisibility = 0

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

# get layout
layout1 = GetLayout()

# layout/tab size in pixels
layout1.SetSize(951, 780)

# current camera placement for renderView1
renderView1.CameraPosition = [0.43520158858360797, 0.5205458377661741, 1.7667811042742507]
renderView1.CameraFocalPoint = [0.43520158858360797, 0.5205458377661741, -2.9652697032946262]
renderView1.CameraParallelScale = 1.224744871391589

# save animation
SaveAnimation(os.path.join(os.curdir, case_dir, "animation-Zaxis.png"), renderView1, ImageResolution=[951, 780],
    TransparentBackground=1,
    FrameWindow=[0, 11], 
    # PNG options
    CompressionLevel='0')

# layout/tab size in pixels
layout1.SetSize(951, 780)

# current camera placement for renderView1
renderView1.CameraPosition = [0.43520158858360797, 0.5205458377661741, 1.7667811042742507]
renderView1.CameraFocalPoint = [0.43520158858360797, 0.5205458377661741, -2.9652697032946262]
renderView1.CameraParallelScale = 1.224744871391589

# save animation
SaveAnimation(os.path.join(os.curdir, case_dir, "animation-Zaxis.ogv"), renderView1, ImageResolution=[951, 780],
    FrameWindow=[0, 11])

animationScene1.Play()

# Properties modified on animationScene1
animationScene1.AnimationTime = 1.484375

# get the time-keeper
timeKeeper1 = GetTimeKeeper()

# Properties modified on animationScene1
animationScene1.AnimationTime = 3.0

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

# update center of rotation
renderView1.CenterOfRotation = [0.3830412161879649, 0.8934297043749463, 0.2523000101267295]

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

animationScene1.Play()

# layout/tab size in pixels
layout1.SetSize(951, 780)

# current camera placement for renderView1
renderView1.CameraPosition = [1.3386063087420734, 1.9341725910949747, -0.29675584917037484]
renderView1.CameraFocalPoint = [-0.9516900784139893, -1.7321971682940716, 1.6279727269756041]
renderView1.CameraViewUp = [-0.2966923719110331, -0.2919856645026584, -0.9092403467591758]
renderView1.CameraParallelScale = 1.224744871391589

# save animation
SaveAnimation(os.path.join(os.curdir, case_dir, "animation.ogv"), renderView1, ImageResolution=[951, 780],
    FrameWindow=[0, 11])

# layout/tab size in pixels
layout1.SetSize(951, 780)

# current camera placement for renderView1
renderView1.CameraPosition = [1.3386063087420734, 1.9341725910949747, -0.29675584917037484]
renderView1.CameraFocalPoint = [-0.9516900784139893, -1.7321971682940716, 1.6279727269756041]
renderView1.CameraViewUp = [-0.2966923719110331, -0.2919856645026584, -0.9092403467591758]
renderView1.CameraParallelScale = 1.224744871391589

# save animation
SaveAnimation(os.path.join(os.curdir, case_dir, "animation.png"), renderView1, ImageResolution=[951, 780],
    TransparentBackground=1,
    FrameWindow=[0, 11], 
    # PNG options
    CompressionLevel='0')

#================================================================
# addendum: following script captures some of the application
# state to faithfully reproduce the visualization during playback
#================================================================

#--------------------------------
# saving layout sizes for layouts

# layout/tab size in pixels
layout1.SetSize(951, 780)

#-----------------------------------
# saving camera placements for views

# current camera placement for renderView1
renderView1.CameraPosition = [1.3386063087420734, 1.9341725910949747, -0.29675584917037484]
renderView1.CameraFocalPoint = [-0.9516900784139893, -1.7321971682940716, 1.6279727269756041]
renderView1.CameraViewUp = [-0.2966923719110331, -0.2919856645026584, -0.9092403467591758]
renderView1.CameraParallelScale = 1.224744871391589

#--------------------------------------------
# uncomment the following to render all views
# RenderAllViews()
# alternatively, if you want to write images, you can use SaveScreenshot(...).
