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
  <img src="./pix/crotation-and-translation.png" width="300">
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
