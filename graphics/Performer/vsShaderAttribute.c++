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
//    VESS Module:  vsShaderAttribute.c++
//
//    Description:  Attribute to handle standard OpenGL ARB_vertex_program
//                  and ARB_fragment_program shaders.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsShaderAttribute.h++"

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Constructor, create a context for the shaders and initializes them to NULL
// ------------------------------------------------------------------------
vsShaderAttribute::vsShaderAttribute()
{
    // Initialize all pointers to NULL and the parameter counts to 0.
    vertexProgram = NULL;
    vertexParameters = NULL;
    vertexProgramFile = NULL;
    vertexProgramSource = NULL;
    vertexParameterCount = 0;

    fragmentProgram = NULL;
    fragmentParameters = NULL;
    fragmentProgramFile = NULL;
    fragmentProgramSource = NULL;
    fragmentParameterCount = 0;

    // Create the arrays that will store a vector of parameter data.
    vertexParameterArray = new vsGrowableArray(96, 16);
    fragmentParameterArray = new vsGrowableArray(96, 16);
}

// ------------------------------------------------------------------------
// Destructor, clear the parameter array and delete created objects.
// ------------------------------------------------------------------------
vsShaderAttribute::~vsShaderAttribute()
{
    int length;
    float *data;

    // If there are any filename and source strings, free them.
    if (vertexProgramFile)
        free(vertexProgramFile);
    if (vertexProgramSource)
        free(vertexProgramSource);
    if (fragmentProgramFile)
        free(fragmentProgramFile);
    if (fragmentProgramSource)
        free(fragmentProgramSource);

    // Delete any vectors we have in the vertex parameter array.
    length = vertexParameterArray->getSize();
    for (length--; length > -1; length--)
    {
        data = (float *) vertexParameterArray->getData(length);
        if (data)
            pfFree(data);
    }

    // Delete any vectors we have in the fragment parameter array.
    length = fragmentParameterArray->getSize();
    for (length--; length > -1; length--)
    {
        data = (float *) fragmentParameterArray->getData(length);
        if (data)
            pfFree(data);
    }

    // Delete the arrays.
    delete vertexParameterArray;
    delete fragmentParameterArray;

    // Delete the programs and parameters if they exist.
    if (vertexProgram)
        delete vertexProgram;
    if (vertexParameters)
        delete vertexParameters;
    if (fragmentProgram)
        delete fragmentProgram;
    if (fragmentParameters)
        delete fragmentParameters;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsShaderAttribute::attachDuplicate(vsNode *theNode)
{
    vsShaderAttribute *newAttrib;

    // Create a new vsShadingAttribute and copy the data from this
    // attribute to the new one
    newAttrib = new vsShaderAttribute();

    // Duplicate the data, this will not handle parameters though.
    newAttrib->setVertexSource(getVertexSource());
    newAttrib->setFragmentSource(getFragmentSource());

    /* Copy the parameters somehow? */

    // Add the new attribute to the given node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsShaderAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Save the current shader state in our save list
    attrSaveList[attrSaveCount++] = gState->getShader();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsShaderAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set the current shader state to this object
    gState->setShader(this);

    // Lock the shader state if overriding is enabled
    if (overrideFlag)
        gState->lockShader(this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsShaderAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Unlock the shader state if overriding was enabled
    if (overrideFlag)
        gState->unlockShader(this);

    // Reset the current shader state to its previous value
    gState->setShader((vsShaderAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsShaderAttribute::setState(pfGeoState *state)
{
    // Enable the shader programs on the geostate if they exist.
    if (vertexProgram)
    {
        state->setAttr(PFSTATE_VTXPROG, vertexProgram);
        state->setMode(PFSTATE_ENVTXPROG, PF_ON);

        // Set the parameters if we have any to set.
        if (vertexParameterCount)
            state->setMultiAttr(PFSTATE_GPROGPARMS, PFGP_VERTEX_LOCAL,
                vertexParameters);
    }

    if (fragmentProgram)
    {
        state->setAttr(PFSTATE_FRAGPROG, fragmentProgram);
        state->setMode(PFSTATE_ENFRAGPROG, PF_ON);

        // Set the parameters if we have any to set.
        if (fragmentParameterCount)
            state->setMultiAttr(PFSTATE_GPROGPARMS, PFGP_FRAGMENT_LOCAL,
                fragmentParameters);
    }
}

// ------------------------------------------------------------------------
// Return false, no sure way to compare Shader Attributes.  They may be
// the same program but with different parameters.
// ------------------------------------------------------------------------
bool vsShaderAttribute::isEquivalent(vsAttribute *attribute)
{
    return false;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsShaderAttribute::getClassName()
{
    return "vsShaderAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsShaderAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SHADER;
}

// ------------------------------------------------------------------------
// Set the ARBvp1.0 assembly source file to use for the vertex program.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexSourceFile(char *filename)
{
    long length;
    FILE *sourceFile;

    // If the filename is a NULL pointer, do nothing.
    if (filename == NULL)
    {
        printf("vsShaderAttribute::setVertexSourceFile: Error: "
            "File name is NULL!\n");
        return;
    }

    // Attempt to open the file.
    sourceFile = fopen(filename, "rb");

    // If there was an error opening the file, print and return.
    if (sourceFile == NULL)
    {
        printf("vsShaderAttribute::setVertexSourceFile: Error: "
            "Cannot open source file: %s\n", vertexProgramFile);
        return;
    }

    // Copy the filename string into a local string.
    length = strlen(filename);
    vertexProgramFile = (char *) malloc((length + 1));
    strncpy(vertexProgramFile, filename, (length + 1));

    // Go to the end of the file and find out how large it is.
    fseek(sourceFile, 0, SEEK_END);
    length = ftell(sourceFile);

    // Go back to the beginning of the file.
    fseek(sourceFile, 0, SEEK_SET);

    // Allocate the space for the assembly code.
    vertexProgramSource = (char *) malloc((length + 1));

    // Read the entire content of the file into the allocated string.
    fread(vertexProgramSource, 1, length, sourceFile);

    // Insure it is NULL terminated.
    vertexProgramSource[length] = '\0';

    // Close the file.
    fclose(sourceFile);

    // Create the program if it has not been created.
    if (vertexProgram == NULL)
    {
        vertexProgram = new pfVertexProgram();
        vertexParameters = new pfGProgramParms(PFGP_VERTEX_LOCAL);
    }

    // Set the source code to the OSG VertexProgram object.
    vertexProgram->setProgram(vertexProgramSource);
}

// ------------------------------------------------------------------------
// Set the ARBfp1.0 assembly source file to use for the fragment program.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentSourceFile(char *filename)
{
    long length;
    FILE *sourceFile;

    // If the filename is a NULL pointer, do nothing.
    if (filename == NULL)
    {
        printf("vsShaderAttribute::setFragmentSourceFile: Error: "
            "File name is NULL!\n");
        return;
    }

    // Attempt to open the file.
    sourceFile = fopen(filename, "rb");

    // If there was an error opening the file, print and return.
    if (sourceFile == NULL)
    {
        printf("vsShaderAttribute::setFragmentSourceFile: Error: "
            "Cannot open source file: %s\n", fragmentProgramFile);
        return;
    }

    // Copy the filename string into a local string.
    length = strlen(filename);
    fragmentProgramFile = (char *) malloc((length + 1));
    strncpy(fragmentProgramFile, filename, (length + 1));

    // Go to the end of the file and find out how large it is.
    fseek(sourceFile, 0, SEEK_END);
    length = ftell(sourceFile);

    // Go back to the beginning of the file.
    fseek(sourceFile, 0, SEEK_SET);

    // Allocate the space for the assembly code.
    fragmentProgramSource = (char *) malloc((length + 1));

    // Read the entire content of the file into the allocated string.
    fread(fragmentProgramSource, 1, length, sourceFile);

    // Insure it is NULL terminated.
    fragmentProgramSource[length] = '\0';

    // Close the file.
    fclose(sourceFile);

    // Create the program if it has not been created.
    if (fragmentProgram == NULL)
    {
        fragmentProgram = new pfFragmentProgram();
        fragmentParameters = new pfGProgramParms(PFGP_FRAGMENT_LOCAL);
    }

    // Set the source code to the OSG FragmentProgram object.
    fragmentProgram->setProgram(fragmentProgramSource);
}

// ------------------------------------------------------------------------
// Set the ARBvp1.0 assembly source code to use for the vertex program.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexSource(char *source)
{
    long length;

    // If the source is a NULL pointer, do nothing.
    if (source == NULL)
    {
        printf("vsShaderAttribute::setVertexSource: Error: "
            "Source name is NULL!\n");
        return;
    }

    // Copy the source string into a local string.
    length = strlen(source);
    vertexProgramSource = (char *) malloc((length + 1));
    strncpy(vertexProgramSource, source, (length + 1));

    // Create the program if it has not been created.
    if (vertexProgram == NULL)
    {
        vertexProgram = new pfVertexProgram();
        vertexParameters = new pfGProgramParms(PFGP_VERTEX_LOCAL);
    }
                                                                                
    // Set the source code to the OSG VertexProgram object.
    vertexProgram->setProgram(vertexProgramSource);
}

// ------------------------------------------------------------------------
// Set the ARBfp1.0 assembly source code to use for the fragment program.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentSource(char *source)
{
    long length;

    // If the source is a NULL pointer, do nothing.
    if (source == NULL)
    {
        printf("vsShaderAttribute::setFragmentSource: Error: "
            "Source name is NULL!\n");
        return;
    }

    // Copy the source string into a local string.
    length = strlen(source);
    fragmentProgramSource = (char *) malloc((length + 1));
    strncpy(fragmentProgramSource, source, (length + 1));

    // Create the program if it has not been created.
    if (fragmentProgram == NULL)
    {
        fragmentProgram = new pfFragmentProgram();
        fragmentParameters = new pfGProgramParms(PFGP_FRAGMENT_LOCAL);
    }
                                                                                
    // Set the source code to the OSG FragmentProgram object.
    fragmentProgram->setProgram(fragmentProgramSource);
}

// ------------------------------------------------------------------------
// Return the ARBvp1.0 assembly source file used for the vertex program.
// ------------------------------------------------------------------------
char *vsShaderAttribute::getVertexSourceFile()
{
    return vertexProgramFile;
}

// ------------------------------------------------------------------------
// Return the ARBfp1.0 assembly source file used for the fragment program.
// ------------------------------------------------------------------------
char *vsShaderAttribute::getFragmentSourceFile()
{
    return fragmentProgramFile;
}

// ------------------------------------------------------------------------
// Return the ARBvp1.0 assembly source used for the vertex program.
// ------------------------------------------------------------------------
char *vsShaderAttribute::getVertexSource()
{
    return vertexProgramSource;
}
                                                                                
// ------------------------------------------------------------------------
// Return the ARBfp1.0 assembly source used for the fragment program.
// ------------------------------------------------------------------------
char *vsShaderAttribute::getFragmentSource()
{
    return fragmentProgramSource;
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x)
{
    float *data;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        vertexParameterArray->setData(index, data);
        vertexParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = 0.0;
    data[2] = 0.0;
    data[3] = 0.0;

    // Set the vector to the vertex program.
    vertexParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    vertexParameters->update();
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y)
{
    float *data;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        vertexParameterArray->setData(index, data);
        vertexParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = y;
    data[2] = 0.0;
    data[3] = 0.0;

    // Set the vector to the vertex program.
    vertexParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    vertexParameters->update();
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y,
                                                float z)
{
    float *data;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        vertexParameterArray->setData(index, data);
        vertexParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = 0.0;

    // Set the vector to the vertex program.
    vertexParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    vertexParameters->update();
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y,
                                                float z, float w)
{
    float *data;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        vertexParameterArray->setData(index, data);
        vertexParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;

    // Set the vector to the vertex program.
    vertexParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    vertexParameters->update();
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index,
                                                const vsVector &value)
{
    float *data;
    int loop, size;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        vertexParameterArray->setData(index, data);
        vertexParameterCount++;
    }

    // Initialized the values to zero.
    data[0] = data[1] = data[2] = data[3] = 0.0;

    // Get the size of the vector.
    size = value.getSize();

    // Set the data to the new values from the vector, depending on its size.
    for (loop = 0; loop < size; loop++)
        data[loop] = value.getValue(loop);

    // Set the vector to the vertex program.
    vertexParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    vertexParameters->update();
}

// ------------------------------------------------------------------------
// Return a vsVector with the values currently set as the local vertex
// parameter at the specified index.
// ------------------------------------------------------------------------
vsVector vsShaderAttribute::getVertexLocalParameter(int index)
{
    float *data;
    vsVector resultVector;

    // Get the vector for the specified position.
    data = (float *) vertexParameterArray->getData(index);

    // If it does not exist, just set the result to all zeros.
    if (data == NULL)
    {
        resultVector.set(0.0, 0.0, 0.0, 0.0);
    }
    // Else set the result to the stored vector's values.
    else
    {
        resultVector.set(data[0], data[1], data[2], data[3]);
    }

    return resultVector;
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x)
{
    float *data;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        fragmentParameterArray->setData(index, data);
        fragmentParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = 0.0;
    data[2] = 0.0;
    data[3] = 0.0;

    // Set the vector to the fragment program.
    fragmentParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    fragmentParameters->update();
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y)
{
    float *data;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        fragmentParameterArray->setData(index, data);
        fragmentParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = y;
    data[2] = 0.0;
    data[3] = 0.0;

    // Set the vector to the fragment program.
    fragmentParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    fragmentParameters->update();
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y,
                                                  float z)
{
    float *data;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        fragmentParameterArray->setData(index, data);
        fragmentParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = 0.0;

    // Set the vector to the fragment program.
    fragmentParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    fragmentParameters->update();
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y,
                                                  float z, float w)
{
    float *data;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        fragmentParameterArray->setData(index, data);
        fragmentParameterCount++;
    }

    // Set the data to the new values, unused ones should be zero.
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
                                                                                
    // Set the vector to the fragment program.
    fragmentParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    fragmentParameters->update();
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index,
                                                  const vsVector &value)
{
    float *data;
    int loop, size;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    data = (float *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (data == NULL)
    {
        data = (float *) pfMalloc((sizeof(float) * 4), pfGetSharedArena());
        fragmentParameterArray->setData(index, data);
        fragmentParameterCount++;
    }

    // Initialized the values to zero.
    data[0] = data[1] = data[2] = data[3] = 0.0;

    // Get the size of the vector.
    size = value.getSize();

    // Set the data to the new values from the vector, depending on its size.
    for (loop = 0; loop < size; loop++)
        data[loop] = value.getValue(loop);

    // Set the vector to the fragment program.
    fragmentParameters->setParameters(index, PF_GPP_FLOAT_4, 1, data);
//    fragmentParameters->update();
}

// ------------------------------------------------------------------------
// Return a vsVector with the values currently set as the local fragment
// parameter at the specified index.
// ------------------------------------------------------------------------
vsVector vsShaderAttribute::getFragmentLocalParameter(int index)
{
    float *data;
    vsVector resultVector;

    // Get the vector for the specified position.
    data = (float *) fragmentParameterArray->getData(index);

    // If it does not exist, just set the result to all zeros.
    if (data == NULL)
    {
        resultVector.set(0.0, 0.0, 0.0, 0.0);
    }
    // Else set the result to the stored vector's values.
    else
    {
        resultVector.set(data[0], data[1], data[2], data[3]);
    }

    return resultVector;
}
