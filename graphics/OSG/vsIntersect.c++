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

#include <stdio.h>
#include "vsIntersect.h++"
#include "vsComponent.h++"
#include "vsDynamicGeometry.h++"
#include "vsScene.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the segment list
// ------------------------------------------------------------------------
vsIntersect::vsIntersect() : segList(5, 10)
{
    int loop;

    // Create the OSG IntersectVisitor
    osgIntersect = new osgUtil::IntersectVisitor();

    // Initialize the segment list
    segListSize = 0;

    // Initialize the path array
    pathsEnabled = 0;
    for (loop = 0; loop < VS_INTERSECT_SEGS_MAX; loop++)
        sectPath[loop] = NULL;

    // Initialize traversal mode
    travMode = VS_INTERSECT_TRAVERSE_CURRENT;
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

    // Unreference the OSG IntersectVisitor
    osgIntersect->unref();
}

// ------------------------------------------------------------------------
// Sets the number of segments to be intersected with
// ------------------------------------------------------------------------
void vsIntersect::setSegListSize(int newSize)
{
    int loop;
    unsigned int result;

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
void vsIntersect::setSeg(int segNum, vsVector startPt, vsVector endPt)
{
    int loop;
    vsVector start, end;
    osg::Vec3 pstart, pend;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setSeg: Segment number out of bounds\n");
        return;
    }
    
    // Copy the points and ensure the size of each vsVector is 3
    start.clearCopy(startPt);
    start.setSize(3);
    end.clearCopy(endPt);
    end.setSize(3);

    // Convert to OSG vectors
    pstart.set(start[VS_X], start[VS_Y], start[VS_Z]);
    pend.set(end[VS_X], end[VS_Y], end[VS_Z]);
    
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
void vsIntersect::setSeg(int segNum, vsVector startPt, vsVector directionVec,
                         double length)
{
    int loop;
    vsVector start, dir;
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
    pstart.set(start[VS_X], start[VS_Y], start[VS_Z]);
    pdir.set(dir[VS_X], dir[VS_Y], dir[VS_Z]);
    
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
vsVector vsIntersect::getSegStartPt(int segNum)
{
    int loop;
    osg::Vec3 segStart;
    vsVector result;
    
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegStartPt: Segment number out of bounds\n");
        return result;
    }
    
    // Get the segment start point
    segStart = ((osg::LineSegment *)segList[segNum])->start();

    // Convert to a vsVector
    result.set(segStart.x(), segStart.y(), segStart.z());

    // Return the result
    return result;
}

// ------------------------------------------------------------------------
// Retrieves the ending point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
vsVector vsIntersect::getSegEndPt(int segNum)
{
    int loop;
    osg::Vec3 segEnd;
    vsVector result;
    
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegEndPt: Segment number out of bounds\n");
        return result;
    }
    
    // Get the segment end point
    segEnd = ((osg::LineSegment *)segList[segNum])->end();

    // Convert to a vsVector
    result.set(segEnd.x(), segEnd.y(), segEnd.z());

    // Return the result
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
    osg::Vec3 start, end;
    vsVector startPt, endPt;
    vsVector dir;
    
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegDirection: Segment number out of bounds\n");
        return vsVector(0.0, 0.0, 0.0);
    }

    // Get the start and end points of the segment
    start = ((osg::LineSegment *)segList[segNum])->start();
    end = ((osg::LineSegment *)segList[segNum])->end();

    // Convert to vsVectors
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
    vsVector startPt, endPt;
    vsVector dir;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegLength: Segment number out of bounds\n");
        return 0.0;
    }
    
    // Get the start and end points of the segment
    start = ((osg::LineSegment *)segList[segNum])->start();
    end = ((osg::LineSegment *)segList[segNum])->end();

    // Convert to vsVectors
    startPt.set(start.x(), start.y(), start.z());
    endPt.set(end.x(), end.y(), end.z());

    // Compute the vector from the start point to the end point
    dir = endPt - startPt;
    dir.normalize();

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
    int paneWidth, paneHeight;
    int winX, winY;
    osg::Vec3 near, far;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::setPickSeg: Segment number out of bounds\n");
        return;
    }
    
    // Get the OSG SceneView
    osgSceneView = pane->getBaseLibraryObject();

    // Convert the x, y coordinates to use integer pixel addresses with
    // the origin at the lower left (OpenGL-style window coordinates)
    pane->getSize(&paneWidth, &paneHeight);
    winX = (int)(x * (double)paneWidth / 2.0);
    winX += paneWidth / 2;
    winY = (int)(-y * (double)paneHeight / 2.0);
    winY += paneHeight / 2;
    
    // Get the points on the near and far planes that correspond to the
    // given x,y window coordinates
    osgSceneView->projectWindowXYIntoObject(winX, winY, near, far);

    // Create the segment structure if one is not already present
    if (segList[segNum] == NULL)
    {
        segList[segNum] = new osg::LineSegment;
        ((osg::LineSegment *)segList[segNum])->ref();
    }

    // Set the endpoints of the segment.  In this case, these are simply
    // the points on the near and far clipping planes, respectively.
    ((osg::LineSegment *)segList[segNum])->set(near, far);
}

// ------------------------------------------------------------------------
// Sets the intersection mask
// ------------------------------------------------------------------------
void vsIntersect::setMask(unsigned int newMask)
{
    osgIntersect->setTraversalMask(newMask);
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsIntersect::getMask()
{
    return osgIntersect->getTraversalMask();
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
// array objects are deleted at the next intersect call.  Note that Open
// Scene Graph will always return a traversal path, but this will not
// be translated into a VESS path if paths are disabled.
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
// Sets the traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test.  NONE means no children, ALL means all children,
// CURRENT means the currently active child of a component with a switch, 
// sequence, or LOD attribute.  For simple components (i.e.: those without
// grouping attributes), all children are considered current.
// ------------------------------------------------------------------------
void vsIntersect::setTraversalMode(int newMode)
{
    travMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the switch traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getTraversalMode()
{
    osg::NodeVisitor::TraversalMode mode;

    // Get the mode from the OSG IntersectVisitor
    mode = osgIntersect->getTraversalMode();

    // Return the corresponding vsIntersect mode
    switch (mode)
    {
        case osg::NodeVisitor::TRAVERSE_NONE:
            return VS_INTERSECT_TRAVERSE_NONE;
        case osg::NodeVisitor::TRAVERSE_ALL_CHILDREN:
            return VS_INTERSECT_TRAVERSE_ALL;
        case osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN:
            return VS_INTERSECT_TRAVERSE_CURRENT;
    }

    // An unrecognized mode is set
    return -1;
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
    int loop, sloop, tloop;
    osgUtil::IntersectVisitor::HitList hitList;
    osgUtil::Hit hit;
    int arraySize;
    osg::Vec3 hitPoint, polyNormal;
    osg::Matrix xformMat;
    osg::NodePath hitNodePath;
    int pathLength;
    vsNode *vessNode;

    // This is where the fun begins.  First figure out what kind of node
    // we're starting from and get the Open Scene Graph node out of it
    if (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        osgNode = ((vsGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY)
        osgNode = ((vsDynamicGeometry *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        osgNode = ((vsComponent *)targetNode)->getBaseLibraryObject();
    else if (targetNode->getNodeType() == VS_NODE_TYPE_SCENE)
        osgNode = ((vsScene *)targetNode)->getBaseLibraryObject();

    // Reset the IntersectVisitor for the new traversal
    osgIntersect->reset();

    // Add all the segments from the segment list to the IntersectVisitor
    for (loop = 0; loop < segListSize; loop++)
    {
        osgIntersect->addLineSegment((osg::LineSegment *)(segList[loop]));
    }

    // Call the Visitor's accept() method to run the intersection traversal
    osgNode->accept(*osgIntersect);
    
    // Interpret and store the results
    for (loop = 0; loop < segListSize; loop++)
    {
        // Get the list of hits for this segment
        hitList = osgIntersect->getHitList((osg::LineSegment *)(segList[loop]));
        
        // Check for intersections, set this segment's results to zero 
        // values and skip to the next segment if there aren't any
        if (hitList.empty())
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
        
        // Set the flag to indicate this segment has a valid intersection
        validFlag[loop] = 1;

        // Get the segment's first hit
        hit = hitList.front();

        // Get the point and normal vector of intersection
        hitPoint = hit.getWorldIntersectPoint();
        polyNormal = hit.getWorldIntersectNormal();

        // See if there is a transform matrix for this intersection
        if (hit._matrix.valid())
        {
            // Get the local-to-global transformation matrix for the 
            // intersection
            xformMat = *(hit._matrix.get());
            
            // Convert to a vsMatrix
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

        // Convert the point and normal to vsVectors
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

// ------------------------------------------------------------------------
// Returns if the last intersection traversal found an intersection for
// the specified segment. The number of the first segment is 0.
// ------------------------------------------------------------------------
int vsIntersect::getIsectValid(int segNum)
{
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIsectValid: Segment number out of bounds\n");
        return 0;
    }

    // Return the valid flag value of the corresponding segment.  This will
    // be VS_TRUE if there was a valid intersection with this segment.
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
vsVector vsIntersect::getIsectNorm(int segNum)
{
    vsVector errResult(3);

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
vsMatrix vsIntersect::getIsectXform(int segNum)
{
    vsMatrix errResult;

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
