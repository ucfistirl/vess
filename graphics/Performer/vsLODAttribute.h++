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
//    VESS Module:  vsLODAttribute.h++
//
//    Description:  Specifies that the children of the component are all
//                  levels-of-detail of the same object and are not to be
//                  drawn all at the same time; only one of the children
//                  should be drawn, with the determination of which to
//                  draw based on the distance from the viewer to the
//                  object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_LOD_ATTRIBUTE_HPP
#define VS_LOD_ATTRIBUTE_HPP

#include <Performer/pf/pfLOD.h>
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsLODAttribute : public vsAttribute
{
private:

    pfLOD       *performerLOD;

VS_INTERNAL:

    virtual bool    canAttach();    
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

public:

                          vsLODAttribute();
    virtual               ~vsLODAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();
    
    void                  setRangeEnd(int childNum, double rangeLimit);
    double                getRangeEnd(int childNum);
};

#endif
