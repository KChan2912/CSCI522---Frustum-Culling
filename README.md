# CSCI522---Frustum-Culling
## Implementing Frustum Culling within a Custom Engine for USC's CSCI-522

**Setting up the Frustum:**
We can start with two distances; the distance of the near plane and the far plane from the camera. Keep in mind that all we truly need to define any plane in a Three Dimensional space is a point and a normal. For the normal of the near plane and the far plane, we can simply re-use the forward normal of the camera itself (defined by *n*). As for the points on the plane, we can define those as the position of the camera plus the distance of the respective plane multiplied by the forward normal of the camera. This gives us the center of the near and far plane. Store these points and normals.

In order to obtain the remaining side planes, we first need to find the minimum and maximum co-ordinates of the far plane. We can first find the height of the triangle formed by the camera, the center of the far plane and the top of the far plane. We can do this by using the trigonometric tangent of the vertical FOV of the camera multiplied by the distance between the camera and the far plane center. *(h = tan(vFOV) x dFarPlane)* The height of the triangle formed by the camera, the center of the far plane and the side of the far plane is simply the previous height we found multiplied by the aspect ratio. Using these two values, we can find the points of the far plane by adding or subtracting these values appropriately to the center of the far plane. Store these points. Furthermore, using these very points we can find the normals of the side planes. The normal of the right side plane, for instance, is the cross product of the vector from the camera to the top right of the far plane and the vector from the camera to the bottom right of the far plane.

Having stored these, our frustum is complete. Make sure to recalculate these each time the camera moves/rotates.The method *do_CALCULATE_TRANSFORMATIONS* within *CameraSceneNode.cpp* is the perfect place to perform these calculations, as it’s called each time the camera moves. Additionally, it would be nice to have all your normals either pointing inwards or outwards with respect to the frustum, since the direction of the normal is important when checking whether something is on the inside or not. 

**Setting up the Meshes:**

We need to create Axis Aligned Bounding Boxes for our meshes, and use it to check whether a mesh is currently in view of the camera or not. In order to do this, I simply stored various combinations of the minimum and maximum X, Y and Z values of the mesh in a Vector3 array. *Mesh.cpp* (as well as the header file, since we need the array to be a member variable), *MeshInstance.cpp*, and *MeshManager.cpp* are files of note.

**Checking whether an object should be culled out:**

Within *SH_Draw.cpp*, within the method *do_GATHER_DRAWCALLS* (which is responsible for sending objects to be rendered) check whether all of the points of the Axis Aligned Bounding Box are outside of the plane or not. Simply check whether the dot product of the plane’s normal with the vector pointing from the camera to the point of the AABB is negative (or positive, depending on your plane normal’s orientation).
