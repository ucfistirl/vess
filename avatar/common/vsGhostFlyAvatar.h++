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
//    VESS Module:  vsGhostFlyAvatar.h++
//
//    Description:  Invisible (no geometry) avatar with a vsFlyingMotion
//                  motion model attached. Automatically sets itself to
//                  view the given scene in the given pane.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_GHOSTFLYAVATAR_HPP
#define VS_GHOSTFLYAVATAR_HPP

#include "vsAvatar.h++"
#include "vsMouse.h++"
#include "vsKinematics.h++"
#include "vsFlyingMotion.h++"
#include "vsWindowSystem.h++"

class vsGhostFlyAvatar : public vsAvatar
{
private:

    virtual void      setup();
    
    vsPane            *pane;
    vsScene           *scene;

    vsView            *view;
    vsKinematics      *ghostKin;
    vsFlyingMotion    *flyMotion;
    vsWindowSystem    *windowSystem;

public:

                          vsGhostFlyAvatar(vsPane *targetPane, 
                                           vsScene *targetScene);
    virtual               ~vsGhostFlyAvatar();

    virtual const char    *getClassName();

    virtual void          update();

    vsKinematics          *getKinematics();
    vsFlyingMotion        *getFlyingMotion();
};

#endif
