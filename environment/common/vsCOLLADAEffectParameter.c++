#include "vsCOLLADAEffectParameter.h++"

// ------------------------------------------------------------------------
// Constructs a vsCOLLADAEffectParameter with a the given type
// ------------------------------------------------------------------------
vsCOLLADAEffectParameter::vsCOLLADAEffectParameter(atString name,
                                                   vsCOLLADAParameterType type)
{
    // Copy the name and parameter type
    parameterName.setString(name);
    parameterType = type;

    // Initialize all parameter values for all types
    memset(boolValue, 0, sizeof(boolValue));
    memset(intValue, 0, sizeof(intValue));
    floatValue.clear();
    floatValue.setSize(4);
    matrixValue.setIdentity();
    textureValue = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCOLLADAEffectParameter::~vsCOLLADAEffectParameter()
{
    // If this is a texture parameter, unreference/delete the texture
    if (textureValue != NULL)
        vsObject::unrefDelete(textureValue);
}

// ------------------------------------------------------------------------
// Simple convenience method.  Retrieves the next token from the given
// string tokenizer and converts it to a bool before returning it
// ------------------------------------------------------------------------
bool vsCOLLADAEffectParameter::getBoolToken(atStringTokenizer *tokens)
{
    atString *tempStr;
    bool value;

    // Get the next token from the tokenizer
    tempStr = tokens->getToken(" \n\r\t");

    // Convert the token to a boolean value
    if ((tempStr->getCharAt(0) == '1') || 
        (tempStr->getCharAt(0) == 'T') || (tempStr->getCharAt(0) == 't') ||
        (tempStr->getCharAt(0) == 'Y') || (tempStr->getCharAt(0) == 'y'))
        value = true;
    else
        value = false;

    // Get rid of the temporary string
    delete tempStr;

    // Return the boolean value
    return value;
}

// ------------------------------------------------------------------------
// Simple convenience method.  Retrieves the next token from the given
// string tokenizer and converts it to an integer before returning it
// ------------------------------------------------------------------------
int vsCOLLADAEffectParameter::getIntToken(atStringTokenizer *tokens)
{
    atString *tempStr;
    int value;

    // Get the next token from the tokenizer
    tempStr = tokens->getToken(" \n\r\t");

    // Convert the token to an integer value
    value = atoi(tempStr->getString());

    // Get rid of the temporary string
    delete tempStr;

    // Return the integer value
    return value;
}

// ------------------------------------------------------------------------
// Simple convenience method.  Retrieves the next token from the given
// string tokenizer and converts it to an floating point number before
// returning it
// ------------------------------------------------------------------------
double vsCOLLADAEffectParameter::getFloatToken(atStringTokenizer *tokens)
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
// Processes the settings for a surface parameter
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::processSurface(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr surfaceNode;
    atXMLDocumentNodePtr tempNode;
    char *text;

    // Look for the texture's filename
    surfaceNode = doc->getNextChildNode(current);
    while (surfaceNode != NULL)
    {
        // See if this is the initialization filename
        if (strcmp(doc->getNodeName(surfaceNode), "init_from") == 0)
        {
            // JPD:  Technically this tag is supposed to include
            //       mip, face, and/or slice attributes to describe
            //       a single surface of a complex image (mipmap,
            //       cubemap, or volume texture)  However, the
            //       COLLADAMax exporter seems to like to use this
            //       for everything, even simple 2D images.

            // Get the filename's text node
            tempNode = doc->getNextChildNode(surfaceNode);

            // Get the image ID and store it.  We don't actually fetch
            // the image data (the texture value) until later
            text = doc->getNodeText(tempNode);
            setSourceImageID(atString(text));
        }

        // Try the next node
        surfaceNode = doc->getNextSiblingNode(surfaceNode);
    }
}

// ------------------------------------------------------------------------
// Processes the settings for a sampler2D parameter
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::processSampler2D(atXMLDocument *doc,
                                                atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr samplerNode;
    atXMLDocumentNodePtr tempNode;
    atString sourceSID;

    // Create a texture attribute to hold the sampler settings
    textureValue = new vsTextureAttribute();
    textureValue->ref();

    // Read the sampler settings, and apply them to the texture
    samplerNode = doc->getNextChildNode(current);
    while (samplerNode != NULL)
    {
        // See if this is a recognized sampler setting
        if (strcmp(doc->getNodeName(samplerNode), "source") == 0)
        {
            // Copy the sid of the surface parameter
            tempNode = doc->getNextChildNode(samplerNode);
            sourceSID = atString(doc->getNodeText(tempNode));
            setSourceSurfaceID(sourceSID);
        }
        else if (strcmp(doc->getNodeName(samplerNode), "wrap_s") == 0)
        {
            // Get the wrap settings
            tempNode = doc->getNextChildNode(samplerNode);
            if (strcmp(doc->getNodeText(tempNode), "WRAP") == 0)
            {
               // Set the texture to wrap in the S (horizontal)
               // direction
               textureValue->setBoundaryMode(VS_TEXTURE_DIRECTION_S,
                   VS_TEXTURE_BOUNDARY_REPEAT);
            }
            else if (strcmp(doc->getNodeText(tempNode), "CLAMP") == 0)
            {
               // Set the texture to clamp in the S (horizontal)
               // direction
               textureValue->setBoundaryMode(VS_TEXTURE_DIRECTION_S,
                   VS_TEXTURE_BOUNDARY_CLAMP);
            }
        }
        else if (strcmp(doc->getNodeName(samplerNode), "wrap_t") == 0)
        {
            // Get the wrap settings
            tempNode = doc->getNextChildNode(samplerNode);
            if (strcmp(doc->getNodeText(tempNode), "WRAP") == 0)
            {
               // Set the texture to wrap in the T (vertical)
               // direction
               textureValue->setBoundaryMode(VS_TEXTURE_DIRECTION_T,
                   VS_TEXTURE_BOUNDARY_REPEAT);
            }
            else if (strcmp(doc->getNodeText(tempNode), "CLAMP") == 0)
            {
               // Set the texture to wrap in the T (vertical)
               // direction
               textureValue->setBoundaryMode(VS_TEXTURE_DIRECTION_T,
                   VS_TEXTURE_BOUNDARY_CLAMP);
            }
        }
        else if (strcmp(doc->getNodeName(samplerNode), "magfilter") == 0)
        {
            // Get the filter settings
            tempNode = doc->getNextChildNode(samplerNode); 
            if ((strcmp(doc->getNodeText(tempNode),
                        "NONE") == 0) ||
                (strcmp(doc->getNodeText(tempNode),
                        "NEAREST") == 0) ||
                (strcmp(doc->getNodeText(tempNode),
                        "LINEAR_MIPMAP_NEAREST") == 0))
            {
                // Set the magnification filter to nearest sample mode
                textureValue->setMagFilter(VS_TEXTURE_MAGFILTER_NEAREST);
            }
            else if ((strcmp(doc->getNodeText(tempNode),
                             "LINEAR") == 0) ||
                     (strcmp(doc->getNodeText(tempNode),
                             "LINEAR_MIPMAP_NEAREST") == 0) ||
                     (strcmp(doc->getNodeText(tempNode),
                             "NEAREST_MIPMAP_LINEAR") == 0) ||
                     (strcmp(doc->getNodeText(tempNode),
                             "LINEAR_MIPMAP_LINEAR") == 0))
            {
                // Set the magnification filter to bilinear interpolation mode
                textureValue->setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
            }
        }
        else if (strcmp(doc->getNodeName(samplerNode), "minfilter") == 0)
        {
            // Get the filter settings
            tempNode = doc->getNextChildNode(samplerNode);
            if ((strcmp(doc->getNodeText(tempNode), "NONE") == 0) ||
                (strcmp(doc->getNodeText(tempNode), "NEAREST") == 0))
            {
               // Set the minification filter to nearest sample mode
               textureValue->setMinFilter(VS_TEXTURE_MINFILTER_NEAREST);
            }
            else if (strcmp(doc->getNodeText(tempNode), "LINEAR") == 0)
            {
               // Set the minification filter to bilinear interpolation mode
               textureValue->setMinFilter(VS_TEXTURE_MINFILTER_NEAREST);
            }
            else if (strcmp(doc->getNodeText(tempNode),
                            "NEAREST_MIPMAP_NEAREST") == 0)
            {
               // Set the minification filter to nearest sample mode with
               // mipmapping enabled
               textureValue->setMinFilter(VS_TEXTURE_MINFILTER_MIPMAP_NEAREST);
            }
            else if ((strcmp(doc->getNodeText(tempNode),
                             "LINEAR_MIPMAP_NEAREST") == 0) ||
                     (strcmp(doc->getNodeText(tempNode),
                             "NEAREST_MIPMAP_LINEAR") == 0) ||
                     (strcmp(doc->getNodeText(tempNode),
                             "LINEAR_MIPMAP_LINEAR") == 0))
            {
               // Set the minification filter to trilinear mipmapping
               // mode (bilinear interpolation with linear mipmap
               // interpolation)
               textureValue->setMinFilter(VS_TEXTURE_MINFILTER_MIPMAP_LINEAR);
            }
        }

        // Try the next node
        samplerNode = doc->getNextSiblingNode(samplerNode);
    }
}


// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAEffectParameter::getClassName()
{
    return "vsCOLLADAEffectParameter";
}

// ------------------------------------------------------------------------
// Return the name of this parameter
// ------------------------------------------------------------------------
atString vsCOLLADAEffectParameter::getName()
{
    return parameterName;
}

// ------------------------------------------------------------------------
// Return the data type of this parameter
// ------------------------------------------------------------------------
vsCOLLADAParameterType vsCOLLADAEffectParameter::getType()
{
    return parameterType;
}

// ------------------------------------------------------------------------
// Return a clone of this parameter
// ------------------------------------------------------------------------
vsCOLLADAEffectParameter *vsCOLLADAEffectParameter::clone()
{
    vsCOLLADAEffectParameter *newParam;

    // Create the new parameter
    newParam = new vsCOLLADAEffectParameter(parameterName, parameterType);

    // Copy all values
    memcpy(newParam->boolValue, boolValue, sizeof(boolValue));
    memcpy(newParam->intValue, intValue, sizeof(intValue));
    newParam->floatValue = floatValue;
    newParam->matrixValue = matrixValue;
    newParam->set(textureValue);
    newParam->enumValue.setString(enumValue);
    newParam->sourceImageID.setString(sourceImageID);
    newParam->sourceSurfaceID.setString(sourceSurfaceID);

    // Return the cloned parameter
    return newParam;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(bool b1)
{
    boolValue[0] = b1;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(bool b1, bool b2)
{
    boolValue[0] = b1;
    boolValue[1] = b2;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(bool b1, bool b2, bool b3)
{
    boolValue[0] = b1;
    boolValue[1] = b2;
    boolValue[2] = b3;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(bool b1, bool b2, bool b3, bool b4)
{
    boolValue[0] = b1;
    boolValue[1] = b2;
    boolValue[2] = b3;
    boolValue[3] = b4;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(int i1)
{
    intValue[0] = i1;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(int i1, int i2)
{
    intValue[0] = i1;
    intValue[1] = i2;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(int i1, int i2, int i3)
{
    intValue[0] = i1;
    intValue[1] = i2;
    intValue[2] = i3;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(int i1, int i2, int i3, int i4)
{
    intValue[0] = i1;
    intValue[1] = i2;
    intValue[2] = i3;
    intValue[3] = i4;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(double f1)
{
    floatValue.setSize(1);
    floatValue[0] = f1;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(double f1, double f2)
{
    floatValue.set(f1, f2);
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(double f1, double f2, double f3)
{
    floatValue.set(f1, f2, f3);
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(double f1, double f2, double f3, double f4)
{
    floatValue.set(f1, f2, f3, f4);
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(atVector vec)
{
    floatValue = vec;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new given value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(atMatrix mat)
{
    // Copy the matrix.  We assume the input matrix has all the necessary
    // values set (regardless of the dimensions)
    matrixValue = mat;
}

// ------------------------------------------------------------------------
// Sets the parameter to the new enumerated value (as a string)
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(atString enumStr)
{
    enumValue.setString(enumStr);
}

// ------------------------------------------------------------------------
// Sets the parameter to the new texture value
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(vsTextureAttribute *tex)
{
    // Unref the old texture (if any)
    if (textureValue != NULL)
        vsObject::unrefDelete(textureValue);

    // Copy and ref the new texture
    textureValue = tex;
    if (tex != NULL)
        textureValue->ref();
}

// ------------------------------------------------------------------------
// Sets the parameter based on the contents of the given XML subtree.
// This expects a node like <bool>, <float3>, or <sampler2D> somewhere
// under the given node
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::set(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr valueNode;
    atString text;
    atStringTokenizer *tokens;

    valueNode = current;
    while (valueNode != NULL)
    {
        // Look for a recognized value type
        if (strcmp(doc->getNodeName(valueNode), "bool") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the boolean value
            boolValue[0] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "bool2") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the boolean values
            boolValue[0] = getBoolToken(tokens);
            boolValue[1] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "bool3") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the boolean values
            boolValue[0] = getBoolToken(tokens);
            boolValue[1] = getBoolToken(tokens);
            boolValue[2] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "bool4") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the boolean values
            boolValue[0] = getBoolToken(tokens);
            boolValue[1] = getBoolToken(tokens);
            boolValue[2] = getBoolToken(tokens);
            boolValue[3] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "int") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the integer values
            intValue[0] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "int2") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the integer values
            intValue[0] = getBoolToken(tokens);
            intValue[1] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "int3") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the integer values
            intValue[0] = getBoolToken(tokens);
            intValue[1] = getBoolToken(tokens);
            intValue[2] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "int4") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the integer values
            intValue[0] = getBoolToken(tokens);
            intValue[1] = getBoolToken(tokens);
            intValue[2] = getBoolToken(tokens);
            intValue[3] = getBoolToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "float") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the float values
            floatValue.setSize(1);
            floatValue[0] = getFloatToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "float2") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the float values
            floatValue.setSize(2);
            floatValue[0] = getFloatToken(tokens);
            floatValue[1] = getFloatToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "float3") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the float values
            floatValue.setSize(3);
            floatValue[0] = getFloatToken(tokens);
            floatValue[1] = getFloatToken(tokens);
            floatValue[2] = getFloatToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "float4") == 0)
        {
            // Get the text under this node and tokenize it
            text = atString(doc->getNodeText(
                doc->getNextChildNode(valueNode)));
            tokens = new atStringTokenizer(text);

            // Parse and set the float values
            floatValue.setSize(4);
            floatValue[0] = getFloatToken(tokens);
            floatValue[1] = getFloatToken(tokens);
            floatValue[2] = getFloatToken(tokens);
            floatValue[3] = getFloatToken(tokens);
            delete tokens;
        }
        else if (strcmp(doc->getNodeName(valueNode), "surface") == 0)
        {
            // Process the surface
            processSurface(doc, valueNode);
        }
        else if (strcmp(doc->getNodeName(valueNode), "sampler2D") == 0)
        {
            // Process the 2D image sampler
            processSampler2D(doc, valueNode);
        }

        // Try the next node
        valueNode = doc->getNextSiblingNode(valueNode);
    }
}

// ------------------------------------------------------------------------
// Return the first boolean parameter value
// ------------------------------------------------------------------------
bool vsCOLLADAEffectParameter::getBool()
{
    return boolValue[0];
}

// ------------------------------------------------------------------------
// Return the requested boolean parameter value
// ------------------------------------------------------------------------
bool vsCOLLADAEffectParameter::getBool(int index)
{
    return boolValue[index];
}

// ------------------------------------------------------------------------
// Return the requested int parameter value
// ------------------------------------------------------------------------
int vsCOLLADAEffectParameter::getInt()
{
    return intValue[0];
}

// ------------------------------------------------------------------------
// Return the requested int parameter value
// ------------------------------------------------------------------------
int vsCOLLADAEffectParameter::getInt(int index)
{
    return intValue[index];
}

// ------------------------------------------------------------------------
// Return the requested int parameter value
// ------------------------------------------------------------------------
double vsCOLLADAEffectParameter::getFloat()
{
    return floatValue[0];
}

// ------------------------------------------------------------------------
// Return the requested int parameter value
// ------------------------------------------------------------------------
atVector vsCOLLADAEffectParameter::getVector()
{
    return floatValue;
}

// ------------------------------------------------------------------------
// Return the requested int parameter value
// ------------------------------------------------------------------------
atMatrix vsCOLLADAEffectParameter::getMatrix()
{
    return matrixValue;
}

// ------------------------------------------------------------------------
// Return the requested texture attribute
// ------------------------------------------------------------------------
vsTextureAttribute *vsCOLLADAEffectParameter::getTexture()
{
    return textureValue;
}

// ------------------------------------------------------------------------
// Set the source image ID for this parameter (used only for surface
// parameters)
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::setSourceImageID(atString source)
{
   sourceImageID.setString(source);
}

// ------------------------------------------------------------------------
// Get the ID of the source image assigned to this parameter
// ------------------------------------------------------------------------
atString vsCOLLADAEffectParameter::getSourceImageID()
{
   return sourceImageID;
}

// ------------------------------------------------------------------------
// Set the source surface parameter ID for this parameter (used only for
// sampler parameters)
// ------------------------------------------------------------------------
void vsCOLLADAEffectParameter::setSourceSurfaceID(atString source)
{
   sourceSurfaceID.setString(source);
}

// ------------------------------------------------------------------------
// Get the ID of the source surface assigned to this parameter
// ------------------------------------------------------------------------
atString vsCOLLADAEffectParameter::getSourceSurfaceID()
{
   return sourceSurfaceID;
}

// ------------------------------------------------------------------------
// Return the requested enumerated parameter value
// ------------------------------------------------------------------------
atString vsCOLLADAEffectParameter::getEnum()
{
    return enumValue;
}

