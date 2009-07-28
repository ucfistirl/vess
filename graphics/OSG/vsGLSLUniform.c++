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
    // Create the OSG Uniform object
    osgUniform = new osg::Uniform((osg::Uniform::Type)type, name);
    osgUniform->ref();

    // Set the number of uniform elements (1 in this case, since it is a
    // scalar uniform)
    elementCount = 1;

    // Save a copy of the name
    strncpy(uniformName, name, sizeof(char) * VS_UNIFORM_NAME_LENGTH);
    uniformName[VS_UNIFORM_NAME_LENGTH - 1];
}

// ------------------------------------------------------------------------
// Constructs a vsGLSLUniform array with a the given type and number of
// elements
// ------------------------------------------------------------------------
vsGLSLUniform::vsGLSLUniform(const char *name, vsGLSLUniformType type,
                             u_long numElements)
{
    // Create the OSG Uniform object
    osgUniform = new osg::Uniform((osg::Uniform::Type)type, name, numElements);
    osgUniform->ref();

    // Set the number of uniform elements (i.e.: the length of the array)
    elementCount = numElements;

    // Save a copy of the name
    strncpy(uniformName, name, sizeof(char) * VS_UNIFORM_NAME_LENGTH);
    uniformName[VS_UNIFORM_NAME_LENGTH - 1];
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsGLSLUniform::~vsGLSLUniform()
{
    // Tell OSG we don't need this Uniform anymore
    osgUniform->unref();
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsGLSLUniform::getClassName()
{
    return "vsGLSLUniform";
}

// ------------------------------------------------------------------------
// Return the name of this Uniform
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
    return (vsGLSLUniformType)osgUniform->getType();
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1)
{
    osgUniform->set(b1);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1, bool b2)
{
    osgUniform->set(b1, b2);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1, bool b2, bool b3)
{
    osgUniform->set(b1, b2, b3);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(bool b1, bool b2, 
                             bool b3, bool b4)
{
    osgUniform->set(b1, b2, b3, b4);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1)
{
    osgUniform->set(i1);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1, int i2)
{
    osgUniform->set(i1, i2);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1, int i2, int i3)
{
    osgUniform->set(i1, i2, i3);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int i1, int i2, int i3, int i4)
{
    osgUniform->set(i1, i2, i3, i4);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(float floatVal)
{
    osgUniform->set(floatVal);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(double doubleVal)
{
    osgUniform->set((float)doubleVal);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(atVector vec)
{
    osg::Vec2 vec2;
    osg::Vec3 vec3;
    osg::Vec4 vec4;

    // Create the correct size of vector based on the atVector's size
    switch (vec.getSize())
    {
        case 2: 
            vec2.set(vec[0], vec[1]);
            osgUniform->set(vec2);
            break;

        case 3: 
            vec3.set(vec[0], vec[1], vec[2]);
            osgUniform->set(vec3);
            break;

        case 4: 
            vec4.set(vec[0], vec[1], vec[2], vec[3]);
            osgUniform->set(vec4);
            break;

        default:
            // Do nothing but print an error
            printf("vsGLSLUniform::set:  Invalid vector size!\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(atMatrix mat)
{
    // Call the alternate form of this setter with a size of 4
    set(4, mat);
}

// ------------------------------------------------------------------------
// Sets the uniform to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::set(int size, atMatrix mat)
{
    osg::Matrix2 mat2;
    osg::Matrix3 mat3;
    osg::Matrixf mat4;
    int i, j;

    // Copy the matrix into a suitable form
    // Construct the OSG Uniform with the appropriate constructor based
    // on the matrix size
    switch (size)
    {
        case 2:
            // Create an osg::Matrix2 from the upper 2x2 portion of the
            // VESS matrix 
            for (i = 0; i < 2; i++)
                for (j = 0; j < 2; j++)
                    mat2(i, j) = mat[j][i];

            // Create the osg::Uniform using the Matrix2
            osgUniform->set(mat2);
            break;

        case 3:
            // Create an osg::Matrix2 from the upper 3x3 portion of the
            // VESS matrix 
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    mat3(i, j) = mat[j][i];

            // Create the osg::Uniform using the Matrix3
            osgUniform->set(mat3);
            break;

        case 4:
            // Create an osg::Matrixf from the VESS matrix 
            for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                    mat4(i, j) = (float)mat[j][i];

            // Create the osg::Uniform using the Matrixf
            osgUniform->set(mat4);
            break;

        default:
            // Do nothing but print an error
            printf("vsGLSLUniform::vsGLSLUniform:  Invalid matrix size!\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, bool b1)
{
    osgUniform->setElement(index, b1);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, bool b1, bool b2)
{
    osgUniform->setElement(index, b1, b2);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, bool b1, bool b2, bool b3)
{
    osgUniform->setElement(index, b1, b2, b3);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, bool b1, bool b2, 
                             bool b3, bool b4)
{
    osgUniform->setElement(index, b1, b2, b3, b4);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, int i1)
{
    osgUniform->setElement(index, i1);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, int i1, int i2)
{
    osgUniform->setElement(index, i1, i2);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, int i1, int i2, int i3)
{
    osgUniform->setElement(index, i1, i2, i3);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, int i1, int i2, int i3, int i4)
{
    osgUniform->setElement(index, i1, i2, i3, i4);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, float floatVal)
{
    osgUniform->setElement(index, floatVal);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, double doubleVal)
{
    osgUniform->setElement(index, (float)doubleVal);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, atVector vec)
{
    osg::Vec2 vec2;
    osg::Vec3 vec3;
    osg::Vec4 vec4;

    // Create the correct size of vector based on the atVector's size
    switch (vec.getSize())
    {
        case 2: 
            vec2.set(vec[0], vec[1]);
            osgUniform->setElement(index, vec2);
            break;

        case 3: 
            vec3.set(vec[0], vec[1], vec[2]);
            osgUniform->setElement(index, vec3);
            break;

        case 4: 
            vec4.set(vec[0], vec[1], vec[2], vec[3]);
            osgUniform->setElement(index, vec4);
            break;

        default:
            // Do nothing but print an error
            printf("vsGLSLUniform::set:  Invalid vector size!\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, atMatrix mat)
{
    // Call the alternate form of this setter with a size of 4
    setEntry(index, 4, mat);
}

// ------------------------------------------------------------------------
// Sets the uniform array element at the given index to the new given value
// ------------------------------------------------------------------------
void vsGLSLUniform::setEntry(u_long index, int size, atMatrix mat)
{
    osg::Matrix2 mat2;
    osg::Matrix3 mat3;
    osg::Matrixf mat4;
    int i, j;

    // Copy the matrix into a suitable form
    // Construct the OSG Uniform with the appropriate constructor based
    // on the matrix size
    switch (size)
    {
        case 2:
            // Create an osg::Matrix2 from the upper 2x2 portion of the
            // VESS matrix 
            for (i = 0; i < 2; i++)
                for (j = 0; j < 2; j++)
                    mat2(i, j) = mat[j][i];

            // Set the matrix element
            osgUniform->setElement(index, mat2);
            break;

        case 3:
            // Create an osg::Matrix2 from the upper 3x3 portion of the
            // VESS matrix 
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    mat3(i, j) = mat[j][i];

            // Set the matrix element
            osgUniform->setElement(index, mat3);
            break;

        case 4:
            // Create an osg::Matrixf from the VESS matrix 
            for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                    mat4(i, j) = (float)mat[j][i];

            // Set the matrix element
            osgUniform->setElement(index, mat4);
            break;

        default:
            // Do nothing but print an error
            printf("vsGLSLUniform::vsGLSLUniform:  Invalid matrix size!\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Return the OSG Uniform object
// ------------------------------------------------------------------------
osg::Uniform *vsGLSLUniform::getBaseLibraryObject()
{
    return osgUniform;
}
