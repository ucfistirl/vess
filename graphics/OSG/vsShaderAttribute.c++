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
}

// ------------------------------------------------------------------------
// Destructor, clear the parameter array and unreference created objects.
// ------------------------------------------------------------------------
vsShaderAttribute::~vsShaderAttribute()
{
    // If there are any filename and source strings, free them.
    if (vertexProgramFile)
        free(vertexProgramFile);
    if (vertexProgramSource)
        free(vertexProgramSource);
    if (fragmentProgramFile)
        free(fragmentProgramFile);
    if (fragmentProgramSource)
        free(fragmentProgramSource);

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
    // Add a clone of this attribute to the given node
    theNode->addAttribute(this->clone());
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
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsShaderAttribute::clone()
{
    vsShaderAttribute *newAttrib;

    // Create a new vsShadingAttribute and copy the data from this
    // attribute to the new one
    newAttrib = new vsShaderAttribute();

    // Duplicate the data, this will not handle parameters though.
    newAttrib->setVertexSource(getVertexSource());
    newAttrib->setFragmentSource(getFragmentSource());

    /* Copy the parameters somehow? */

    // Return the clone
    return newAttrib;
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
    osg::Vec4 osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, 0.0, 0.0, 0.0);
    vertexProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y)
{
    osg::Vec4 osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, y, 0.0, 0.0);
    vertexProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y,
                                                float z)
{
    osg::Vec4 osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, y, z, 0.0);
    vertexProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index, float x, float y,
                                                float z, float w)
{
    osg::Vec4 osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, y, z, w);
    vertexProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local vertex parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setVertexLocalParameter(int index,
                                                const atVector &value)
{
    atVector v;
    osg::Vec4 osgVector;

    // Insure the vertex program is valid.
    if (vertexProgram == NULL)
    {
        printf("vsShaderAttribute::setVertexLocalParameter: Error: "
            "No vertex program available\n");
        return;
    }

    // Clear-copy the input vector, so the extra elements (beyond the
    // vector's size) are set to zero.  Then, set the copy to size 4.
    v.clearCopy(value);
    v.setSize(4);

    // Create an OSG vector for the parameter, and set it to the
    // vertex program.
    osgVector.set(v[AT_X], v[AT_Y], v[AT_Z], v[AT_W]);
    vertexProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Return a atVector with the values currently set as the local vertex
// parameter at the specified index.
// ------------------------------------------------------------------------
atVector vsShaderAttribute::getVertexLocalParameter(int index)
{
    osg::VertexProgram::LocalParamList           params;
    osg::VertexProgram::LocalParamList::iterator itr;
    osg::Vec4                                    value;

    atVector resultVector;

    // Fetch the vertex program's local parameters
    params = vertexProgram->getLocalParameters();

    // Get the vector for the specified position.
    itr = params.find(index);

    // See if we got a valid parameter
    if (itr == params.end())
    {
        // There is no parameter at this index, so return an empty vector
        resultVector.setSize(4);
        resultVector.clear();
    }
    else
    {
        // Get the parameter's value
        value = itr->second;

        // Translate to an atVector
        resultVector.set(value.x(), value.y(), value.z(), value.w());
    }

    // Return the result
    return resultVector;
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x)
{
    osg::Vec4 osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, 0.0, 0.0, 0.0);
    fragmentProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y)
{
    osg::Vec4 osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, y, 0.0, 0.0);
    fragmentProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y,
                                                  float z)
{
    osg::Vec4 osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, y, z, 0.0);
    fragmentProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index, float x, float y,
                                                  float z, float w)
{
    osg::Vec4 osgVector;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Create a vector for the parameter, and set it to the vertex program.
    osgVector.set(x, y, z, w);
    fragmentProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Set the local fragment parameter vector at the indicated index to given
// values.  Unspecified values are set to 0.0.
// ------------------------------------------------------------------------
void vsShaderAttribute::setFragmentLocalParameter(int index,
                                                  const atVector &value)
{
    osg::Vec4 osgVector;
    atVector v;

    // Insure the fragment program is valid.
    if (fragmentProgram == NULL)
    {
        printf("vsShaderAttribute::setFragmentLocalParameter: Error: "
            "No fragment program available\n");
        return;
    }

    // Clear-copy the input vector, to ensure any extra elements (beyond
    // the vector's size) are set to zero.  Then set the copy's size to 4.
    v.clearCopy(value);
    v.setSize(4);

    // Create an OSG vector for the parameter, and set it to the
    // vertex program.
    osgVector.set(v[AT_X], v[AT_Y], v[AT_Z], v[AT_W]);
    fragmentProgram->setProgramLocalParameter(index, osgVector);
}

// ------------------------------------------------------------------------
// Return a atVector with the values currently set as the local fragment
// parameter at the specified index.
// ------------------------------------------------------------------------
atVector vsShaderAttribute::getFragmentLocalParameter(int index)
{
    osg::FragmentProgram::LocalParamList           params;
    osg::FragmentProgram::LocalParamList::iterator itr;
    osg::Vec4                                      value;

    atVector resultVector;

    // Fetch the fragment program's local parameters
    params = fragmentProgram->getLocalParameters();

    // Get the vector for the specified position.
    itr = params.find(index);

    // See if we got a valid parameter
    if (itr == params.end())
    {
        // There is no parameter at this index, so return an empty vector
        resultVector.setSize(4);
        resultVector.clear();
    }
    else
    {
        // Get the parameter's value
        value = itr->second;

        // Translate to an atVector
        resultVector.set(value.x(), value.y(), value.z(), value.w());
    }

    return resultVector;
}
