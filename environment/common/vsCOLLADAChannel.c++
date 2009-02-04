
#include "vsCOLLADAChannel.h++"

#include "atStringTokenizer.h++"

// ------------------------------------------------------------------------
// Create an animation channel using the given XML subtree and sampler data
// ------------------------------------------------------------------------
vsCOLLADAChannel::vsCOLLADAChannel(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current,
                                   atMap *samplers)
{
    char *attr;
    atString chanSourceID;
    atString chanTargetID;
    atStringTokenizer *tokenizer;
    atString *token;
    
    // Get the channel source (a sampler's ID)
    attr = doc->getNodeAttribute(current, "source");
    if (attr != NULL)
        chanSourceID.setString(attr);

    // Get the channel target (a node ID and transform SID)
    attr = doc->getNodeAttribute(current, "target");

    // Make sure the target is specified
    if (attr != NULL)
    {
        // Parse the target string into the separate node ID/transform SID
        // components
        chanTargetID.setString(attr);
        tokenizer = new atStringTokenizer(chanTargetID);

        // Get the target node ID
        token = tokenizer->getToken("/");
        targetNodeID.setString(*token);
        delete token;

        // Get the target transform SID
        token = tokenizer->getToken("/");
        targetXformSID.setString(*token);
        delete token;

        // Done with the tokenizer
        delete tokenizer;

        // Get the sampler object by ID
        sampler = getSampler(samplers, chanSourceID);
    
        // Make sure we got a valid sampler
        if ((sampler != NULL) && (sampler->isValid()))
        {
            // Reference the sampler object
            sampler->ref();

            // Mark the channel as valid
            validFlag = true;
        }
        else
        {
            // Mark the channel as invalid
            validFlag = false;
        }
    }
    else
    {
        // Mark the channel as invalid
        validFlag = false;
    }
}

// ------------------------------------------------------------------------
// Clean up the animation channel data
// ------------------------------------------------------------------------
vsCOLLADAChannel::~vsCOLLADAChannel()
{
    // Unreference (and most likely delete) the sampler
    if (sampler != NULL)
        vsObject::unrefDelete(sampler);
}

// ------------------------------------------------------------------------
// Return the sampler from the given map specified by the given ID
// ------------------------------------------------------------------------
vsCOLLADASampler *vsCOLLADAChannel::getSampler(atMap *samplers, atString id)
{
    char *idStr;
    atString newID;

    // Check the ID string to see what kind of URI this is
    idStr = id.getString();
    if (idStr[0] == '#')
    {
        // This is a URI fragment, meaning we should look for the sampler
        // in our local scope
        newID.setString(&idStr[1]);

        // Look in the dataSources map for the source and return it
        return (vsCOLLADASampler *)samplers->getValue(&newID);
    }
    else
    {
        // Other URI forms are forbidden here
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAChannel::getClassName()
{
    return "vsCOLLADAChannel";
}

// ------------------------------------------------------------------------
// Return whether or not this animation channel is valid
// ------------------------------------------------------------------------
bool vsCOLLADAChannel::isValid()
{
    return validFlag;
}

// ------------------------------------------------------------------------
// Return the number of keyframes in this channel
// ------------------------------------------------------------------------
int vsCOLLADAChannel::getNumKeyframes()
{
    return sampler->getNumKeyframes();
}

// ------------------------------------------------------------------------
// Return the first keyframe in this channel
// ------------------------------------------------------------------------
vsCOLLADAKeyframe *vsCOLLADAChannel::getFirstKeyframe()
{
    return sampler->getFirstKeyframe();
}

// ------------------------------------------------------------------------
// Return the next keyframe in this channel
// ------------------------------------------------------------------------
vsCOLLADAKeyframe *vsCOLLADAChannel::getNextKeyframe()
{
    return sampler->getNextKeyframe();
}

// ------------------------------------------------------------------------
// Return the keyframe at the given index
// ------------------------------------------------------------------------
vsCOLLADAKeyframe *vsCOLLADAChannel::getKeyframe(int index)
{
    return sampler->getKeyframe(index);
}

// ------------------------------------------------------------------------
// Return the position interpolation mode
// ------------------------------------------------------------------------
vsPathPosInterpolationMode vsCOLLADAChannel::getPositionInterpMode()
{
    return sampler->getPositionInterpMode();
}

// ------------------------------------------------------------------------
// Return the orientation interpolation mode
// ------------------------------------------------------------------------
vsPathOrientInterpolationMode vsCOLLADAChannel::getOrientationInterpMode()
{
    return sampler->getOrientationInterpMode();
}

// ------------------------------------------------------------------------
// Return the ID of the node that is the target of this animation channel
// ------------------------------------------------------------------------
atString vsCOLLADAChannel::getTargetNodeID()
{
    return targetNodeID;
}

// ------------------------------------------------------------------------
// Return the SID of the transform that is the target of this animation
// channel
// ------------------------------------------------------------------------
atString vsCOLLADAChannel::getTargetXformSID()
{
    return targetXformSID;
}

