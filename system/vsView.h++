// File vsView.h++

#ifndef VS_VIEW_HPP
#define VS_VIEW_HPP

class vsView;

#include <math.h>

#include "vsSystem.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsViewpointAttribute.h++"

class vsView
{
private:

    vsVector                viewLocation;
    vsQuat                  viewRotation;
    
    double		    nearClip, farClip;
    
    vsViewpointAttribute    *viewAttribute;

VS_INTERNAL:

    int         attachViewAttribute(vsViewpointAttribute *theAttribute);
    void        detachViewAttribute();
    void        updateFromAttribute();

public:

                vsView();
    virtual     ~vsView();

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
    
    void	setClipDistances(double nearPlane, double farPlane);
    void	getClipDistances(double *nearPlane, double *farPlane);

    vsVector    getDirection();
    vsVector    getUpDirection();

    vsQuat      getRotationQuat();
};

#endif
