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
//    VESS Module:  vsCgShaderAttribute.h++
//
//    Description:  Attribute that defines a Cg fragment and vertex shader
//                  to be applied in the subgraph it is attached.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CG_SHADER_ATTRIBUTE_HPP
#define VS_CG_SHADER_ATTRIBUTE_HPP

#include <osgNVCg/Context>
#include <osgNVCg/Program>
#include "vsCgVectorParameter.h++"
#include "vsCgMatrixParameter.h++"
#include "vsCgStateMatrixParameter.h++"
#include "vsCgTextureParameter.h++"
#include "vsStateAttribute.h++"

enum VS_GRAPHICS_DLL vsShaderProfile
{
    VS_SHADER_UNKNOWN = osgNVCg::Program::UNKNOWN,
    VS_SHADER_VP20 = osgNVCg::Program::VP20,
    VS_SHADER_FP20 = osgNVCg::Program::FP20,
    VS_SHADER_VP30 = osgNVCg::Program::VP30,
    VS_SHADER_FP30 = osgNVCg::Program::FP30,
    VS_SHADER_ARBVP1 = osgNVCg::Program::ARBVP1,
    VS_SHADER_ARBFP1 = osgNVCg::Program::ARBFP1
};

class VS_GRAPHICS_DLL vsCgShaderAttribute : public vsStateAttribute
{
private:

    osgNVCg::Context    *cgContext;
    osgNVCg::Program    *cgVertexProgram;
    osgNVCg::Program    *cgFragmentProgram;

    virtual void        setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    void                setCgContext(osgNVCg::Context *newContext);
    osgNVCg::Context    *getCgContext();

    virtual void        attach(vsNode *node);
    virtual void        detach(vsNode *node);
    virtual void        attachDuplicate(vsNode *theNode);
    virtual bool        isEquivalent(vsAttribute *attribute);

public:

                               vsCgShaderAttribute();
    virtual                    ~vsCgShaderAttribute();

    virtual const char         *getClassName();
    virtual int                getAttributeType();


    void                        setCgVertexSourceFile(char *filename);
    char                        *getCgVertexSourceFile();
    void                        setCgVertexEntryPoint(char *entry);
    char                        *getCgVertexEntryPoint();
    void                        setCgVertexProfile(vsShaderProfile profile);
    vsShaderProfile             getCgVertexProfile();
    vsCgVectorParameter         *getVertexVectorParameter(char *name);
    vsCgMatrixParameter         *getVertexMatrixParameter(char *name);
    vsCgStateMatrixParameter    *getVertexStateMatrixParameter(char *name);
    vsCgTextureParameter        *getVertexTextureParameter(char *name);

    void                        setCgFragmentSourceFile(char *filename);
    char                        *getCgFragmentSourceFile();
    void                        setCgFragmentEntryPoint(char *entry);
    char                        *getCgFragmentEntryPoint();
    void                        setCgFragmentProfile(vsShaderProfile profile);
    vsShaderProfile             getCgFragmentProfile();
    vsCgVectorParameter         *getFragmentVectorParameter(char *name);
    vsCgMatrixParameter         *getFragmentMatrixParameter(char *name);
    vsCgStateMatrixParameter    *getFragmentStateMatrixParameter(char *name);
    vsCgTextureParameter        *getFragmentTextureParameter(char *name);
};

#endif