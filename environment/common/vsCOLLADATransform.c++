
#include <stdio.h>
#include <string.h>
#include "vsCOLLADATransform.h++"


// ------------------------------------------------------------------------
// Creates a COLLADA transform from the given XML subtree
// ------------------------------------------------------------------------
vsCOLLADATransform::vsCOLLADATransform(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current)
{
    char *attr;
    atString text;
    atStringTokenizer *tokens;
    int valueCount;
    int i;

    // See if this transform has been given a scoped identifier (this
    // typically happens when a transform will be animated)
    attr = doc->getNodeAttribute(current, "sid");
    if (attr != NULL)
        scopedID.setString(attr);

    // Process the transform itself
    if (strcmp(doc->getNodeName(current), "lookat") == 0)
    {
        // Set the transform type
        transformType = VS_COLLADA_XFORM_LOOKAT;

        // Look-At transforms take 9 values (position, look-at point, and
        // up direction)
        valueCount = 9;
    }
    else if (strcmp(doc->getNodeName(current), "matrix") == 0)
    {
        // Set the transform type
        transformType = VS_COLLADA_XFORM_MATRIX;

        // Matrix transforms take 16 values (for a 4x4 matrix)
        valueCount = 16;
    }
    else if (strcmp(doc->getNodeName(current), "scale") == 0)
    {
        // Set the transform type
        transformType = VS_COLLADA_XFORM_SCALE;

        // Scale transforms take 3 values (x, y, and z scale factors)
        valueCount = 3;
    }
    else if (strcmp(doc->getNodeName(current), "skew") == 0)
    {
        // Set the transform type
        transformType = VS_COLLADA_XFORM_SKEW;

        // Skew transforms take 7 values (a rotation angle, an "along" vector,
        // and an "around" vector, I'd explain it better, but I don't
        // understand it myself yet)
        valueCount = 7;
    }
    else if (strcmp(doc->getNodeName(current), "rotate") == 0)
    {
        // Set the transform type
        transformType = VS_COLLADA_XFORM_ROTATE;

        // Rotate transforms take 4 values (x, y, and z axis of rotation,
        // and an angle of rotation)
        valueCount = 4;
    }
    else if (strcmp(doc->getNodeName(current), "translate") == 0)
    {
        // Set the transform type
        transformType = VS_COLLADA_XFORM_TRANSLATE;

        // Translate transforms take 3 values (x, y, and z axis translation
        // amounts)
        valueCount = 3;
    }
    else
    {
        // This is an invalid transform, so set all memebers to default
        // values
        transformType = VS_COLLADA_XFORM_UNKNOWN;
        memset(values, 0, sizeof(values));
        resultMatrix.setIdentity();
    }

    // Make sure this is a valid transform before we try to read the
    // transform values
    if (transformType != VS_COLLADA_XFORM_UNKNOWN)
    {
        // Create a string tokenizer to parse the text
        text.setString(doc->getNodeText(doc->getNextChildNode(current)));
        tokens = new atStringTokenizer(text);

        // Initialize the value string
        memset(values, 0, sizeof(values));

        // Read in the required number of values
        for (i = 0; i < valueCount; i++)
            values[i] = getFloatToken(tokens);

        // Clean up the tokenizer
        delete tokens;

        // Update the resulting transformation matrix with the new
        // values
        updateMatrix();
    }
}

// ------------------------------------------------------------------------
// Destructor, does nothing
// ------------------------------------------------------------------------
vsCOLLADATransform::~vsCOLLADATransform()
{
}

// ------------------------------------------------------------------------
// Simple convenience method.  Retrieves the next token from the given
// string tokenizer and converts it to an floating point number before
// returning it
// ------------------------------------------------------------------------
double vsCOLLADATransform::getFloatToken(atStringTokenizer *tokens)
{
    atString *tempStr;
    double value;   

    // Get the next token from the tokenizer
    tempStr = tokens->getToken(" \n\r\t");
    
    // Convert the token to a floating point value
    value = atof(tempStr->getString());

    // Get rid of the temporary string
    delete tempStr; 

    // Return the floating point value
    return value;
}   

// ------------------------------------------------------------------------
// Converts the transform values from the native COLLADA format to a
// transformation matrix for use in vsKinematics and/or
// vsTransformAttributes
// ------------------------------------------------------------------------
void vsCOLLADATransform::updateMatrix()
{
    atVector view, target, up;
    atVector forward, side;
    int i, j;
    atQuat quat;

    // Figure out what kind of transform this is
    switch (transformType)
    {
        case VS_COLLADA_XFORM_LOOKAT:

            // Assign the values to view and target points, and an up vector
            view.set(values[0], values[1], values[2]);
            target.set(values[3], values[4], values[5]);
            up.set(values[6], values[7], values[8]);

            // Use the two points and the vector to create the desired
            // coordinate system
            forward = target - view;
            forward.normalize();
            side = forward.getCrossProduct(up);
            side.normalize();
            up = side.getCrossProduct(forward);
            up.normalize();

            // Convert these vectors to a matrix
            // JPD:  This looks like an OpenGL coordinate system, might
            //       need to rearrange this
            resultMatrix.clear();
            resultMatrix[0][0] = side[0];
            resultMatrix[0][1] = side[1];
            resultMatrix[0][2] = side[2];
            resultMatrix[0][3] = -view[0];
            resultMatrix[1][0] = up[0];
            resultMatrix[1][1] = up[1];
            resultMatrix[1][2] = up[2];
            resultMatrix[1][3] = -view[1];
            resultMatrix[2][0] = -forward[0];
            resultMatrix[2][1] = -forward[1];
            resultMatrix[2][2] = -forward[2];
            resultMatrix[2][3] = -view[2];
            resultMatrix[3][3] = 1.0;
            break;

        case VS_COLLADA_XFORM_MATRIX:

            // Assign the 16 transform values to the result matrix
            for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                    resultMatrix[i][j] = values[i*4+j];
            break;

        case VS_COLLADA_XFORM_SCALE:
     
            // Create a scale matrix from the three scalar values
            resultMatrix.setScale(values[0], values[1], values[2]);
            break;

        case VS_COLLADA_XFORM_SKEW:

            // JPD:  I'm having trouble finding how to construct a skew
            //       matrix from the given values
            resultMatrix.setIdentity();
            break;

        case VS_COLLADA_XFORM_ROTATE:

            // Create a quaternion to represent the axis/angle rotation,
            // and use it to create a rotation matrix
            quat.setAxisAngleRotation(values[0], values[1], values[2],
                                      values[3]);
            resultMatrix.setQuatRotation(quat);
            break;

        case VS_COLLADA_XFORM_TRANSLATE:

            // Create a translation matrix from the three axis values
            resultMatrix.setTranslation(values[0], values[1], values[2]);
            break;

        default:

            // Just use the identity matrix
            resultMatrix.setIdentity();
            break;
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADATransform::getClassName()
{
    return "vsCOLLADATransform";
}

// ------------------------------------------------------------------------
// Return the type of transform contained in this object
// ------------------------------------------------------------------------
vsCOLLADATransformType vsCOLLADATransform::getType()
{
    return transformType;
}

// ------------------------------------------------------------------------
// Return the scoped ID of this transform
// ------------------------------------------------------------------------
atString vsCOLLADATransform::getSID()
{
    return scopedID.clone();
}

// ------------------------------------------------------------------------
// Returns this transform as a matrix
// ------------------------------------------------------------------------
atMatrix vsCOLLADATransform::getMatrix()
{
    return resultMatrix;
}

// ------------------------------------------------------------------------
// Returns the translation component of this transform as a vector
// ------------------------------------------------------------------------
atVector vsCOLLADATransform::getPosition()
{
    atVector temp;

    // Get the translation portion of the matrix for the vector
    resultMatrix.getTranslation(&temp[AT_X], &temp[AT_Y], &temp[AT_Z]);

    // Return the resulting translation
    return temp;
}

// ------------------------------------------------------------------------
// Returns the rotation component of this transform as a quaternion
// ------------------------------------------------------------------------
atQuat vsCOLLADATransform::getOrientation()
{
    atQuat orient;

    // Return the matrix's rotation component as a quaternion
    orient.setMatrixRotation(resultMatrix);
    return orient;
}
