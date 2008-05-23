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
//    VESS Module:  vsGLSLProgramAttribute.c++
//
//    Description:  Attribute to handle OpenGL Shading Language (GLSL)
//                  shader programs.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsGLSLProgramAttribute.h++"
#include <string>

// ------------------------------------------------------------------------
// Constructor, create a context for the shaders and initializes them to NULL
// ------------------------------------------------------------------------
vsGLSLProgramAttribute::vsGLSLProgramAttribute()
                      : attachedNodes(5, 5)
{
    // Create the osg::Program
    osgProgram = new osg::Program();
    osgProgram->ref();

    // Initialize the shader and uniform arrays
    memset(shaders, 0, sizeof(shaders));
    numShaders = 0;
    memset(uniforms, 0, sizeof(uniforms));
    numUniforms = 0;
    memset(bindingName, 0, sizeof(bindingName));
    memset(bindingLocation, 0, sizeof(bindingLocation));
    numVertexAttrBindings = 0;
}

// ------------------------------------------------------------------------
// Destructor, clear the parameter array and unreference created objects.
// ------------------------------------------------------------------------
vsGLSLProgramAttribute::~vsGLSLProgramAttribute()
{
    // Remove all shaders and uniforms
    while (numShaders > 0)
        removeShader(shaders[0]);
    while (numUniforms > 0)
        removeUniform(uniforms[0]);
    while (numVertexAttrBindings > 0)
        removeVertexAttrBinding(bindingName[0]);

    // Unreference the OSG Program object
    osgProgram->unref();
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    int i;

    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsShadingAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet on the given node
    osgStateSet = getOSGStateSet(node);

    // Apply the osg::Program on the stateset
    osgStateSet->setAttributeAndModes(osgProgram, attrMode);

    // Apply all our Uniforms as well
    for (i = 0; i < numUniforms; i++)
        osgStateSet->addUniform(uniforms[i]->getBaseLibraryObject());
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::attach(vsNode *node)
{
    // Keep track of the node we're attaching to
    attachedNodes[attachedCount] = node;

    // Do normal vsStateAttribute attaching (this increments the attached
    // count)
    vsStateAttribute::attach(node);

    // Set up the osg::StateSet on this node to use the osg::Program
    // we've created
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    int i;
    int attachedIndex;

    // Get the node's StateSet
    osgStateSet = getOSGStateSet(node);

    // Reset the Program mode to inherit
    osgStateSet->setAttributeAndModes(osgProgram,
            osg::StateAttribute::INHERIT);

    // Remove all Uniforms
    for (i = 0; i < numUniforms; i++)
        osgStateSet->removeUniform(uniforms[i]->getBaseLibraryObject());

    // Locate the node in our list of attached nodes
    attachedIndex = 0;
    while ((attachedIndex < attachedCount) && 
           ((vsNode *)(attachedNodes[attachedIndex]) != node))
        attachedIndex++;

    // If we found the node, remove it from the list
    if (attachedIndex < attachedCount)
    {
        // Remove the attached node from our list
        attachedNodes[attachedIndex] = NULL;

        // If the attached node isn't at the end of the list, slide the rest
        // into its place
        if (attachedIndex < (attachedCount-1))
        {
            memmove(&attachedNodes[attachedIndex], 
                &attachedNodes[attachedIndex+1], 
                (attachedCount-attachedIndex-1) * sizeof(vsNode *));
            attachedNodes[attachedCount-1] = NULL;
        }
    }

    // Finish detaching the attribute (this will decrement the attachedCount)
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::attachDuplicate(vsNode *theNode)
{
    // Add the a clone of this attribute to the given node
    theNode->addAttribute(this->clone());
}

// ------------------------------------------------------------------------
// Only return true if the given attribute is exactly the same as this 
// attribute (pointers are equivalent).  There are too many possibilities
// for the programs to be different otherwise.
// ------------------------------------------------------------------------
bool vsGLSLProgramAttribute::isEquivalent(vsAttribute *attribute)
{
    if (attribute == this)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsGLSLProgramAttribute::getClassName()
{
    return "vsGLSLProgramAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsGLSLProgramAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_GLSL_PROGRAM;
}

// ------------------------------------------------------------------------
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsGLSLProgramAttribute::clone()
{
    vsGLSLProgramAttribute *newAttrib;
    int i;

    // Create a new vsShadingAttribute and copy the data from this
    // attribute to the new one
    newAttrib = new vsGLSLProgramAttribute();

    // Attach our shaders and uniforms to the new attribute (the cloned
    // program will reference them, so they won't disappear until
    // we're both done with them)
    for (i = 0; i < numShaders; i++)
        newAttrib->addShader(shaders[i]);
    for (i = 0; i < numUniforms; i++)
        newAttrib->addUniform(uniforms[i]);

    // Duplicate the vertex attribute bindings
    for (i = 0; i < numVertexAttrBindings; i++)
        newAttrib->bindVertexAttr(bindingName[i], bindingLocation[i]);

    // Return the clone
    return newAttrib;
}

// ------------------------------------------------------------------------
// Adds a vsGLSLShader to this Program
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::addShader(vsGLSLShader *shader)
{
    // Make sure the shader is valid
    if (shader == NULL)
    {
        printf("vsGLSLProgramAttribute::addShader:  NULL shader specified!\n");
        return;
    }
    
    // Add the shader to our array
    shaders[numShaders] = shader;
    shaders[numShaders]->ref();
    numShaders++;

    // Add the shader to the OSG Program
    osgProgram->addShader(shader->getBaseLibraryObject());
}

// ------------------------------------------------------------------------
// Removes a vsGLSLShader from this Program
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::removeShader(vsGLSLShader *shader)
{
    int i;

    // Make sure the shader is valid
    if (shader == NULL)
    {
        printf("vsGLSLProgramAttribute::removeShader:  "
            "NULL shader specified!\n");
        return;
    }
    
    // Locate the shader in our array
    i = 0;
    while ((i < numShaders) && (shaders[i] != shader))
        i++;

    // See if we found the shader
    if (i >= numShaders)
    {
        printf("vsGLSLProgramAttribute::removeShader:\n");
        printf("    Shader is not attached to this program\n");
    }
    else
    {
        // Remove the shader from the OSG Program
        osgProgram->removeShader(shader->getBaseLibraryObject());

        // Remove it from our array
        vsObject::unrefDelete(shaders[i]);
        shaders[i] = NULL;

        // Slide the remaining shaders down into the removed shader's place
        if (i < (numShaders-1))
        {
            memmove(&shaders[i], &shaders[i+1], 
                (numShaders-i-1) * sizeof(vsGLSLShader *));
            shaders[numShaders-1] = NULL;
        }
        numShaders--;
    }
}

// ------------------------------------------------------------------------
// Return the number of shaders attached to this program
// ------------------------------------------------------------------------
int vsGLSLProgramAttribute::getNumShaders()
{
    return numShaders;
}

// ------------------------------------------------------------------------
// Return the shader with the specified index
// ------------------------------------------------------------------------
vsGLSLShader *vsGLSLProgramAttribute::getShader(int index)
{
    if ((index < 0) || (index >= numShaders))
    {
        printf("vsGLSLProgramAttribute::getShader:  Index out of bounds\n");
        return NULL;
    }

    return shaders[index];
}

// ------------------------------------------------------------------------
// Adds a vsGLSLUniform to this Program
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::addUniform(vsGLSLUniform *uniform)
{
    int i;

    // Make sure the uniform is valid
    if (uniform == NULL)
    {
        printf("vsGLSLProgramAttribute::addUniform:  "
            "NULL uniform specified!\n");
        return;
    }
    
    // Add the uniform to our array
    uniforms[numUniforms] = uniform;
    uniforms[numUniforms]->ref();
    numUniforms++;

    // Add the uniform to the OSG StateSet, if the program is attached to
    // something
    if (attachedCount > 0)
        for (i = 0; i < attachedCount; i++)
            getOSGStateSet((vsNode *)(attachedNodes[i]))->
                addUniform(uniform->getBaseLibraryObject());
}

// ------------------------------------------------------------------------
// Removes a vsGLSLUniform from this Program
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::removeUniform(vsGLSLUniform *uniform)
{
    int i, j;

    // Make sure the uniform is valid
    if (uniform == NULL)
    {
        printf("vsGLSLProgramAttribute::removeUniform:  "
            "NULL uniform specified!\n");
        return;
    }
    
    // Locate the uniform in our array
    i = 0;
    while ((i < numUniforms) && (uniforms[i] != uniform))
        i++;

    // See if we found the uniform
    if (i >= numUniforms)
    {
        printf("vsGLSLProgramAttribute::removeUniform:\n");
        printf("    Uniform is not attached to this program\n");
    }
    else
    {
        // Remove the uniform from the OSG StateSet, if the program is
        // attached to something
        if (attachedCount > 0)
        {
            for (j = 0; j < attachedCount; j++)
                getOSGStateSet((vsNode *)(attachedNodes[j]))->
                    removeUniform(uniform->getBaseLibraryObject());
        }

        // Remove it from our array
        vsObject::unrefDelete(uniforms[i]);
        uniforms[i] = NULL;

        // Slide the remaining uniforms down into the removed uniform's place
        if (i < (numUniforms-1))
        {
            memmove(&uniforms[i], &uniforms[i+1], 
                (numUniforms-i-1) * sizeof(vsGLSLUniform *));
            uniforms[numUniforms-1] = NULL;
        }
        numUniforms--;
    }
}

// ------------------------------------------------------------------------
// Return the number of uniforms attached to this program
// ------------------------------------------------------------------------
int vsGLSLProgramAttribute::getNumUniforms()
{
    return numUniforms;
}

// ------------------------------------------------------------------------
// Return the uniform with the specified index
// ------------------------------------------------------------------------
vsGLSLUniform *vsGLSLProgramAttribute::getUniform(int index)
{
    if ((index < 0) || (index >= numUniforms))
    {
        printf("vsGLSLProgramAttribute::getUniform:  Index out of bounds\n");
        return NULL;
    }

    return uniforms[index];
}

// ------------------------------------------------------------------------
// Return the uniform with the specified name
// ------------------------------------------------------------------------
vsGLSLUniform *vsGLSLProgramAttribute::getUniform(const char *name)
{
    int index;

    // Search the uniforms array for the specified uniform
    index = 0;
    while ((index < numUniforms) &&
           (strcmp(uniforms[index]->getName(), name) != 0))
        index++;

    // Return the uniform if we found it, or NULL if not
    if (index < numUniforms)
        return uniforms[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Binds the given OpenGL vertex attribute list to the given variable name
// in the GLSL program 
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::bindVertexAttr(const char *name, 
                                            unsigned int loc)
{
    // Store the name and location
    strcpy(bindingName[numVertexAttrBindings], name);
    bindingLocation[numVertexAttrBindings] = loc;
    numVertexAttrBindings++;
    
    // Then pass this call along to the OSG object
    osgProgram->addBindAttribLocation(std::string(name), (GLuint)loc);
}

// ------------------------------------------------------------------------
// Removes the named vertex attribute binding from the program
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::removeVertexAttrBinding(const char *name)
{
    int index;

    // Search the binding name array for the specified name
    index = 0;
    while ((index < numUniforms) &&
           (strcmp(bindingName[index], name) != 0))
        index++;

    // See if we found the binding
    if (index >= numVertexAttrBindings)
    {
        printf("vsGLSLProgramAttribute::removeVertexAttrBinding:\n");
        printf("    Binding %s not found in this program\n", name);
    }
    else
    {
        // Then pass this call along to the OSG object
        osgProgram->removeBindAttribLocation(std::string(name));

        // Slide the remaining bindings down into the removed binding's place
        if (index < (numVertexAttrBindings-1))
        {
            memmove(&bindingName[index], &bindingName[index+1], 
                (numVertexAttrBindings-index-1) * sizeof(bindingName[index]));
            memmove(&bindingLocation[index], &bindingLocation[index+1], 
                (numVertexAttrBindings-index-1) * 
                sizeof(bindingLocation[index]));
        }

        // Decrement the binding count
        numVertexAttrBindings--;
    }
}

// ------------------------------------------------------------------------
// Return the number of vertex attributes bound to the program
// ------------------------------------------------------------------------
int vsGLSLProgramAttribute::getNumVertexAttrBindings()
{
    return numVertexAttrBindings;
}

// ------------------------------------------------------------------------
// Return the name and attribute location of the given vertex attribute
// binding
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::getVertexAttrBinding(int index, char **name,
                                                  unsigned int *loc)
{
    // Validate the index
    if ((index < 0) || (index >= numVertexAttrBindings))
    {
        printf("vsGLSLProgramAttribute::getVertexAttrBinding:  "
            "Index out of bounds\n");
        return;
    }

    // Return the name and/or location, provided the corresponding pointer
    // is valid
    if (name != NULL)
        *name = bindingName[index];
    if (loc != NULL)
        *loc = bindingLocation[index];
}
