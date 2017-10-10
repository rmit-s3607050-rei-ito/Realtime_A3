##### Interactive 3D Graphics and Animation Assignment 2: Frogger (part 2) #####
Submission done as a pair for:
  - s3607050 - Rei Ito
  - s3429648 - Pacific Thai

[Completed requirements]
 [1] Drawing
    - Plane drawn (Grid on the xz plane).
    - Cylinders drawn representing logs, Cubes to represent cars.
    - Normal vectors for all geometry being rendered (Tangents for the parabola instead).
    - Frog drawn as a sphere using parametric equations.
    - Parabola drawn via parametric equations (3D).
 
 [2] Animation
    - Controlling x and z direction of frog (ie, rotation around the y axis) 
      and speed/rotation of the frog’s jump (as per assignment 1).
    - Frog animation using numerical integration (3D)
	 - Logs and Cars move from left to right
 
 [2] Camera
    - Use a third person perspective camera, that follows the frog
 
 [3] Lighting and Texturing
    - Lighting applied to all geometry.
 
 [4] Keyboard controls
    - All keyboard controls implemented
		- Toggling axis
		- Toggling normals/tangents for parabola
		- Toggling lighting
		- Toggling textures
		- Increase/Decrease Speed
		- Increase/Decrease Angle
		- Jump
		- Rotate camera
		- Zoom camera
		- Rotating frog left/right
		- Doubling/Halving segments

[Additions]
  - Restricted speed so it cannot be negative
  - Restricted angle of frog rotation so it can only move forward in an arc shape
  - Initialized and created cars, logs and frog from arrays of vertex points (Primative shapes not used)
  
[Extra Notes]
  - Since 3 planes are used for the grid (one for water, one road and one grass), the road and water
  planes are slightly elevated. When the frog jumps and lands on those sections, it appears to have
  sunk into the ground, but this is only the case because the ground itself has been raised a small fraction
  
  - In carVerticies[], the texture coordinates are all set to 0,0 as it is not used. Texturing for the car was optional
  but it is left in there for Assignment 3 if needed then.
  
  - Individually both rotation along the x axis and y axis work fine, but if combined and you rotate the camera
  diagonally (with changes both to the x and y axis) the rotation looks strange. As a result we added a few extra
  key bindings: 1, 2, 3 which sets the state of the left-click rotation.
	1. Left->Right rotation only
	2. Up->Down rotation only
	3. Both rotations apply
  By default the rotation is set to 3.
  
  - The normals toggle button is set to toggle both the normal vectors and the generated normals on the shapes.
  I'm not sure if this was the intended function of the button but if not, we have the knowledge to undo it.
  
  - The lighting doesn't look perfect.
  
[Incomplete]
  - Wasn't able to put the plane's vertices into an array. There is commented code to show the attempt but some feedback on
  what went wrong would be appreciated.
  
  - The texture on the log wasn't implemented since we ran out of time. (Using windows devices, hard to test)
  
  - This submission wasn't tested on linux since the room was closed over the weekend.