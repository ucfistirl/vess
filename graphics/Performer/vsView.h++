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
//    VESS Module:  vsView.h++
//
//    Description:  Class for storing and maintaining the viewpoint of a
//                  vsPane
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_VIEW_HPP
#define VS_VIEW_HPP

class vsView;

#include <math.h>

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsViewpointAttribute.h++"

enum vsViewProjectionMode
{
    VS_VIEW_PROJMODE_PERSP,
    VS_VIEW_PROJMODE_ORTHO
};

class vsView : public vsObject
{
private:

    vsVector                viewLocation;
    vsMatrix                viewRotation;

    double                  nearClip, farClip;

    int                     projMode;
    double                  projHval, projVval;
    
    vsViewpointAttribute    *viewAttribute;

VS_INTERNAL:

    void        getProjectionData(int *mode, double *horizVal,
                                  double *vertiVal);

    bool        attachViewAttribute(vsViewpointAttribute *theAttribute);
    void        detachViewAttribute();
    void        updateFromAttribute();

public:

                vsView();
    virtual     ~vsView();

    virtual const char    *getClassName();

    void        setViewpoint(double xPosition, double yPosition,
                             double zPosition);
    void        setViewpoint(vsVector newPosition);
    void        getViewpoint(double *xPosition, double *yPosition,
                             double *zPosition);
    vsVector    getViewpoint();

    void        setDirectionFromVector(vsVector direction,
                                       vsVector upDirection);
    void        lookAtPoint(vsVector targetPoint, vsVector upDirection);
    void        setDirectionFromRotation(vsQuat rotQuat);
    void        setDirectionFromRotation(vsMatrix rotMatrix);
    
    void        setClipDistances(double nearPlane, double farPlane);
    void        getClipDistances(double *nearPlane, double *farPlane);

    void        setPerspective(double horizFOV, double vertiFOV);
    void        setOrthographic(double horizSize, double vertiSize);

    vsVector    getDirection();
    vsVector    getUpDirection();

    vsMatrix    getRotationMat();
};

#endif
