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

#include <osg/Camera>
#include "vsVector.h++"
#include "vsMatrix.h++"

enum VS_GRAPHICS_DLL vsViewProjectionMode
{
    VS_VIEW_PROJMODE_PERSP,
    VS_VIEW_PROJMODE_ORTHO
};

class VS_GRAPHICS_DLL vsView
{
private:

    // OSG Camera object for handling the viewpoint parameters
    osg::Camera    *osgCamera;

    // Projection mode and FOV values
    int            projMode;
    double         projHval, projVval;
    
VS_INTERNAL:

    void        getProjectionData(int *mode, double *horizVal,
                                  double *vertiVal);

public:

                   vsView();
    virtual        ~vsView();

    // Inherited functions
    virtual const char    *getClassName();

    // Position manipulations
    void           setViewpoint(double xPosition, double yPosition,
                                double zPosition);
    void           setViewpoint(vsVector newPosition);
    void           getViewpoint(double *xPosition, double *yPosition,
                                double *zPosition);
    vsVector       getViewpoint();

    // Orientation manipulations
    void           setDirectionFromVector(vsVector direction,
                                       vsVector upDirection);
    void           lookAtPoint(vsVector targetPoint, vsVector upDirection);
    void           setDirectionFromRotation(vsQuat rotQuat);
    void           setDirectionFromRotation(vsMatrix rotMatrix);
    
    // Near/far clip plane functions
    void           setClipDistances(double nearPlane, double farPlane);
    void           getClipDistances(double *nearPlane, double *farPlane);

    // Projection manipulations
    void           setPerspective(double horizFOV, double vertiFOV);
    void           setOrthographic(double horizSize, double vertiSize);

    // Orientation accessors
    vsVector       getDirection();
    vsVector       getUpDirection();

    vsMatrix       getRotationMat();

    // Returns the OSG Camera object
    osg::Camera    *getBaseLibraryObject();
};

#endif
