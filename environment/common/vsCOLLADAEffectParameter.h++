#ifndef VS_COLLADA_EFFECT_PARAMETER_HPP
#define VS_COLLADA_EFFECT_PARAMETER_HPP

#include "atString.h++"
#include "atStringTokenizer.h++"
#include "atXMLDocument.h++"
#include "vsObject.h++"
#include "vsTextureAttribute.h++"

enum vsCOLLADAParameterType
{
    VS_COLLADA_BOOL,
    VS_COLLADA_BOOL2,
    VS_COLLADA_BOOL3,
    VS_COLLADA_BOOL4,
    VS_COLLADA_INT,
    VS_COLLADA_INT2,
    VS_COLLADA_INT3,
    VS_COLLADA_INT4,
    VS_COLLADA_FLOAT,
    VS_COLLADA_FLOAT2,
    VS_COLLADA_FLOAT3,
    VS_COLLADA_FLOAT4,
    VS_COLLADA_FLOAT1X1,
    VS_COLLADA_FLOAT1X2,
    VS_COLLADA_FLOAT1X3,
    VS_COLLADA_FLOAT1X4,
    VS_COLLADA_FLOAT2X1,
    VS_COLLADA_FLOAT2X2,
    VS_COLLADA_FLOAT2X3,
    VS_COLLADA_FLOAT2X4,
    VS_COLLADA_FLOAT3X1,
    VS_COLLADA_FLOAT3X2,
    VS_COLLADA_FLOAT3X3,
    VS_COLLADA_FLOAT3X4,
    VS_COLLADA_FLOAT4X1,
    VS_COLLADA_FLOAT4X2,
    VS_COLLADA_FLOAT4X3,
    VS_COLLADA_FLOAT4X4,
    VS_COLLADA_SURFACE,
    VS_COLLADA_TEXTURE_1D,
    VS_COLLADA_TEXTURE_2D,
    VS_COLLADA_TEXTURE_3D,
    VS_COLLADA_TEXTURE_CUBE,
    VS_COLLADA_TEXTURE_RECT,
    VS_COLLADA_ENUM
};

class VESS_SYM vsCOLLADAEffectParameter : public vsObject
{
protected:

    atString                  parameterName;
    vsCOLLADAParameterType    parameterType;

    bool                      boolValue[4];
    int                       intValue[4];
    atVector                  floatValue;
    atMatrix                  matrixValue;
    vsTextureAttribute        *textureValue;
    atString                  enumValue;
    atString                  sourceImageID;
    atString                  sourceSurfaceID;

    bool                      getBoolToken(atStringTokenizer *tokens);
    int                       getIntToken(atStringTokenizer *tokens);
    double                    getFloatToken(atStringTokenizer *tokens);

    void                      processSurface(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current);
    void                      processSampler2D(atXMLDocument *doc,
                                               atXMLDocumentNodePtr current);

public:

                              vsCOLLADAEffectParameter(
                                                 atString name,
                                                 vsCOLLADAParameterType type);
                              ~vsCOLLADAEffectParameter();

    virtual const char        *getClassName();

    virtual vsCOLLADAEffectParameter    *clone();

    void                      set(bool b1);
    void                      set(bool b1, bool b2);
    void                      set(bool b1, bool b2, bool b3);
    void                      set(bool b1, bool b2, bool b3, bool b4);
    void                      set(int i1);
    void                      set(int i1, int i2);
    void                      set(int i1, int i2, int i3);
    void                      set(int i1, int i2, int i3, int i4);
    void                      set(double f1);
    void                      set(double f1, double f2);
    void                      set(double f1, double f2, double f3);
    void                      set(double f1, double f2, double f3, double f4);
    void                      set(atVector vec);
    void                      set(atMatrix mat);
    void                      set(vsTextureAttribute *tex);
    void                      set(atString enumStr);
    void                      set(atXMLDocument *doc, 
                                  atXMLDocumentNodePtr valueNode);

    atString                  getName();
    vsCOLLADAParameterType    getType();

    bool                      getBool();
    bool                      getBool(int index);

    int                       getInt();
    int                       getInt(int index);

    double                    getFloat();
    double                    getFloat(int index);
    atVector                  getVector();
    atMatrix                  getMatrix();

    vsTextureAttribute        *getTexture();
    void                      setSourceImageID(atString sid);
    atString                  getSourceImageID();
    void                      setSourceSurfaceID(atString sid);
    atString                  getSourceSurfaceID();

    atString                  getEnum();
};

#endif
