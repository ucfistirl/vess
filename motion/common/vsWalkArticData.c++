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
//    VESS Module:  vsWalkArticData.h++
//
//    Description:  Class to hold a single keyframe position for the
//                  vsWalkArticulation motion model.
//
//    Author(s):    Jason Daly, Bryan Kline
//
//------------------------------------------------------------------------

#include "vsWalkArticData.h++"

// ------------------------------------------------------------------------
// Default constructor.  Sets all values to zero or identity
// ------------------------------------------------------------------------
vsWalkArticData::vsWalkArticData()
{
    // Initialize all values
    leftHip.set(0.0, 0.0, 0.0, 1.0);
    leftKnee.set(0.0, 0.0, 0.0, 1.0);
    leftAnkle.set(0.0, 0.0, 0.0, 1.0);
    rightHip.set(0.0, 0.0, 0.0, 1.0);
    rightKnee.set(0.0, 0.0, 0.0, 1.0);
    rightAnkle.set(0.0, 0.0, 0.0, 1.0);
    distance = 0.0;
}

// ------------------------------------------------------------------------
// Sets all values to the given input values
// ------------------------------------------------------------------------
vsWalkArticData::vsWalkArticData(atQuat lhip, atQuat lknee, atQuat lankle,
                                 atQuat rhip, atQuat rknee, atQuat rankle,
                                 double dist)
{
    // Store all input values
    leftHip = lhip;
    leftKnee = lknee;
    leftAnkle = lankle;
    rightHip = rhip;
    rightKnee = rknee;
    rightAnkle = rankle;
    distance = dist;
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsWalkArticData::~vsWalkArticData()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsWalkArticData::getClassName()
{
   return "vsWalkArticData";
}

// ------------------------------------------------------------------------
// Return a clone of this object
// ------------------------------------------------------------------------
vsWalkArticData *vsWalkArticData::clone()
{
   vsWalkArticData *newClone;

   // Create a new object with the same values as this one
   newClone = new vsWalkArticData(leftHip, leftKnee, leftAnkle,
                                  rightHip, rightKnee, rightAnkle, distance);

   // Return the clone
   return newClone;
}

// ------------------------------------------------------------------------
// Returns the left hip rotation
// ------------------------------------------------------------------------
atQuat vsWalkArticData::getLeftHip()
{
   return leftHip;
}

// ------------------------------------------------------------------------
// Sets the left hip rotation
// ------------------------------------------------------------------------
void vsWalkArticData::setLeftHip(atQuat newRotation)
{
   leftHip = newRotation;
}

// ------------------------------------------------------------------------
// Returns the left knee rotation
// ------------------------------------------------------------------------
atQuat vsWalkArticData::getLeftKnee()
{
   return leftKnee;
}

// ------------------------------------------------------------------------
// Sets the left knee rotation
// ------------------------------------------------------------------------
void vsWalkArticData::setLeftKnee(atQuat newRotation)
{
   leftKnee = newRotation;
}

// ------------------------------------------------------------------------
// Returns the left ankle rotation
// ------------------------------------------------------------------------
atQuat vsWalkArticData::getLeftAnkle()
{
   return leftAnkle;
}

// ------------------------------------------------------------------------
// Sets the left ankle rotation
// ------------------------------------------------------------------------
void vsWalkArticData::setLeftAnkle(atQuat newRotation)
{
   leftAnkle = newRotation;
}

// ------------------------------------------------------------------------
// Returns the right hip rotation
// ------------------------------------------------------------------------
atQuat vsWalkArticData::getRightHip()
{
   return rightHip;
}

// ------------------------------------------------------------------------
// Sets the right hip rotation
// ------------------------------------------------------------------------
void vsWalkArticData::setRightHip(atQuat newRotation)
{
   rightHip = newRotation;
}

// ------------------------------------------------------------------------
// Returns the right knee rotation
// ------------------------------------------------------------------------
atQuat vsWalkArticData::getRightKnee()
{
   return rightKnee;
}

// ------------------------------------------------------------------------
// Sets the right knee rotation
// ------------------------------------------------------------------------
void vsWalkArticData::setRightKnee(atQuat newRotation)
{
   rightKnee = newRotation;
}

// ------------------------------------------------------------------------
// Returns the right ankle rotation
// ------------------------------------------------------------------------
atQuat vsWalkArticData::getRightAnkle()
{
   return rightAnkle;
}

// ------------------------------------------------------------------------
// Sets the right ankle rotation
// ------------------------------------------------------------------------
void vsWalkArticData::setRightAnkle(atQuat newRotation)
{
   rightAnkle = newRotation;
}

// ------------------------------------------------------------------------
// Returns the keyframe's active distance value
// ------------------------------------------------------------------------
double vsWalkArticData::getDistance()
{
   return distance;
}

// ------------------------------------------------------------------------
// Sets the active distance value
// ------------------------------------------------------------------------
void vsWalkArticData::setDistance(double newDist)
{
   distance = newDist;
}

