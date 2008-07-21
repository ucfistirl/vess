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
//    VESS Module:  vsDynamicGeometry.h++
//
//    Description:  vsGeometryBase subclass that handles dynamic geometry
//                  Under Open Scene Graph, the only real difference
//                  between this class and vsGeometry is that this class
//                  disables the use of display lists.
//
//    Author(s):    Bryan Kline, Jason Daly, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_DYNAMIC_GEOMETRY_HPP
#define VS_DYNAMIC_GEOMETRY_HPP

#include "vsGeometryBase.h++"

class VESS_SYM vsDynamicGeometry : public vsGeometryBase
{
protected:

    bool    dataChanged[VS_GEOMETRY_LIST_COUNT];
    bool    primitivesChanged;

public:

                          vsDynamicGeometry();
    virtual               ~vsDynamicGeometry();

    virtual const char    *getClassName();

    virtual int           getNodeType();

    void                  beginNewState();
    void                  finishNewState();

    void                  setPrimitiveType(int newType);
    void                  setPrimitiveCount(int newCount);
    void                  setPrimitiveLength(int index, int length);
    void                  setPrimitiveLengths(int *lengths);

    void                  setData(int whichData, int dataIndex, atVector data);
    void                  setDataList(int whichData, atVector *dataBuffer);
    void                  setDataListSize(int whichData, int newSize);

    void                  setIndex(int indexIndex, u_int index);
    void                  setIndexList(u_int *indexBuffer);
    void                  setIndexListSize(int newSize);
};

#endif
