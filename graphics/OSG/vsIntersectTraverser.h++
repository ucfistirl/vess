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
//    VESS Module:  vsIntersectTraverser
//
//    Description:  OSG NodeVisitor traversal visitor to control how
//                  switches, sequences, and LOD nodes are traversed
//                  during an IntersectVisitor traversal
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_INTERSECT_TRAVERSER_HPP
#define VS_INTERSECT_TRAVERSER_HPP

#include "vsGlobals.h++"
#include <osgUtil/IntersectVisitor>
#include <osg/Node>
#include <osg/Sequence>
#include <osg/Switch>
#include <osg/LOD>

enum VS_GRAPHICS_DLL vsIntersectSwitchTraversalMode
{
    VS_INTERSECT_SWITCH_NONE,
    VS_INTERSECT_SWITCH_CURRENT,
    VS_INTERSECT_SWITCH_ALL
};

enum VS_GRAPHICS_DLL vsIntersectSequenceTraversalMode
{
    VS_INTERSECT_SEQUENCE_NONE,
    VS_INTERSECT_SEQUENCE_CURRENT,
    VS_INTERSECT_SEQUENCE_ALL
};

enum VS_GRAPHICS_DLL vsIntersectLODTraversalMode
{
    VS_INTERSECT_LOD_NONE,
    VS_INTERSECT_LOD_FIRST,
    VS_INTERSECT_LOD_ALL
};

class VS_GRAPHICS_DLL vsIntersectTraverser : public osgUtil::IntersectVisitor
{
private:

    int         switchTravMode;
    int         sequenceTravMode;
    int         lodTravMode;

VS_INTERNAL:

                    vsIntersectTraverser();
    virtual         ~vsIntersectTraverser();

    void            setSequenceTravMode(int newMode);
    int             getSequenceTravMode();
    void            setSwitchTravMode(int newMode);
    int             getSwitchTravMode();
    void            setLODTravMode(int newMode);
    int             getLODTravMode();

    virtual void    apply(osg::Sequence &node);
    virtual void    apply(osg::Switch &node);
    virtual void    apply(osg::LOD &node);
};

#endif
