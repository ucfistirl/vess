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
//    VESS Module:  vsIntersect.c++
//
//    Description:  Class for performing intersection tests between line
//                  segments and a whole or part of a VESS scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsIntersect.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the segment list
// ------------------------------------------------------------------------
vsIntersect::vsIntersect()
{
    segListSize = 0;
    
    performerSegSet.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;
    performerSegSet.userData = NULL;
    performerSegSet.activeMask = 0;
    performerSegSet.isectMask = 0xFFFFFFFF;
    performerSegSet.bound = NULL;
    performerSegSet.discFunc = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsIntersect::~vsIntersect()
{
}

// ------------------------------------------------------------------------
// Sets the number of segments to be intersected with
// ------------------------------------------------------------------------
void vsIntersect::setSegListSize(int newSize)
{
    int loop;
    unsigned int result;

    if (newSize > VS_INTERSECT_SEGS_MAX)
    {
        printf("vsIntersect::setSegListSize: Segment list is limited to a "
            "size of %d segments\n", VS_INTERSECT_SEGS_MAX);
        return;
    }
    if (newSize < 0)
    {
        printf("vsIntersect::setSegListSize: Invalid segment list size\n");
        return;
    }
    
    result = 0;
    for (loop = 0; loop < newSize; loop++)
        result |= (1 << loop);

    segListSize = newSize;
    performerSegSet.activeMask = result;
}

// ------------------------------------------------------------------------
// Retrieves the number of segments to be intersected with
// ------------------------------------------------------------------------
int vsIntersect::getSegListSize()
{
    return segListSize;
}

// ------------------------------------------------------------------------
// Sets the location of one of the intersection segments by its starting
// and ending points. The segNum value determines which segment is to be
// set; the number of the first segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setSeg(int segNum, vsVector startPt, vsVector endPt)
{
    int loop;
    vsVector start, end;
    pfVec3 pstart, pend;

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    start.clearCopy(startPt);
    start.setSize(3);
    end.clearCopy(endPt);
    end.setSize(3);
    
    for (loop = 0; loop < 3; loop++)
    {
        pstart[loop] = start[loop];
        pend[loop] = end[loop];
    }
    
    performerSegSet.segs[segNum].makePts(pstart, pend);
}

// ------------------------------------------------------------------------
// Sets the location of one of the intersection segments by its starting
// point, direction, and length. The segNum value determines which segment
// is to be set; the number of the first segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setSeg(int segNum, vsVector startPt, vsVector directionVec,
                         double length)
{
    int loop;
    vsVector start, dir;

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    start.clearCopy(startPt);
    start.setSize(3);
    dir.clearCopy(directionVec);
    dir.setSize(3);
    dir.normalize();
    
    for (loop = 0; loop < 3; loop++)
    {
        performerSegSet.segs[segNum].pos[loop] = start[loop];
        performerSegSet.segs[segNum].dir[loop] = dir[loop];
    }
    performerSegSet.segs[segNum].length = length;
}

// ------------------------------------------------------------------------
// Retrieves the starting point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsVector vsIntersect::getSegStartPt(int segNum)
{
    int loop;
    vsVector result(3);
    
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegStartPt: Segment number out of bounds\n");
        return result;
    }
    
    for (loop = 0; loop < 3; loop++)
        result[loop] = performerSegSet.segs[segNum].pos[loop];

    return result;
}

// ------------------------------------------------------------------------
// Retrieves the ending point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsVector vsIntersect::getSegEndPt(int segNum)
{
    int loop;
    vsVector result(3);
    
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegEndPt: Segment number out of bounds\n");
        return result;
    }
    
    for (loop = 0; loop < 3; loop++)
        result[loop] = performerSegSet.segs[segNum].pos[loop] +
            (performerSegSet.segs[segNum].dir[loop] *
            performerSegSet.segs[segNum].length);

    return result;
}

// ------------------------------------------------------------------------
// Returns a unit vector indicating the direction from the start point
// to the end point of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
vsVector vsIntersect::getSegDirection(int segNum)
{
    int loop;
    vsVector result(3);
    
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegDirection: Segment number out of bounds\n");
        return result;
    }
    
    for (loop = 0; loop < 3; loop++)
        result[loop] = performerSegSet.segs[segNum].dir[loop];

    return result;
}

// ------------------------------------------------------------------------
// Returns the length of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
double vsIntersect::getSegLength(int segNum)
{
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegLength: Segment number out of bounds\n");
        return 0.0;
    }
    
    return (performerSegSet.segs[segNum].length);
}

// ------------------------------------------------------------------------
// Sets the intersection mask
// ------------------------------------------------------------------------
void vsIntersect::setMask(unsigned int newMask)
{
    performerSegSet.isectMask = newMask;
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsIntersect::getMask()
{
    return performerSegSet.isectMask;
}

// ------------------------------------------------------------------------
// Initiates an intersection traversal over the indicated geometry tree.
// The results of the traversal are stored and can be retrieved with the
// getIsect* functions.
// ------------------------------------------------------------------------
void vsIntersect::intersect(vsNode *targetNode)
{
    pfNode *performerNode, *geoNode;
    pfHit **hits[PFIS_MAX_SEGS];
    int loop;
    int flags;
    pfVec3 hitPoint, polyNormal;
    pfMatrix xformMat;

    // This is where the fun begins
    
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        performerNode = ((vsGeometry *)targetNode)->getBaseLibraryObject();
    else
        performerNode = ((vsComponent *)targetNode)->getBaseLibraryObject();

    // Run the intersection traversal
    performerNode->isect(&performerSegSet, hits);
    
    // Interpret and store the results
    for (loop = 0; loop < segListSize; loop++)
    {
        (hits[loop][0])->query(PFQHIT_FLAGS, &flags);
        
        // Check for no intersection
        if (!(flags & PFHIT_POINT))
        {
            validFlag[loop] = 0;
            sectPoint[loop].set(0, 0, 0);
            sectNorm[loop].set(0, 0, 0);
            sectGeom[loop] = NULL;
            sectPrim[loop] = 0;
            continue;
        }
        
        validFlag[loop] = 1;

        (hits[loop][0])->query(PFQHIT_POINT, &hitPoint);
        (hits[loop][0])->query(PFQHIT_NORM, &polyNormal);
        if (flags & PFHIT_XFORM)
        {
            (hits[loop][0])->query(PFQHIT_XFORM, &xformMat);
            hitPoint.xformPt(hitPoint, xformMat);
            polyNormal.xformVec(polyNormal, xformMat);
            polyNormal.normalize();
        }
        sectPoint[loop].set(hitPoint[0], hitPoint[1], hitPoint[2]);
        sectNorm[loop].set(polyNormal[0], polyNormal[1], polyNormal[2]);
        
        (hits[loop][0])->query(PFQHIT_NODE, &geoNode);
        sectGeom[loop] = (vsGeometry *)((vsSystem::systemObject)->
            getNodeMap()->mapSecondToFirst(geoNode));
        (hits[loop][0])->query(PFQHIT_PRIM, &(sectPrim[loop]));
    }
}

// ------------------------------------------------------------------------
// Returns if the last intersection traversal found an intersection for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
int vsIntersect::getIsectValid(int segNum)
{
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectValid: Segment number out of bounds\n");
        return 0;
    }

    return validFlag[segNum];
}

// ------------------------------------------------------------------------
// Returns the point of intersection determined during the last
// intersection traversal for the specified segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsVector vsIntersect::getIsectPoint(int segNum)
{
    vsVector errResult(3);

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPoint: Segment number out of bounds\n");
        return errResult;
    }

    return sectPoint[segNum];
}

// ------------------------------------------------------------------------
// Returns the polygon normal at the point of intersection determined
// during the last intersection traversal for the specified segment. The
// number of the first segment is 0.
// ------------------------------------------------------------------------
vsVector vsIntersect::getIsectNorm(int segNum)
{
    vsVector errResult(3);

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectNorm: Segment number out of bounds\n");
        return errResult;
    }

    return sectNorm[segNum];
}

// ------------------------------------------------------------------------
// Returns the geometry object intersected with determined during the last
// intersection traversal for the specified segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsGeometry *vsIntersect::getIsectGeometry(int segNum)
{
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectGeometry: Segment number out of bounds\n");
        return NULL;
    }

    return sectGeom[segNum];
}

// ------------------------------------------------------------------------
// Returns the index of the primitive within the geometry object
// intersected with, determined during the last intersection traversal for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
int vsIntersect::getIsectPrimNum(int segNum)
{
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPrimNum: Segment number out of bounds\n");
        return 0;
    }

    return sectPrim[segNum];
}
