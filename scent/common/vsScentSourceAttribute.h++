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
//    VESS Module:  vsScentSourceAttribute.h++
//
//    Description:  Attribute to maintain the location of a source of odor
//                  in the VESS scene.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SCENT_SOURCE_ATTRIBUTE_HPP
#define VS_SCENT_SOURCE_ATTRIBUTE_HPP

#include "vsAttribute.h++"
#include "vsComponent.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsScent.h++"

#define VS_SA_DEFAULT_SCALE        1.0
#define VS_SA_DEFAULT_MIN_STRENGTH 0.0
#define VS_SA_DEFAULT_MAX_STRENGTH 1.0
#define VS_SA_DEFAULT_REF_DIST     0.1
#define VS_SA_DEFAULT_MAX_DIST     -1.0
#define VS_SA_DEFAULT_ROLLOFF      1.0

class vsScentSourceAttribute : public vsAttribute
{
protected:

    vsComponent    *parentComponent;
    vsMatrix       offsetMatrix;
    vsScent        *scent;

    vsVector       currentPosition;

    double         scale;
    double         minStrength, maxStrength;
    double         refDistance, maxDistance;
    double         rolloffFactor;

    bool           scentOn;

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    vsVector        getPosition();

    void            update();

public:

                          vsScentSourceAttribute(vsScent *theScent);
                          ~vsScentSourceAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    void                  setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix              getOffsetMatrix();

    vsScent               *getScent();

    void                  on();
    void                  off();
    bool                  isOn();

    double                getStrengthScale();
    void                  setStrengthScale(double newScale);
    double                getMinStrength();
    void                  setMinStrength(double newMin);
    double                getMaxStrength();
    void                  setMaxStrength(double newMax);

    double                getReferenceDistance();
    void                  setReferenceDistance(double distance);
    double                getMaxDistance();
    void                  setMaxDistance(double distance);
    double                getRolloffFactor();
    void                  setRolloffFactor(double factor);
};

#endif
