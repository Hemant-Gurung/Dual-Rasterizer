# Dual-Rasterizer

Rasterization is essentially a method to solve the visibility problem.Visibility consists of being able to tell 
which parts of 3D objects are visible to the camera. Some parts of these objects can be bidden because they are 
either outside the camera's visible area or hidden by others objects.

Rasterization aproaches this process by projecting the 3D vertices making up object onto the screen using perspective projection. 
we loop over all pixel in the image and test whether they lie within the resulting 2D object. If they do, the pixel is filled with the 
corresponding color of the object.

![afbeelding](https://user-images.githubusercontent.com/84324141/199485311-3179ab1d-b2cf-4889-bf4e-db20e80331e7.png)


for (each pixel in image) {  
     for (each triangle in scene) { 
 
