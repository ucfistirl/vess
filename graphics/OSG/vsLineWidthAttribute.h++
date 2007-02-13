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
//    VESS Module:  vsLineWidthAttribute.h++
//
//    Description:  Specifies the line width in pixels for geometries
//
//    Author(s):    Jeremy Elbourn
//
//------------------------------------------------------------------------
#ifndef VS_LINE_WIDTH_ATTRIBUTE_HPP
#define VS_LINE_WIDTH_ATTRIBUTE_HPP

#include <osg/LineWidth>
#include "vsStateAttribute.h++"

class VS_GRAPHICS_DLL vsLineWidthAttribute : public vsStateAttribute
{
private:
   
    osg::LineWidth    *osgLineWidth;
      
    virtual void      setOSGAttrModes(vsNode *node);
      
VS_INTERNAL:
    
    virtual void       attach(vsNode * node);
    virtual void       detach(vsNode * node);
       
    virtual void       attachDuplicate(vsNode * theNode);
       
    virtual bool       isEquivalent(vsAttribute * attribute);
       
public:
    
                          vsLineWidthAttribute();
    virtual               ~vsLineWidthAttribute();
       
    virtual const char    *getClassName();
      
    virtual int           getAttributeType();
       
    void                  setLineWidth(double newWidth);
    double                getLineWidth();
};

#endif
