
#include "vsCOLLADAAnimation.h++"
#include "vsCOLLADASampler.h++"

#include "atStringTokenizer.h++"

#include <string.h>

// ------------------------------------------------------------------------
// Creates a vsCOLLADAAnimation object
// ------------------------------------------------------------------------
vsCOLLADAAnimation::vsCOLLADAAnimation(atString id, atXMLDocument *doc,
                                       atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;
    char tempStr[32];
    atString name;
    atString *mapID;
    atMap *samplers;
    vsCOLLADASampler *sampler;
    vsCOLLADAChannel *channel;

    // Set the animation's ID
    animationID.setString(id);

    // Create the map that will hold our data sources
    sources = new atMap();

    // Create a map to store the samplers
    samplers = new atMap();

    // Create a list to store the animation channels
    channels = new atList();

    // Parse the data sources first
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // Process this node if it's a data source node
        if (strcmp(doc->getNodeName(child), "source") == 0)
            processSource(doc, child);

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }

    // Next, parse the samplers
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a sampler node
        if (strcmp(doc->getNodeName(child), "sampler") == 0)
        {
            // Create the sampler from the XML subtree and the available
            // data sources
            sampler = new vsCOLLADASampler(doc, child, sources);

            // See if it's valid
            if (sampler->isValid())
            {
                // Add the sampler to the samplers map
                mapID = new atString(sampler->getID());
                samplers->addEntry(mapID, sampler);
            }
            else
            {
                // Delete the invalid sampler
                delete sampler;
            }
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }

    // Finally, parse the animation channels
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is a channel node
        if (strcmp(doc->getNodeName(child), "channel") == 0)
        {
            // Create the channel from the XML subtree and the samplers we've
            // created
            channel = new vsCOLLADAChannel(doc, child, samplers);

            // See if it's valid
            if (channel->isValid())
            {
                // Add the channel to the channels list
                channels->addEntry(channel);
            }
            else
            {
                // Delete the invalid channel
                delete channel;
            }
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }

    // Now, see if this animation has any children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an animation node
        if (strcmp(doc->getNodeName(child), "animation") == 0)
        {
            // JPD:  Child animations not supported yet.  We don't have a
            //       way to represent them in VESS, and I haven't yet come
            //       across a document that uses them.  I expect this will
            //       change, though, as the nested animation concept is
            //       mentioned in detail in the COLLADA spec.
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }

    // We're done with the samplers now.  It is not permitted to use
    // a sampler to drive several channels (they are always used in
    // pairs).  We've encoded all of the samplers' information into the
    // channel objects, so we're safe to delete the samplers now.
    delete samplers;
    samplers = NULL;
}

// ------------------------------------------------------------------------
// Destroys this COLLADA animation object
// ------------------------------------------------------------------------
vsCOLLADAAnimation::~vsCOLLADAAnimation()
{
    // Delete the data sources
    delete sources;

    // Delete the channels
    delete channels;
}

// ------------------------------------------------------------------------
// Processes a <source> XML subtree and creates a COLLADA data source for
// it
// ------------------------------------------------------------------------
void vsCOLLADAAnimation::processSource(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current)
{
    vsCOLLADADataSource *source;
    atString *sourceID;

    // Create the source
    source = new vsCOLLADADataSource(doc, current);

    // If the source has useful data, add it to our data source map
    if (source->getDataCount() > 0)
    {
        sourceID = new atString(source->getID());
        sources->addEntry(sourceID, source);
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAAnimation::getClassName()
{
    return "vsCOLLADAAnimation";
}

// ------------------------------------------------------------------------
// Return the identifier for this animation object
// ------------------------------------------------------------------------
atString vsCOLLADAAnimation::getID()
{
    return animationID.clone();
}

// ------------------------------------------------------------------------
// Return the number of animation channels contained in this animation
// ------------------------------------------------------------------------
int vsCOLLADAAnimation::getNumChannels()
{
    return channels->getNumEntries();
}

// ------------------------------------------------------------------------
// Return the requested animation channel by index
// ------------------------------------------------------------------------
vsCOLLADAChannel *vsCOLLADAAnimation::getChannel(int index)
{
    return (vsCOLLADAChannel *)channels->getNthEntry(index);
}

// ------------------------------------------------------------------------
// Return the requested animation channel by index
// ------------------------------------------------------------------------
int vsCOLLADAAnimation::getNumChildren()
{
    return children->getNumEntries();
}

// ------------------------------------------------------------------------
// Return the requested animation channel by index
// ------------------------------------------------------------------------
vsCOLLADAAnimation *vsCOLLADAAnimation::getChild(int index)
{
    return (vsCOLLADAAnimation *)children->getNthEntry(index);
}

