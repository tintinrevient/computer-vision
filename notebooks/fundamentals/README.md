# Fundamentals

## Translation
<p float="left">
  <img src="./pix/translation.png" width="700">
</p>

## Rotation
<p float="left">
  <img src="./pix/rotation-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/rotation-2.png" width="700">
</p>

## Extrinsic Parameters
<p float="left">
  <img src="./pix/extrinsic-R-t-2d.png" width="700">
</p>

<p float="left">
  <img src="./pix/extrinsic-R-t-3D.png" width="700">
</p>

## Intrinsic Parameters
<p float="left">
  <img src="./pix/intrinsic-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/intrinsic-2.png" width="700">
</p>

<p float="left">
  <img src="./pix/intrinsic-K.png" width="700">
</p>

## Extrinsic & Intrinsic Parameters
<p float="left">
  <img src="./pix/extrinsic-intrinsic-parameters.png" width="700">
</p>

<p float="left">
  <img src="./pix/extrinsic-intrinsic-P.png" width="700">
</p>

## Pinhole Camera Model
<p float="left">
  <img src="./pix/pinhole-camera-model-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/pinhole-camera-model-2.png" width="700">
</p>

## Projective Geometry
* **Homogeneous coordinates**: (x, y, z) in a projective space of dimension 3 becomes (x, y, z, w)
* **Homogeneous coordinates**: two points whose values are **proportional** are **equivalent** points
* M = camera intrinsics matrix
  * c<sub>x</sub> and c<sub>y</sub> = displacement between **principal point** (intersection of the optical axis and the image plane) and **center of coordinates** on the projection screen
  * f<sub>x</sub> = F * s<sub>x</sub> and f<sub>y</sub> = F * s<sub>y</sub>
    * Two different focal lengths f<sub>x</sub> and f<sub>y</sub>, because the individual pixels on a typical low-cost imager are rectangular rather than square
    * F = physical focal length of the lens
    * s<sub>x</sub> and s<sub>y</sub> = the size of the individual imager elements, which is measured by **pixels per millimeter**

<p float="left">
  <img src="./pix/projective-geometry-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/projective-geometry-2.png" width="500">
</p>

## Distortion
distortion coefficients = (k<sub>1</sub>, k<sub>2</sub>, p<sub>1</sub>, p<sub>2</sub>, k<sub>3</sub>)

### Radial Distortion
<p float="left">
  <img src="./pix/radial-distortion-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/radial-distortion-2.png" width="500">
</p>

### Tangential Distortion
<p float="left">
  <img src="./pix/tangential-distortion-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/tangential-distortion-2.png" width="500">
</p>

## Rodrigues Transform
<p float="left">
  <img src="./pix/rodrigues-transform.png" width="700">
</p>

## Camera Coordinates and Object Coordinates
* R = **rotation matrix**
* T = **translation vector**

<p float="left">
  <img src="./pix/rotation-and-translation.png" width="300">
</p>

<p float="left">
  <img src="./pix/camera-and-object-coordinates.png" width="700">
</p>

## Rotation
* rotate around x-, y-, and z-axes in sequence with respective rotation angles ψ, φ, and θ
* the total rotation matrix R = R<sub>x</sub>(ψ) * R<sub>y</sub>(φ) * R<sub>z</sub>(θ)
* R * R<sup>T</sup> = R<sup>T</sup> * R = I<sub>3</sub>, where I<sub>3</sub> is the 3x3 identity matrix
* R's **inverse** = R's **transpose**

<p float="left">
  <img src="./pix/rotation-matrix-1.png" width="400">
</p>

<p float="left">
  <img src="./pix/rotation-matrix-2.png" width="700">
</p>

## Translation
* **translation vector** = origin<sub>object</sub> - origin<sub>camera</sub>

## Planar Homography
<p float="left">
  <img src="./pix/planar-homography-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/planar-homography-2.png" width="700">
</p>

<p float="left">
  <img src="./pix/planar-homography-3.png" width="500">
</p>

## Transform
* **affine transform**: rectangle -> parallelogram
* **perspective transform**: rectangle -> trapezoid

<p float="left">
  <img src="./pix/affine-and-perspective-transform.png" width="700">
</p>

## Stereo Imaging (Two Cameras)
* **undistortion**: remove the radial and tangential lens distortion -> undistorted images
* **rectification**: adjust the angles and distances between cameras -> rectified and **row-aligned**: two image planes are coplanar & two image rows are collinear (**frontal parallel**)
* **correspondence**: find the same features in the left and right camera views -> **disparity map**: x<sub>l</sub> - x<sub>r</sub>
* **reprojection**: disparity map -> **triangulation** -> depth map

## Triangulation
<p float="left">
  <img src="./pix/triangulation-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/disparity-depth-1.png" width="500">
</p>

<p float="left">
  <img src="./pix/disparity-depth-2.png" width="700">
</p>

<p float="left">
  <img src="./pix/triangulation-2.png" width="700">
</p>

## Epipolar Geometry
<p float="left">
  <img src="./pix/epipolar-line.png" width="700">
</p>

## Essential Matrix E and Fundamental Matrix F
* **essential matrix E** operates in physical coordinates
* **fundamental matrix F** operates in image pixel coordinates

<p float="left">
  <img src="./pix/E.png" width="300">
  <img src="./pix/F.png" width="300">
</p>

<p float="left">
  <img src="./pix/essential-matrix.png" width="700">
</p>

## Stereo Rectification
**Hartley's algorithm**:
* cv::stereoRectifyUncalibrated() -> H1 and H2 (3x3 rectification/homography matrix)
* find **homographies** that map the **epipoles** to infinity
* minimize the computed **disparities** between the two stereo images

<p float="left">
  <img src="./pix/hartleys-algorithm.png" width="700">
</p>

**Bouguet's algorithm**:
* cv::stereoCalibrate() -> cv::stereoRectify -> R1 and R2 (3x3 rectification/rotation matrix), P1 and P2 (3x4 projection matrix), Q (disparity to depth mapping matrix)
* minimize the amount of change **reprojection** produces for each of the two images
* maximize the common viewing area

**P**: 3x4 projection matrix (P1 and P2)

**Q**: 4x4 reprojection matrix (Q)

<p float="left">
  <img src="./pix/projection-matrix-1.png" width="300">
  <img src="./pix/projection-matrix-2.png" width="300">
</p>

**cv::initUndistortRectifyMap()** -> map1 and map2 (undistort map) -> remap()
* it is called once for the left image and once for the right image from the stereo image pair

<p float="left">
  <img src="./pix/rectification-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/rectification-2.png" width="700">
</p>

## Stereo Correspondence
**convert two images, one left and one right, into a single depth image**

**block matching (BM) algorithm**:
* prefiltering to **normalize** image brightness and enhance **texture**
* correspondence search along **horizontal epipolar lines** using a **SAD (Sum of Absolute Difference)** window
* postfiltering to eliminate bad correspondence matches

<p float="left">
  <img src="./pix/bm-algorithm.png" width="700">
</p>

<p float="left">
  <img src="./pix/disparity-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/disparity-2.png" width="700">
</p>

**disparity increment** -> **depth range resolution**
<p float="left">
  <img src="./pix/disparity-increment.png" width="700">
</p>

<p float="left">
  <img src="./pix/stereo-correspondence.png" width="700">
</p>

**semi-global block matching (SGBM) algorithm**: 
* precompute a C(x, y, d) per-pixel cost map that matches left_image(x, y) and right_image(x-d, y) using **Birchfield-Tomasi metrics**
* initialize the accumulator 3D cost map S(x, y, d) with 0
* for each of the 3-, 5-, or 8-direction (r), compute S<sup>(r)</sup>(x, y, d) using an iterative procedure
* add S<sup>(r)</sup>(x, y, d) for all r's to S(x, y, d)
* once S(x, y, d) is complete, we find d<sup>*</sup>(x, y) as argmin of S(x, y, d)
* we use the same **uniqueness check** and the same **subpixel interpolation** as in the StereoBM algorithm
* do the left-right check to make sure that left-to-right and right-to-left correspondences are consistent
* filter speckles using cv::filterSpeckles

<p float="left">
  <img src="./pix/path-1.png" width="700">
</p>

<p float="left">
  <img src="./pix/path-2.png" width="700">
</p>

## References
* https://opencv.org/
* https://docs.opencv.org/2.4/doc/tutorials/calib3d/table_of_content_calib3d/table_of_content_calib3d.html
* https://michaelbach.de/ot/
* https://electronics.howstuffworks.com/camera.htm
* https://electronics.howstuffworks.com/cameras-photography/digital/digital-camera.htm
* https://en.wikipedia.org/wiki/Homogeneous_coordinates
* https://en.wikipedia.org/wiki/Camera_matrix
* https://micro.magnet.fsu.edu/primer/lightandcolor/index.html
* https://www.cs.cmu.edu/~german/research/RealTime/realtime.html
* http://www.cs.toronto.edu/~kyros/pubs/00.ijcv.carve.pdf
* https://people.csail.mit.edu/drdaniel/mesh_animation/
