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
//    VESS Module:  vsScentDetectorAttribute.h++
//
//    Description:  Attribute to maintain the location of the recipient of
//                  scent sources in the scene (usually tied to the user's
//                  viewpoint).
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_SCENT_DETECTOR_ATTRIBUTE_HPP
#define VS_SCENT_DETECTOR_ATTRIBUTE_HPP

#include "vsObject.h++"
#include "vsAttribute.h++"
#include "vsComponent.h++"
#include "vsMatrix.h++"
#include "vsVector.h++"

#define VS_SD_DEFAULT_SENSITIVITY 1.0

class VS_SCENT_DLL vsScentDetectorAttribute : public vsAttribute
{
protected:

    vsComponent    *parentComponent;
    vsMatrix       offsetMatrix;
    vsVector       currentPosition;

    double         sensitivity;

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    vsVector        getPosition();

    void            update();

public:

                          vsScentDetectorAttribute();
                          ~vsScentDetectorAttribute();

    virtual const char    *getClassName();
    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    void                  setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix              getOffsetMatrix();

    double                getSensitivity();
    void                  setSensitivity(double newSensitivity);
};

#endif
