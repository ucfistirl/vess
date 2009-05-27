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
//    VESS Module:  vsLineSegment.h++
//
//    Description:  This class represents a line segment as a start point
//                  and an end point.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_LINE_SEGMENT_HPP
#define VS_LINE_SEGMENT_HPP

#include "vsGlobals.h++"
#include "vsObject.h++"
#include "atVector.h++"

class VESS_SYM vsLineSegment : public vsObject
{
protected:

    atVector    startPoint;
    atVector    endPoint;

public:

                          vsLineSegment();
                          vsLineSegment(const atVector &start,
                              const atVector &end);
    virtual               ~vsLineSegment();

    virtual const char    *getClassName();

    void                  setStartPoint(const atVector &start);
    void                  setEndPoint(const atVector &end);

    atVector              getStartPoint() const;
    atVector              getEndPoint() const;
};

#endif

