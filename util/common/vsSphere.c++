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
//    VESS Module:  vsSphere.c++
//
//    Description:  Math library class that contains a representation of
//                  a sphere as a center point and radius.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include "vsMatrix.h++"
#include "vsSphere.h++"

//------------------------------------------------------------------------
// Constructor - Sets the sphere to an empty sphere
//------------------------------------------------------------------------
vsSphere::vsSphere()
{
    setEmpty();
}

//------------------------------------------------------------------------
// Constructor - Sets the sphere to have the designated center point and
// radius
//------------------------------------------------------------------------
vsSphere::vsSphere(const vsVector &centerPoint, const double &sphereRadius)
{
    setSphere(centerPoint, sphereRadius);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsSphere::~vsSphere()
{
}

//------------------------------------------------------------------------
// Sets the sphere to an empty sphere
//------------------------------------------------------------------------
void vsSphere::setEmpty()
{
    center.set(0.0, 0.0, 0.0);
    radius = -1.0;
}

//------------------------------------------------------------------------
// Sets the sphere to have the designated center point and radius
//------------------------------------------------------------------------
void vsSphere::setSphere(const vsVector &centerPoint,
    const double &sphereRadius)
{
    center.clearCopy(centerPoint);
    center.setSize(3);
    radius = sphereRadius;
}

//------------------------------------------------------------------------
// Gets the center point of the sphere
//------------------------------------------------------------------------
vsVector vsSphere::getCenterPoint() const
{
    return center;
}

//------------------------------------------------------------------------
// Gets the radius of the sphere. Empty spheres have negative radii.
//------------------------------------------------------------------------
double vsSphere::getRadius() const
{
    return radius;
}

//------------------------------------------------------------------------
// Sets this sphere to a sphere that encloses both the original sphere,
// as well as the specified point. The resulting sphere is radius zero if
// it previously was an empty sphere. Has no effect if the specified point
// is already inside of the sphere.
//------------------------------------------------------------------------
void vsSphere::addPoint(const vsVector &point)
{
    vsVector pt;
    double distSqr, dist;
    vsVector moveDir(3);
    double moveDist;

    // Clean up the point by making a size-3 copy of it
    pt.clearCopy(point);
    pt.setSize(3);

    // Check the radius of this sphere
    if (radius < 0.0)
    {
        // If this in an empty sphere, then the result should be centered on
        // the target point and have a radius of zero
        center = pt;
        radius = 0.0;
    }
    else
    {
        // Check to make sure that the point isn't already within the sphere
        distSqr = (pt - center).getMagnitudeSquared();

        // Comparing the squares of the distances is just as effective as
        // comparing the distances themselves, and it doesn't require taking a
        // square root.
        if (distSqr < VS_SQR(radius))
            return;

        // * Extend the sphere around the new point. This involves moving the
        // center of the sphere to the midpoint of the segment from the target
        // point to the point on the sphere farthest away from the target
        // point. Then the radius of the sphere is increased by the amount
        // that the center point moved.

        // The direction to move is from the center, towards the new point
        moveDir = pt - center;
        moveDir.normalize();

        // The distance to move is half of the distance from the boundary of
        // the sphere (the radius) to the target point
        dist = sqrt(distSqr);
        moveDist = (dist - radius) / 2.0;

        // Move the center point of the sphere
        center += moveDir.getScaled(moveDist);

        // Increase the sphere's radius to cover both the old sphere and the
        // new point
        radius += moveDist;
    }
}

//------------------------------------------------------------------------
// Sets this sphere to a sphere that encloses both the original sphere,
// as well as the specified sphere. The resulting sphere is equal to the
// input sphere if this sphere is empty. Has no effect if the specified
// sphere is already completely inside of this sphere.
//------------------------------------------------------------------------
void vsSphere::addSphere(const vsSphere &sphere)
{
    vsVector pt;
    double dist, rad;
    vsVector moveDir(3);
    double moveDist;

    // Copy the input sphere's data to temporary variables for convenience
    pt = sphere.getCenterPoint();
    rad = sphere.getRadius();

    // Check the radius of this sphere
    if (radius < 0.0)
    {
        center = pt;
        radius = rad;
    }
    else
    {
        // Check to make sure that the new sphere isn't already within
        // this sphere
        dist = (pt - center).getMagnitude();

        // The new sphere is inside if the distance between the centers plus
        // the new sphere's radius is less then this sphere's radius.
        if ((dist + rad) < radius)
            return;

        // * Extend the sphere around the new sphere. This involves moving the
        // center of the sphere to the midpoint of the segment that ranges
        // from the point on this sphere that is farthest away from the center
        // of the new sphere, to the point on the new sphere that is farthest
        // away from this sphere. Then the radius of the sphere is increased
        // by the amount that the center point moved.

        // Extend this sphere around the farthest point on the
        // new sphere

        // The direction to move is from the center, towards the new point
        moveDir = pt - center;
        moveDir.normalize();

        // The distance to move is half of the distance from the boundary of
        // the sphere (the radius) to the far point on the target sphere
        moveDist = ((dist + rad) - radius) / 2.0;

        // Move the center point of the sphere
        center += moveDir.getScaled(moveDist);

        // Increase the sphere's radius to cover both the old sphere and the
        // new sphere
        radius += moveDist;
    }
}
    
//------------------------------------------------------------------------
// Sets this sphere to the smallest sphere that contains all of the given
// points. Can modify the order of the points within the list.
//------------------------------------------------------------------------
void vsSphere::enclosePoints(vsVector *points, int pointCount)
{
    vsSphere result;
    int ssize;
    int loop;
    double sqrDist, maxSqrDist;
    int maxIdx;
    int going;

    // Bounds checking
    if (pointCount < 0)
    {
        printf("vsSphere::enclosePoints: Bad pointCount value (%d)\n",
            pointCount);
        return;
    }

    // If pointCount is zero, then the result will be an empty sphere
    if (pointCount == 0)
    {
        setEmpty();
        return;
    }

    // Start with a sphere that just encompasses the first point
    result = moveToFront(points, 1, NULL, 0, &ssize);

    // Keep iterating as long as the function result is changing
    do
    {
        // Initialization
        going = 0;
        maxSqrDist = -1.0;
        maxIdx = -1;

        // Determine which point is the farthest away from the center point of
        // the current result sphere
        for (loop = ssize; loop < pointCount; loop++)
        {
            // Find the distance from the result center point to the loop'th
            // point
            sqrDist = (result.getCenterPoint() - points[loop])
                .getMagnitudeSquared();

            // If this point is farther away than any previous points, then
            // record its index and distance
            if (sqrDist > maxSqrDist)
            {
                maxSqrDist = sqrDist;
                maxIdx = loop;
            }
        }

        // If the farthest away point is outside of the result sphere, then
        // run the move-to-front algorithm to calculate a new sphere with the
        // new point in it.
        if (maxSqrDist > (VS_SQR(result.getRadius()) + 1E-6))
        {
            // Run the algorithm with the new point
            result = moveToFront(points, ssize, &(points[maxIdx]), 1,
                &ssize);

            // Move the new point to the front of the point list
            promote(points, maxIdx);

            // Keep iterating
            going = 1;
        }
    }
    while (going);

    // Copy the result to this sphere
    *this = result;
}

//------------------------------------------------------------------------
// Sets this sphere to the smallest sphere that encompasses all of the
// given spheres. Can modify the order of the spheres within the list.
//------------------------------------------------------------------------
void vsSphere::encloseSpheres(vsSphere *spheres, int sphereCount)
{
    vsSphere result;
    int ssize;
    int loop;
    double dist, maxDist;
    int maxIdx;
    int going;

    // Bounds checking
    if (sphereCount < 0)
    {
        printf("vsSphere::encloseSpheres: Bad sphereCount value (%d)\n",
            sphereCount);
        return;
    }

    // If sphereCount is zero, then the result will be an empty sphere
    if (sphereCount == 0)
    {
        setEmpty();
        return;
    }

    // Start with a sphere that just encompasses the first sphere
    result = moveToFront(spheres, 1, NULL, 0, &ssize);

    // Keep iterating as long as the function result is changing
    do
    {
        // Initialization
        going = 0;
        maxDist = -1.0;
        maxIdx = -1;

        // Determine which sphere is the farthest outside of the current
        // result sphere
        for (loop = ssize; loop < sphereCount; loop++)
        {
            // Find the distance from the result center point to the
            // farthest-away point on the loop'th sphere
            dist = (result.getCenterPoint() - spheres[loop].getCenterPoint())
                .getMagnitude() + spheres[loop].getRadius();

            // If this far point is farther away than any previous points,
            // then record its sphere's index and distance
            if (dist > maxDist)
            {
                maxDist = dist;
                maxIdx = loop;
            }
        }

        // If the farthest away point is outside of the result sphere, then
        // run the move-to-front algorithm to calculate a new sphere with the
        // new sphere in it.
        if (maxDist > (result.getRadius() + 1E-6))
        {
            // Run the algorithm with the new sphere
            result = moveToFront(spheres, ssize, &(spheres[maxIdx]), 1,
                &ssize);

            // Move the new sphere to the front of the sphere list
            promote(spheres, maxIdx);

            // Keep iterating
            going = 1;
        }
    }
    while (going);

    // Copy the result to this sphere
    *this = result;
}

//------------------------------------------------------------------------
// Determines if the given point is within or on the boundary of this
// sphere. If this sphere is empty, the function always returns false.
//------------------------------------------------------------------------
bool vsSphere::isPointInside(const vsVector &point) const
{
    vsVector pt;
    double distSqr;

    // If the sphere is empty, return false
    if (radius < 0.0)
        return false;

    // Make a clean duplicate of the input point
    pt.clearCopy(point);
    pt.setSize(3);

    // Compute the squared distance between the target point and this sphere's
    // center
    distSqr = (pt - center).getMagnitudeSquared();

    // Comparing the squares of the distances is just as effective as
    // comparing the distances themselves, and it doesn't require
    // taking a square root.
    if (distSqr > VS_SQR(radius))
        return false;

    // If we've gotten this far, then the point must be inside
    return true;
}

//------------------------------------------------------------------------
// Determines if the given sphere is within this sphere. If this sphere is
// empty, the function always returns false.
//------------------------------------------------------------------------
bool vsSphere::isSphereInside(const vsSphere &sphere) const
{
    double dist;

    // If either this sphere or the target sphere are empty, return false
    if (radius < 0.0)
        return false;
    if (sphere.getRadius() < 0.0)
        return false;

    // Get the distance between the two spheres' centers
    dist = (sphere.getCenterPoint() - center).getMagnitude();

    // The sphere is considered inside if the distance between the two
    // spheres' centers plus the radius of the sphere parameter is less
    // than the radius of this sphere.
    if ((dist + sphere.getRadius()) > (radius + VS_DEFAULT_TOLERANCE))
        return false;

    // If we've gotten this far, then the sphere must be inside
    return true;
}

//------------------------------------------------------------------------
// Determines if the segment specified by the two given points intersects
// the sphere. If this sphere is empty, this function always returns
// false.
//------------------------------------------------------------------------
bool vsSphere::isSegIsect(const vsVector &segStart, const vsVector &segEnd)
    const
{
    vsVector start, end;
    vsVector v0, v1;
    vsVector norm;
    double param, distSqr;

    // If the sphere is empty, return false
    if (radius < 0.0)
        return false;

    // Make copies of the parameters that are of size 3
    start.clearCopy(segStart);
    start.setSize(3);
    end.clearCopy(segEnd);
    end.setSize(3);

    // Compute temporary vectors consisting of the vector from the segment
    // start to the end, and the vector from the segment start to the center
    // point of this sphere.
    v0 = end - start;
    v1 = center - start;

    // Compute the 'key point': the point on the line (not necessarily on the
    // segment) closest to the center of the sphere. This key point is
    // represented by a parameter value that indicates the scaled distance
    // (0.0 - 1.0) from the segment start to end that is the closest point to
    // the center of the sphere.
    param = (v0.getDotProduct(v1) / v0.getDotProduct(v0));

    // Compute the square of the magnitude of the vector from this
    // key point to the center of the sphere
    norm = v1 - v0.getScaled(param);
    distSqr = norm.getDotProduct(norm);

    // If the key point is actually off one of the ends of the segment,
    // increase the point-center distance by the square of the distance
    // from the key point to the closest end of the segment
    if (param > 1.0)
        distSqr += VS_SQR((param - 1.0) * v0.getMagnitude());
    else if (param < 0.0)
        distSqr += VS_SQR(param * v0.getMagnitude());

    // Compare the final (squared) distance to the (square of the) radius
    // of the circle
    if (distSqr > VS_SQR(radius))
        return false;

    // If we've gotten this far, the segment must intersect the sphere
    return true;
}

//------------------------------------------------------------------------
// Determines if the given sphere intersects this sphere. If this sphere
// is empty, this function always returns false.
//------------------------------------------------------------------------
bool vsSphere::isSphereIsect(const vsSphere &sphere) const
{
    double dist;

    // If this sphere is empty, return false
    if (radius < 0.0)
        return false;

    // If the target sphere is empty, return false
    if (sphere.getRadius() < 0.0)
        return false;

    // The target sphere intersects this sphere if the distance between the
    // two spheres' centers is less than the sum of the two spheres' radii
    dist = (center - sphere.getCenterPoint()).getMagnitude();
    if (dist > (radius + sphere.getRadius()))
        return false;

    return true;
}

//------------------------------------------------------------------------
// Prints a textual representation of this sphere to stdout
//------------------------------------------------------------------------
void vsSphere::print() const
{
    // Use vsVector's print function to print the center point
    center.print();

    // Print the radius
    printf(" (%0.4lf)", radius);
}

//------------------------------------------------------------------------
// Prints a textual representation of this sphere to the specified file
//------------------------------------------------------------------------
void vsSphere::print(FILE *fp) const
{
    // Use vsVector's print function to print the center point
    center.print(fp);

    // Print the radius
    fprintf(fp, " (%0.4lf)", radius);
}

//------------------------------------------------------------------------
// Internal function
// Moves the point at the specified index to the front of the list
//------------------------------------------------------------------------
void vsSphere::promote(vsVector *points, int index)
{
    vsVector temp;
    int loop;

    // No work to do if the index refers to the first entry
    if (index < 1)
        return;

    // Copy the index'th point
    temp = points[index];

    // Move all of the points from the first entry to the (index-1)'th entry
    // down the list by one
    for (loop = index-1; loop >= 0; loop--)
        points[loop+1] = points[loop];

    // Place the index'th point at the front of the list
    points[0] = temp;
}

//------------------------------------------------------------------------
// Internal function
// Calculates the smallest sphere that has the specified points exactly
// on its boundary
//------------------------------------------------------------------------
vsSphere vsSphere::calcSphereOn(vsVector *points, int pointCount)
{
    vsSphere result;
    int loop, sloop;
    vsMatrix linSysMat;
    vsVector linSysVec;
    vsVector qvec[3];
    vsVector cvec;
    vsVector resultCenter;
    double resultRadius;

    // Bounds checking
    if ((pointCount < 0) || (pointCount > 4))
    {
        printf("vsSphere::calcSphereOn: pointCount parameter out of bounds\n");
        return result;
    }

    // The cases for which the number of points is less than three are
    // trivial ones; handle them first.
    if (pointCount < 3)
    {
        // For one or two points, the addPoint way of extending a sphere will
        // still yield the smallest possible sphere.
        for (loop = 0; loop < pointCount; loop++)
            result.addPoint(points[loop]);

        return result;
    }

    // Initialize the linear system matrix and vector
    linSysMat.setIdentity();
    for (loop = 0; loop < 4; loop++)
        linSysVec[loop] = 1.0;

    // Calculate the intermediate vectors; each vector is from the first point
    // to one of the other points
    for (loop = 0; loop < pointCount-1; loop++)
        qvec[loop] = points[loop+1] - points[0];

    // Fill in the linear system matrix and vector, up to the number
    // of points passed in minus one
    for (loop = 0; loop < pointCount-1; loop++)
    {
        linSysVec[loop] = qvec[loop].getDotProduct(qvec[loop]);
        for (sloop = loop; sloop < pointCount-1; sloop++)
        {
            linSysMat[loop][sloop] = 2.0 * qvec[loop].getDotProduct(qvec[sloop]);
            linSysMat[sloop][loop] = linSysMat[loop][sloop];
        }
    }

    // Solve the linear system by inverting the matrix and multiplying
    // by the vector
    linSysMat.invert();
    linSysVec = linSysMat.getFullXform(linSysVec);

    // Compute the (center - first point) vector by taking a weighted
    // average of the intermediate vectors, based on the results of
    // the linear system solution
    cvec.set(0.0, 0.0, 0.0);
    for (loop = 0; loop < pointCount-1; loop++)
        cvec += qvec[loop].getScaled(linSysVec[loop]);

    // Compute the final center by adding the first point to the
    // (center - first point) vector
    resultCenter = cvec + points[0];

    // Compute the final radius as the magnitude of the
    // (center - first point) vector
    resultRadius = cvec.getMagnitude();

    // Clean up and return
    result.setSphere(resultCenter, resultRadius);
    return result;
}

//------------------------------------------------------------------------
// Internal function
// Implementation of the Welzl move-to-front recursive algorithm for
// selecting the points that lie on the boundary of the smallest sphere
// that encompasses all of the points.
//------------------------------------------------------------------------
vsSphere vsSphere::moveToFront(vsVector *points, int pointCount,
    vsVector *basis, int basisCount, int *supportSize)
{
    vsSphere result;
    int loop;
    vsVector basisStore[4];
    int ssize;

    // Calculate the sphere that goes through all of the basis points
    result = calcSphereOn(basis, basisCount);

    // Copy the size of the basis point set to the basis size return parameter
    *supportSize = basisCount;

    // If the set of basis points contains four points, then we can't add any
    // more. (More than four might mean that there is no sphere that goes
    // through all of them.) Just return.
    if (basisCount == 4)
        return result;

    // Copy the basis points to a temporary array
    for (loop = 0; loop < basisCount; loop++)
        basisStore[loop] = basis[loop];

    // For each point, check if that point is outside of the sphere indicated
    // by the basis points
    for (loop = 0; loop < pointCount; loop++)
    {
        if (!result.isPointInside(points[loop]))
        {
            // If the point is outside our sphere, add that point to the basis
            // array, and recurse.
            basisStore[basisCount] = points[loop];
            result = moveToFront(points, loop, basisStore, basisCount+1,
                &ssize);

            // Copy the new size of the basis point set to the return
            // parameter
            *supportSize = ssize;

            // Move the point in question to the front of the list of points
            promote(points, loop);
        }
    }

    // Return the last sphere we've found that encompasses all of the points
    return result;
}

//------------------------------------------------------------------------
// Internal function
// Moves the sphere at the specified index to the front of the list
//------------------------------------------------------------------------
void vsSphere::promote(vsSphere *spheres, int index)
{
    vsSphere temp;
    int loop;

    // No work to do if the index refers to the first entry
    if (index < 1)
        return;

    // Copy the index'th sphere
    temp = spheres[index];

    // Move all of the spheres from the first entry to the (index-1)'th entry
    // down the list by one
    for (loop = index-1; loop >= 0; loop--)
        spheres[loop+1] = spheres[loop];

    // Place the index'th sphere at the front of the list
    spheres[0] = temp;
}

//------------------------------------------------------------------------
// Internal function
// Calculates the smallest sphere that is tangent to and encompasses the
// specified spheres
//------------------------------------------------------------------------
vsSphere vsSphere::calcSphereAround(vsSphere *spheres, int sphereCount)
{
    vsSphere result;
    int loop, sloop;
    vsMatrix dotMatrix, dotMatrixInv;
    vsVector mvec, vvec;
    double a, b, c, disc;

    vsVector qvec[3];
    vsVector cvec;
    vsVector resultCenter;
    double resultRadius;

    // Bounds checking
    if ((sphereCount < 0) || (sphereCount > 4))
    {
        printf("vsSphere::calcSphereAround: sphereCount parameter "
            "out of bounds\n");
        return result;
    }

    // The cases for which the number of spheres is less than three are
    // trivial ones; handle them first.
    if (sphereCount < 3)
    {
        for (loop = 0; loop < sphereCount; loop++)
            result.addSphere(spheres[loop]);
        return result;
    }

    // Initialize the dot product matrix, radius vector, and constant vector
    dotMatrix.setIdentity();
    for (loop = 0; loop < 4; loop++)
    {
        mvec[loop] = 0.0;
        vvec[loop] = 1.0;
    }

    // Calculate the intermediate vectors; each vector is from the first
    // sphere's center point to one of the other center points
    for (loop = 0; loop < sphereCount-1; loop++)
        qvec[loop] = spheres[loop+1].getCenterPoint() -
            spheres[0].getCenterPoint();

    // Fill in the matrix and vectors, up to the number
    // of spheres passed in minus one
    for (loop = 0; loop < sphereCount-1; loop++)
    {
        for (sloop = loop; sloop < sphereCount-1; sloop++)
        {
            dotMatrix[loop][sloop] = qvec[loop].getDotProduct(qvec[sloop]);
            dotMatrix[sloop][loop] = dotMatrix[loop][sloop];
        }

        mvec[loop] = spheres[0].getRadius() - spheres[loop+1].getRadius();
        vvec[loop] = ( (VS_SQR(spheres[0].getRadius()) -
                        VS_SQR(spheres[loop+1].getRadius()) +
                        dotMatrix[loop][loop]) / 2.0 );
    }

    // Invert the dot product matrix and transform both vectors by it
    dotMatrixInv = dotMatrix.getInverse();
    mvec = dotMatrixInv.getFullXform(mvec);
    vvec = dotMatrixInv.getFullXform(vvec);

    // Compute the values needed to solve the quadratic equation that
    // describes the enclosing sphere's radius
    a = 0.0;
    b = 0.0;
    c = 0.0;
    for (loop = 0; loop < sphereCount-1; loop++)
        for (sloop = 0; sloop < sphereCount-1; sloop++)
        {
            a += (mvec[loop] * mvec[sloop] * dotMatrix[loop][sloop]);
            b += (mvec[loop] * vvec[sloop] * dotMatrix[loop][sloop]);
            c += (vvec[loop] * vvec[sloop] * dotMatrix[loop][sloop]);
        }
    a = 1.0 - a;
    b = -2.0 * (spheres[0].getRadius() - b);
    c = VS_SQR(spheres[0].getRadius()) - c;

    // Solve the quadratic equation for the enclosing sphere's radius
    disc = VS_SQR(b) - (4.0 * a * c);
    resultRadius = (sqrt(disc) - b) / (2.0 * a);

    // Compute the (center - first point) vector by taking a weighted
    // average of the intermediate vectors, plugging the enclosing
    // sphere's radius back in to obtain the weights
    cvec.set(0.0, 0.0, 0.0);
    for (loop = 0; loop < sphereCount-1; loop++)
        cvec += qvec[loop].getScaled(
            vvec[loop] - (mvec[loop] * resultRadius) );

    // Compute the final center by adding the first point to the
    // (center - first point) vector
    resultCenter = cvec + spheres[0].getCenterPoint();

    // Clean up and return
    result.setSphere(resultCenter, resultRadius);
    return result;
}

//------------------------------------------------------------------------
// Internal function
// Implementation of the Welzl move-to-front recursive algorithm for
// selecting the spheres that are tangent to the boundary of the smallest
// sphere that encompasses all of the spheres.
//------------------------------------------------------------------------
vsSphere vsSphere::moveToFront(vsSphere *spheres, int sphereCount,
    vsSphere *basis, int basisCount, int *supportSize)
{
    vsSphere result;
    int loop;
    vsSphere basisStore[4];
    int ssize;

    // Calculate the sphere that encompasses and is tangent to all of the
    // basis spheres
    result = calcSphereAround(basis, basisCount);

    // Copy the size of the basis sphere set to the basis size return
    // parameter
    *supportSize = basisCount;

    // If the set of basis spheres contains four spheres, then we can't add
    // any more. (More than four might mean that there is no sphere that is
    // tangent to all of them.) Just return.
    if (basisCount == 4)
        return result;

    // Copy the basis spheres to a temporary array
    for (loop = 0; loop < basisCount; loop++)
        basisStore[loop] = basis[loop];

    // For each sphere, check if that sphere is partially or completely
    // outside of the sphere indicated by the basis spheres
    for (loop = 0; loop < sphereCount; loop++)
    {
        if (!result.isSphereInside(spheres[loop]))
        {
            // If the sphere is outside our sphere, add that sphere to the
            // basis array, and recurse.
            basisStore[basisCount] = spheres[loop];
            result = moveToFront(spheres, loop, basisStore, basisCount+1,
                &ssize);

            // Copy the new size of the basis sphere set to the return
            // parameter
            *supportSize = ssize;

            // Move the sphere in question to the front of the list of spheres
            promote(spheres, loop);
        }
    }

    // Return the last sphere we've found that encompasses all of the spheres
    return result;
}