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

// ------------------------------------------------------------------------
// Constructor, create a context for the shaders and initializes them to NULL
// ------------------------------------------------------------------------
vsShaderAttribute::vsShaderAttribute()
{
    // Initialize all pointers to NULL.
    vertexProgram = NULL;
    vertexProgramFile = NULL;
    vertexProgramSource = NULL;

    fragmentProgram = NULL;
    fragmentProgramFile = NULL;
    fragmentProgramSource = NULL;

    // Create the arrays that will store a vector of parameter data.
    vertexParameterArray = new vsGrowableArray(96, 16);
    fragmentParameterArray = new vsGrowableArray(96, 16);
}

// ------------------------------------------------------------------------
// Destructor, clear the parameter array and unreference created objects.
// ------------------------------------------------------------------------
vsShaderAttribute::~vsShaderAttribute()
{
    int length;
    osg::Vec4 *vector;

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
        vector = (osg::Vec4 *) vertexParameterArray->getData(length);
        if (vector)
            delete vector;
    }

    // Delete any vectors we have in the fragment parameter array.
    length = fragmentParameterArray->getSize();
    for (length--; length > -1; length--)
    {
        vector = (osg::Vec4 *) fragmentParameterArray->getData(length);
        if (vector)
            delete vector;
    }

    // Delete the arrays.
    delete vertexParameterArray;
    delete fragmentParameterArray;

    // Unreference the programs if they exist.
    if (vertexProgram)
        vertexProgram->unref();
    if (fragmentProgram)
        fragmentProgram->unref();
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsShaderAttribute::setOSGAttrModes(vsNode *node)
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

    // Apply the osg::VertexProgram and osg::FragmentProgram on the StateSet
    if (vertexProgram)
        osgStateSet->setAttributeAndModes(vertexProgram, attrMode);
    if (fragmentProgram)
        osgStateSet->setAttributeAndModes(fragmentProgram, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsShaderAttribute::attach(vsNode *node)
{
    // Do normal vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set up the osg::StateSet on this node to use the osg::VertexProgram
    // and or osg::FragmentProgram we've created
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsShaderAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the node's StateSet
    osgStateSet = getOSGStateSet(node);

    // Reset the Program modes to inherit
    if (vertexProgram)
        osgStateSet->setAttributeAndModes(vertexProgram,
            osg::StateAttribute::INHERIT);
    if (fragmentProgram)
        osgStateSet->setAttributeAndModes(fragmentProgram,
            osg::StateAttribute::INHERIT);

    // Finish detaching the attribute
    vsStateAttribute::detach(node);
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
        vertexProgram = new osg::VertexProgram();
        vertexProgram->ref();
    }

    // Set the source code to the OSG VertexProgram object.
    vertexProgram->setVertexProgram(vertexProgramSource);
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
        fragmentProgram = new osg::FragmentProgram();
        fragmentProgram->ref();
    }

    // Set the source code to the OSG FragmentProgram object.
    fragmentProgram->setFragmentProgram(fragmentProgramSource);
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
        vertexProgram = new osg::VertexProgram();
        vertexProgram->ref();
    }
                                                                                
    // Set the source code to the OSG VertexProgram object.
    vertexProgram->setVertexProgram(vertexProgramSource);
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
        fragmentProgram = new osg::FragmentProgram();
        fragmentProgram->ref();
    }
                                                                                
    // Set the source code to the OSG FragmentProgram object.
    fragmentProgram->setFragmentProgram(fragmentProgramSource);
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
    osg::Vec4 *osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        vertexParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, 0.0, 0.0, 0.0);

    // Set the vector to the vertex program.
    vertexProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y)
{
    osg::Vec4 *osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        vertexParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, y, 0.0, 0.0);

    // Set the vector to the vertex program.
    vertexProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y,
                                                float z)
{
    osg::Vec4 *osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        vertexParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, y, z, 0.0);

    // Set the vector to the vertex program.
    vertexProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y,
                                                float z, float w)
{
    osg::Vec4 *osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        vertexParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, y, z, w);

    // Set the vector to the vertex program.
    vertexProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index,
                                                const vsVector &value)
{
    osg::Vec4 *osgVector;
    float v[4];
    int loop, size;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) vertexParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        vertexParameterArray->setData(index, osgVector);
    }

    // Initialized the values to zero.
    v[0] = v[1] = v[2] = v[3] = 0.0;

    // Get the size of the vector.
    size = value.getSize();

    // Get the valid values from the vector, depending on its size.
    for (loop = 0; loop < size; loop++)
        v[loop] = value.getValue(loop);

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(v[0], v[1], v[2], v[3]);

    // Set the vector to the vertex program.
    vertexProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Return a vsVector with the values currently set as the local vertex
// parameter at the specified index.
// ------------------------------------------------------------------------
vsVector vsShaderAttribute::getVertexLocalParameter(int index)
{
    osg::Vec4 *osgVector;
    vsVector resultVector;

    // Get the vector for the specified position.
    osgVector = (osg::Vec4 *) vertexParameterArray->getData(index);
                                                                                                                                                                                   
    // If it does not exist, just set the result to all zeros.
    if (osgVector == NULL)
    {
        resultVector.set(0.0, 0.0, 0.0, 0.0);
    }
    // Else set the result to the stored vector's values.
    else
    {
        resultVector.set(osgVector->x(), osgVector->y(), osgVector->z(),
            osgVector->w());
    }

    return resultVector;
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x)
{
    osg::Vec4 *osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        fragmentParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, 0.0, 0.0, 0.0);

    // Set the vector to the fragment program.
    fragmentProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y)
{
    osg::Vec4 *osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        fragmentParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, y, 0.0, 0.0);

    // Set the vector to the fragment program.
    fragmentProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y,
                                                  float z)
{
    osg::Vec4 *osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        fragmentParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, y, z, 0.0);

    // Set the vector to the fragment program.
    fragmentProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y,
                                                  float z, float w)
{
    osg::Vec4 *osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        fragmentParameterArray->setData(index, osgVector);
    }

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(x, y, z, w);
                                                                                
    // Set the vector to the fragment program.
    fragmentProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index,
                                                  const vsVector &value)
{
    osg::Vec4 *osgVector;
    float v[4];
    int loop, size;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Get whatever we happen to have in the array at the given index.
    osgVector = (osg::Vec4 *) fragmentParameterArray->getData(index);

    // If the data is NULL, create a new vector and place it there.
    if (osgVector == NULL)
    {
        osgVector = new osg::Vec4();
        fragmentParameterArray->setData(index, osgVector);
    }

    // Initialized the values to zero.
    v[0] = v[1] = v[2] = v[3] = 0.0;

    // Get the size of the vector.
    size = value.getSize();

    // Get the valid values from the vector, depending on its size.
    for (loop = 0; loop < size; loop++)
        v[loop] = value.getValue(loop);

    // Set the osgVector to the new values, unused ones should be zero.
    osgVector->set(v[0], v[1], v[2], v[3]);

    // Set the vector to the fragment program.
    fragmentProgram->setProgramLocalParameter(index, *osgVector);
}

// ------------------------------------------------------------------------
// Return a vsVector with the values currently set as the local fragment
// parameter at the specified index.
// ------------------------------------------------------------------------
vsVector vsShaderAttribute::getFragmentLocalParameter(int index)
{
    osg::Vec4 *osgVector;
    vsVector resultVector;

    // Get the vector for the specified position.
    osgVector = (osg::Vec4 *) fragmentParameterArray->getData(index);

    // If it does not exist, just set the result to all zeros.
    if (osgVector == NULL)
    {
        resultVector.set(0.0, 0.0, 0.0, 0.0);
    }
    // Else set the result to the stored vector's values.
    else
    {
        resultVector.set(osgVector->x(), osgVector->y(), osgVector->z(),
            osgVector->w());
    }

    return resultVector;
}
