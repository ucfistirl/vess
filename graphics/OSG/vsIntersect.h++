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
//    VESS Module:  vsIntersect.h++
//
//    Description:  Class for performing intersection tests between line
//                  segments and a whole or part of a VESS scene graph
//
//    Author(s):    Bryan Kline, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_INTERSECT_HPP
#define VS_INTERSECT_HPP

#include "atArray.h++"
#include "atVector.h++"

#include "vsGeometry.h++"
#include "vsPane.h++"
#include "vsIntersectResult.h++"
#include "vsIntersectTraverser.h++"

#include <osgUtil/LineSegmentIntersector>

#define VS_INTERSECT_SEGS_MAX 32

enum vsIntersectFacingMode
{
    VS_INTERSECT_IGNORE_NONE,
    VS_INTERSECT_IGNORE_FRONTFACE,
    VS_INTERSECT_IGNORE_BACKFACE
};


class VESS_SYM vsIntersect : public vsObject
{
private:

    int                          facingMode;
    bool                         clipSensitivity;
    bool                         pathsEnabled;

    osgUtil::IntersectorGroup    *osgIsectGroup;
    vsIntersectTraverser         *intersectTraverser;

    int                          segListSize;
    atArray                      *segList;
    atArray                      *resultList;

    osg::Node                    *getBaseLibraryObject(vsNode *node);

    typedef osgUtil::LineSegmentIntersector::Intersection    SegIntersection;

    void                         clearIntersectionResults();
    void                         populateIntersection(
                                         int index,
                                         const SegIntersection *intersection);
    bool                         isClipped(osg::Node * node, osg::Vec3 point);

public:

                         vsIntersect();
    virtual              ~vsIntersect();

    virtual const char   *getClassName();

    void                 setSegListSize(int newSize);
    int                  getSegListSize();

    void                 setSeg(int segNum, atVector startPt, atVector endPt);
    void                 setSeg(int segNum, atVector startPt,
                                atVector directionVec, double length);
    atVector             getSegStartPt(int segNum);
    atVector             getSegEndPt(int segNum);
    atVector             getSegDirection(int segNum);
    double               getSegLength(int segNum);

    void                 setPickSeg(int segNum, vsPane *pane,
                                    double x, double y);

    void                 setMask(unsigned int newMask);
    unsigned int         getMask();

    void                 enableClipSensitivity();
    void                 disableClipSensitivity();

    void                 enablePaths();
    void                 disablePaths();

    void                 setFacingMode(int newMode);
    int                  getFacingMode();

    void                 setSequenceTravMode(int newMode);
    int                  getSequenceTravMode();

    void                 setSwitchTravMode(int newMode);
    int                  getSwitchTravMode();

    void                 setLODTravMode(int newMode);
    int                  getLODTravMode();

    void                 intersect(vsNode *targetNode);

    vsIntersectResult    *getIntersection(int segNum);
};

#endif
