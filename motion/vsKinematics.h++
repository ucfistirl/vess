// File vsKinematics.h++

#ifndef VS_KINEMATICS_HPP
#define VS_KINEMATICS_HPP

#include "vsComponent.h++"
#include "vsTransformAttribute.h++"
#include "vsVector.h++"
#include "vsQuat.h++"

class vsKinematics
{
private:

    vsComponent             *component;
    vsTransformAttribute    *transform;

    vsVector		    position;
    vsQuat		    orientation;

    vsVector                velocity;
    vsVector	            angularVelocity;
    
    double		    lastTime;

public:

		vsKinematics(vsComponent *theComponent);
		~vsKinematics();

    void	setPosition(vsVector newPosition);
    vsVector    getPosition();
    void	modifyPosition(vsVector deltaPosition);
    
    void	setOrientation(vsQuat newOrientation);
    vsQuat	getOrientation();
    void	preModifyOrientation(vsQuat deltaOrientation);
    void	postModifyOrientation(vsQuat deltaOrientation);
    
    void	setVelocity(vsVector newVelocity);
    vsVector    getVelocity();
    void	modifyVelocity(vsVector deltaVelocity);
    
    void	setAngularVelocity(vsVector rotAxis, double degreesPerSec);
    vsVector	getAngularVelocity();
    void	modifyAngularVelocity(vsVector rotAxis, double degreesPerSec);
    
    void	setCenterOfMass(vsVector newCenter);
    vsVector	getCenterOfMass();
    
    void	update();
    
    void	reset();
    void	resetTimer();
};

#endif
