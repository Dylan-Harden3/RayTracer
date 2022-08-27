
# Ray Tracer
Shoots "rays" through each pixel and uses 3d vector colision detection to determine color, includes support for shadows and reflections.
The objects incuded are a sphere, ellipsoid, plane and 2 meshes.

## Scene 1
Scene simply includes 3 spheres with an overlapping shadow.
<br>
<br>
<img src="https://github.com/Dylan-Harden3/RayTracer/blob/master/build/scene1.jpg" width=600 />

## Scene 2
Scene inlcudes an ellipsoid, which is calculated the same way however transformed across all steps to match its new scale.
<br>
<br>
<img src="https://github.com/Dylan-Harden3/RayTracer/blob/master/build/scene3.jpg" width=600 />

## Scene 3
Includes 3 spheres and with recursive reflections.
<br>
<br>
<img src="https://github.com/Dylan-Harden3/RayTracer/blob/master/build/scene4.jpg" width=600 />

## Scene 4
Now renders a mesh, can render an arbitrary mesh however in our case its just the bunny. Uses a bounding sphere to increase performance as to not check every triangle against every ray.
<br>
<br>
<img src="https://github.com/Dylan-Harden3/RayTracer/blob/master/build/scene6.jpg" width=600 />

## Scene 5
Same as scene 1 however now the camera can be positioned arbitrarily in the world, you can compare this and 1 to see a different perspective.
<br>
<br>
<img src="https://github.com/Dylan-Harden3/RayTracer/blob/master/build/scene8.jpg" width=600 />
