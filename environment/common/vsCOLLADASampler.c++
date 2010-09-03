
#include "vsCOLLADASampler.h++"

// ------------------------------------------------------------------------
// Construct a COLLADA animation sampler from the given XML subtree (using
// the given data sources)
// ------------------------------------------------------------------------
vsCOLLADASampler::vsCOLLADASampler(atXMLDocument *doc, 
                                   atXMLDocumentNodePtr current,
                                   atMap *sources)
{
    char *attr;
    atXMLDocumentNodePtr child;
    vsCOLLADADataSource *source;
    bool foundInput;
    bool foundOutput;
    bool foundInterp;

    // Start out assuming this sampler is invalid
    validFlag = false;

    // Create the list of keyframes
    keyframes = new atList();

    // Get the sampler's ID
    attr = doc->getNodeAttribute(current, "id");
    if (attr != NULL)
        samplerID.setString(attr);

    // Parse the sampler
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an input node (the only node type we care about)
        if (strcmp(doc->getNodeName(child), "input") == 0)
        {
            // Check the semantic on the input
            attr = doc->getNodeAttribute(child, "semantic");
            if (strcmp(attr, "INPUT") == 0)
            {
                // Get the data source
                attr = doc->getNodeAttribute(child, "source");
                source = getDataSource(sources, atString(attr));

                // Process the input source
                foundInput = processSamplerInput(source);
            }
            else if (strcmp(attr, "OUTPUT") == 0)
            {
                // Get the data source
                attr = doc->getNodeAttribute(child, "source");
                source = getDataSource(sources, atString(attr));

                // Process the output source
                foundOutput = processSamplerOutput(source);
            }
            else if (strcmp(attr, "INTERPOLATION") == 0)
            {
                // Get the data source
                attr = doc->getNodeAttribute(child, "source");
                source = getDataSource(sources, atString(attr));

                // Process the interpolation source
                foundInterp = processSamplerInterpolation(source);
            }
            else if (strcmp(attr, "IN_TANGENT") == 0)
            {
                // Tangents are needed for BEZIER and HERMITE interpolation
                // which we don't currently support
            }
            else if (strcmp(attr, "OUT_TANGENT") == 0)
            {
                // Tangents are needed for BEZIER and HERMITE interpolation
                // which we don't currently support
            }
        }

        //  Try the next node
        child = doc->getNextSiblingNode(child);
    }

    // Update the valid flag (we're valid if there is input data, output data,
    // and a way to interpolate)
    validFlag = (foundInput && foundOutput && foundInterp);
}

// ------------------------------------------------------------------------
// Clean up the sampler
// ------------------------------------------------------------------------
vsCOLLADASampler::~vsCOLLADASampler()
{
    // Delete the keyframes list
    delete keyframes;
}

// ------------------------------------------------------------------------
// Returns the data source specified by the given identifier
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADASampler::getDataSource(atMap *sources,
                                                     atString id)
{
    char *idStr; 
    atString newID;

    // Check the ID string to see what kind of URI this is
    idStr = id.getString();
    if (idStr[0] == '#')
    {
        // This is a URI fragment, meaning the source is local to this
        // COLLADA file.  We should already have the source in our data 
        // source map, so we should only need to strip the leading '#' and
        // look up the ID
        newID.setString(&idStr[1]);

        // Look in the map of data sources for the desired source and return
        // it
        return (vsCOLLADADataSource *)sources->getValue(&newID);
    }
    else
    {
        // Other URI forms aren't currently supported
        return NULL;
    }
}   

// ------------------------------------------------------------------------
// Process the input data to an animation sampler
// ------------------------------------------------------------------------
bool vsCOLLADASampler::processSamplerInput(vsCOLLADADataSource *source)
{
    bool validInput;
    int paramCount;
    atString paramName;
    vsCOLLADADataSourceFormat dataFormat;
    int dataCount;
    vsCOLLADAKeyframe *keyframe;
    int i;
    double timeValue;

    // Start out assuming no valid input
    validInput = false;

    // Make sure we have a valid source
    if (source != NULL)
    {
        // Make sure the source is called "TIME", and that it
        // is a single int or float (we don't support animating to
        // anything other than a time input)
        paramCount = source->getParamCount();
        paramName = source->getParamName(0);
        dataFormat = source->getDataFormat();
        if ((paramCount == 1) &&
            (strcmp(paramName.getString(), "TIME") == 0) &&
            ((dataFormat == VS_CDS_FLOAT) || (dataFormat == VS_CDS_INT)))
        {
            // Mark that we found valid input data
            validInput = true;

            // Get the count of input data elements, and allocate
            // the keyframe array if necessary
            dataCount = source->getDataCount();

            // Copy the time indices into the keyframe array
            for (i = 0; i < dataCount; i++)
            {
                // Create a new keyframe
                keyframe = new vsCOLLADAKeyframe();

                // Be sure to ask for the appropriate data type
                if (dataFormat == VS_CDS_FLOAT)
                    timeValue = source->getFloat(i);
                else
                    timeValue = (double)source->getInt(i);

                // Make sure the time is positive
                if (timeValue < 0)
                {
                    // Only print this error message once
                    if (validInput)
                    {
                        printf("vsCOLLADASampler::processSamplerInput:"
                            "  Negative time in animation input\n");

                        // Mark that we don't have valid input
                        validInput = false;
                    }
                }
                else
                {
                    // Set the keyframe's time
                    keyframe->setTime(timeValue);

                    // Add the keyframe to the list, unless we've encountered
                    // bad input
                    if (validInput)
                        keyframes->addEntry(keyframe);
                }
            }
        }
        else
        {
            printf("vsCOLLADASampler::processSamplerInput:  "
                "Animation input must be TIME (int or float)\n");
        }
    }
    else
    {
        printf("vsCOLLADASampler::processSamplerInput:  "
            "Animation input data missing\n");
    }

    // Return whether or not we got valid input
    return validInput;
}

// ------------------------------------------------------------------------
// Process the output data from an animation sampler
// ------------------------------------------------------------------------
bool vsCOLLADASampler::processSamplerOutput(vsCOLLADADataSource *source)
{
    bool validOutput;
    int dataCount; 
    vsCOLLADADataSourceFormat dataFormat;
    int i, j, k;
    atVector dataVec;
    atMatrix dataMat;
    vsCOLLADAKeyframe *keyframe;
    int dataSize;
    double dataValues[16];

    // Start out assuming no valid output
    validOutput = false;

    // Make sure we have a valid source
    if (source != NULL)
    {
        // Mark that we found valid output data
        validOutput = true;

        // Get the count of output data elements
        dataCount = source->getDataCount();

        // Get the first keyframe from the list (the output data elements
        // correspond one-for-one with the elements in the keyframe list)
        keyframe = (vsCOLLADAKeyframe *)keyframes->getFirstEntry();

        // Iterate over the output data items
        i = 0;
        while ((i < dataCount) && (validOutput))
        {
            // Be sure to ask for the appropriate data type
            dataFormat = source->getDataFormat();
            switch (dataFormat)
            {
                case VS_CDS_INT:

                    // Only one data element
                    dataSize = 1;
                    dataValues[0] = source->getInt(i);
                    break;

                case VS_CDS_FLOAT:

                    // Only one data element
                    dataSize = 1;
                    dataValues[0] = source->getFloat(i);
                    break;

                case VS_CDS_VECTOR:

                    // Get the vector from the data source
                    dataVec = source->getVector(i);

                    // Copy the vector's elements
                    dataSize = dataVec.getSize();
                    for (j = 0; j < dataSize; j++)
                        dataValues[j] = dataVec[j];
                    break;

                case VS_CDS_MATRIX:

                    // Get the matrix from the data source
                    dataMat = source->getMatrix(i);

                    // Set the data size
                    dataSize = 16;

                    // Copy the matrix's elements
                    for (j = 0; j < 4; j++)
                        for (k = 0; k < 4; k++)
                            dataValues[j*4+k] = dataMat[j][k];
                    break;

                default:

                    // The data format isn't valid for animation output
                    printf("vsCOLLADASampler::processSamplerOutput: "
                        "  Invalid output data format\n");

                    // Mark the output data as invalid
                    validOutput = false; 
                    break;
            }

            // If we don't have a keyframe to put this data in, we can't
            // have valid output
            if (keyframe == NULL)
                validOutput = false;

            // Make sure we're still getting valid output
            if (validOutput)
            {
                // Set the current keyframe's data value
                keyframe->setData(dataSize, dataValues);

                // Get the next keyframe and output item
                keyframe = (vsCOLLADAKeyframe *)keyframes->getNextEntry();
                i++;
            }
        }
    }
    else
    {
        printf("vsCOLLADASampler::processSampler:  ",
            "Animation output data missing\n");
    }

    // Return whether or not we have valid output
    return validOutput;
}

// ------------------------------------------------------------------------
// Process the interpolation mode for an animation sampler
// ------------------------------------------------------------------------
bool vsCOLLADASampler::processSamplerInterpolation(vsCOLLADADataSource *source)
{
    bool validInterpolation;
    atString mode;

    // Interpolations are converted to vsPathMotion modes
    // as follows:
    //
    //     COLLADA     vsPathMotion (position)   (orientation)
    //     ---------------------------------------------------
    //     STEP                      NONE           NONE
    //     LINEAR                    LINEAR         NLERP
    //     CARDINAL                  SPLINE         SPLINE
    //     BEZIER                    SPLINE         SPLINE
    //     HERMITE                   SPLINE         SPLINE
    //     BSPLINE                   SPLINE         SPLINE
    //
    // Note that vsPathMotion's SPLINE mode uses Catmull-Rom
    // splines, which are a specific case of cardinal splines
    // (a Catmull-Rom spline is a cardinal spline with its
    // tension constant set to zero).  Because COLLADA allows the
    // runtime to specify the Cardinal spline's constant, VESS is
    // actually fully compliant with COLLADA's specification for
    // Cardinal spline interpolation.
    //
    // BEZIER, HERMITE, and BSPLINE modes are not accurately
    // supported at this time (we simply use Catmull-Rom
    // interpolation for these modes as well).
    //
    // Also note that VESS doesn't support changing interpolation
    // modes on the fly (the interpolation mode at the first
    // keyframe is used throughout the animation)

    // Read the interpolation mode
    if (source != NULL)
    {
        // Assume we have a valid interpolation mode at first
        validInterpolation = true;

        // Translate the mode to the appropriate vsPathMotion interpolation
        // modes (see comments above)
        mode = source->getString(0);
        if (strcmp(mode.getString(), "STEP") == 0)
        { 
            // STEP means no interpolation
            positionInterp = VS_PATH_POS_IMODE_NONE;
            orientationInterp = VS_PATH_ORI_IMODE_NONE;
        }  
        else if (strcmp(mode.getString(), "LINEAR") == 0)
        {  
            // LINEAR translates to linear interpolation on positions,
            // and NLERP (Normalized Linear intERPolation) on orientations.
            // Since we're interpolating between sample points on an animation
            // curve, each orientation sample isn't likely to be that different
            // from the last.  This means that the velocity distortion from
            // the non-spherical interpolation is not likely to be noticeable,
            // and nlerp is much cheaper to compute than slerp.  Also, unlike
            // slerp, nlerp is commutative, so it's easier to blend multiple
            // animation curves together.
            positionInterp = VS_PATH_POS_IMODE_LINEAR;
            orientationInterp = VS_PATH_ORI_IMODE_NLERP;
        }
        else if ((strcmp(mode.getString(), "CARDINAL") == 0) ||
                 (strcmp(mode.getString(), "BEZIER") == 0) ||
                 (strcmp(mode.getString(), "HERMITE") == 0) ||
                 (strcmp(mode.getString(), "BSPLINE") == 0))
        {
            // All spline curves translate to Catmull-Rom splines
            // (see comments above for a discussion on this)
            positionInterp = VS_PATH_POS_IMODE_SPLINE;
            orientationInterp = VS_PATH_ORI_IMODE_SPLINE;
        }
        else
        {
            // This is an invalid interpolation mode
            printf("vsCOLLADASampler::processSamplerInterpolation:\n");
            printf("  Invalid interpolation mode (%s)\n", mode.getString());
            validInterpolation = false;
        }
    }

    // Return whether or not we have a valid interpolation mode
    return validInterpolation;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADASampler::getClassName()
{
    return "vsCOLLADASampler";
}

// ------------------------------------------------------------------------
// Return whether or not this sampler is valid
// ------------------------------------------------------------------------
bool vsCOLLADASampler::isValid()
{
    return validFlag;
}

// ------------------------------------------------------------------------
// Return the identifier of this sampler
// ------------------------------------------------------------------------
atString vsCOLLADASampler::getID()
{
    return samplerID;
}

// ------------------------------------------------------------------------
// Return the number of keyframes in this sampler
// ------------------------------------------------------------------------
int vsCOLLADASampler::getNumKeyframes()
{
    return keyframes->getNumEntries();
}

// ------------------------------------------------------------------------
// Return the first keyframe in this sampler
// ------------------------------------------------------------------------
vsCOLLADAKeyframe *vsCOLLADASampler::getFirstKeyframe()
{
    return (vsCOLLADAKeyframe *)keyframes->getFirstEntry();
}

// ------------------------------------------------------------------------
// Return the next keyframe in this sampler
// ------------------------------------------------------------------------
vsCOLLADAKeyframe *vsCOLLADASampler::getNextKeyframe()
{
    return (vsCOLLADAKeyframe *)keyframes->getNextEntry();
}

// ------------------------------------------------------------------------
// Return the requested keyframe
// ------------------------------------------------------------------------
vsCOLLADAKeyframe *vsCOLLADASampler::getKeyframe(int index)
{
    return (vsCOLLADAKeyframe *)keyframes->getNthEntry(index);
}

// ------------------------------------------------------------------------
// Return the method of interpolation for the position
// ------------------------------------------------------------------------
vsPathPosInterpolationMode vsCOLLADASampler::getPositionInterpMode()
{
    return positionInterp;
}

// ------------------------------------------------------------------------
// Return the method of interpolation for the orientation
// ------------------------------------------------------------------------
vsPathOrientInterpolationMode vsCOLLADASampler::getOrientationInterpMode()
{
    return orientationInterp;
}

