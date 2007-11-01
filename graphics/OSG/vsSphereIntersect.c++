//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsSphereIntersect.c++
//
//    Description:  Class for performing intersection tests between a
//                  set of spheres and a whole or part of a VESS scene 
//                  graph
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsSphereIntersect.h++"
#include "vsComponent.h++"
#include "vsDynamicGeometry.h++"
#include "vsScene.h++"
#include "vsTransformAttribute.h++"
#include "vsSwitchAttribute.h++"
#include "vsSequenceAttribute.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the sphere list
// ------------------------------------------------------------------------
vsSphereIntersect::vsSphereIntersect() : sphereList(5, 10), currentPath(0, 10)
{
    int loop;

    // Initialize the sphere list
    sphereListSize = 0;

    // Initialize the path array
    pathsEnabled = 0;
    currentPathLength = 0;
    for (loop = 0; loop < VS_SPH_ISECT_MAX_SPHERES; loop++)
        sectPath[loop] = NULL;

    // Initialize grouping traversal modes
    switchTravMode = VS_SPH_ISECT_SWITCH_CURRENT;
    sequenceTravMode = VS_SPH_ISECT_SEQUENCE_CURRENT;
    lodTravMode = VS_SPH_ISECT_LOD_FIRST;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSphereIntersect::~vsSphereIntersect()
{
    int loop;

    // Clean up any intersect node paths that have been created
    for (loop = 0; loop < VS_SPH_ISECT_MAX_SPHERES; loop++)
        if (sectPath[loop] != NULL)
            delete (sectPath[loop]);
}

// ------------------------------------------------------------------------
// Corrects the s,t coordinates of a point in the given region of the s,t
// plane to be within the triangle defined by s = 0, t = 0, and s+t=1.
// This results in the point on the triangle closest to the initial point.
// See the getClosestPoint() method for the definition of the seven 
// regions.
// ------------------------------------------------------------------------
void vsSphereIntersect::computePointInRegion(int regionNum)
{
    double invDet;
    double numerator, denominator;
    double tmp0, tmp1;

    switch(regionNum)
    {
        case 0:

            // The closest point is interior to the triangle, simply divide
            // the parameters by the determinant.
            invDet = 1/det;
            s *= invDet;
            t *= invDet;
            break;

        case 1:
            // F(s) is the intersection of the distance function Q(s,t) with 
            // the s = 1 plane ( Q(s, 1-s) ).  The minimum of this function 
            // bounded to the interval [0,1] is the closest point.  Recall 
            // that the minimum of a function F(s) occurs where F'(s) = 0.
            // (F'(s) denotes the 1st derivative of F w.r.t. s)
            //
            // F(s) = Q(s, 1-s) = (a-2b+c)s^2 + 2(b-c+d-e)s + (c+2e+f)
            // F'(s)/2 = (a-2b+c)s + (b-c+d-e)
            // F'(S) = 0 when S = (c+e-b-d)/(a-2b+c)
            // a-2b+c = |E0-E1|^2 > 0, so only sign of c+e-b-d need be 
            //  considered

            // If the numerator is nonpositive, the closest point is at s=0
            numerator = c+e-b-d;
            if (numerator <= 0)
            {
                s = 0;
            }
            else
            {
                // Otherwise, compute the denominator
                denominator = a-2*b+c;

                // Make sure s doesn't exceed 1 (this would put it beyond the 
                // length of the edge)
                s = ((numerator >= denominator) ? 1 : numerator / denominator);
            }

            // T is simply 1-s, since the edge is characterized by s+t=1
            t = 1-s;
            break;

        case 2:
            // Region 2 uses the gradient of the distance function compared 
            // to the two nearest edges, in this case t=0 and s+t=1.  The 
            // nearest edge can be determined based on the dot products of 
            // each edge with the gradient of the distance function Q at the 
            // nearest vertex (s=0, t=1 in this case).
            //
            // Grad(Q) = 2(as+bt+d,bs+ct+e)
            // (0,-1) . Grad(Q(0,1)) = (0,-1) . (b+d,c+e) = -(c+e)
            // (1,-1) . Grad(Q(0,1)) = (1,-1) . (b+d,c+e) = (b+d)-(c+e)
            // Minimum is on edge s+t=1 if (1,-1) . Grad(Q(0,1)) < 0
            // otherwise, it is on edge s=0

            // Compute these two terms for efficiency, they are used multiple 
            // times
            tmp0 = b+d;
            tmp1 = c+e;

            // Check the sign of (1,-1) . Grad(Q(0,1)) to determine which edge 
            // the minimum is on
            if (tmp1 > tmp0)
            {
                // Minimum is on edge s+t=1, compute the closest point on this 
                // edge which will be ((c+e)-(b+d)) / (a-2b+c)
                numerator = tmp1 - tmp0;
                denominator = a - 2*b + c;

                // Make sure we don't run off the edge of the triangle
                if (numerator >= denominator)
                    s = 1;
                else
                    s = numerator / denominator;

                t = 1-s;
            }
            else
            {
                // Minimum is on edge s=0.  Compute the t coordinate using the
                // projection of the distance vector on the t axis.  Make sure
                // we stay bounded to the edge.
                s = 0;
                if (tmp1 <= 0)
                    t = 1;
                else if (e >= 0)
                    t = 0;
                else
                    t = -e/c;
            }
            break;

        case 3:
            // Similar to region 1. This time, we're intersecting Q with the 
            // s axis
            //
            // F(t) = Q(0, t) = ct^2 + 2et + f
            // F'(t)/2 = ct + e
            // F'(T) = 0 when T = -e/c

            // We know that we're on the s=0 edge
            s = 0;

            // Compute the t coordinate using the projection of the distance
            // vector on the t axis.  Make sure we stay bounded to the edge.
            if (e >= 0)
            {
                t = 0;
            }
            else
            {
                if (-e >= c)
                    t = 1;
                else
                    t = -e/c;
            }
            break;

        case 4:
            // Similar to region 2.  In this case, the nearest vertex is 
            // (s=0, t=0) and we're deciding between the s=0 and t=0 edge.
            //
            // Grad(Q) = 2(as+bt+d,bs+ct+e)
            // (1,0) . Grad(Q(0,0)) = (1,0) . (d,e) = d
            // (0,1) . Grad(Q(0,0)) = (0,1) . (d,e) = e
            // minimum is on edge s=0 if (1,0) . Grad(Q(0,0)) > 0
            // otherwise, it is on edge t=0

            // Check the sign of the projection of the distance vector on
            // the s axis
            if (d > 0)
            {
                // Minimum is on edge s=0.  Compute the t coordinate using
                // the projection of the distance vector on the t axis.
                s = 0;
                if (e > 0)
                    t = 0;
                else if (-e > c)
                    t = 1;
                else
                    t = -e/c;
            }
            else
            {
                // Minimum is on edge t=0; compute the s coordinate using
                // the projection of the distance vector on the s axis.
                t = 0;
                if (-d > a)
                    s = 1;
                else
                    s = -d/a;
            }
            break;

        case 5:
            // See region1() for a brief description of this math.  This time,
            // we're intersecting Q with the t axis
            //
            // F(s) = Q(s, 0) = as^2 + 2ds + f
            // F'(s)/2 = as + d
            // F'(S) = 0 when S = -d/a

            // We know that were on the t=0 edge
            t = 0;

            // Compute the s coordinate using the projection of the distance
            // vector on the s axis.  Make sure we stay bounded to the edge.
            if (d >= 0)
            {
                s = 0;
            }
            else
            {
                if (-d >= a)
                    s = 1;
                else
                    s = -d/a;
            }
            break;

        case 6:
            // Similar to region 2.  In this case the nearest vertex is 
            // (s=1, t=0) and we're deciding between the s+t=1 and t=0 edge.
            //
            // Grad(Q) = 2(as+bt+d,bs+ct+e)
            // (-1,0) . Grad(Q(1,0)) = (-1,0) . (a+d,b+e) = -(a+d)
            // (-1,1) . Grad(Q(1,0)) = (-1,1) . (a+d,b+e) = -(a+d)+(b+e)
            // minimum is on edge s+t=1 if (-1,1) . Grad(Q(1,0)) > 0
            // otherwise, it is on edge t=0

            // Compute these two terms for efficiency, they are used 
            // multiple times
            tmp0 = a+d;
            tmp1 = b+e;

            // Check the sign of (1,-1) . Grad(Q(1,0)) to determine which edge
            // the minimum is on.
            if (tmp1 < tmp0)
            {
                // Minimum is on edge s+t=1.  Compute the nearest point, which
                // will be ((c+e)-(b+d)) / (a-2b+c)
                numerator = c+e-b-d;
                denominator = a - 2*b + c;

                // Make sure we stay bounded to the edge
                s = ((numerator < 0) ? 0 : numerator/denominator);

                // Compute t by substituting the computed s into s+t=1
                t = 1-s;
            }
            else
            {
                // Minimum is on edge t=0, compute using the projection of 
                // the distance vector on the s axis.  Make sure we stay 
                // bounded to the edge.
                t = 0;
                if (tmp0 <= 0)
                    s = 1;
                else if (d >= 0)
                    s = 0;
                else
                    s = -d/a;
            }
            break;
    }

    if ((s > 1.0) || (t > 1.0))
    {
        printf("Error!  region %d\n", regionNum);
        printf("a = %g, b = %g, c = %g\n", a, b, c);
        printf("d = %g, e = %g, f = %g\n", d, e, f);
        printf("s = %g, t = %g\n", s, t);
    }
}

// ------------------------------------------------------------------------
// Determines the closest point on a triangle to any point in 3D space,
// from "Distance Between Point and Triangle in 3D" by David Eberly
// available at http://www.magic-software.com
//
// Solved parametrically based on a point P and triangle T.  If T's
// vertices are A, B, C, then it is defined parametrically by:
//
//  T(s,t) = A + sE0 + tE1  where E0 = B-A, E1 = C-A and
//    (s,t) in D, where D = {(s,t): s in [0,1], t in [0,1], s+t <= 1}
//
// Given these definitions, the distance function from P to T is
//   Q(s,t) = as^2 + 2bst + ct^2 + 2ds + 2et + f
//
// where a = E0 . E0, b = E0 . E1, c = E1 . E1, d = E0 . (A-P),
//       e = E1 . (A-P) and f = (A-P) . (A-P)
// ------------------------------------------------------------------------
bool vsSphereIntersect::getClosestPoint(atVector sphereCenter, atVector A,
                                        atVector B, atVector C, 
                                        atVector *closestPoint)
{
    // Compute the edge vectors and distance vector (vector from vertex A
    // to the center of the test sphere)
    atVector E0 = B-A;
    atVector E1 = C-A;
    atVector D = A - sphereCenter;

    // Compute several dot products that will be used along the way:

    //   Square length s axis edge
    a = E0.getDotProduct(E0);

    //   Dot product of s axis edge with t axis edge
    b = E0.getDotProduct(E1);

    //   Square length of t axis edge
    c = E1.getDotProduct(E1);

    //   Dot product of s axis edge with distance vector
    d = E0.getDotProduct(D);

    //   Dot product of t axis edge with distance vector
    e = E1.getDotProduct(D);

    //   Square length of distance vector
    f = D.getDotProduct(D);

    // Also compute some other important values:
    //   Determinant (magnitude of the cross product of the
    //   two edge vectors)
    det = a*c - b*b;

    //   Sphere center projected onto the s,t plane
    s = b*e - c*d;
    t = b*d - a*e;

    // Check for collinearity of the vertices (the determinant will be
    // zero if this is the case)
    if (det < 1.0e-12)
    {
        closestPoint->setSize(3);
        closestPoint->clear();
        return false;
    }

    // Classify the projected center point into one of the seven regions, and
    // correct the s,t coordinates so they fall into the constraints
    // 0<=s<=1, 0<=t<=1, s+t<=1.  The regions are enumerated as follows:
    //
    //        t
    //        ^
    //   \    |
    //    \ 2 |
    //     \  |
    //      \ |
    //       \|
    //        |
    //        |\
    //        | \
    //    3   |  \    1
    //        |   \
    //        | 0  \
    //        |     \
    //   -----------------------> s
    //        |       \
    //    4   |   5    \    6
    //        |         \
    //
    // (Note that region 0 is within the triangle itself)
    //
    // First, check the s+t<=1 condition.  If this holds, it eliminates
    // regions 1, 2, and 6, since s+t>1 in those cases.  Note that we
    // haven't scaled s and t by the determinant yet (to avoid a possibly
    // unnecessary division).
    if (s + t <= det)
    {
        // See if either (or both) parameters are negative
        if (s < 0)
        {
            // s negative, so either region 3 or 4, check t to
            // classify            
            if (t < 0)
            {
                // t negative
                computePointInRegion(4);
            }
            else
            {
                // t positive
                computePointInRegion(3);
            }
        }
        else if (t < 0)
        {
            // s positive, t negative
            computePointInRegion(5);
        }
        else
        {
            // The projected point is inside the triangle
            computePointInRegion(0);
        }
    }
    else
    {
        // We know that we're beyond the s+t=1 line, so classify the
        // region by the sign of s and t
        if (s < 0)
            computePointInRegion(2);
        else if (t < 0)
            computePointInRegion(6);
        else
            computePointInRegion(1);
    }

    // Convert from parametric back to rectangular coordinates
    closestPoint->setSize(3);
    *closestPoint = A + E0.getScaled(s) + E1.getScaled(t);
    return true;
}

// ------------------------------------------------------------------------
// Private function.  Tests the given intersection sphere against the given 
// OSG bounding box.  Returns true if an intersection occurs.
// ------------------------------------------------------------------------
bool vsSphereIntersect::intersectWithBox(vsSphere sphere, osg::BoundingBox box)
{
    double sqrDist;
    double sqrRadius;
    atMatrix invXform;
    atVector center;
    atVector radiusVec;
    double centerX, centerY, centerZ;

    // Initialize the square distance accumulator.  We use the squared
    // distance because it can be computed more efficiently
    sqrDist = 0.0;

    // Transform the sphere's center to local coordinates
    // and apply any scaling transform to the radius
    invXform = currentXform.getInverse();
    center = sphere.getCenterPoint();
    center = invXform.getPointXform(center);
    radiusVec.set(0.0, sphere.getRadius(), 0.0);
    radiusVec = invXform.getVectorXform(radiusVec);

    // Compute the square of the transformed sphere's radius
    sqrRadius = radiusVec.getDotProduct(radiusVec);

    // Get the x,y,z coordinates of the sphere (mainly for
    // readability)
    centerX = center[AT_X];
    centerY = center[AT_Y];
    centerZ = center[AT_Z];

    // Initialize the distance accumulator
    sqrDist = 0.0;
    
    // Check the x-axis, if the center is outside the range of the
    // bounding box, compute the square of the x distance.
    if (centerX < box.xMin())
        sqrDist += AT_SQR(centerX - box.xMin());
    else if (centerX > box.xMax())
        sqrDist += AT_SQR(centerX - box.xMax());
  
    // Check the y-axis, same procedure
    if (centerY < box.yMin())
        sqrDist += AT_SQR(centerY - box.yMin());
    else if (centerY > box.yMax())
        sqrDist += AT_SQR(centerY - box.yMax());

    // Check the z-axis, same procedure
    if (centerZ < box.zMin())
        sqrDist += AT_SQR(centerZ - box.zMin());
    else if (centerZ > box.zMax())
        sqrDist += AT_SQR(centerZ - box.zMax());

    // See if the sphere touches the box
    if (sqrDist < sqrRadius)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// VESS internal function.  Retrieves the normal of the triangle given
// by the specified geometry object and indices.
// ------------------------------------------------------------------------
atVector vsSphereIntersect::getNormal(vsGeometry *geometry, int aIndex,
                                      int bIndex, int cIndex, int primIndex)
{
    atVector normal;
    atVector aVertex, bVertex, cVertex;
    atVector aNorm, bNorm, cNorm;
    atVector abDelta, acDelta;

    // Extract the vertices in the closestVertices array to the A,B,C 
    // vectors (mainly for readability).  A, B, and C are the vertices
    // of the triangle that the sphere collided with.
    aVertex = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, aIndex);
    bVertex = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, bIndex);
    cVertex = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, cIndex);

    // Get the normal from the geometry
    switch (geometry->getBinding(VS_GEOMETRY_NORMALS))
    {
        case VS_GEOMETRY_BIND_NONE:
            // Compute the normal of the triangle
            normal = (cVertex-bVertex).getCrossProduct(aVertex-bVertex);
            normal.normalize();
            break;

        case VS_GEOMETRY_BIND_OVERALL:
            // Get the normal from the geometry object
            normal = geometry->getData(VS_GEOMETRY_NORMALS, 0);
            break;

        case VS_GEOMETRY_BIND_PER_PRIMITIVE:
            // Get the normal from the geometry object
            normal = geometry->getData(VS_GEOMETRY_NORMALS, primIndex);
            break;

        case VS_GEOMETRY_BIND_PER_VERTEX:
            // Get the three normals from the triangle in question
            aNorm = geometry->getData(VS_GEOMETRY_NORMALS, aIndex);
            bNorm = geometry->getData(VS_GEOMETRY_NORMALS, bIndex);
            cNorm = geometry->getData(VS_GEOMETRY_NORMALS, cIndex);

            // Compute the change in normal from point a to b and point
            // a to c
            abDelta = bNorm - aNorm;
            acDelta = cNorm - aNorm;

            // Use the s,t coordinates of the last intersection on the 
            // triangle to interpolate the normal over the face of the 
            // triangle
            normal = aNorm + abDelta.getScaled(s) + acDelta.getScaled(t);
            normal.normalize();
            break;
    }

    // Transform the normal by whatever our current transform is.
    normal = currentXform.getVectorXform(normal);

    return normal;
}

// ------------------------------------------------------------------------
// VESS internal function.  Tests the given sphere in the sphere list with
// the given vsGeometry object.  Updates all intersection state object
// variables for the given sphere index.
// ------------------------------------------------------------------------
void vsSphereIntersect::intersectWithGeometry(int sphIndex, 
                                              vsGeometry *geometry)
{
    vsSphere *sphere;
    atVector center;
    double radius;
    int i, j;
    int primCount;
    int triCount;
    int lengthSum;
    atVector a, b, c;
    int aIndex, bIndex, cIndex;
    atVector point;
    atVector normal;
    double sqrDist;
    int result;
    double oldDot, newDot;
    atVector closestPoint;
    double localSqrDist;
    int closestPrim;
    atVector closestNormal;
    int closestVertIndices[3];
    atVector distVec;

    // Get the center point and radius of the sphere
    sphere = (vsSphere *)sphereList[sphIndex];
    center = sphere->getCenterPoint();
    radius = sphere->getRadius();

    // Get the number of primitives in the geometry
    primCount = geometry->getPrimitiveCount();

    // Initialize the local square distance variable (used to remember the
    // closest point so far) to a large number
    localSqrDist = 1.0e9;

    // Initialize the length accumulator (this is used so we don't
    // have to sum the lengths array up to the current i to compute the 
    // index of the next triangle's first vertex in the j loop below)
    lengthSum = 0;

    // For each primitive
    for (i = 0; i < primCount; i++)
    {
        // We need to triangulate each primitive if not already using
        // triangles.  First, compute the number of triangles.
        switch (geometry->getPrimitiveType())
        {
            case VS_GEOMETRY_TYPE_POINTS:
            case VS_GEOMETRY_TYPE_LINES:
            case VS_GEOMETRY_TYPE_LINE_STRIPS:
            case VS_GEOMETRY_TYPE_LINE_LOOPS:
                // This method does not work with points or lines
                return;
                break;
 
            case VS_GEOMETRY_TYPE_TRIS:
                // Trivial case, one triangle per triangle
                triCount = 1;
                break;

            case VS_GEOMETRY_TYPE_TRI_STRIPS:
            case VS_GEOMETRY_TYPE_TRI_FANS:
                // Look up the vertex count in the lengths array and
                // subtract 2 to get the number of triangles
                triCount = geometry->getPrimitiveLength(i) - 2;
                break;

            case VS_GEOMETRY_TYPE_QUADS:
                // 2 triangles per quad
                triCount = 2;
                break;

            case VS_GEOMETRY_TYPE_QUAD_STRIPS:
                // Look up the vertex count in the lengths array.
                // Number of quads is (numVertices - 2) / 2 and
                // number of triangles is number of quads * 2, so
                // number of triangles is simply numVertices - 2
                triCount = geometry->getPrimitiveLength(i) - 2;
                break;

            case VS_GEOMETRY_TYPE_POLYS:
                // Look up the vertex count in the lengths array and
                // subtract 2 to get the primitive count (essentially same as 
                // a tri fan).
                triCount = geometry->getPrimitiveLength(i) - 2;
                break;

            default:
                // Invalid primitive type
                printf("vsSphereIntersect::intersectWithGeometry: "
                    "Invalid primitive type in geometry\n");
                return;
                break;
        }

        // Now intersect the sphere with each triangle of each primitive
        for (j = 0; j < triCount; j++)
        {
            // Extract the jth triangle from the primitive.  This is 
            // slightly different for each kind of primitive
            switch (geometry->getPrimitiveType())
            {
                case VS_GEOMETRY_TYPE_TRIS:
                    // Get the three vertices of the triangle
                    aIndex = 3*i;
                    bIndex = 3*i + 1;
                    cIndex = 3*i + 2;
                    break;

                case VS_GEOMETRY_TYPE_TRI_STRIPS:
                    // Get the vertices of the jth triangle
                    aIndex = lengthSum + j;
                    bIndex = lengthSum + j + 1;
                    cIndex = lengthSum + j + 2;
                    break;

                case VS_GEOMETRY_TYPE_TRI_FANS:
                    // Get the vertices of the jth triangle
                    aIndex = lengthSum;
                    bIndex = lengthSum + j + 1;
                    cIndex = lengthSum + j + 2;
                    break;

                case VS_GEOMETRY_TYPE_QUADS:
                    // Get the vertices of the jth triangle
                    aIndex = 4*i;
                    bIndex = 4*i + j + 1;
                    cIndex = 4*i + j + 2;
                    break;

                case VS_GEOMETRY_TYPE_QUAD_STRIPS:
                    // Get the vertices of the jth triangle
                    aIndex = lengthSum + j;
                    bIndex = lengthSum + j + 1;
                    cIndex = lengthSum + j + 2;
                    break;

                case VS_GEOMETRY_TYPE_POLYS:
                    // Get the vertices of the jth triangle
                    aIndex = lengthSum;
                    bIndex = lengthSum + j + 1;
                    cIndex = lengthSum + j + 2;
                    break;

                default:
                    // Invalid primitive type
                    printf("vsSphereIntersect::intersectWithGeometry: "
                        "Invalid primitive type in geometry\n");
                    return;
                    break;
            }

            // Get the three vertices from the triangle
            a = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, aIndex);
            b = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, bIndex);
            c = geometry->getData(VS_GEOMETRY_VERTEX_COORDS, cIndex);

            // Transform the vertices using the transformation matrix
            // we've accumulated during our traversal
            a = currentXform.getPointXform(a);
            b = currentXform.getPointXform(b);
            c = currentXform.getPointXform(c);

            // Intersect the sphere and triangle
            result = getClosestPoint(center, a, b, c, &point);

            // Make sure the closest point is valid (i.e.: ensure that the
            // triangle we picked was not collinear)
            if (result)
            {
                // Compute the squared distance and keep track of this 
                // triangle if it comes as close or closer than any other 
                // so far.  If it's equally close as another point, use
                // the normal to break ties.
                sqrDist = (point - center).getMagnitudeSquared();
                if (fabs(sqrDist - localSqrDist) < 1.0E-6)
                {
                    // Get the intersection normal
                    normal = getNormal(geometry, aIndex, bIndex, cIndex, i);

                    // Compute a vector from the intersection point to the
                    // center of the intersecting sphere (call this the
                    // distance vector)
                    distVec = center - point;
                    distVec.normalize();

                    // Compute the dot products of the current normal and
                    // the previous closest point's normal with the distance
                    // vector.
                    oldDot = closestNormal.getDotProduct(distVec);
                    newDot = normal.getDotProduct(distVec);

                    // The normal that more closely matches the distance 
                    // vector corresponds to the primitive that is facing
                    // the intersection sphere more directly.  We want to
                    // report this primitive as the intersecting primitive.
                    if (newDot > oldDot)
                    {
                        // Record point, normal distance, and primitive
                        closestPoint = point;
                        closestNormal = normal;
                        localSqrDist = sqrDist;
                        closestPrim = i;
                        closestVertIndices[0] = aIndex;
                        closestVertIndices[1] = bIndex;
                        closestVertIndices[2] = cIndex;
                    }
                }
                else if (sqrDist < localSqrDist)
                {
                    // Record point, normal, distance, and primitive
                    closestPoint = point;
                    closestNormal = 
                        getNormal(geometry, aIndex, bIndex, cIndex, i);
                    localSqrDist = sqrDist;
                    closestPrim = i;
                    closestVertIndices[0] = aIndex;
                    closestVertIndices[1] = bIndex;
                    closestVertIndices[2] = cIndex;
                }
            }
        }

        // Finished with primitive i, add the length of the primitive to
        // the length accumulator so we know where to start for the next
        // primitive
        lengthSum += geometry->getPrimitiveLength(i);
    }

    // Evaluate the closest point and see if we've found a collision
    if ((localSqrDist < closestSqrDist[sphIndex]) && 
        (localSqrDist < AT_SQR(radius)))
    {
        // Set all the intersection parameters (valid flag, point, normal,
        // transform, geometry, and primitive index)
        validFlag[sphIndex] = true;
        sectPoint[sphIndex] = closestPoint;
        sectNorm[sphIndex] = closestNormal;
        sectXform[sphIndex] = currentXform;
        sectGeom[sphIndex] = geometry;
        sectPrim[sphIndex] = closestPrim;
        sectVertIndices[sphIndex][0] = closestVertIndices[0];
        sectVertIndices[sphIndex][1] = closestVertIndices[1];
        sectVertIndices[sphIndex][2] = closestVertIndices[2];

        // Remember this distance as the closest distance for the current
        // sphere
        closestSqrDist[sphIndex] = localSqrDist;

        // Handle the intersection path, if it's enabled
        if (pathsEnabled)
        {
            // Create the array for the path, if necessary
            if (sectPath[sphIndex] == NULL)
                sectPath[sphIndex] = new vsGrowableArray(10, 10);

            // Copy the current path into the sphere's intersect path slot
            for (i = 0; i < currentPathLength; i++)
                (sectPath[sphIndex])->setData(i, currentPath[i]);

            (sectPath[sphIndex])->setData(currentPathLength, NULL);
        }
    }
}

// ------------------------------------------------------------------------
// Private function.  Recursively traverses the given subgraph testing each 
// sphere in the sphere list for intersection with the scene.
// ------------------------------------------------------------------------
void vsSphereIntersect::intersectSpheres(vsNode *targetNode)
{
    osg::Geode *osgGeode;
    osg::BoundingBox osgBox;
    atVector center;
    double radius;
    int i;
    vsSphere nodeSphere;
    atMatrix previousXform;
    atMatrix localXform;
    vsSphere previousBoundSphere;
    atVector currentCenter, radiusVec;
    double currentRadius;
    vsTransformAttribute *nodeXformAttr;
    atMatrix nodeXform;
    vsAttribute *groupAttr;
    vsSwitchAttribute *switchAttr;
    vsSequenceAttribute *sequenceAttr;
    int currentChild;

    // Check the intersection mask with the node's intersection value
    // and bail if they AND to zero
    if ((intersectMask & targetNode->getIntersectValue()) == 0)
    {
        return;
    }

    // Next, see if paths are enabled and add this node onto the
    // path
    if (pathsEnabled)
    {
        currentPath[currentPathLength] = targetNode;
        currentPathLength++;
    }

    // See if this is a leaf node or internal node of the graph
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        // Get the bounding box of the geometry (need to ask the
        // graphics API for this).  First get the osgGeode from the
        // vsGeometry.
        osgGeode = ((vsGeometry *)targetNode)->getBaseLibraryObject();
       
        // There should only be one drawable in the Geode (this is how
        // the vsDatabaseLoader works), print a warning if this isn't so
        if (osgGeode->getNumDrawables() > 1)
        {
            printf("vsSphereIntersect::intersectSpheres:  osg::Geode has "
                "more than 1 drawable!\n");
        }

        // See if the intersector's bounding sphere intersects with the 
        // bounding box.  If not, we need go no farther. 
        osgBox = osgGeode->getDrawable(0)->getBound();
        if (!intersectWithBox(boundSphere, osgBox))
        {
            return;
        }
        
        // Test the individual spheres against the bounding box and
        // then against the geometry, if the bounding box test passes
        for (i = 0; i < sphereListSize; i++)
        {
            if (intersectWithBox(*(vsSphere *)sphereList[i], osgBox))
            {
                intersectWithGeometry(i, (vsGeometry *)targetNode);
            }
        }
    }
    else
    {
        // Get the bounding sphere for the target node and
        // transform the nodes' bounding sphere by the 
        // current transform
        targetNode->getBoundSphere(&center, &radius);
        center = currentXform.getPointXform(center);
        radiusVec.set(0.0, radius, 0.0);
        radiusVec = currentXform.getVectorXform(radiusVec);
        currentRadius = radiusVec.getMagnitude();
        nodeSphere.setSphere(center, currentRadius);

        // Save the current global transform so we can restore it when
        // finished with this subgraph
        previousXform = currentXform;
         
        // Check for a transform and add it to the current transform
        // if one exists
        nodeXformAttr = (vsTransformAttribute *)(targetNode->
            getTypedAttribute(VS_ATTRIBUTE_TYPE_TRANSFORM, 0));
        if (nodeXformAttr != NULL)
        {
            // Get the combined (pre-, dynamic-, and post-) transform on 
            // the current node
            localXform = nodeXformAttr->getCombinedTransform();

            // Multiply in the transform on this node
            currentXform = currentXform * localXform;
        }

        // Check the node's bounding sphere with the bounding sphere
        // of the intersector spheres
        if (nodeSphere.isSphereIsect(boundSphere))
        {
            // Spheres intersect, start traversing children.
            // If there is a grouping attribute on this node, we may need
            // to traverse its children in a special way
            groupAttr = targetNode->getCategoryAttribute(
                VS_ATTRIBUTE_CATEGORY_GROUPING, 0);
            if (groupAttr != NULL)
            {
                // Determine what kind of grouping attribute this is
                if (groupAttr->getAttributeType() == VS_ATTRIBUTE_TYPE_SWITCH)
                {
                    // Switch attribute, check the current traversal mode
                    // and traverse accordingly
                    if (switchTravMode == VS_SPH_ISECT_SWITCH_ALL)
                    {
                        // Traverse all children
                        for (i = 0; i < targetNode->getChildCount(); i++)
                            intersectSpheres(targetNode->getChild(i));
                    }
                    else if (switchTravMode == VS_SPH_ISECT_SWITCH_CURRENT)
                    {
                        // Only traverse children that are enabled on the
                        // switch attribute
                        for (i = 0; i < targetNode->getChildCount(); i++)
                        {
                            switchAttr = (vsSwitchAttribute *)groupAttr;
                            if (switchAttr->isEnabled(i))
                                intersectSpheres(targetNode->getChild(i));
                        }
                    }
                }
                else if (groupAttr->getAttributeType() == 
                    VS_ATTRIBUTE_TYPE_SEQUENCE)
                {
                    // Sequence attribute, check the current traversal mode
                    // and traverse accordingly
                    if (sequenceTravMode == VS_SPH_ISECT_SEQUENCE_ALL)
                    {
                        // Traverse all children
                        for (i = 0; i < targetNode->getChildCount(); i++)
                            intersectSpheres(targetNode->getChild(i));
                    }
                    else if (sequenceTravMode == VS_SPH_ISECT_SEQUENCE_CURRENT)
                    {
                        // Only traverse the currently active child
                        sequenceAttr = (vsSequenceAttribute *)groupAttr;
                        currentChild = sequenceAttr->getCurrentChildNum();
                        intersectSpheres(targetNode->getChild(currentChild));
                    }
                }
                else if (groupAttr->getAttributeType() == 
                    VS_ATTRIBUTE_TYPE_LOD)
                {
                    // LOD attribute, check the current traversal mode
                    // and traverse accordingly
                    if (lodTravMode == VS_SPH_ISECT_LOD_ALL)
                    {
                        // Traverse all children
                        for (i = 0; i < targetNode->getChildCount(); i++)
                            intersectSpheres(targetNode->getChild(i));
                    }
                    else if (lodTravMode == VS_SPH_ISECT_LOD_FIRST)
                    {
                        // Only traverse the first child (highest LOD)
                        intersectSpheres(targetNode->getChild(0));
                    }
                }
                else
                {
                    // Other grouping attribute type (such as a decal),
                    // just traverse all children
                    for (i = 0; i < targetNode->getChildCount(); i++)
                        intersectSpheres(targetNode->getChild(i));
                }
            }
            else
            {
                // Traverse all children
                for (i = 0; i < targetNode->getChildCount(); i++)
                    intersectSpheres(targetNode->getChild(i));
            }
        }

        // Restore the current transform so that it no longer includes the 
        // transform on this node
        currentXform = previousXform;
    }

    // Remove the current node from the current path
    if (pathsEnabled)
        currentPathLength--;
}

// ------------------------------------------------------------------------
// Sets the number of spheres to be intersected with
// ------------------------------------------------------------------------
void vsSphereIntersect::setSphereListSize(int newSize)
{
    int loop;

    // Make sure we don't exceed the maximum list size
    if (newSize > VS_SPH_ISECT_MAX_SPHERES)
    {
        printf("vsSphereIntersect::setSphereListSize: Sphere list is limited "
            "to a size of %d spheres\n", VS_SPH_ISECT_MAX_SPHERES);
       return;
    }

    // Make sure the list size is valid (non-negative)
    if (newSize < 0)
    {
        printf("vsSphereIntersect::setSphereListSize: Invalid sphere list "
            "size\n");
        return;
    }

    // Check to see if we're shrinking the list
    if (newSize < sphereListSize)
    {
        // Unreference any vsSpheres we've created in the list slots 
        // that are going away.
        for (loop = newSize; loop < sphereListSize; loop++)
        {
            delete (vsSphere *)(sphereList[loop]);
        }
    }
    
    // Re-size the list
    sphereList.setSize(newSize);
    sphereListSize = newSize;
}

// ------------------------------------------------------------------------
// Retrieves the number of spheres to be intersected with
// ------------------------------------------------------------------------
int vsSphereIntersect::getSphereListSize()
{
    return sphereListSize;
}

// ------------------------------------------------------------------------
// Sets the location of one of the intersection spheres by its starting
// point, direction, and length. The sphNum value determines which sphere
// is to be set; the number of the first sphere is 0.
// ------------------------------------------------------------------------
void vsSphereIntersect::setSphere(int sphNum, atVector center, double radius)
{
    atVector sphCenter;

    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::setSphere: Sphere number out of bounds\n");
        return;
    }
    
    // Copy the center point and make sure it is 3 dimensional
    sphCenter.clearCopy(center);
    sphCenter.setSize(3);

    // Create the sphere structure if one is not already present
    if (sphereList[sphNum] == NULL)
    {
        sphereList[sphNum] = new vsSphere(sphCenter, radius);
    }
    else
    {
        ((vsSphere *)(sphereList[sphNum]))->setSphere(sphCenter, radius);
    }

    // Mark the sphere's intersection invalid (in case intersect() is never
    // called)
    validFlag[sphNum] = false;
}

// ------------------------------------------------------------------------
// Retrieves the center point of the indicated sphere. The number of the
// first sphere is 0.
// ------------------------------------------------------------------------
atVector vsSphereIntersect::getSphereCenter(int sphNum)
{
    atVector center;
    
    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getSphereStartPt: Sphere number out "
            "of bounds\n");
        return atVector(0.0, 0.0, 0.0);
    }
    
    // Get the sphere's center point
    center = ((vsSphere *)sphereList[sphNum])->getCenterPoint();

    // Return the result
    return center;
}

// ------------------------------------------------------------------------
// Retrieves the radius of the indicated sphere. The number of the first 
// sphere is 0.  Returns a negative radius if the requested sphere number
// is invalid. 
// ------------------------------------------------------------------------
double vsSphereIntersect::getSphereRadius(int sphNum)
{
    double result;
    
    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getSphereRadius: Sphere number out of "
            "bounds\n");
        return -1.0;
    }
    
    // Get the sphere end point
    result = ((vsSphere *)sphereList[sphNum])->getRadius();

    // Return the result
    return result;
}

// ------------------------------------------------------------------------
// Sets the intersection mask
// ------------------------------------------------------------------------
void vsSphereIntersect::setMask(unsigned int newMask)
{
    intersectMask = newMask;
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsSphereIntersect::getMask()
{
    return intersectMask;
}

// ------------------------------------------------------------------------
// Enables node path generation for intersection traversals. Paths will not
// be generated until the next intersect call.
// ------------------------------------------------------------------------
void vsSphereIntersect::enablePaths()
{
    pathsEnabled = true;
}

// ------------------------------------------------------------------------
// Disables node path generation for intersection traversals. Existing path
// array objects are deleted at the next intersect call.  Note that Open
// Scene Graph will always return a traversal path, but this will not
// be translated into a VESS path if paths are disabled.
// ------------------------------------------------------------------------
void vsSphereIntersect::disablePaths()
{
    pathsEnabled = false;
}

// ------------------------------------------------------------------------
// Sets the switch traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test when the component has a switch attribute.
// ------------------------------------------------------------------------
void vsSphereIntersect::setSwitchTravMode(int newMode)
{
    switchTravMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the switch traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsSphereIntersect::getSwitchTravMode()
{
    return switchTravMode;
}

// ------------------------------------------------------------------------
// Sets the sequence traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test when the component has a sequence attribute.
// ------------------------------------------------------------------------
void vsSphereIntersect::setSequenceTravMode(int newMode)
{
    sequenceTravMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the sequence traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsSphereIntersect::getSequenceTravMode()
{
    return sequenceTravMode;
}

// ------------------------------------------------------------------------
// Sets the level-of-detail traversal mode for the intersection object.
// This mode tells the object which of components' children should be used
// in the the intersection test when the component has an LOD attribute.
// ------------------------------------------------------------------------
void vsSphereIntersect::setLODTravMode(int newMode)
{
    lodTravMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the level-of-detail traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsSphereIntersect::getLODTravMode()
{
    return lodTravMode;
}

// ------------------------------------------------------------------------
// Initiates an intersection traversal over the indicated geometry tree.
// The results of the traversal are stored and can be retrieved with the
// getIsect* functions.
// ------------------------------------------------------------------------
void vsSphereIntersect::intersect(vsNode *targetNode)
{
    vsSphere sphereArray[VS_SPH_ISECT_MAX_SPHERES];
    int i;
    vsSphere nodeBound;
    atVector center;
    double radius;

    // Make sure we have at least one sphere to intersect with
    if (sphereListSize <= 0)
        return;

    // Initialize the intersection results
    memset(validFlag, 0, sizeof(validFlag));

    // Initialize the current transform
    currentXform.setIdentity();

    // Initialize the length of the intersection path
    currentPathLength = 0;

    // Initialize the closest distance variables
    for (i = 0; i < sphereListSize; i++)
    {
        closestSqrDist[i] = 1.0e9;
    }

    // Construct a bounding sphere around the intersection spheres
    for (i = 0; i < sphereListSize; i++)
    {
        sphereArray[i] = *((vsSphere *)sphereList[i]);
    }
    boundSphere.encloseSpheres(sphereArray, sphereListSize);

    // Get the bounding sphere for the target node
    targetNode->getBoundSphere(&center, &radius);
    nodeBound.setSphere(center, radius);

    // Check the bounding spheres of the node and the intersectors and 
    // call the recursive intersect method, if necessary
    intersectSpheres(targetNode);
}

// ------------------------------------------------------------------------
// Returns if the last intersection traversal found an intersection for
// the specified sphere. The number of the first sphere is 0.
// ------------------------------------------------------------------------
bool vsSphereIntersect::getIsectValid(int sphNum)
{
    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectValid: Sphere number out of "
            "bounds\n");
        return 0;
    }

    // Return the valid flag value of the corresponding sphere.  This will
    // be true if there was a valid intersection with this sphere.
    return validFlag[sphNum];
}

// ------------------------------------------------------------------------
// Returns the point of intersection in global coordinates determined
// during the last intersection traversal for the specified sphere. The
// number of the first sphere is 0.
// ------------------------------------------------------------------------
atVector vsSphereIntersect::getIsectPoint(int sphNum)
{
    atVector errResult(3);

    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectPoint: Sphere number out of "
            "bounds\n");
        return errResult;
    }

    // Return the point of intersection for this sphere
    return sectPoint[sphNum];
}

// ------------------------------------------------------------------------
// Returns the polygon normal in global coordinates at the point of
// intersection determined during the last intersection traversal for the
// specified sphere. The number of the first sphere is 0.
// ------------------------------------------------------------------------
atVector vsSphereIntersect::getIsectNorm(int sphNum)
{
    atVector errResult(3);

    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectNorm: Sphere number out of "
            "bounds\n");
        return errResult;
    }

    // Return the normal vector at this sphere's intersection point
    return sectNorm[sphNum];
}

// ------------------------------------------------------------------------
// Returns a matrix containing the local-to-global coordinate transform for
// the object intersected with during the last intersection traversal for
// the specified sphere. Note that the point and normal values for the
// same sphere already have this data multiplied in. The number of the
// first sphere is 0.
// ------------------------------------------------------------------------
atMatrix vsSphereIntersect::getIsectXform(int sphNum)
{
    atMatrix errResult;

    // Make sure this sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectXform: Sphere number out of "
            "bounds\n");

        // Return an identity matrix
        errResult.setIdentity();
        return errResult;
    }

    // Return the global transform of this intersection
    return sectXform[sphNum];
}

// ------------------------------------------------------------------------
// Returns the geometry object intersected with determined during the last
// intersection traversal for the specified sphere. The number of the
// first sphere is 0.
// ------------------------------------------------------------------------
vsGeometry *vsSphereIntersect::getIsectGeometry(int sphNum)
{
    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectGeometry: Sphere number out of "
            "bounds\n");
        return NULL;
    }

    // Return the vsGeometry intersected by this sphere
    return sectGeom[sphNum];
}

// ------------------------------------------------------------------------
// Returns the index of the primitive within the geometry object
// intersected with, determined during the last intersection traversal for
// the specified sphere. The number of the first sphere is 0.
// ------------------------------------------------------------------------
int vsSphereIntersect::getIsectPrimNum(int sphNum)
{
    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectPrimNum: Sphere number out of "
            "bounds\n");
        return 0;
    }

    // Return the primitive index within the intersected vsGeometry
    return sectPrim[sphNum];
}

// ------------------------------------------------------------------------
// Returns a pointer to a vsGrowableArray containing the node path from the
// scene root node to the intersected node. This array is reused by the
// intersection object after each intersect call and should not be deleted.
// Returns NULL if path calculation was not enabled during the last
// intersection traversal, or if there was no intersection. The number of
// the first sphere is 0.
// ------------------------------------------------------------------------
vsGrowableArray *vsSphereIntersect::getIsectPath(int sphNum)
{
    // Make sure the sphere number is valid
    if ((sphNum < 0) || (sphNum >= sphereListSize))
    {
        printf("vsSphereIntersect::getIsectPath: Sphere number out of "
            "bounds\n");
        return NULL;
    }

    // Return NULL if traversal path tracking is not enabled
    if (!pathsEnabled)
        return NULL;

    // Return the intersection node path for this sphere.
    return sectPath[sphNum];
}

// ------------------------------------------------------------------------
// Return the index into a geometry's data for the vertices of the
// polygon where the intersection point was calculated to be.
// ------------------------------------------------------------------------
int vsSphereIntersect::getIsectVertIndex(int sphNum, int index)
{
    return sectVertIndices[sphNum][index];
}
