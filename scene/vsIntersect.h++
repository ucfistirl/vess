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
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_INTERSECT_HPP
#define VS_INTERSECT_HPP

#include <Performer/pr/pfGeoSet.h>
#include <Performer/pf/pfNode.h>
#include "vsVector.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsGrowableArray.h++"
#include "vsSystem.h++"

#define VS_INTERSECT_SEGS_MAX 32

class vsIntersect
{
private:

    pfSegSet           performerSegSet;
    int                segListSize;

    int                pathsEnabled;

    // Intersection results
    int                validFlag[VS_INTERSECT_SEGS_MAX];
    vsVector           sectPoint[VS_INTERSECT_SEGS_MAX];
    vsVector           sectNorm[VS_INTERSECT_SEGS_MAX];
    vsGeometry         *sectGeom[VS_INTERSECT_SEGS_MAX];
    int                sectPrim[VS_INTERSECT_SEGS_MAX];
    vsGrowableArray    *sectPath[VS_INTERSECT_SEGS_MAX];

public:

                       vsIntersect();
                       ~vsIntersect();

    void               setSegListSize(int newSize);
    int                getSegListSize();

    void               setSeg(int segNum, vsVector startPt, vsVector endPt);
    void               setSeg(int segNum, vsVector startPt,
                              vsVector directionVec, double length);
    vsVector           getSegStartPt(int segNum);
    vsVector           getSegEndPt(int segNum);
    vsVector           getSegDirection(int segNum);
    double             getSegLength(int segNum);

    void               setMask(unsigned int newMask);
    unsigned int       getMask();
    
    void               enablePaths();
    void               disablePaths();

    void               intersect(vsNode *targetNode);

    int                getIsectValid(int segNum);
    vsVector           getIsectPoint(int segNum);
    vsVector           getIsectNorm(int segNum);
    vsGeometry         *getIsectGeometry(int segNum);
    int                getIsectPrimNum(int segNum);
    vsGrowableArray    *getIsectPath(int segNum);
};

#endif
