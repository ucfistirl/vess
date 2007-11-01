
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

#include <Performer/pf/pfTraverser.h>
#include <Performer/pr/pfLinMath.h>
#include <Performer/pf/pfSCS.h>

#include "vsDynamicGeometry.h++"
#include "vsSkeletonMeshGeometry.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the segment list
// ------------------------------------------------------------------------
vsIntersect::vsIntersect()
{
    int loop;

    // Initialize to zero active segments
    segListSize = 0;

    // Set path finding to off and clear the path pointers
    pathsEnabled = 0;
    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        sectPath[loop] = NULL;

    // Set default intersection modes: intersect with back faces, intersect
    // with only the currently active child (or children) of a switch,
    // intersect with only the currently visible child of a sequence, and
    // intersect only with the most-detailed child of an LOD.
    facingMode = VS_INTERSECT_IGNORE_NONE;
    switchMode = VS_INTERSECT_SWITCH_CURRENT;
    seqMode = VS_INTERSECT_SEQUENCE_CURRENT;
    lodMode = VS_INTERSECT_LOD_FIRST;
    
    // Set up the segset
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

    // Delete the intersection path growable array objects, if they exist
    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        if (sectPath[loop] != NULL)
            delete (sectPath[loop]);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsIntersect::getClassName()
{
    return "vsIntersect";
}

// ------------------------------------------------------------------------
// Sets the number of segments to be intersected with
// ------------------------------------------------------------------------
void vsIntersect::setSegListSize(int newSize)
{
    int loop;
    unsigned int result;

    // Bounds checks
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
    
    // Create the segment mask, which contains one bit on for each
    // active segment
    result = 0;
    for (loop = 0; loop < newSize; loop++)
        result |= (1 << loop);

    // Store the list size, and set the active segment mask in the
    // Performer segment set
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
void vsIntersect::setSeg(int segNum, atVector startPt, atVector endPt)
{
    int loop;
    atVector start, end;
    pfVec3 pstart, pend;

    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    // Clean up the input vectors
    start.clearCopy(startPt);
    start.setSize(3);
    end.clearCopy(endPt);
    end.setSize(3);
    
    // Convert atVector to Performer pfVec3
    for (loop = 0; loop < 3; loop++)
    {
        pstart[loop] = start[loop];
        pend[loop] = end[loop];
    }
    
    // Make the intersection segment from the start and end points
    performerSegSet.segs[segNum].makePts(pstart, pend);
}

// ------------------------------------------------------------------------
// Sets the location of one of the intersection segments by its starting
// point, direction, and length. The segNum value determines which segment
// is to be set; the number of the first segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setSeg(int segNum, atVector startPt, atVector directionVec,
                         double length)
{
    int loop;
    atVector start, dir;

    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    // Clean up the input vectors
    start.clearCopy(startPt);
    start.setSize(3);
    dir.clearCopy(directionVec);
    dir.setSize(3);
    dir.normalize();
    
    // Copy into the segset
    for (loop = 0; loop < 3; loop++)
    {
        performerSegSet.segs[segNum].pos[loop] = start[loop];
        performerSegSet.segs[segNum].dir[loop] = dir[loop];
    }

    // Copy the length of the segment
    performerSegSet.segs[segNum].length = length;
}

// ------------------------------------------------------------------------
// Retrieves the starting point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegStartPt(int segNum)
{
    int loop;
    atVector result(3);
    
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegStartPt: Segment number out of bounds\n");
        return result;
    }
    
    // Copy pfVec3 to atVector
    for (loop = 0; loop < 3; loop++)
        result[loop] = performerSegSet.segs[segNum].pos[loop];

    // Return the desired point
    return result;
}

// ------------------------------------------------------------------------
// Retrieves the ending point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegEndPt(int segNum)
{
    int loop;
    atVector result(3);
    
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegEndPt: Segment number out of bounds\n");
        return result;
    }
    
    // Calculate the segment end point from the Performer segment direction
    // and length
    for (loop = 0; loop < 3; loop++)
        result[loop] = performerSegSet.segs[segNum].pos[loop] +
            (performerSegSet.segs[segNum].dir[loop] *
            performerSegSet.segs[segNum].length);

    // Return the desired point
    return result;
}

// ------------------------------------------------------------------------
// Returns a unit vector indicating the direction from the start point
// to the end point of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegDirection(int segNum)
{
    int loop;
    atVector result(3);
    
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegDirection: Segment number out of bounds\n");
        return result;
    }
    
    // Copy pfVec3 to atVector
    for (loop = 0; loop < 3; loop++)
        result[loop] = performerSegSet.segs[segNum].dir[loop];

    // Return the desired vector
    return result;
}

// ------------------------------------------------------------------------
// Returns the length of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
double vsIntersect::getSegLength(int segNum)
{
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegLength: Segment number out of bounds\n");
        return 0.0;
    }
    
    // Return the length taken from the Performer segset
    return (performerSegSet.segs[segNum].length);
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
    atVector upperLeft, upperRight, lowerLeft;
    atVector rightDirection, downDirection;
    atVector nearPt, farPt;

    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setPickSeg: Segment number out of bounds\n");
        return;
    }
    
    // Get the pfChannel
    paneChannel = pane->getBaseLibraryObject();
    
    // Calculate the pick point on the near clipping plane by interpolating
    // between the near plane's corner points
    paneChannel->getNear(ll, lr, ul, ur);
    upperLeft.set(ul[0], ul[1], ul[2]);
    upperRight.set(ur[0], ur[1], ur[2]);
    lowerLeft.set(ll[0], ll[1], ll[2]);

    rightDirection = upperRight - upperLeft;
    downDirection = lowerLeft - upperLeft;
    
    nearPt = upperLeft + rightDirection.getScaled((x + 1.0) / 2.0) +
        downDirection.getScaled((y + 1.0) / 2.0);
    
    // Calculate the pick point on the far clipping plane by interpolating
    // between the far plane's corner points
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
    // Set the intersection mask in the Performer segset
    performerSegSet.isectMask = newMask;
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsIntersect::getMask()
{
    // Get the intersection mask from the Performer segset
    return performerSegSet.isectMask;
}

// ------------------------------------------------------------------------
// Enables node path generation for intersection traversals. Paths will not
// be generated until the next intersect call.
// ------------------------------------------------------------------------
void vsIntersect::enablePaths()
{
    pathsEnabled = true;
}

// ------------------------------------------------------------------------
// Disables node path generation for intersection traversals. Existing path
// array objects are deleted at the next intersect call.
// ------------------------------------------------------------------------
void vsIntersect::disablePaths()
{
    pathsEnabled = false;
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
// Sets the switch traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test when the component has a switch attribute.
// ------------------------------------------------------------------------
void vsIntersect::setSwitchTravMode(int newMode)
{
    switchMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the switch traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getSwitchTravMode()
{
    return switchMode;
}

// ------------------------------------------------------------------------
// Sets the sequence traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test when the component has a sequence attribute.
// ------------------------------------------------------------------------
void vsIntersect::setSequenceTravMode(int newMode)
{
    seqMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the sequence traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getSequenceTravMode()
{
    return seqMode;
}

// ------------------------------------------------------------------------
// Sets the level-of-detail traversal mode for the intersection object.
// This mode tells the object which of components' children should be used
// in the the intersection test when the component has an LOD attribute.
// ------------------------------------------------------------------------
void vsIntersect::setLODTravMode(int newMode)
{
    lodMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the level-of-detail traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getLODTravMode()
{
    return lodMode;
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
    
    // Get the Performer node to start intersecting with, based on the
    // type of the VESS node we got
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        performerNode = ((vsGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
        performerNode = 
            ((vsDynamicGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
        performerNode = 
            ((vsSkeletonMeshGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        performerNode = ((vsComponent *)targetNode)->getBaseLibraryObject();

    // Compute the intersection mode for the segSet based on the
    // user's preferences
    
    // Always intersect to primitive level and calculate normals
    performerSegSet.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;

    // If intersection paths are desired, add the path calculate mode
    // constant to the segset
    if (pathsEnabled)
        performerSegSet.mode |= PFTRAV_IS_PATH;

    // If front or backface ignoring is desired, add the appropriate
    // cull face mode constant to the segset
    if (facingMode == VS_INTERSECT_IGNORE_FRONTFACE)
        performerSegSet.mode |= PFTRAV_IS_CULL_FRONT;
    else if (facingMode == VS_INTERSECT_IGNORE_BACKFACE)
        performerSegSet.mode |= PFTRAV_IS_CULL_BACK;

    // If the switch traversal mode is something other than 'just the
    // current child', then add the appropriate switch traversal mode
    // constant to the segset
    if (switchMode == VS_INTERSECT_SWITCH_NONE)
        performerSegSet.mode |= PFTRAV_SW_NONE;
    else if (switchMode == VS_INTERSECT_SWITCH_ALL)
        performerSegSet.mode |= PFTRAV_SW_ALL;

    // If the sequence traversal mode is something other than 'just the
    // current child', then add the appropriate sequence traversal mode
    // constant to the segset
    if (seqMode == VS_INTERSECT_SEQUENCE_NONE)
        performerSegSet.mode |= PFTRAV_SEQ_NONE;
    else if (seqMode == VS_INTERSECT_SEQUENCE_ALL)
        performerSegSet.mode |= PFTRAV_SEQ_ALL;

    // If the LOD traversal mode is something other than 'just the highest
    // detail child', then add the appropriate LOD traversal mode
    // constant to the segset
    if (lodMode == VS_INTERSECT_LOD_NONE)
        performerSegSet.mode |= PFTRAV_LOD_NONE;
    else if (lodMode == VS_INTERSECT_LOD_ALL)
        performerSegSet.mode |= PFTRAV_LOD_ALL;

    // Run the intersection traversal
    performerNode->isect(&performerSegSet, hits);
    
    // Interpret and store the results
    for (loop = 0; loop < segListSize; loop++)
    {
        // Get the intersection success flag for the segment
        (hits[loop][0])->query(PFQHIT_FLAGS, &flags);
        
        // Check for no intersection
        if (!(flags & PFHIT_POINT))
        {
            // Set the intersection result values to failed or default values
            validFlag[loop] = false;
            sectPoint[loop].set(0, 0, 0);
            sectNorm[loop].set(0, 0, 0);
            sectGeom[loop] = NULL;
            sectPrim[loop] = 0;

            // Set the intersection path to NULL
            if (sectPath[loop])
                delete (sectPath[loop]);
            sectPath[loop] = NULL;

	    // Resume with the next segment
            continue;
        }
        
        // Mark the intersection as valid
        validFlag[loop] = true;

	// Retrieve the point of intersection and intersection normal
        (hits[loop][0])->query(PFQHIT_POINT, &hitPoint);
        (hits[loop][0])->query(PFQHIT_NORM, &polyNormal);

        // Check for an available global transform matrix
        if (flags & PFHIT_XFORM)
        {
            // Get the global transform matrix
            (hits[loop][0])->query(PFQHIT_XFORM, &xformMat);
            
            // Copy the global transform matrix to our global matrix
	    // result array
            for (sloop = 0; sloop < 4; sloop++)
                for (tloop = 0; tloop < 4; tloop++)
                    sectXform[loop][sloop][tloop] = xformMat[tloop][sloop];

            // Transform the point of intersection by the matrix
            hitPoint.xformPt(hitPoint, xformMat);

            // Transform the intersection normal by the matrix
            polyNormal.xformVec(polyNormal, xformMat);
            polyNormal.normalize();
        }
        else
        {
            // No matrix; set the result array entry to identity
            sectXform[loop].setIdentity();
        }

        // Copy the point of intersection and the intersection normal to
	// the result arrays
        sectPoint[loop].set(hitPoint[0], hitPoint[1], hitPoint[2]);
        sectNorm[loop].set(polyNormal[0], polyNormal[1], polyNormal[2]);

	// Retrieve the node intersected with
        (hits[loop][0])->query(PFQHIT_NODE, &geoNode);

        // Map the Performer pfGeode to its associated vsGeometry, and
	// put that vsGeometry in the result array
        sectGeom[loop] = (vsGeometry *)((vsNode::getMap())->
            mapSecondToFirst(geoNode));

	// Retrieve the primitive intersected with
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

            // Interpret the path data, if any
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

                // For each node in the path returned, try to match it with
                // its VESS node counterpart.  Continue until we have
                // processed all nodes in the path.
                while (pathIndex < pathLength)
                {
                    // Get the next node in the path
                    pathNode = (pfNode *)(hitNodePath->get(workingIndex));

                    // Usually, invalid nodes show up as NULL in the path,
                    // but occasionally, there are invalid nodes that are
                    // not NULL.  Because of this, we can't trust any node 
                    // deeper than 32 in the path, so we re-intersect every 
                    // 32 nodes.
                    if (workingIndex >= 32)
                    {
                        // Get the previous node in the path (the last
                        // one that was valid).
                        newIsectNode = 
                            (pfNode *)(performerPath->getData(pathIndex-1));

                        // Get the position and direction of the original
                        // intersection segment
                        segPos = performerSegSet.segs[loop].pos;
                        segDir = performerSegSet.segs[loop].dir;

                        // Transform the position and direction of the 
                        // original segment to the current coordinate
                        // frame.  The lastXformAccum matrix contains
                        // the global transform before newIsectNode.
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

                        // Get the point of intersection, making sure an
                        // intersection happened
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
                            // No intersection happened, print an error
                            printf("vsIntersect::intersect: Unable to "
                                   "complete the intersection path!\n");

                            // Fill the rest of the path with NULLs
                            for (sloop = pathIndex; sloop < pathLength; sloop++)
                                performerPath->setData(sloop, NULL);

                            // Exit the loop by setting the current node index
                            // to the length of the path (we just processed
                            // the rest of the path by filling it with NULLs).
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
                            // Keep track of the previous accumulated
                            // transform (we might need it for a subsequent
                            // re-intersection)
                            lastXformAccum = xformAccum;

                            // Get the transform on the current node
                            nodeXform = 
                                (pfMatrix *)((pfSCS *)pathNode)->getMatPtr();

                            // Combine the current node's transform with
                            // the transform we've accumulated in our path
                            // trace so far.
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

                    // If the path node is valid
                    if (pathNode != NULL)
                    {
                        // Get a VESS node from the Performer node
                        vessNode = (vsNode *)
			    ((vsNode::getMap())->mapSecondToFirst(pathNode));

                        // If the vess node is valid, add it to the path
                        if (vessNode)
                            (sectPath[loop])->setData(arraySize++, vessNode);
                    }
                }
                
                // Terminate the path with a NULL
                (sectPath[loop])->setData(arraySize, NULL);
                
                // We don't need the Performer path anymore; delete it.
                delete performerPath;
            }
            else
                printf("vsIntersect::intersect: Performer path object "
                    "not found\n");
        }
        else if (sectPath[loop])
        {
            // If paths are disabled, but there's a path object left over
	    // from a previous intersection run, delete it.
            delete (sectPath[loop]);
            sectPath[loop] = NULL;
        }

    }
}

// ------------------------------------------------------------------------
// Returns if the last intersection traversal found an intersection for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
bool vsIntersect::getIsectValid(int segNum)
{
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectValid: Segment number out of bounds\n");
        return false;
    }

    // Return the desired value
    return validFlag[segNum];
}

// ------------------------------------------------------------------------
// Returns the point of intersection in global coordinates determined
// during the last intersection traversal for the specified segment. The
// number of the first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getIsectPoint(int segNum)
{
    atVector errResult(3);

    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPoint: Segment number out of bounds\n");
        return errResult;
    }

    // Return the desired value
    return sectPoint[segNum];
}

// ------------------------------------------------------------------------
// Returns the polygon normal in global coordinates at the point of
// intersection determined during the last intersection traversal for the
// specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getIsectNorm(int segNum)
{
    atVector errResult(3);

    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectNorm: Segment number out of bounds\n");
        return errResult;
    }

    // Return the desired value
    return sectNorm[segNum];
}

// ------------------------------------------------------------------------
// Returns a matrix containing the local-to-global coordinate transform for
// the object intersected with during the last intersection traversal for
// the specified segment. Note that the point and normal values for the
// same segment already have this data multiplied in. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atMatrix vsIntersect::getIsectXform(int segNum)
{
    atMatrix errResult;

    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectXform: Segment number out of bounds\n");
        errResult.setIdentity();
        return errResult;
    }

    // Return the desired value
    return sectXform[segNum];
}

// ------------------------------------------------------------------------
// Returns the geometry object intersected with determined during the last
// intersection traversal for the specified segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsGeometry *vsIntersect::getIsectGeometry(int segNum)
{
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectGeometry: Segment number out of bounds\n");
        return NULL;
    }

    // Return the desired value
    return sectGeom[segNum];
}

// ------------------------------------------------------------------------
// Returns the index of the primitive within the geometry object
// intersected with, determined during the last intersection traversal for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
int vsIntersect::getIsectPrimNum(int segNum)
{
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPrimNum: Segment number out of bounds\n");
        return 0;
    }

    // Return the desired value
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
    // Bounds check
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPath: Segment number out of bounds\n");
        return 0;
    }

    // Return the desired value
    return sectPath[segNum];
}
