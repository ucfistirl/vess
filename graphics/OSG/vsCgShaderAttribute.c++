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
//    VESS Module:  vsCgShaderAttribute.c++
//
//    Description:  Attribute that defines a Cg fragment and vertex shader
//                  to be applied in the subgraph it is attached.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCgShaderAttribute.h++"

// ------------------------------------------------------------------------
// Constructor, create a context for the shaders and initializes them to NULL
// ------------------------------------------------------------------------
vsCgShaderAttribute::vsCgShaderAttribute()
{
    cgContext = new osgNVCg::Context();
    cgVertexProgram = NULL;
    cgFragmentProgram = NULL;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
vsCgShaderAttribute::~vsCgShaderAttribute()
{
/* How to clean up osgNVCg? */
/* Cannot delete programs or contexts at the time of this writing. */
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;

    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsShadingAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet on the given node
    osgStateSet = getOSGStateSet(node);

    // Apply the osg::ShadeModel on the StateSet
    osgStateSet->setAttributeAndModes(cgContext, attrMode);
}

// ------------------------------------------------------------------------
// Set the context to the programs to the passed in context.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgContext(osgNVCg::Context *newContext)
{
    // If the programs excist, set them.
    if (cgVertexProgram)
        cgVertexProgram->setContext(newContext);
    if (cgFragmentProgram)
        cgFragmentProgram->setContext(newContext);

    // Cannot delete contexts at the time of this writing.
//    delete cgContext;
    cgContext = newContext;
}

// ------------------------------------------------------------------------
// Return the current Cg context for the programs.
// ------------------------------------------------------------------------
osgNVCg::Context *vsCgShaderAttribute::getCgContext()
{
    return cgContext;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsCgShaderAttribute::attach(vsNode *node)
{
    // Do normal vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set up the osg::StateSet on this node to use the osgNVCg::Context
    // we've created
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsCgShaderAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the node's StateSet
    osgStateSet = getOSGStateSet(node);

    // According to Marco Jez, who may define the value at some point.
    osgStateSet->setAttributeToInherit(0xa7b7);

    // Finish detaching the attribute
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void vsCgShaderAttribute::attachDuplicate(vsNode *theNode)
{
    vsCgShaderAttribute *newAttrib;

    // Create a new vsCgShadingAttribute and copy the data from this
    // attribute to the new one
    newAttrib = new vsCgShaderAttribute();

    // Duplicate the data, this will not handle parameters though.
    newAttrib->setCgVertexSourceFile(getCgVertexSourceFile());
    newAttrib->setCgVertexEntryPoint(getCgVertexEntryPoint());
    newAttrib->setCgVertexProfile(getCgVertexProfile());
    newAttrib->setCgFragmentSourceFile(getCgFragmentSourceFile());
    newAttrib->setCgFragmentEntryPoint(getCgFragmentEntryPoint());
    newAttrib->setCgFragmentProfile(getCgFragmentProfile());

    /* Copy the stuff somehow. */

    // Add the new attribute to the given node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Return false, no sure way to compare Shader Attributes.  They may be
// the same program but with different parameters.
// ------------------------------------------------------------------------
bool vsCgShaderAttribute::isEquivalent(vsAttribute *attribute)
{
    return false;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCgShaderAttribute::getClassName()
{
    return "vsCgShaderAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsCgShaderAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_CG_SHADER;
}

// ------------------------------------------------------------------------
// Return a vsCgVectorParameter object linked to the specified varibale name
// in the vertex shader source code.  The returned object can then be used
// to set the variable.
// ------------------------------------------------------------------------
vsCgVectorParameter *vsCgShaderAttribute::getVertexVectorParameter(char *name)
{
    // If the program has not been created, return NULL.
    if (cgVertexProgram == NULL)
        return (NULL);
    else
        return (new vsCgVectorParameter(cgVertexProgram, name));
}

// ------------------------------------------------------------------------
// Return a vsCgVectorParameter object linked to the specified varibale name
// in the fragment shader source code.  The returned object can then be used
// to set the variable.
// ------------------------------------------------------------------------
vsCgVectorParameter *vsCgShaderAttribute::getFragmentVectorParameter(char *name)
{
    // If the program has not been created, return NULL.
    if (cgFragmentProgram == NULL)
        return (NULL);
    else
        return (new vsCgVectorParameter(cgFragmentProgram, name));
}

// ------------------------------------------------------------------------
// Return a vsCgMatrixParameter object linked to the specified varibale name
// in the vertex shader source code.  The returned object can then be used
// to set the variable.
// ------------------------------------------------------------------------
vsCgMatrixParameter *vsCgShaderAttribute::getVertexMatrixParameter(char *name)
{
    // If the program has not been created, return NULL.
    if (cgVertexProgram == NULL)
        return (NULL);
    else
        return (new vsCgMatrixParameter(cgVertexProgram, name));
}

// ------------------------------------------------------------------------
// Return a vsCgMatrixParameter object linked to the specified varibale name
// in the fragment shader source code.  The returned object can then be used
// to set the variable.
// ------------------------------------------------------------------------
vsCgMatrixParameter *vsCgShaderAttribute::getFragmentMatrixParameter(char *name)
{
    // If the program has not been created, return NULL.
    if (cgFragmentProgram == NULL)
        return (NULL);
    else
        return (new vsCgMatrixParameter(cgFragmentProgram, name));
}

// ------------------------------------------------------------------------
// Return a vsCgStateMatrixParameter object linked to the specified varibale
// name in the vertex shader source code.  The returned object can then be
// used to set the variable.
// ------------------------------------------------------------------------
vsCgStateMatrixParameter *vsCgShaderAttribute::getVertexStateMatrixParameter(
                                                                     char *name)
{
    // If the program has not been created, return NULL.
    if (cgVertexProgram == NULL)
        return (NULL);
    else
        return (new vsCgStateMatrixParameter(cgVertexProgram, name));
}
                                                                                
// ------------------------------------------------------------------------
// Return a vsCgStateMatrixParameter object linked to the specified varibale
// name in the fragment shader source code.  The returned object can then be
// used to set the variable.
// ------------------------------------------------------------------------
vsCgStateMatrixParameter *vsCgShaderAttribute::getFragmentStateMatrixParameter(
                                                                     char *name)
{
    // If the program has not been created, return NULL.
    if (cgFragmentProgram == NULL)
        return (NULL);
    else
        return (new vsCgStateMatrixParameter(cgFragmentProgram, name));
}

// ------------------------------------------------------------------------
// Return a vsCgTextureParameter object linked to the specified varibale
// name in the vertex shader source code.  The returned object can then be
// used to set the variable.
// ------------------------------------------------------------------------
vsCgTextureParameter *vsCgShaderAttribute::getVertexTextureParameter(char *name)
{
    // If the program has not been created, return NULL.
    if (cgVertexProgram == NULL)
        return (NULL);
    else
        return (new vsCgTextureParameter(cgVertexProgram, name));
}

// ------------------------------------------------------------------------
// Return a vsCgTextureParameter object linked to the specified varibale
// name in the fragment shader source code.  The returned object can then be
// used to set the variable.
// ------------------------------------------------------------------------
vsCgTextureParameter *vsCgShaderAttribute::getFragmentTextureParameter(char
                                                                       *name)
{
    // If the program has not been created, return NULL.
    if (cgFragmentProgram == NULL)
        return (NULL);
    else
        return (new vsCgTextureParameter(cgFragmentProgram, name));
}

// ------------------------------------------------------------------------
// Set the Cg source file to use for the vertex program.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgVertexSourceFile(char *filename)
{
    // If the filename is a NULL pointer, do nothing.
    if (filename == NULL)
        return;

    // Create the program if it has not been created.
    if (cgVertexProgram == NULL)
        cgVertexProgram = new osgNVCg::Program(cgContext);

    cgVertexProgram->setFileName(filename);
}

// ------------------------------------------------------------------------
// Set the Cg source file to use for the fragment program.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgFragmentSourceFile(char *filename)
{
    // If the filename is a NULL pointer, do nothing.
    if (filename == NULL)
        return;

    // Create the program if it has not been created.
    if (cgFragmentProgram == NULL)
        cgFragmentProgram = new osgNVCg::Program(cgContext);

    cgFragmentProgram->setFileName(filename);
}

// ------------------------------------------------------------------------
// Return the Cg source file used for the vertex program.
// ------------------------------------------------------------------------
char *vsCgShaderAttribute::getCgVertexSourceFile()
{
    if (cgVertexProgram == NULL)
        return NULL;
    else
        return (char *) cgVertexProgram->getFileName().c_str();
}

// ------------------------------------------------------------------------
// Return the Cg source file used for the fragment program.
// ------------------------------------------------------------------------
char *vsCgShaderAttribute::getCgFragmentSourceFile()
{
    if (cgFragmentProgram == NULL)
        return NULL;
    else
        return (char *) cgFragmentProgram->getFileName().c_str();
}

// ------------------------------------------------------------------------
// Set the entry point function to use for the vertex program.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgVertexEntryPoint(char *entry)
{
    // If the entry pointer is NULL, do nothing.
    if (entry == NULL)
        return;

    // Create the program if it has not been created.
    if (cgVertexProgram == NULL)
        cgVertexProgram = new osgNVCg::Program(cgContext);

    cgVertexProgram->setEntryPoint(entry);
}

// ------------------------------------------------------------------------
// Set the entry point function to use for the fragment program.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgFragmentEntryPoint(char *entry)
{
    // If the entry pointer is NULL, do nothing.
    if (entry == NULL)
        return;

    // Create the program if it has not been created.
    if (cgFragmentProgram == NULL)
        cgFragmentProgram = new osgNVCg::Program(cgContext);

    cgFragmentProgram->setEntryPoint(entry);
}

// ------------------------------------------------------------------------
// Return what we have set as the entry point function for the vertex program.
// ------------------------------------------------------------------------
char *vsCgShaderAttribute::getCgVertexEntryPoint()
{
    // If the program has not been created, return NULL.
    if (cgVertexProgram == NULL)
        return NULL;
    else
        return (char *) cgVertexProgram->getEntryPoint().c_str();
}

// ------------------------------------------------------------------------
// Return what we have set as the entry point function for the fragment program.
// ------------------------------------------------------------------------
char *vsCgShaderAttribute::getCgFragmentEntryPoint()
{
    // If the program has not been created, return NULL.
    if (cgFragmentProgram == NULL)
        return NULL;
    else
        return (char *) cgFragmentProgram->getEntryPoint().c_str();
}

// ------------------------------------------------------------------------
// Set the profile to compile the vertex program with.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgVertexProfile(vsShaderProfile profile)
{
    // If the profile is unknown, do nothing.
    if (profile == VS_SHADER_UNKNOWN)
        return;

    // Create the program if it has not been created.
    if (cgVertexProgram == NULL)
        cgVertexProgram = new osgNVCg::Program(cgContext);

    cgVertexProgram->setProfile((osgNVCg::Program::Profile_type) profile);
}

// ------------------------------------------------------------------------
// Set the profile to compile the fragment program with.
// ------------------------------------------------------------------------
void vsCgShaderAttribute::setCgFragmentProfile(vsShaderProfile profile)
{
    // If the profile is unknown, do nothing.
    if (profile == VS_SHADER_UNKNOWN)
        return;

    // Create the program if it has not been created.
    if (cgFragmentProgram == NULL)
        cgFragmentProgram = new osgNVCg::Program(cgContext);

    cgFragmentProgram->setProfile((osgNVCg::Program::Profile_type) profile);
}

// ------------------------------------------------------------------------
// Return what we have set as the profile for the vertex program.
// ------------------------------------------------------------------------
vsShaderProfile vsCgShaderAttribute::getCgVertexProfile()
{
    // If the program has not been created, return an unknown profile.
    if (cgVertexProgram == NULL)
        return VS_SHADER_UNKNOWN;
    else
        return (vsShaderProfile) cgVertexProgram->getProfile();
}

// ------------------------------------------------------------------------
// Return what we have set as the profile for the fragment program.
// ------------------------------------------------------------------------
vsShaderProfile vsCgShaderAttribute::getCgFragmentProfile()
{
    // If the program has not been created, return an unknown profile.
    if (cgFragmentProgram == NULL)
        return VS_SHADER_UNKNOWN;
    else
        return (vsShaderProfile) cgFragmentProgram->getProfile();
}
