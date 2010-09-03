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
//    Author(s):    Bryan Kline, Jason Daly, Casey Thurston
//
//------------------------------------------------------------------------

#include "vsIntersect.h++"
#include "vsComponent.h++"
#include "vsDynamicGeometry.h++"
#include "vsLineSegment.h++"
#include "vsSkeletonMeshGeometry.h++"
#include "vsScene.h++"
#include "vsUnmanagedNode.h++"
#include "vsOSGNode.h++"

#include <osg/StateAttribute>
#include <osg/StateSet>
#include <osg/ClipPlane>
#include <osg/Transform>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>

#include <stdio.h>


// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsIntersect::vsIntersect()
{
    // Set the facing mode to accept intersections with both sides by default
    facingMode = VS_INTERSECT_IGNORE_NONE;

    // Begin with no sensitivity to clipping and paths disabled.
    clipSensitivity = false;
    pathsEnabled = false;

    // Initialize the segment list
    segListSize = 0;
    segList = new atArray();
    resultList = new atArray();

    // Create the auxiliary visitor for the IntersectVisitor that will
    // control traversals.  This will handle the special traversal
    // modes for sequence, switches, and LOD's
    intersectTraverser = new vsIntersectTraverser();
    intersectTraverser->ref();
    
    // TODO: Database read callback?
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsIntersect::~vsIntersect()
{
    // Clean up any intersect node paths that have been created.
    delete resultList;
    delete segList;

    // Delete the vsIntersectTraverser. Note that this is an OSG object (not a
    // vsObject), so unref is the proper method to call.
    intersectTraverser->unref();
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
    vsLineSegment *segment;
    atList *results;

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
        for (loop = segListSize - 1; loop >= newSize; loop--)
        {
            // Set the object at the location to NULL in case the number of
            // segments increases at a later time. The pointer previously at
            // that location is stored so its memory may be freed if necessary.
            segment = (vsLineSegment *)segList->setEntry(loop, NULL);
            results = (atList *)resultList->setEntry(loop, NULL);

            // Delete the input segment if it existed.
            if (segment)
                delete segment;

            // Delete the result list if it existed.
            if (results)
                delete results;
        }
    }

    // Store the new capacity.
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
    vsLineSegment *segment;

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

    // Create the segment structure if one is not already present.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment == NULL)
    {
        // Create a new line segment and place it at the appropriate location
        // in the array.
        segment = new vsLineSegment(start, end);
        segList->setEntry(segNum, segment);
    }
    else
    {
        // The pointer is already set. Give it the correct vectors.
        segment->setStartPoint(start);
        segment->setEndPoint(end);
    }
}

// ------------------------------------------------------------------------
// Sets the location of one of the intersection segments by its starting
// point, direction, and length. The segNum value determines which segment
// is to be set; the number of the first segment is 0.
// ------------------------------------------------------------------------
void vsIntersect::setSeg(int segNum, atVector startPt, atVector directionVec,
                         double length)
{
    vsLineSegment *segment;
    atVector start, dir, end;

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

    // Calculate the end point.
    end = start + (dir * length);

    // Create the segment structure if one is not already present.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment == NULL)
    {
        // Create a new line segment and place it at the appropriate location
        // in the array.
        segment = new vsLineSegment(start, end);
        segList->setEntry(segNum, segment);
    }
    else
    {
        // The pointer is already set. Give it the correct vectors.
        segment->setStartPoint(start);
        segment->setEndPoint(end);
    }
}

// ------------------------------------------------------------------------
// Retrieves the starting point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegStartPt(int segNum)
{
    vsLineSegment *segment;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegStartPt: Segment number out of bounds\n");
        return atVector(0.0, 0.0, 0.0);
    }

    // Attempt to fetch the segment.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment)
    {
        // Return the start point.
        return segment->getStartPoint();
    }

    // If no segment yet exists, return a default.
    return atVector(0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// Retrieves the ending point of the indicated segment. The number of the
// first segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegEndPt(int segNum)
{
    vsLineSegment *segment;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegEndPt: Segment number out of bounds\n");
        return atVector(0.0, 0.0, 0.0);
    }

    // Attempt to fetch the segment.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment)
    {
        // Return the end point.
        return segment->getEndPoint();
    }

    // If no segment yet exists, return a default.
    return atVector(0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// Returns a unit vector indicating the direction from the start point
// to the end point of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
atVector vsIntersect::getSegDirection(int segNum)
{
    vsLineSegment *segment;
    atVector start, end, dir;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegDirection: Segment number out of bounds\n");
        return atVector(0.0, 0.0, 0.0);
    }

    // Attempt to fetch the segment.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment)
    {
        // Fetch the start and end points of the segment.
        start = segment->getStartPoint();
        end = segment->getEndPoint();

        // Calculate the vector they form, normalizing it to yield direction.
        dir = end - start;
        dir.normalize();

        // Return the direction vector.
        return dir;
    }

    // If no segment yet exists, return a default.
    return atVector(0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// Returns the length of the indicated segment. The number of the first
// segment is 0.
// ------------------------------------------------------------------------
double vsIntersect::getSegLength(int segNum)
{
    vsLineSegment *segment;
    atVector start, end, dir;

    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getSegLength: Segment number out of bounds\n");
        return 0.0;
    }

    // Attempt to fetch the segment.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment)
    {
        // Fetch the start and end points of the segment.
        start = segment->getStartPoint();
        end = segment->getEndPoint();

        // Calculate the vector they form and return its magnitude.
        dir = end - start;
        return dir.getMagnitude();
    }

    // If no segment yet exists, return a default.
    return 0.0;
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
    vsLineSegment *segment;
    osg::Vec3 nearPt, farPt;
    atVector start, end;

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

    // Convert the OSG vectors into atVectors.
    start.set(nearPt[0], nearPt[1], nearPt[2]);
    end.set(farPt[0], farPt[1], farPt[2]);

    // Create the segment structure if one is not already present.
    segment = (vsLineSegment *)segList->getEntry(segNum);
    if (segment == NULL)
    {
        // Create a new line segment and place it at the appropriate location
        // in the array.
        segment = new vsLineSegment(start, end);
        segList->setEntry(segNum, segment);
    }
    else
    {
        // The pointer is already set. Give it the correct vectors.
        segment->setStartPoint(start);
        segment->setEndPoint(end);
    }
}

// ------------------------------------------------------------------------
// Sets the intersection mask
// ------------------------------------------------------------------------
void vsIntersect::setMask(unsigned int newMask)
{
    intersectTraverser->setTraversalMask(newMask);
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsIntersect::getMask()
{
    return intersectTraverser->getTraversalMask();
}

// ------------------------------------------------------------------------
// Enables clip sensitivity for the intersection traversal. Starting with
// the next call to the intersect method, if an intersection occurs in a
// subgraph that has been clipped out, it will be ignored. 
// ------------------------------------------------------------------------
void vsIntersect::enableClipSensitivity()
{
    clipSensitivity = true;
}

// ------------------------------------------------------------------------
// Disables clip sensitivity for the intersection traversal. Starting with
// the next call to the intersect method, even intersections occuring in
// clipped subgraphs will be returned.
// ------------------------------------------------------------------------
void vsIntersect::disableClipSensitivity()
{
    clipSensitivity = false;
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
    intersectTraverser->setSwitchTravMode(newMode);
}

// ------------------------------------------------------------------------
// Gets the switch traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getSwitchTravMode()
{
    return intersectTraverser->getSwitchTravMode();
}

// ------------------------------------------------------------------------
// Sets the sequence traversal mode for the intersection object. This mode
// tells the object which of components' children should be used in the
// the intersection test when the component has a sequence attribute.
// ------------------------------------------------------------------------
void vsIntersect::setSequenceTravMode(int newMode)
{
    intersectTraverser->setSequenceTravMode(newMode);
}

// ------------------------------------------------------------------------
// Gets the sequence traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getSequenceTravMode()
{
    return intersectTraverser->getSequenceTravMode();
}

// ------------------------------------------------------------------------
// Sets the level-of-detail traversal mode for the intersection object.
// This mode tells the object which of components' children should be used
// in the the intersection test when the component has an LOD attribute.
// ------------------------------------------------------------------------
void vsIntersect::setLODTravMode(int newMode)
{
    intersectTraverser->setLODTravMode(newMode);
}

// ------------------------------------------------------------------------
// Gets the level-of-detail traversal mode for the intersection object
// ------------------------------------------------------------------------
int vsIntersect::getLODTravMode()
{
    return intersectTraverser->getLODTravMode();
}

// ------------------------------------------------------------------------
// Initiates an intersection traversal over the indicated geometry tree.
// The results of the traversal are stored and can be retrieved with the
// getIsect* functions.
// ------------------------------------------------------------------------
void vsIntersect::intersect(vsNode *targetNode)
{
    osg::Node *osgNode;
    osgUtil::IntersectorGroup *intersectorGroup;
    osgUtil::LineSegmentIntersector **segmentIntersectors;
    osgUtil::LineSegmentIntersector *segmentIntersector;
    osgUtil::LineSegmentIntersector::Intersections *intersections;
    osgUtil::LineSegmentIntersector::Intersections::const_iterator iterator;
    const SegIntersection * intersection;
    osg::Vec3d osgStart, osgEnd;
    osg::Vec3 polyNormal;
    vsLineSegment *segment;
    atVector atStart, atEnd;
    atVector viewVec, normalVec;
    double viewDot;
    int loop;

    // Before doing anything else, clear out the existing intersection results.
    clearIntersectionResults();

    // Fetch the OSG node from the VESS node.
    osgNode = getBaseLibraryObject(targetNode);

    // Create a new intersector group to handle this request.
    intersectorGroup = new osgUtil::IntersectorGroup();
    intersectorGroup->ref();

    // Create a temporary array to store the intersector for each line segment.
    // This is necessary because the IntersectorGroup doesn't guarantee order.
    segmentIntersectors = new osgUtil::LineSegmentIntersector*[segListSize];

    // Add all the segments from the segment list to the IntersectVisitor
    for (loop = 0; loop < segListSize; loop++)
    {
        // Fetch the segment from the array. It may be NULL.
        segment = (vsLineSegment *)segList->getEntry(loop);
        if (segment)
        {
            // Convert the line segment atVectors into OSG vectors.
            atStart = segment->getStartPoint();
            atEnd = segment->getEndPoint();
            osgStart.set(atStart[AT_X], atStart[AT_Y], atStart[AT_Z]);
            osgEnd.set(atEnd[AT_X], atEnd[AT_Y], atEnd[AT_Z]);

            // Create a new LineSegmentIntersector to handle this segment and
            // add it to the IntersectorGroup.
            segmentIntersector =
                new osgUtil::LineSegmentIntersector(osgStart, osgEnd);
            segmentIntersector->ref();
            intersectorGroup->addIntersector(segmentIntersector);
            segmentIntersectors[loop] = segmentIntersector;
        }
        else
        {
            // Set the segmentIntersector to NULL just to be safe.
            segmentIntersectors[loop] = NULL;
        }
    }

    // The IntersectorGroup is now configured. Associate it with the traverser.
    intersectTraverser->setIntersector(intersectorGroup);

    // Call the target accept method to perform the intersection traversal.
    osgNode->accept(*intersectTraverser);

    // Interpret and store the results.
    for (loop = 0; loop < segListSize; loop++)
    {
        // Attempt to retrieve the segment intersector at this index. If there
        // was no intersector defined then no intersection could have occurred.
        segmentIntersector = segmentIntersectors[loop];
        if ((segmentIntersector != NULL) &&
            (segmentIntersector->containsIntersections()))
        {
            // Fetch the multimap of intersections.
            intersections = &(segmentIntersector->getIntersections());

            // If the facing mode isn't VS_INTERSECT_IGNORE_NONE, back and
            // front culling checks must be made to find valid
            // intersections.
            if (facingMode != VS_INTERSECT_IGNORE_NONE)
            {
                // Fetch the segment at this location and use it to calculate
                // the view direction. This will be necessary to perform
                // culling based on facing.
                segment = (vsLineSegment *)segList->getEntry(loop);
                viewVec = segment->getEndPoint() - segment->getStartPoint();
            }

            // Create an iterator over the intersection results.
            iterator = intersections->begin();
            while (resultList->getEntry(loop) == NULL)
            {
                // Get a pointer to the intersection structure from the
                // iterator (the *iterator statement returns a reference to
                // the data in question, and we take the address of this
                // reference to get the pointer).
                intersection = &(*iterator);

                // If the facing mode isn't VS_INTERSECT_IGNORE_NONE, back and
                // front culling checks must be made to find valid
                // intersections.
                if (facingMode != VS_INTERSECT_IGNORE_NONE)
                {
                    // Find the normal and convert it to an atVector.
                    polyNormal = intersection->getWorldIntersectNormal();
                    normalVec.set(polyNormal.x(), polyNormal.y(),
                        polyNormal.z());

                    // Get the dot product of the view vector with the normal
                    // of the intersected geometry at this hit
                    viewDot = viewVec.getDotProduct(normalVec);

                    // If the dot product of the view vector with the normal of
                    // the intersected polygon is positive, then the polygon
                    // was hit from the back and should only be counted if the
                    // facing mode is not ignoring the back. The same is true
                    // for a positive dot product and a facing mode not
                    // ignoring the front.
                    if ((facingMode == VS_INTERSECT_IGNORE_BACKFACE) &&
                        (viewDot < 0.0))
                    {
                        populateIntersection(loop, intersection);
                    }
                    else if ((facingMode == VS_INTERSECT_IGNORE_FRONTFACE) &&
                        (viewDot > 0.0))
                    {
                        populateIntersection(loop, intersection);
                    }

                }
                else
                {
                    // Populate with the data from this intersection.
                    populateIntersection(loop, intersection);
                }

                // Advance the iterator and check whether this was the last
                // node.
                iterator++;
                if (iterator == intersections->end())
                {
                    // We're out of potential results. Fill in the field
                    // with default data to halt the traversal.
                    if (resultList->getEntry(loop) == NULL)
                        populateIntersection(loop, NULL);
                }
            }
        }
        else
        {
            // Fill in the index with an empty result.
            populateIntersection(loop, NULL);
        }
    }

    // Remove all the segments from the segment list
    for (loop = 0; loop < segListSize; loop++)
    {
        // Unref the segment
        if (segmentIntersectors[loop] != NULL)
            segmentIntersectors[loop]->unref();
    }

    // Delete the array of segment intersectors
    delete [] segmentIntersectors;

    // Unref the intersectorGroup
    intersectorGroup->unref();
}

// ------------------------------------------------------------------------
// Returns the result of the intersection for the specified segment. The
// number of the first segment is 0.
// ------------------------------------------------------------------------
vsIntersectResult *vsIntersect::getIntersection(int segNum)
{
    // Make sure the segment number is valid
    if ((segNum < 0) || (segNum >= segListSize))
    {
        printf("vsIntersect::getIntersection: Segment number out of bounds\n");
        return 0;
    }

    // Fetch the result for this segment and return it.
    return (vsIntersectResult *)resultList->getEntry(segNum);
}

// ------------------------------------------------------------------------
// Private method
// Retrieves the underlying library object based on the type of the vsNode.
// ------------------------------------------------------------------------
osg::Node *vsIntersect::getBaseLibraryObject(vsNode *node)
{
    switch (node->getNodeType())
    {
        case VS_NODE_TYPE_GEOMETRY:
            return ((vsGeometry *)node)->getBaseLibraryObject();

        case VS_NODE_TYPE_DYNAMIC_GEOMETRY:
            return ((vsDynamicGeometry *)node)->getBaseLibraryObject();

        case VS_NODE_TYPE_SKELETON_MESH_GEOMETRY:
            return ((vsSkeletonMeshGeometry*)node)->getBaseLibraryObject();

        case VS_NODE_TYPE_COMPONENT:
            return ((vsComponent *)node)->getBaseLibraryObject();

        case VS_NODE_TYPE_SCENE:
            return ((vsScene *)node)->getBaseLibraryObject();

        case VS_NODE_TYPE_UNMANAGED:
            return ((vsUnmanagedNode *)node)->getBaseLibraryObject();

        default:
            return NULL;
    }
}

// ------------------------------------------------------------------------
// Private function
// Clears the array of intersection results
// ------------------------------------------------------------------------
void vsIntersect::clearIntersectionResults()
{
    vsIntersectResult *result;
    int i;

    // See if there is already a result.
    for (i = 0; i < segListSize; i++)
    {
        // Set the result list for this segment to NULL, keeping a pointer so
        // any memory may be freed.
        result = (vsIntersectResult *)resultList->setEntry(i, NULL);
        if (result)
        {
            delete result;
        }
    }
}

// ------------------------------------------------------------------------
// Private function
// Populates the intersection result fields at the specified index with
// the information from the provided intersection.
// ------------------------------------------------------------------------
void vsIntersect::populateIntersection(int index,
    const osgUtil::LineSegmentIntersector::Intersection *intersection)
{
    vsIntersectResult *intersectResult;
    osg::NodePath nodePath;
    osg::Drawable *osgDrawable;
    osg::Node *pathNode;
    int sloop, tloop;
    osg::Vec3 hitPoint, polyNormal;
    osg::Matrix xformMat;
    int pathLength;
    vsNode *vessNode;
    atVector sectPoint, sectNorm;
    atMatrix sectXform;
    osg::Geode *osgGeode;
    vsOSGNode *osgNode;
    vsGeometryBase *sectGeom;
    int sectPrim;
    vsList *sectPath;
    osg::Vec3 sectPointLocal;
    osg::Transform *xform;
    osg::MatrixTransform *matXform; 

    // Check whether the intersection is considered valid or not.
    if (intersection == NULL)
    {
        // This is not a valid intersection. Create a default result and add it
        // to the list.
        intersectResult = new vsIntersectResult();
        resultList->setEntry(index, intersectResult);

        // This method is done.
        return;
    }

    // See if there is a transform matrix for this intersection.
    if (intersection->matrix.valid())
    {
        // Get the local-to-global transformation matrix for the intersection.
        xformMat = *(intersection->matrix.get());
            
        // Convert to an atMatrix.
        for (sloop = 0; sloop < 4; sloop++)
            for (tloop = 0; tloop < 4; tloop++)
                sectXform[sloop][tloop] = xformMat(tloop, sloop);
    }
    else
    {
        // Set the local-to-global transform matrix to identity (no transform).
        sectXform.setIdentity();
    }

    // Get the point and normal vector of intersection
    hitPoint = intersection->getWorldIntersectPoint();
    polyNormal = intersection->getWorldIntersectNormal();

    // Convert the point and normal to atVectors and store them.
    sectPoint.set(hitPoint[0], hitPoint[1], hitPoint[2]);
    sectNorm.set(polyNormal[0], polyNormal[1], polyNormal[2]);

    // Get the OSG Drawable (probably Geometry) that was intersected
    osgDrawable = intersection->drawable.get();

    // Get its parent Geode (VESS enforces only 1 Geometry per Geode on
    // all managed geometry)
    osgGeode = (osg::Geode *)osgDrawable->getParent(0);

    // Look up the corresponding vsGeometryBase node in the map
    osgNode = new vsOSGNode(osgGeode);
    sectGeom = (vsGeometryBase *)((vsNode::getMap())->
        mapSecondToFirst(osgNode));

    // Done with the osgNode wrapper
    delete osgNode;

    // Get the index of the primitive within the geometry that was intersected
    sectPrim = intersection->primitiveIndex;

    // Create the intersection result now and place it in the array.
    intersectResult = new vsIntersectResult(sectPoint, sectNorm, sectXform,
        sectGeom, sectPrim);

    // Create path information if so requested
    if (pathsEnabled || clipSensitivity)
    {
        // Fetch the intersection result path list so the data may be added
        sectPath = intersectResult->getPath();

        // Get the intersection path from OSG.
        nodePath = intersection->nodePath;

        // Get the length of the path
        pathLength = nodePath.size();

        // Initialize the clip point to its world coordinates.
        sectPointLocal.set(sectPoint[AT_X], sectPoint[AT_Y], sectPoint[AT_Z]);

        // Traverse the path and translate it into an array of VESS nodes
        for (sloop = 0; sloop < pathLength; sloop++)
        {
            // Attempt to get the next path node from the OSG path array.
            pathNode = (osg::Node *)(nodePath[sloop]);
            if (pathNode)
            {
                // If clip sensitivity is enabled, then clip attributes on this
                // node will have to be checked.
                if (clipSensitivity)
                {
                    // If the current node is a transform, modify the
                    // intersection point according to its matrix.
                    xform = pathNode->asTransform();
                    if (xform)
                    {
                        // Confirm that it is specifically a matrix transform.
                        matXform = xform->asMatrixTransform();
                        if (matXform)
                        {
                            // Modify the clip point by the transform matrix.
                            sectPointLocal = osg::Matrix::transform3x3(
                                matXform->getMatrix(), sectPointLocal);
                        }
                    }

                    // Call the internal method to determine whether the OSG
                    // node clips the point.
                    if (isClipped(pathNode, sectPointLocal))
                    {
                        // The intersection point is clipped out. Delete the
                        // object and return.
                        delete intersectResult;
                        return;
                    }
                }

                // If paths are to be stored, then this information must be
                // processed further.
                if (pathsEnabled)
                {
                    // Determine if there is a valid mapping (not all OSG nodes
                    // will have a corresponding VESS node).
                    osgNode = new vsOSGNode(pathNode);
                    vessNode = (vsNode *)
                        ((vsNode::getMap())->mapSecondToFirst(osgNode));
                    if (vessNode)
                    {
                        // Add this node to the array.
                        sectPath->addEntry(vessNode);
                    }
                  
                    // Done with the wrapped node
                    delete osgNode;
                }
            }
        }
    }

    // The object hasn't been clipped, and its path information has been set as
    // necessary. Place it in the result list.
    resultList->setEntry(index, intersectResult);
}

// ------------------------------------------------------------------------
// Private function
// Determines whether the provided OSG Node contains a clip plane that covers
// the provided point.
// ------------------------------------------------------------------------
bool vsIntersect::isClipped(osg::Node *node, osg::Vec3 point)
{
    const osg::StateSet * stateSet;
    const osg::ClipPlane * clipPlaneAttr;
    int attributeIndex;
    osg::Vec4d plane;

    // Confirm that a state set exists.
    stateSet = node->getStateSet();
    if (stateSet)
    {
        // Check the OSG state set to determine if any clip planes are attached
        // to the node.
        attributeIndex = 0;
        clipPlaneAttr = (osg::ClipPlane *)stateSet->getAttribute(
            osg::StateAttribute::CLIPPLANE, attributeIndex);
        while (clipPlaneAttr != NULL)
        {
            // Retrieve the four values describing the equation for this plane.
            plane = clipPlaneAttr->getClipPlane();

            // Test whether the point satisfies the equation.
            if ((plane[0] * point[0]) + (plane[1] * point[1]) +
                (plane[2] * point[2]) + plane[3] < 0.0)
            {
                // The point is clipped according to the plane equation.
                return true;
            }

            // Advance to the next clip plane associated with this node.
            attributeIndex++;
            clipPlaneAttr = (osg::ClipPlane *)stateSet->getAttribute(
                osg::StateAttribute::CLIPPLANE, attributeIndex);
        }
    }

    // The point is not clipped by anything associated with this node.
    return false;
}

