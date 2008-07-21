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
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_VIEW_HPP
#define VS_VIEW_HPP

#include "atVector.h++"
#include "atMatrix.h++"
#include "vsObject.h++"

enum vsViewProjectionMode
{
    VS_VIEW_PROJMODE_PERSP,
    VS_VIEW_PROJMODE_ORTHO,
    VS_VIEW_PROJMODE_OFFAXIS_PERSP
};

class VESS_SYM vsView : public vsObject
{
private:

    // Current viewpoint and orientation values
    atVector    viewpoint;
    atVector    forwardDir, upDir;

    // Near/far clipping plane distances
    double      nearClipDist, farClipDist;

    // Projection mode and FOV values
    int         projMode;
    double      projHval, projVval;
    double      projLeft, projRight, projTop, projBottom;

    // Marker for noting if the view values changed
    int         changeNum;
    
VS_INTERNAL:

    void        getProjectionData(int *mode, double *horizVal,
                                  double *vertiVal);
    void        getOffAxisProjectionData(double *left, double *right, 
                                         double *bottom, double *top);

    int         getChangeNum();

public:

                vsView();
    virtual     ~vsView();

    // Inherited functions
    virtual const char    *getClassName();

    // Position manipulations
    void        setViewpoint(double xPosition, double yPosition,
                             double zPosition);
    void        setViewpoint(atVector newPosition);
    void        getViewpoint(double *xPosition, double *yPosition,
                             double *zPosition);
    atVector    getViewpoint();

    // Orientation manipulations
    void        setDirectionFromVector(atVector direction,
                                       atVector upDirection);
    void        lookAtPoint(atVector targetPoint, atVector upDirection);
    void        setDirectionFromRotation(atQuat rotQuat);
    void        setDirectionFromRotation(atMatrix rotMatrix);
    
    // Near/far clip plane functions
    void        setClipDistances(double nearPlane, double farPlane);
    void        getClipDistances(double *nearPlane, double *farPlane);

    // Projection manipulations
    void        setPerspective(double horizFOV, double vertiFOV);
    void        setOrthographic(double horizSize, double vertiSize);
    void        setOffAxisPerspective(double left, double right, double bottom,
                                      double top);

    // Orientation accessors
    atVector    getDirection();
    atVector    getUpDirection();

    atMatrix    getRotationMat();
};

#endif
