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
//    VESS Module:  vsBillboardCallback.h++
//
//    Description:  OSG-specific class that implements a callback which
//                  is called when an OSG cull traversal reaches a
//                  vsComponent with a vsBillboardAttribute attached
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <osgUtil/CullVisitor>
#include "vsBillboardCallback.h++"
#include "vsBillboardAttribute.h++"

//------------------------------------------------------------------------
// Constructor
// Stores the pointer to the parent billboard attribute
//------------------------------------------------------------------------
vsBillboardCallback::vsBillboardCallback(vsBillboardAttribute *billAttr)
{
    billboardAttr = billAttr;
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsBillboardCallback::~vsBillboardCallback()
{
}

//------------------------------------------------------------------------
// OSG callback function
// Called when a cull traversal reaches a vsComponent with a billboard
// attribute attached. Retrieves the current view and transform matrices
// from the visitor object, and calls the billboard attribute to adjust
// its transform matrix.
//------------------------------------------------------------------------
void vsBillboardCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    osg::Matrix osgMatrix;
    vsMatrix viewMatrix, xformMatrix;
    int loop, sloop;
    osgUtil::CullVisitor *osgCullVisitor;
    vsMatrix coordXform, coordXformInv;
    
    // Verify that the NodeVisitor parameter is actually an osg
    // CullVisitor object; bail out if it isn't.
    osgCullVisitor = dynamic_cast<osgUtil::CullVisitor *>(nv);
    if (!osgCullVisitor)
        return;

    // Obtain the view matrix
    osgMatrix = osgCullVisitor->getModelViewMatrix();
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            viewMatrix[loop][sloop] = osgMatrix(sloop, loop);

    // Invert the matrix to get the transform from the object to the
    // viewpoint (instead of vice-versa)
    viewMatrix.invert();
    
    // Obtain the accumulated global transform matrix
    osgMatrix.makeIdentity();
    osgMatrix = osgCullVisitor->getModelViewMatrix();
    for (loop = 0; loop < 4; loop++)
        for (sloop = 0; sloop < 4; sloop++)
            xformMatrix[loop][sloop] = osgMatrix(sloop, loop);

    // Transform the current view matrix by the local to world matrix of
    // the object to get the viewpoint and orientation in world coordinates
    viewMatrix = xformMatrix * viewMatrix;

    // Pass both matrices to the vsBillboardAttribute so that it
    // can do its job
    billboardAttr->adjustTransform(viewMatrix, xformMatrix);
    
    // Continue the cull traversal
    traverse(node,nv);
}
