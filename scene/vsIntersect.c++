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

#include <Performer/pf/pfTraverser.h>
#include "vsIntersect.h++"
#include <Performer/pr/pfLinMath.h>
#include <Performer/pf/pfSCS.h>

// ------------------------------------------------------------------------
// Constructor - Initializes the segment list
// ------------------------------------------------------------------------
vsIntersect::vsIntersect()
{
    int loop;

    segListSize = 0;

    pathsEnabled = 0;
    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        sectPath[loop] = NULL;

    facingMode = VS_INTERSECT_IGNORE_NONE;
    
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
    int loop;

    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        if (sectPath[loop] != NULL)
            delete (sectPath[loop]);
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
// Sets up the specified segment within the intersect object for a picking
// intersection. (Picking indicates choosing an object visible within a
// pane, often by clicking it with the mouse.)  The projection and
// viewpoint of the specified pane's view object, as well as the current
// position of the mouse as specified by the vsMouse object, are used in
// the pick segment determination. The number of the first segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setPickSeg(int segNum, vsPane *pane, vsMouse *mousePos)
{
    float xval, yval;

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setPickSeg: Segment number out of bounds\n");
        return;
    }

    xval = mousePos->getAxis(PF_X)->getPosition();
    yval = mousePos->getAxis(PF_Y)->getPosition();
    setPickSeg(segNum, pane, xval, yval);
}

// ------------------------------------------------------------------------
// Sets up the specified segment within the intersect object for a picking
// intersection. The projection and viewpoint of the specified pane's view
// object, as well as the current position of the mouse as specified by the
// x and y parameters, are used in the pick segment determination. x and y
// should be in the range [-1.0, 1.0] to indicate the scene visible in the
// pane (the center of the pane is (0, 0) for this purpose); it is not an
// error to specify values outside of this range. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setPickSeg(int segNum, vsPane *pane, double x, double y)
{
    pfChannel *paneChannel;
    pfVec3 ll, lr, ul, ur;
    vsVector upperLeft, upperRight, lowerLeft;
    vsVector rightDirection, downDirection;
    vsVector nearPt, farPt;

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setPickSeg: Segment number out of bounds\n");
        return;
    }
    
    paneChannel = pane->getBaseLibraryObject();
    
    // Calculate the pick point on the near clipping plane
    paneChannel->getNear(ll, lr, ul, ur);
    upperLeft.set(ul[0], ul[1], ul[2]);
    upperRight.set(ur[0], ur[1], ur[2]);
    lowerLeft.set(ll[0], ll[1], ll[2]);

    rightDirection = upperRight - upperLeft;
    downDirection = lowerLeft - upperLeft;
    
    nearPt = upperLeft + rightDirection.getScaled((x + 1.0) / 2.0) +
        downDirection.getScaled((y + 1.0) / 2.0);
    
    // Calculate the pick point on the far clipping plane
    paneChannel->getFar(ll, lr, ul, ur);
    upperLeft.set(ul[0], ul[1], ul[2]);
    upperRight.set(ur[0], ur[1], ur[2]);
    lowerLeft.set(ll[0], ll[1], ll[2]);

    rightDirection = upperRight - upperLeft;
    downDirection = lowerLeft - upperLeft;
    
    farPt = upperLeft + rightDirection.getScaled((x + 1.0) / 2.0) +
        downDirection.getScaled((y + 1.0) / 2.0);

    // Add the newly-built segment to the list using one of the other
    // segment setting calls
    setSeg(segNum, nearPt, farPt);
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
// Enables node path generation for intersection traversals. Paths will not
// be generated until the next intersect call.
// ------------------------------------------------------------------------
void vsIntersect::enablePaths()
{
    pathsEnabled = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables node path generation for intersection traversals. Existing path
// array objects are deleted at the next intersect call.
// ------------------------------------------------------------------------
void vsIntersect::disablePaths()
{
    pathsEnabled = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the facing mode for the intersection object. The face mode tells
// the object if it should ignore intersections with a particular side
// of a polygon.
// ------------------------------------------------------------------------
void vsIntersect::setFacingMode(int newMode)
{
    facingMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the facing mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getFacingMode()
{
    return facingMode;
}

// ------------------------------------------------------------------------
// Initiates an intersection traversal over the indicated geometry tree.
// The results of the traversal are stored and can be retrieved with the
// getIsect* functions.
// ------------------------------------------------------------------------
void vsIntersect::intersect(vsNode *targetNode)
{
    pfNode *performerNode, *geoNode;
    pfNode *pathNode;
    pfHit **hits[PFIS_MAX_SEGS];
    int loop, sloop, tloop;
    int arraySize;
    int flags;
    pfVec3 hitPoint, polyNormal;
    pfMatrix xformMat;
    pfPath *hitNodePath;
    int pathLength;
    int pathIndex;
    int workingIndex;
    pfNode *newIsectNode;
    vsNode *vessNode;
    pfMatrix xformAccum, lastXformAccum;
    pfMatrix *nodeXform;
    pfMatrix segmentXform;
    pfVec3 segPos, segDir;
    vsGrowableArray *performerPath;
    pfSegSet secondIsectSegs;
    pfHit **secondHits[PFIS_MAX_SEGS];

    // This is where the fun begins
    
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        performerNode = ((vsGeometry *)targetNode)->getBaseLibraryObject();
    else
        performerNode = ((vsComponent *)targetNode)->getBaseLibraryObject();

    // Set the intersection run to calculate paths or not depending on
    // if path generation is enabled for this object
    if (pathsEnabled)
        performerSegSet.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM | PFTRAV_IS_PATH;
    else
        performerSegSet.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;

    if (facingMode == VS_INTERSECT_IGNORE_FRONTFACE)
	performerSegSet.mode |= PFTRAV_IS_CULL_FRONT;
    else if (facingMode == VS_INTERSECT_IGNORE_BACKFACE)
	performerSegSet.mode |= PFTRAV_IS_CULL_BACK;

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
            if (sectPath[loop])
                delete (sectPath[loop]);
            sectPath[loop] = NULL;
            continue;
        }
        
        validFlag[loop] = 1;

        (hits[loop][0])->query(PFQHIT_POINT, &hitPoint);
        (hits[loop][0])->query(PFQHIT_NORM, &polyNormal);
        if (flags & PFHIT_XFORM)
        {
            (hits[loop][0])->query(PFQHIT_XFORM, &xformMat);
            
            for (sloop = 0; sloop < 4; sloop++)
                for (tloop = 0; tloop < 4; tloop++)
                    sectXform[loop][sloop][tloop] = xformMat[tloop][sloop];

            hitPoint.xformPt(hitPoint, xformMat);
            polyNormal.xformVec(polyNormal, xformMat);
            polyNormal.normalize();
        }
        else
            sectXform[loop].setIdentity();
        sectPoint[loop].set(hitPoint[0], hitPoint[1], hitPoint[2]);
        sectNorm[loop].set(polyNormal[0], polyNormal[1], polyNormal[2]);

        (hits[loop][0])->query(PFQHIT_NODE, &geoNode);
        sectGeom[loop] = (vsGeometry *)((vsSystem::systemObject)->
            getNodeMap()->mapSecondToFirst(geoNode));
        (hits[loop][0])->query(PFQHIT_PRIM, &(sectPrim[loop]));
        
        // Create path information if so requested
        if (pathsEnabled)
        {
            // If the path array for this segment hasn't been allocated,
            // do it now.
            if (sectPath[loop] == NULL)
                sectPath[loop] = new vsGrowableArray(10, 10);

            // Query the intersection path from Performer
            (hits[loop][0])->query(PFQHIT_PATH, &hitNodePath);

            if (hitNodePath)
            {
                // Get the length of the path
                pathLength = hitNodePath->getNum();

                // WORKAROUND:
                // Performer has the undesirable habit of not returning
                // the complete intersection path if it is deeper than
                // 32 nodes.  The loop below attempts to alleviate this
                // by tracing down until the path prematurely ends, then
                // performing another intersection from that point. This
                // is repeated until the original length of the intersection
                // path is reached.

                // Create a vsGrowableArray to store the pfNodes in the
                // intersection path.  Initialize index counters.
                pathIndex = 0;
                workingIndex = 0;
                performerPath = new vsGrowableArray(pathLength, 10);

                // Set up a matrix to accumulate transforms as we go down
                xformAccum.makeIdent();
                lastXformAccum.makeIdent();
                while (pathIndex < pathLength)
                {
                    // Get the next node in the path
                    pathNode = (pfNode *)(hitNodePath->get(workingIndex));

                    // If the node we get back is NULL, but it should
                    // be valid based on the path length reported,
                    // run the intersection again, starting with the
                    // previous node in the path
                    if (workingIndex >= 32)
                    {
                        newIsectNode = 
                            (pfNode *)(performerPath->getData(pathIndex-1));

                        // Transform the position and direction of the 
                        // original segment to the current coordinate
                        // frame.  The lastXformAccum matrix contains
                        // the global transform before newIsectNode.
                        segPos = performerSegSet.segs[loop].pos;
                        segDir = performerSegSet.segs[loop].dir;

                        segmentXform.invertFull(lastXformAccum);
                        segPos.xformPt(segPos, segmentXform);
                        segDir.xformVec(segDir, segmentXform);
                        segDir.normalize();

                        // Duplicate the current segment and put in the
                        // transformed position and direction
                        secondIsectSegs.mode = performerSegSet.mode;
                        secondIsectSegs.userData = NULL;
                        secondIsectSegs.segs[0].pos = segPos;
                        secondIsectSegs.segs[0].dir = segDir;
                        secondIsectSegs.segs[0].length = 
                            performerSegSet.segs[loop].length;
                        secondIsectSegs.activeMask = 0x1;
                        secondIsectSegs.isectMask = performerSegSet.isectMask;
                        secondIsectSegs.bound = NULL;
                        secondIsectSegs.discFunc = NULL;
                        
                        // Run the intersection with the new segment from
                        // the previous node in the path.
                        newIsectNode->isect(&secondIsectSegs, secondHits);

                        // Query the path again
                        (secondHits[0][0])->query(PFQHIT_FLAGS, &flags);
                        if (flags & PFQHIT_POINT)
                        {
                            // Retrieve the new path
                            (secondHits[0][0])->query(PFQHIT_PATH, 
                                 &hitNodePath);

                            // Reset the working index so that it points
                            // to first node after the beginning of the new 
                            // path (we already have the node at index 0).
                            workingIndex = 1;
                        }
                        else
                        {
                            // Print an error
                            printf("vsIntersect::intersect: Unable to "
                                   "complete the intersection path!\n");

                            // Fill the rest of the path with NULLs
                            for (sloop = pathIndex; sloop < pathLength; sloop++)
                                performerPath->setData(sloop, NULL);

                            // Exit the loop
                            pathIndex = pathLength;
                        }
                    }
                    else
                    {
                        // The node is valid.
                        // If it's a SCS/DCS node, multiply its matrix into
                        // the tranform accumulation matrix
                        if (pathNode->isOfType(pfSCS::getClassType()))
                        {
                            lastXformAccum = xformAccum;

                            nodeXform = 
                                (pfMatrix *)((pfSCS *)pathNode)->getMatPtr();

                            xformAccum.preMult(*nodeXform);
                        }
 
                        // Copy the pathNode into the path array and increment
                        // the indices.
                        performerPath->setData(pathIndex, pathNode);
                        pathIndex++;
                        workingIndex++;
                    }
                }

                // Initialize the index for the array of vsNodes
                arraySize = 0;
                for (sloop = 0; sloop < pathLength; sloop++)
                {
                    // Get the path node from the Performer path array
                    pathNode = (pfNode *)(performerPath->getData(sloop));

                    if (pathNode != NULL)
                    {
                        vessNode = (vsNode *)((vsSystem::systemObject)->
                            getNodeMap()->mapSecondToFirst(pathNode));
                        if (vessNode)
                        {
                            (sectPath[loop])->setData(arraySize++, vessNode);
                        }
                    }
                }
                
                // Terminate the path with a NULL
                (sectPath[loop])->setData(arraySize, NULL);
            }
            else
                printf("vsIntersect::intersect: Performer path object "
                    "not found\n");
        }
        else if (sectPath[loop])
        {
            delete (sectPath[loop]);
            sectPath[loop] = NULL;
        }

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
// Returns the point of intersection in global coordinates determined
// during the last intersection traversal for the specified segment. The
// number of the first segment is 0.
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
// Returns the polygon normal in global coordinates at the point of
// intersection determined during the last intersection traversal for the
// specified segment. The number of the first segment is 0.
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
// Returns a matrix containing the local-to-global coordinate transform for
// the object intersected with during the last intersection traversal for
// the specified segment. Note that the point and normal values for the
// same segment already have this data multiplied in. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsMatrix vsIntersect::getIsectXform(int segNum)
{
    vsMatrix errResult;

    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectXform: Segment number out of bounds\n");
        errResult.setIdentity();
        return errResult;
    }

    return sectXform[segNum];
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

// ------------------------------------------------------------------------
// Returns a pointer to a vsGrowableArray containing the node path from the
// scene root node to the intersected node. This array is reused by the
// intersection object after each intersect call and should not be deleted.
// Returns NULL if path calculation was not enabled during the last
// intersection traversal, or if there was no intersection. The number of
// the first segment is 0.
// ------------------------------------------------------------------------
vsGrowableArray *vsIntersect::getIsectPath(int segNum)
{
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPath: Segment number out of bounds\n");
        return 0;
    }

    return sectPath[segNum];
}
