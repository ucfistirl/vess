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

#include "atVector.h++"
#include "atMatrix.h++"
#include "vsViewpointAttribute.h++"

enum vsViewProjectionMode
{
    VS_VIEW_PROJMODE_PERSP,
    VS_VIEW_PROJMODE_ORTHO,
    VS_VIEW_PROJMODE_OFFAXIS_PERSP
};

class VS_GRAPHICS_DLL vsView : public vsObject
{
private:

    atVector                viewLocation;
    atMatrix                viewRotation;

    double                  nearClip, farClip;

    int                     projMode;
    double                  projHval, projVval;
    double                  projLeft, projRight, projTop, projBottom;
    int                     changeNum;
    
    vsViewpointAttribute    *viewAttribute;

VS_INTERNAL:

    void        getProjectionData(int *mode, double *horizVal,
                                  double *vertiVal);
    void        getOffAxisProjectionData(double *left, double *right,
                                         double *bottom, double *top);

    bool        attachViewAttribute(vsViewpointAttribute *theAttribute);
    void        detachViewAttribute();
    void        updateFromAttribute();

    int         getChangeNum();

public:

                vsView();
    virtual     ~vsView();

    virtual const char    *getClassName();

    void        setViewpoint(double xPosition, double yPosition,
                             double zPosition);
    void        setViewpoint(atVector newPosition);
    void        getViewpoint(double *xPosition, double *yPosition,
                             double *zPosition);
    atVector    getViewpoint();

    void        setDirectionFromVector(atVector direction,
                                       atVector upDirection);
    void        lookAtPoint(atVector targetPoint, atVector upDirection);
    void        setDirectionFromRotation(atQuat rotQuat);
    void        setDirectionFromRotation(atMatrix rotMatrix);
    
    void        setClipDistances(double nearPlane, double farPlane);
    void        getClipDistances(double *nearPlane, double *farPlane);

    void        setPerspective(double horizFOV, double vertiFOV);
    void        setOrthographic(double horizSize, double vertiSize);
    void        setOffAxisPerspective(double left, double right, double bottom,
                                      double top);

    atVector    getDirection();
    atVector    getUpDirection();

    atMatrix    getRotationMat();
};

#endif
