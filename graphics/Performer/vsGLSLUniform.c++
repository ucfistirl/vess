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
//    VESS Module:  vsGLSLUniform.c++
//
//    Description:  Encapsulates an OpenSceneGraph Uniform object (which
//                  encapsulates an OpenGL Shading Language uniform 
//                  attribute)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsGLSLUniform.h++"

// ------------------------------------------------------------------------
// Constructs a vsGLSLUniform with a the given type
// ------------------------------------------------------------------------
vsGLSLUniform::vsGLSLUniform(const char *name, vsGLSLUniformType type)
{
    int i;

    // Set the type
    uniformType = type;

    // Copy the name
    strncpy(uniformName, name, sizeof(uniformName));
    uniformName[sizeof(uniformName)-1] = 0;

    // Clear the array of attached parents
    memset(parentPrograms, 0, sizeof(parentPrograms));
    numParentPrograms = 0;

    // Initialize the map of attached parents to the corresponding uniform
    // index
    for (i = 0; i < VS_GLSL_UNIFORM_MAX_PARENTS; i++)
        parentUniformIndex[i] = -1;

    // Create a pfMemory object that holds the uniform's data.  This allows
    // pfShaderPrograms to share the data held by this uniform.  (It's the 
    // Performer way).
    uniformData = (vsGLSLUniformData *)
        pfMemory::malloc(sizeof(vsGLSLUniformData), pfGetSharedArena());
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsGLSLUniform::~vsGLSLUniform()
{
}

// ------------------------------------------------------------------------
// Adds this uniform's data to the given pfShaderProgram, or updates the
// data of the uniform already added to it.
// ------------------------------------------------------------------------
void vsGLSLUniform::updateParentPrograms()
{
    int i;

    // Make sure this uniform is of a valid type, otherwise we don't update
    // anything
    if (uniformType == VS_UNIFORM_UNDEFINED)
        return;

    // Iterate through our attached Performer shader programs
    for (i = 0; i < numParentPrograms; i++)
    {
        // Check the index we have, and make sure it's valid
        if (parentUniformIndex[i] >= 0)
        {
            // Found it, so just update the uniform data
            parentPrograms[i]->setUniform(parentUniformIndex[i], uniformData);
        }
        else
        {
            // The uniform hasn't been added yet, so add it to the
            // shader program with the current data
            parentUniformIndex[i] = parentPrograms[i]->
                addUniform(uniformName, uniformType, 1, uniformData);
        }
    }
}

// ------------------------------------------------------------------------
// Adds the given pfShaderProgram to our list of parent programs
// ------------------------------------------------------------------------
void vsGLSLUniform::addParentProgram(pfShaderProgram *parent)
{
    // Add the shader program to our list, and add the uniform to the 
    // Performer program
    parentPrograms[numParentPrograms] = parent;
    parentUniformIndex[numParentPrograms] = 
        parent->addUniform(uniformName, uniformType, 1, uniformData);
    
    // Update the number of attached programs
    numParentPrograms++;
}

// ------------------------------------------------------------------------
// Remove the given pfShaderProgram from our list of parent programs
// ------------------------------------------------------------------------
void vsGLSLUniform::removeParentProgram(pfShaderProgram *parent)
{
    int i;

    // Find the given program in the array
    i = 0;
    while (parent != parentPrograms[i])
        i++;

    // See if we have a match
    if (i < numParentPrograms)
    {
        // Remove the program
        parentPrograms[i] = NULL;
        parentUniformIndex[i] = -1;

        // Slide the remaining programs into its place
        if (i < (numParentPrograms - 1))
        {
            memmove(&parentPrograms[i], &parentPrograms[i+1],
                (numParentPrograms-i-1) * sizeof(pfShaderProgram *));
            parentPrograms[numParentPrograms-1] = NULL;
            parentUniformIndex[numParentPrograms-1] = -1;
        }

        // Decrement the number of attached programs
        numParentPrograms--;
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsGLSLUniform::getClassName()
{
    return "vsGLSLUniform";
}

// ------------------------------------------------------------------------
// Return the data type of this Uniform
// ------------------------------------------------------------------------
const char *vsGLSLUniform::getName()
{
    return uniformName;
}

// ------------------------------------------------------------------------
// Return the data type of this Uniform
// ------------------------------------------------------------------------
vsGLSLUniformType vsGLSLUniform::getType()
{
    return uniformType;
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_BOOL)
    {
        // Update the data values
        uniformData->boolVecData[0] = b1;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1, bool b2)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_BOOL_VEC2)
    {
        // Update the data values
        uniformData->boolVecData[0] = b1;
        uniformData->boolVecData[1] = b2;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1, bool b2, bool b3)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_BOOL_VEC3)
    {
        // Update the data values
        uniformData->boolVecData[0] = b1;
        uniformData->boolVecData[1] = b2;
        uniformData->boolVecData[2] = b3;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1, bool b2, bool b3, bool b4)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_BOOL_VEC4)
    {
        // Update the data values
        uniformData->boolVecData[0] = b1;
        uniformData->boolVecData[1] = b2;
        uniformData->boolVecData[2] = b3;
        uniformData->boolVecData[3] = b4;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1)
{
    // Make sure the data is valid for this type of uniform.  A single 
    // integer can either be integer data, or a texture sampler.
    if (uniformType == VS_UNIFORM_INT)
    {
        // Update the data values
        uniformData->intVecData[0] = i1;

        // Update the attached programs
        updateParentPrograms();
    }
    else if ((uniformType == VS_UNIFORM_SAMPLER_1D) ||
             (uniformType == VS_UNIFORM_SAMPLER_2D) ||
             (uniformType == VS_UNIFORM_SAMPLER_3D) ||
             (uniformType == VS_UNIFORM_SAMPLER_1D_SHADOW) ||
             (uniformType == VS_UNIFORM_SAMPLER_2D_SHADOW))
    {
        // Update the data values
        uniformData->samplerData = i1;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1, int i2)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_INT_VEC2)
    {
        // Update the data values
        uniformData->intVecData[0] = i1;
        uniformData->intVecData[1] = i2;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1, int i2, int i3)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_INT_VEC3)
    {
        // Update the data values
        uniformData->intVecData[0] = i1;
        uniformData->intVecData[1] = i2;
        uniformData->intVecData[2] = i3;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1, int i2, int i3, int i4)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_INT_VEC4)
    {
        // Update the data values
        uniformData->intVecData[0] = i1;
        uniformData->intVecData[1] = i2;
        uniformData->intVecData[2] = i3;
        uniformData->intVecData[3] = i4;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(float floatVal)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_FLOAT)
    {
        // Update the data values
        uniformData->floatData[0] = floatVal;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(double doubleVal)
{
    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if (uniformType == VS_UNIFORM_FLOAT)
    {
        // Update the data values
        uniformData->floatData[0] = (float)doubleVal;

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(vsVector vec)
{
    int i;

    // Make sure the data is valid for this type of uniform, then store it
    // in the uniform's data block appropriately.
    if ((uniformType == VS_UNIFORM_FLOAT) ||
        (uniformType == VS_UNIFORM_FLOAT_VEC2) ||
        (uniformType == VS_UNIFORM_FLOAT_VEC3) ||
        (uniformType == VS_UNIFORM_FLOAT_VEC4))
    {
        // Update the data values
        for (i = 0; i < vec.getSize(); i++)
            uniformData->floatData[i] = vec[i];

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(vsMatrix mat)
{
    int i, j;

    // Make sure the data is valid for this type of uniform, and if so,
    // store the matrix as a float array, based on its size.  We also need
    // to transpose the VESS matrix into an OpenGL compatible one along the
    // way.
    if (uniformType == VS_UNIFORM_FLOAT_MAT2)
    {
        // Store the 2x2 matrix as an array of four floats.
        for (i = 0; i < 2; i++)
            for (j = 0; j < 2; j++)
                uniformData->floatData[i*2 + j] = mat[j][i];

        // Update the attached programs
        updateParentPrograms();
    }
    else if (uniformType == VS_UNIFORM_FLOAT_MAT3)
    {
        // Store the 3x3 matrix as an array of nine floats
        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
                uniformData->floatData[i*3 + j] = mat[j][i];

        // Update the attached programs
        updateParentPrograms();
    }
    else if (uniformType == VS_UNIFORM_FLOAT_MAT4)
    {
        // Store the 4x4 matrix as an array of sixteen floats
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                uniformData->floatData[i*4 + j] = mat[j][i];

        // Update the attached programs
        updateParentPrograms();
    }
    else
        printf("vsGLSLUniform::set:  Invalid data for uniform type\n");
}
