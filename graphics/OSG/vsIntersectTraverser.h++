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
//                  during an IntersectionVisitor traversal
//
//    Author(s):    Jason Daly, Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_INTERSECT_TRAVERSER_HPP
#define VS_INTERSECT_TRAVERSER_HPP

#include "vsGlobals.h++"

#include <osgUtil/IntersectionVisitor>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Billboard>
#include <osg/Group>
#include <osg/Camera>
#include <osg/LOD>
#include <osg/PagedLOD>
#include <osgSim/MultiSwitch>
#include <osg/Projection>
#include <osg/Sequence>
#include <osg/Switch>
#include <osg/Transform>

// This comes from osg::NodeVisitor::TraversalMode
#define VS_INTERSECT_DEFAULT_TRAV_MODE TRAVERSE_ACTIVE_CHILDREN

enum vsIntersectSwitchTraversalMode
{
    VS_INTERSECT_SWITCH_NONE,
    VS_INTERSECT_SWITCH_CURRENT,
    VS_INTERSECT_SWITCH_ALL
};

enum vsIntersectSequenceTraversalMode
{
    VS_INTERSECT_SEQUENCE_NONE,
    VS_INTERSECT_SEQUENCE_CURRENT,
    VS_INTERSECT_SEQUENCE_ALL
};

enum vsIntersectLODTraversalMode
{
    VS_INTERSECT_LOD_NONE,
    VS_INTERSECT_LOD_FIRST,
    VS_INTERSECT_LOD_CURRENT,
    VS_INTERSECT_LOD_ALL
};

class VESS_SYM vsIntersectTraverser : public osgUtil::IntersectionVisitor
{
private:

    int         switchTravMode;
    int         sequenceTravMode;
    int         lodTravMode;

    osg::NodeVisitor::TraversalMode    updateTraversalMode(TraversalMode mode);

VS_INTERNAL:

                    vsIntersectTraverser();
    virtual         ~vsIntersectTraverser();

    void            setSequenceTravMode(int newMode);
    int             getSequenceTravMode();
    void            setSwitchTravMode(int newMode);
    int             getSwitchTravMode();
    void            setLODTravMode(int newMode);
    int             getLODTravMode();

    virtual void    apply(osg::Node &node);
    virtual void    apply(osg::Geode &node);
    virtual void    apply(osg::Billboard &node);
    virtual void    apply(osg::Group &node);
    virtual void    apply(osg::Camera &node);
    virtual void    apply(osg::LOD &node);
    virtual void    apply(osg::PagedLOD &node);
    virtual void    apply(osgSim::MultiSwitch &node);
    virtual void    apply(osg::Projection &node);
    virtual void    apply(osg::Sequence &node);
    virtual void    apply(osg::Switch &node);
    virtual void    apply(osg::Transform &node);
};

#endif
