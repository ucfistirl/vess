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

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsObject.h++"

enum VS_GRAPHICS_DLL vsViewProjectionMode
{
    VS_VIEW_PROJMODE_PERSP,
    VS_VIEW_PROJMODE_ORTHO,
    VS_VIEW_PROJMODE_OFFAXIS_PERSP
};

class VS_GRAPHICS_DLL vsView : public vsObject
{
private:

    // Current viewpoint and orientation values
    vsVector    viewpoint;
    vsVector    forwardDir, upDir;

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
    void        setViewpoint(vsVector newPosition);
    void        getViewpoint(double *xPosition, double *yPosition,
                             double *zPosition);
    vsVector    getViewpoint();

    // Orientation manipulations
    void        setDirectionFromVector(vsVector direction,
                                       vsVector upDirection);
    void        lookAtPoint(vsVector targetPoint, vsVector upDirection);
    void        setDirectionFromRotation(vsQuat rotQuat);
    void        setDirectionFromRotation(vsMatrix rotMatrix);
    
    // Near/far clip plane functions
    void        setClipDistances(double nearPlane, double farPlane);
    void        getClipDistances(double *nearPlane, double *farPlane);

    // Projection manipulations
    void        setPerspective(double horizFOV, double vertiFOV);
    void        setOrthographic(double horizSize, double vertiSize);
    void        setOffAxisPerspective(double left, double right, double bottom,
                                      double top);

    // Orientation accessors
    vsVector    getDirection();
    vsVector    getUpDirection();

    vsMatrix    getRotationMat();
};

#endif
