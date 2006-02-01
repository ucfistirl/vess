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

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include "vsGLSLProgramAttribute.h++"

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Constructor, create a context for the shaders and initializes them to NULL
// ------------------------------------------------------------------------
vsGLSLProgramAttribute::vsGLSLProgramAttribute()
{
    // Create the pfShaderProgram
    performerProgram = new pfShaderProgram();
    performerProgram->ref();

    // Initialize the shader and uniform arrays
    memset(shaders, 0, sizeof(shaders));
    numShaders = 0;
    memset(uniforms, 0, sizeof(uniforms));
    numUniforms = 0;

    // Initialize the vertex attribute bindings
    attrBindingsChanged = false;
    memset(attrBindings, 0, sizeof(attrBindings));
}

// ------------------------------------------------------------------------
// Destructor, clear the parameter array and delete created objects.
// ------------------------------------------------------------------------
vsGLSLProgramAttribute::~vsGLSLProgramAttribute()
{
    // Remove all shaders and uniforms
    while (numShaders > 0)
        removeShader(shaders[0]);
    while (numUniforms > 0)
        removeUniform(uniforms[0]);

    // Delete the performer shader program
    performerProgram->unref();
    pfDelete(performerProgram);
}

// ------------------------------------------------------------------------
// Draw process traversal callback to handle changes to the vertex 
// attribute bindings, since Performer doesn't provide an API for this.
//
// NOTE:  The getGLHandle() function of pfShaderProgram does not work as
//        documented, and I was unable to get it to work any other way.
//        Because of this, there is no way to provide vertex attribute
//        binding support for GLSL programs under Performer.
// ------------------------------------------------------------------------
int vsGLSLProgramAttribute::travCallback(pfTraverser *trav, void *userData)
{
    vsGLSLProgramAttribute *instance;
    int i;
    GLhandleARB programHandle;

    // The whole getGLHandle() call with pfShaderPrograms is just plumb
    // broken.  Bail out before we do any damage to the running program.
    // I'm leaving the code below for when Performer gets its act together
    // in a future release.
    return PFTRAV_CONT;

    // Get the instance that is calling this function
    instance = (vsGLSLProgramAttribute *)userData;

    // Get the OpenGL handle for the GLSL program
    programHandle = instance->performerProgram->getGLHandle();

    // Check the program handle to see if it's valid.  If not, bail out 
    // before we start trying to bind attributes to a bogus program.
    #ifdef OPENGL_2_0
        if (!glIsProgram((GLuint)programHandle))
            return PFTRAV_CONT;
    #else
        if (!glIsProgramARB(programHandle))
            return PFTRAV_CONT;
    #endif

    // Re-bind all the attribute bindings
    for (i = 0; i < VS_GPROG_MAX_ATTR_BINDINGS; i++)
    {
        // See if this attribute is bound
        if (strlen(instance->attrBindings[i]) > 0)
        {
            // Call either the native OpenGL function (for OpenGL 2.0+) 
            // or the ARB_shanding_language function (for OpenGL 1.x).
            #ifdef OPENGL_2_0
                glBindAttribLocation((GLuint)programHandle, i, 
                    instance->attrBindings[i]);
            #else
                glBindAttribLocationARB(programHandle, i, 
                    instance->attrBindings[i]);
            #endif
        }
    }

    return PFTRAV_CONT;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches this attribute to the given node.  This involves setting up
// a traversal callback for the DRAW process.
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::attach(vsNode *theNode)
{
    pfNode *performerNode;

    // Do the normal state attribute attaching
    vsStateAttribute::attach(theNode);

    // Get the Performer node related to the given VESS node
    performerNode = theNode->getBaseLibraryObject();

    // Set up the DRAW process traversal function that will handle the
    // vertex attribute bindings
    performerNode->setTravFuncs(PFTRAV_DRAW, travCallback, NULL);
    performerNode->setTravData(PFTRAV_DRAW, this);
}

// ------------------------------------------------------------------------
// Internal function
// Detaches this attribute from the given node.  This involves clearing
// the DRAW callback we set up earlier.
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::detach(vsNode *theNode)
{
    pfNode *performerNode;

    // Get the Performer node related to the given VESS node
    performerNode = theNode->getBaseLibraryObject();

    // Clean up the DRAW process traversal function
    performerNode->setTravFuncs(PFTRAV_DRAW, NULL, NULL);
    performerNode->setTravData(PFTRAV_DRAW, NULL);

    // Do the normal state attribute detaching
    vsStateAttribute::detach(theNode);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::attachDuplicate(vsNode *theNode)
{
    vsGLSLProgramAttribute *newAttrib;
    int i;

    // Create a new vsShadingAttribute and copy the data from this
    // attribute to the new one
    newAttrib = new vsGLSLProgramAttribute();

    // Duplicate the data, this will not handle parameters though.
    for (i = 0; i < numShaders; i++)
        newAttrib->addShader(shaders[i]);
    for (i = 0; i < numUniforms; i++)
        newAttrib->addUniform(uniforms[i]);

    // Add the new attribute to the given node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Save the current shader state in our save list
    attrSaveList[attrSaveCount++] = gState->getGLSLProgram();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set the current shader state to this object
    gState->setGLSLProgram(this);

    // Lock the shader state if overriding is enabled
    if (overrideFlag)
        gState->lockGLSLProgram(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Unlock the shader state if overriding was enabled
    if (overrideFlag)
        gState->unlockGLSLProgram(this);

    // Reset the current shader state to its previous value
    gState->setGLSLProgram((vsGLSLProgramAttribute *)
        (attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::setState(pfGeoState *state)
{
    // Enable the shader programs on the geostate if they exist.
    state->setAttr(PFSTATE_SHADPROG, performerProgram);
    state->setMode(PFSTATE_ENSHADPROG, PF_ON);
}

// ------------------------------------------------------------------------
// Return true only if the given pointer matches this object.  There are
// too many possibilites for differences otherwise.
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
    numShaders++;

    // Add the shader to the Performer program
    performerProgram->addShader(shader->getBaseLibraryObject());
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
        performerProgram->removeShader(shader->getBaseLibraryObject());

        // Remove it from our array
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
    // Make sure the uniform is valid
    if (uniform == NULL)
    {
        printf("vsGLSLProgramAttribute::addUniform:  "
            "NULL uniform specified!\n");
        return;
    }
    
    // Add the uniform to our array
    uniforms[numUniforms] = uniform;
    numUniforms++;

    // Add the Performer program to the uniform object
    uniform->addParentProgram(performerProgram);
}

// ------------------------------------------------------------------------
// Removes a vsGLSLUniform from this Program
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::removeUniform(vsGLSLUniform *uniform)
{
    int i;

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
        // NOTE:  There isn't really any way to remove the uniform from 
        //        the Performer program.

        // Remove it from our array
        uniforms[i] = NULL;

        // Slide the remaining uniforms down into the removed uniform's place
        if (i < (numUniforms-1))
        {
            memmove(&uniforms[i], &uniforms[i+1], 
                (numUniforms-i-1) * sizeof(vsGLSLUniform *));
            uniforms[numUniforms-1] = NULL;
        }
        numUniforms--;

        // Remove the Performer program from the uniform
        uniform->removeParentProgram(performerProgram);
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
// Binds the given OpenGL vertex attribute list to the given variable name
// in the GLSL program 
//
// NOTE:  As of Performer 3.2.1, there is no support for vertex attribute
//        bindings, and it was also impossible to provide support via a 
//        draw callback (see the travCallback() function above).
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::bindVertexAttr(const char *name, 
                                            unsigned int index)
{
    int currentIndex;

    printf("vsGLSLProgramAttribute::bindVertexAttr:\n");
    printf("    Vertex attribute bindings are currently broken under "
        "Performer\n");

    // Look for the given name in the current list of attribute bindings
    currentIndex = 0;
    while ((currentIndex < VS_GPROG_MAX_ATTR_BINDINGS) &&
           (strcmp(attrBindings[currentIndex], name) != 0))
        currentIndex++;
    
    // See if the given attribute name is already bound
    if (currentIndex >= VS_GPROG_MAX_ATTR_BINDINGS)
    {
        // Store the binding in our attribute binding map
        strncpy(attrBindings[index], name, VS_GPROG_ATTR_NAME_LENGTH);
        attrBindings[index][VS_GPROG_ATTR_NAME_LENGTH-1] = 0;
    }
    else
    {
        // Change the existing binding in our attribute binding map
        memset(attrBindings[currentIndex], 0, 
            sizeof(attrBindings[currentIndex]));
        strncpy(attrBindings[index], name, VS_GPROG_ATTR_NAME_LENGTH);
        attrBindings[index][VS_GPROG_ATTR_NAME_LENGTH-1] = 0;
    }
}

// ------------------------------------------------------------------------
// Removes the named vertex attribute binding from the program
//
// NOTE:  As of Performer 3.2.1, there is no support for vertex attribute
//        bindings, and it was also impossible to provide support via a 
//        draw callback (see the travCallback() function above).
// ------------------------------------------------------------------------
void vsGLSLProgramAttribute::removeVertexAttrBinding(const char *name)
{
    int currentIndex;

    printf("vsGLSLProgramAttribute::bindVertexAttr:\n");
    printf("    Vertex attribute bindings are currently broken under "
        "Performer\n");

    // Look for the given name in the current list of attribute bindings
    currentIndex = 0;
    while ((currentIndex < VS_GPROG_MAX_ATTR_BINDINGS) &&
           (strcmp(attrBindings[currentIndex], name) != 0))
        currentIndex++;
    
    // See if we found it
    if (currentIndex < VS_GPROG_MAX_ATTR_BINDINGS)
    {
        // Remove the binding from our array
        memset(attrBindings[currentIndex], 0, 
            sizeof(attrBindings[currentIndex]));
    }
}
