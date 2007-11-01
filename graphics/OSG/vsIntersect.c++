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
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsIntersect.h++"
#include "vsComponent.h++"
#include "vsDynamicGeometry.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsScene.h++"
#include "vsUnmanagedNode.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the segment list
// ------------------------------------------------------------------------
vsIntersect::vsIntersect() : segList(5, 10)
{
    int loop;

    // Initialize the segment list
    segListSize = 0;

    // Initialize the path array
    pathsEnabled = false;
    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        sectPath[loop] = NULL;

    // Create the auxiliary visitor for the IntersectVisitor that will
    // control traversals.  This will handle the special traversal
    // modes for sequence, switches, and LOD's
    traverser = new vsIntersectTraverser();
    traverser->ref();

    // Set the facing mode to accept intersections with both sides by default
    facingMode = VS_INTERSECT_IGNORE_NONE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsIntersect::~vsIntersect()
{
    int loop;

    // Clean up any intersect node paths that have been created
    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        if (sectPath[loop] != NULL)
            delete (sectPath[loop]);

    // Clean up any line segments that were allocated
    for (loop = 0; loop < segListSize; loop++)
        if (segList[loop] != NULL)
            ((osg::LineSegment *)(segList[loop]))->unref();

    // Delete the vsIntersectTraverser
    traverser->unref();
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

    // Make sure we don't exceed the maximum list size
    if (newSize > VS_INTERSECT_SEGS_MAX)
    {
        printf("vsIntersect::setSegListSize: Segment list is limited to a "
            "size of %d segments\n", VS_INTERSECT_SEGS_MAX);
       return;
    }

    // Make sure the list size is valid (non-negative)
    if (newSize < 0)
    {
        printf("vsIntersect::setSegListSize: Invalid segment list size\n");
        return;
    }

    // Check to see if we're shrinking the list
    if (newSize < segListSize)
    {
        // Unreference any LineSegments we've created in the list slots 
        // that are going away.
        for (loop = newSize; loop < segListSize; loop++)
        {
            if (segList[loop] != NULL)
                ((osg::LineSegment *)(segList[loop]))->unref();
        }
    }
    
    // Re-size the list
    segList.setSize(newSize);
    segListSize = newSize;
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
    atVector start, end;
    osg::Vec3 pstart, pend;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    // Copy the points and ensure the size of each atVector is 3
    start.clearCopy(startPt);
    start.setSize(3);
    end.clearCopy(endPt);
    end.setSize(3);

    // Convert to OSG vectors
    pstart.set(start[AT_X], start[AT_Y], start[AT_Z]);
    pend.set(end[AT_X], end[AT_Y], end[AT_Z]);
    
    // Create the segment structure if one is not already present
    if (segList[segNum] == NULL)
    {
        segList[segNum] = new osg::LineSegment();
        ((osg::LineSegment *)segList[segNum])->ref();
    }

    // Set the endpoints of the segment
    ((osg::LineSegment *)segList[segNum])->set(pstart, pend);
}

// ------------------------------------------------------------------------
// Sets the location of one of the intersection segments by its starting
// point, direction, and length. The segNum value determines which segment
// is to be set; the number of the first segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setSeg(int segNum, atVector startPt, atVector directionVec,
                         double length)
{
    atVector start, dir;
    osg::Vec3 pstart, pdir;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    // Copy the vectors and make sure the size of each vector is 3
    start.clearCopy(startPt);
    start.setSize(3);
    dir.clearCopy(directionVec);
    dir.setSize(3);

    // Normalize the direction vector
    dir.normalize();
    
    // Convert to OSG vectors
    pstart.set(start[AT_X], start[AT_Y], start[AT_Z]);
    pdir.set(dir[AT_X], dir[AT_Y], dir[AT_Z]);
    
    // Create the segment structure if one is not already present
    if (segList[segNum] == NULL)
    {
        segList[segNum] = new osg::LineSegment;
        ((osg::LineSegment *)segList[segNum])->ref();
    }

    // Set the endpoints of the segment.  The "end" point is computed
    // using the start point and the direction vector scaled by the length
    // of the segment.
    ((osg::LineSegment *)segList[segNum])->set(pstart, pstart + pdir*length);
}

// ------------------------------------------------------------------------
// Retrieves the starting point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegStartPt(int segNum)
{
    osg::Vec3 segStart;
    atVector result;
    
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegStartPt: Segment number out of bounds\n");
        return result;
    }
    
    // Get the segment start point
    segStart = ((osg::LineSegment *)segList[segNum])->start();

    // Convert to a atVector
    result.set(segStart.x(), segStart.y(), segStart.z());

    // Return the result
    return result;
}

// ------------------------------------------------------------------------
// Retrieves the ending point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegEndPt(int segNum)
{
    osg::Vec3 segEnd;
    atVector result;
    
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegEndPt: Segment number out of bounds\n");
        return result;
    }
    
    // Get the segment end point
    segEnd = ((osg::LineSegment *)segList[segNum])->end();

    // Convert to a atVector
    result.set(segEnd.x(), segEnd.y(), segEnd.z());

    // Return the result
    return result;
}

// ------------------------------------------------------------------------
// Returns a unit vector indicating the direction from the start point
// to the end point of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegDirection(int segNum)
{
    osg::Vec3 start, end;
    atVector startPt, endPt;
    atVector dir;
    
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegDirection: Segment number out of bounds\n");
        return atVector(0.0, 0.0, 0.0);
    }

    // Get the start and end points of the segment
    start = ((osg::LineSegment *)segList[segNum])->start();
    end = ((osg::LineSegment *)segList[segNum])->end();

    // Convert to atVectors
    startPt.set(start.x(), start.y(), start.z());
    endPt.set(end.x(), end.y(), end.z());

    // Compute the direction vector
    dir = endPt - startPt;
    dir.normalize();
    
    // Return the resulting direction
    return dir;
}

// ------------------------------------------------------------------------
// Returns the length of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
double vsIntersect::getSegLength(int segNum)
{
    osg::Vec3 start, end;
    atVector startPt, endPt;
    atVector dir;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegLength: Segment number out of bounds\n");
        return 0.0;
    }
    
    // Get the start and end points of the segment
    start = ((osg::LineSegment *)segList[segNum])->start();
    end = ((osg::LineSegment *)segList[segNum])->end();

    // Convert to atVectors
    startPt.set(start.x(), start.y(), start.z());
    endPt.set(end.x(), end.y(), end.z());

    // Compute the vector from the start point to the end point
    dir = endPt - startPt;

    // Return the magnitude of the vector
    return dir.getMagnitude();
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
    osgUtil::SceneView *osgSceneView;;
    int winWidth, winHeight;
    int winX, winY;
    osg::Vec3 nearPt, farPt;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setPickSeg: Segment number out of bounds\n");
        return;
    }
    
    // Get the OSG SceneView
    osgSceneView = pane->getBaseLibraryObject();

    // Convert -1 to 1 coordinate range to the 
    // 0 to (height or width) pixel range
    pane->getParentWindow()->getDrawableSize(&winWidth, &winHeight);
    winX = (int)(x * (double)winWidth / 2.0);
    winX += winWidth / 2;
    winY = (int)(y * (double)winHeight / 2.0);
    winY += winHeight / 2;

    // The origin of the VESS coordinate system is in the upper-left corner,
    // but OSG and OpenGL use the lower-left corner. Perform this conversion.
    winY = winHeight - winY;


    // Get the points on the near and far planes that correspond to the
    // given x,y window coordinates
    osgSceneView->projectWindowXYIntoObject(winX, winY, nearPt, farPt);

    // Create the segment structure if one is not already present
    if (segList[segNum] == NULL)
    {
        segList[segNum] = new osg::LineSegment;
        ((osg::LineSegment *)segList[segNum])->ref();
    }

    // Set the endpoints of the segment.  In this case, these are simply
    // the points on the near and far clipping planes, respectively.
    ((osg::LineSegment *)segList[segNum])->set(nearPt, farPt);
}

// ------------------------------------------------------------------------
// Sets the intersection mask
// ------------------------------------------------------------------------
void vsIntersect::setMask(unsigned int newMask)
{
    traverser->setTraversalMask(newMask);
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsIntersect::getMask()
{
    return traverser->getTraversalMask();
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
// array objects are deleted at the next intersect call.  Note that Open
// Scene Graph will always return a traversal path, but this will not
// be translated into a VESS path if paths are disabled.
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
    traverser->setSwitchTravMode(newMode);
}

// ------------------------------------------------------------------------
// Gets the switch traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getSwitchTravMode()
{
    return traverser->getSwitchTravMode();
}

// ------------------------------------------------------------------------
// Sets the sequence traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test when the component has a sequence attribute.
// ------------------------------------------------------------------------
void vsIntersect::setSequenceTravMode(int newMode)
{
    traverser->setSequenceTravMode(newMode);
}

// ------------------------------------------------------------------------
// Gets the sequence traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getSequenceTravMode()
{
    return traverser->getSequenceTravMode();
}

// ------------------------------------------------------------------------
// Sets the level-of-detail traversal mode for the intersection object.
// This mode tells the object which of components' children should be used
// in the the intersection test when the component has an LOD attribute.
// ------------------------------------------------------------------------
void vsIntersect::setLODTravMode(int newMode)
{
    traverser->setLODTravMode(newMode);
}

// ------------------------------------------------------------------------
// Gets the level-of-detail traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getLODTravMode()
{
    return traverser->getLODTravMode();
}

// ------------------------------------------------------------------------
// Initiates an intersection traversal over the indicated geometry tree.
// The results of the traversal are stored and can be retrieved with the
// getIsect* functions.
// ------------------------------------------------------------------------
void vsIntersect::intersect(vsNode *targetNode)
{
    osg::Node *osgNode; 
    osg::Geode *geoNode;
    osg::Node *pathNode;
    int loop, sloop, tloop, hloop;
    osgUtil::IntersectVisitor::HitList hitList;
    osgUtil::Hit hit;
    int validHit;
    int arraySize;
    osg::Vec3 hitPoint, polyNormal, viewRay;
    osg::Matrix xformMat;
    osg::NodePath hitNodePath;
    int pathLength;
    vsNode *vessNode;
    atVector viewVec, normalVec;
    double viewDot;

    // This is where the fun begins.  First figure out what kind of node
    // we're starting from and get the Open Scene Graph node out of it
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        osgNode = ((vsGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
        osgNode = ((vsDynamicGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY)
        osgNode = ((vsSkeletonMeshGeometry*)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        osgNode = ((vsComponent *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_SCENE)
        osgNode = ((vsScene *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_UNMANAGED)
        osgNode = ((vsUnmanagedNode *)targetNode)->getBaseLibraryObject();

    // Reset the IntersectVisitor for the new traversal
    traverser->reset();

    // Add all the segments from the segment list to the IntersectVisitor
    for (loop = 0; loop < segListSize; loop++)
    {
        if (segList[loop])
            traverser->addLineSegment((osg::LineSegment *)(segList[loop]));
    }

    // Call the Visitor's accept() method to run the intersection traversal
    osgNode->accept(*traverser);
    
    // Interpret and store the results
    for (loop = 0; loop < segListSize; loop++)
    {
        // If there was no segment defined for this segment position, then treat
        // it as if no intersection occurred
        if (segList[loop] == NULL)
        {
            validFlag[loop] = false;
            sectPoint[loop].set(0, 0, 0);
            sectNorm[loop].set(0, 0, 0);
            sectGeom[loop] = NULL;
            sectPrim[loop] = 0;
            if (sectPath[loop])
                delete (sectPath[loop]);
            sectPath[loop] = NULL;
            continue;
        }

        // Get the list of hits for this segment
        hitList = traverser->getHitList((osg::LineSegment *)(segList[loop]));

        // If the facing mode is anything other than VS_INTERSECT_IGNORE_NONE,
        // separate back and front culling checks must be made to find valid
        // intersections
        if (facingMode != VS_INTERSECT_IGNORE_NONE)
        {
            // Set up a vector indicating the view direction for this segment
            viewRay = ((osg::LineSegment *)segList[loop])->end() -
                      ((osg::LineSegment *)segList[loop])->start();
            viewVec.set(viewRay.x(), viewRay.y(), viewRay.z());
           
            // Set the hit-searching variables to their defaults
            hloop = 0;
            validHit = -1;

            // Test all possible hits from front to back until finding the
            // first that is valid under the current culling mode
            while ((hloop < hitList.size()) && (validHit == -1))
            {
                // Get the next hit for the segment
                hit = hitList.at(hloop);

                // Find the normal 
                polyNormal = hit.getWorldIntersectNormal();

                // Convert the normal into a atVector
                normalVec.set(polyNormal.x(), polyNormal.y(), polyNormal.z());

                // Get the dot product of the view vector with the normal of
                // the intersected geometry at this hit
                viewDot = viewVec.getDotProduct(normalVec);

                // If the dot product of the view vector with the normal of the
                // intersected polygon is positive, then the polygon was hit
                // from the back and should only be counted if the facing mode
                // is not ignoring the back. The same is true for a positive
                // dot product and a facing mode not ignoring the front.
                if ((facingMode == VS_INTERSECT_IGNORE_BACKFACE) &&
                    (viewDot < 0.0))
                {
                    validHit = hloop;
                }
                else if ((facingMode == VS_INTERSECT_IGNORE_FRONTFACE) &&
                    (viewDot > 0.0))
                {
                    validHit = hloop;
                }

                // Move on to the next potential hit
                hloop++;
            }
        }
        else
        {
            // The first hit is always valid in this mode
            validHit = 0;
        }

        // Check for intersections, set this segment's results to zero 
        // values and skip to the next segment if there aren't any
        if ((hitList.empty()) || (validHit == -1))
        {
            validFlag[loop] = false;
            sectPoint[loop].set(0, 0, 0);
            sectNorm[loop].set(0, 0, 0);
            sectGeom[loop] = NULL;
            sectPrim[loop] = 0;
            if (sectPath[loop])
                delete (sectPath[loop]);
            sectPath[loop] = NULL;
        }
        else
        {
            // Get the first valid hit from the list
            hit = hitList.at(validHit);

            // Set the flag to indicate this segment has a valid intersection
            validFlag[loop] = true;

            // Get the point and normal vector of intersection
            hitPoint = hit.getWorldIntersectPoint();
            polyNormal = hit.getWorldIntersectNormal();

            // See if there is a transform matrix for this intersection
            if (hit._matrix.valid())
            {
                // Get the local-to-global transformation matrix for the 
                // intersection
                xformMat = *(hit._matrix.get());
            
                // Convert to a atMatrix
                for (sloop = 0; sloop < 4; sloop++)
                    for (tloop = 0; tloop < 4; tloop++)
                        sectXform[loop][sloop][tloop] = xformMat(tloop, sloop);
            }
            else
            {
                // Set the local-to-global transform matrix to identity (no
                // transform)
                sectXform[loop].setIdentity();
            }

            // Convert the point and normal to atVectors
            sectPoint[loop].set(hitPoint[0], hitPoint[1], hitPoint[2]);
            sectNorm[loop].set(polyNormal[0], polyNormal[1], polyNormal[2]);

            // Get the vsGeometry and the primitive index intersected
            geoNode = hit._geode.get();
            sectGeom[loop] = (vsGeometry *)((vsNode::getMap())->
                mapSecondToFirst(geoNode));
            sectPrim[loop] = hit._primitiveIndex;
        
            // Create path information if so requested
            if (pathsEnabled)
            {
                // If the path array for this segment hasn't been allocated,
                // do it now.
                if (sectPath[loop] == NULL)
                    sectPath[loop] = new vsGrowableArray(10, 10);

                // Get the intersection path from OSG
                hitNodePath = hit._nodePath;

                // Get the length of the path
                pathLength = hitNodePath.size();

                // Initialize the index for the array of vsNodes
                arraySize = 0;

                // Traverse the path and translate it into an array of VESS nodes
                for (sloop = 0; sloop < pathLength; sloop++)
                {
                    // Get the next path node from the Performer path array
                    pathNode = (osg::Node *)(hitNodePath[sloop]);

                    // If the path node is valid, try to map it to a VESS node
                    if (pathNode != NULL)
                    {
                        vessNode = (vsNode *)
                            ((vsNode::getMap())->mapSecondToFirst(pathNode));

                        // Add this node to the array if there is a valid mapping
                        // (not all OSG nodes will have a corresponding VESS node)
                        if (vessNode)
                        {
                            (sectPath[loop])->setData(arraySize++, vessNode);
                        }
                    }
                }
                
                // Terminate the path with a NULL
                (sectPath[loop])->setData(arraySize, NULL);
            }
            else if (sectPath[loop])
            {
                // Paths have been turned off, so delete the existing path
                // array
                delete (sectPath[loop]);
                sectPath[loop] = NULL;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Returns if the last intersection traversal found an intersection for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
bool vsIntersect::getIsectValid(int segNum)
{
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectValid: Segment number out of bounds\n");
        return false;
    }

    // Return the valid flag value of the corresponding segment.  This will
    // be true if there was a valid intersection with this segment.
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

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPoint: Segment number out of bounds\n");
        return errResult;
    }

    // Return the point of intersection for this segment
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

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectNorm: Segment number out of bounds\n");
        return errResult;
    }

    // Return the normal vector at this segment's intersection point
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

    // Make sure this segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectXform: Segment number out of bounds\n");
        errResult.setIdentity();
        return errResult;
    }

    // Return the global transform of this intersection
    return sectXform[segNum];
}

// ------------------------------------------------------------------------
// Returns the geometry object intersected with determined during the last
// intersection traversal for the specified segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsGeometry *vsIntersect::getIsectGeometry(int segNum)
{
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectGeometry: Segment number out of bounds\n");
        return NULL;
    }

    // Return the vsGeometry intersected by this segment
    return sectGeom[segNum];
}

// ------------------------------------------------------------------------
// Returns the index of the primitive within the geometry object
// intersected with, determined during the last intersection traversal for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
int vsIntersect::getIsectPrimNum(int segNum)
{
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPrimNum: Segment number out of bounds\n");
        return 0;
    }

    // Return the primitive index within the intersected vsGeometry
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
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectPath: Segment number out of bounds\n");
        return 0;
    }

    // Return the intersection node path for this segment.
    return sectPath[segNum];
}
