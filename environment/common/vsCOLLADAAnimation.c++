
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
    atString name;
    atString *mapID;
    atMap *samplers;
    vsCOLLADASampler *sampler;
    vsCOLLADAChannel *channel;
    atString childID;
    char tmpID[1024];
    vsCOLLADAAnimation *childAnim;
    atList *idList;
    atList *samplerList;
    atString *samplerID;

    // Set the animation's ID
    animationID.setString(id);

    // Create the map that will hold our data sources
    sources = new atMap();

    // Create a map to store the samplers
    samplers = new atMap();

    // Create a list to store the animation channels
    channels = new atList();

    // Create a list to store our child animations
    children = new atList();

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
                sampler->ref();
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
                // Reference the channel and add it to the channels list
                channel->ref();
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

    // We're done with the map of samplers, so clean it up now.  First,
    // get lists representing the data in the map of samplers we created
    idList = new atList();
    samplerList = new atList();
    samplers->getSortedList(idList, samplerList);

    // Unref/delete the samplers in the map
    samplerID = (atString *)idList->getFirstEntry();
    sampler = (vsCOLLADASampler *)samplerList->getFirstEntry();
    while (sampler != NULL)
    {
        // Remove the entry from the map and the two lists
        samplers->removeEntry(samplerID);
        idList->removeCurrentEntry();
        samplerList->removeCurrentEntry();

        // Delete the ID
        delete samplerID;

        // Unref/delete the sampler
        vsObject::unrefDelete(sampler);

        // Get the next id/sampler pair
        samplerID = (atString *)idList->getNextEntry();
        sampler = (vsCOLLADASampler *)samplerList->getNextEntry();
    }
    
    // Delete the lists and the samplers map
    delete idList;
    delete samplerList;
    delete samplers;

    // Now, see if this animation has any children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an animation node
        if (strcmp(doc->getNodeName(child), "animation") == 0)
        {
            // Get the animation's ID
            attr = doc->getNodeAttribute(child, "id");
            if (attr == NULL)
            {
                // Create an ID based on the parent's ID and the index of
                // this child
                sprintf(tmpID, "%s_%d", animationID.getString(), 
                    children->getNumEntries());
                childID = atString(tmpID);
            }

            // Get the child animation, passing our data sources so it can
            // reference them
            childAnim = new vsCOLLADAAnimation(childID, doc, child, sources);

            // Add to our list
            childAnim->ref();
            children->addEntry(childAnim);
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Creates a child vsCOLLADAAnimation object, using the list of channels
// from the parent animation
// ------------------------------------------------------------------------
vsCOLLADAAnimation::vsCOLLADAAnimation(atString id, atXMLDocument *doc,
                                       atXMLDocumentNodePtr current,
                                       atMap *parentSources)
{
    atList *idList;
    atList *sourceList;
    atXMLDocumentNodePtr child;
    atString *sourceID;
    vsCOLLADADataSource *source;
    char *attr;
    atString name;
    atString *mapID;
    atMap *samplers;
    vsCOLLADASampler *sampler;
    vsCOLLADAChannel *channel;
    atString childID;
    char tmpID[1024];
    vsCOLLADAAnimation *childAnim;
    atList *samplerList;
    atString *samplerID;

    // Set the animation's ID
    animationID.setString(id);

    // Create the map that will hold our data sources
    sources = new atMap();

    // Get lists representing the data in the map of data sources inherited
    // from the parent animation
    idList = new atList();
    sourceList = new atList();
    parentSources->getSortedList(idList, sourceList);

    // Add our parent's sources to our channel map
    sourceID = (atString *)idList->getFirstEntry();
    source = (vsCOLLADADataSource *)sourceList->getFirstEntry();
    while (source != NULL)
    {
        // Create a duplicate of the source ID
        mapID = new atString(*sourceID);

        // Reference the source, and add it to our own source map
        source->ref();
        sources->addEntry(mapID, source);

        // Get the next id/source pair
        sourceID = (atString *)idList->getNextEntry();
        source = (vsCOLLADADataSource *)sourceList->getNextEntry();
    }

    // Clean up the lists (but leave the members intact, as they're still
    // living in the maps)
    idList->removeAllEntries();
    delete idList;
    sourceList->removeAllEntries();
    delete sourceList;

    // Create a map to store the samplers
    samplers = new atMap();

    // Create a list to store the animation channels
    channels = new atList();

    // Create a list to store our child animations
    children = new atList();

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
                sampler->ref();
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
                // Reference the channel and add it to the channels list
                channel->ref();
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

    // We're done with the map of samplers, so clean it up now.  First,
    // get lists representing the data in the map of samplers we created
    idList = new atList();
    samplerList = new atList();
    samplers->getSortedList(idList, samplerList);

    // Unref/delete the samplers in the map
    samplerID = (atString *)idList->getFirstEntry();
    sampler = (vsCOLLADASampler *)samplerList->getFirstEntry();
    while (sampler != NULL)
    {
        // Remove the entry from the map and the two lists
        samplers->removeEntry(samplerID);
        idList->removeCurrentEntry();
        samplerList->removeCurrentEntry();

        // Delete the ID
        delete samplerID;

        // Unref/delete the sampler
        vsObject::unrefDelete(sampler);

        // Get the next id/sampler pair
        samplerID = (atString *)idList->getNextEntry();
        sampler = (vsCOLLADASampler *)samplerList->getNextEntry();
    }
    
    // Delete the lists and the samplers map
    delete idList;
    delete samplerList;
    delete samplers;

    // Now, see if this animation has any children
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is an animation node
        if (strcmp(doc->getNodeName(child), "animation") == 0)
        {
            // Get the animation's ID
            attr = doc->getNodeAttribute(child, "id");
            if (attr == NULL)
            {
                // Create an ID based on the parent's ID and the index of
                // this child
                sprintf(tmpID, "%s_%d", animationID.getString(), 
                    children->getNumEntries());
                childID = atString(tmpID);
            }

            // Get the child animation, passing our list of channels
            childAnim = new vsCOLLADAAnimation(childID, doc, child, sources);

            // Add to our list
            childAnim->ref();
            children->addEntry(childAnim);
        }

        // Move on to the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Destroys this COLLADA animation object
// ------------------------------------------------------------------------
vsCOLLADAAnimation::~vsCOLLADAAnimation()
{
    atList *idList;
    atList *sourceList;
    atString *sourceID;
    vsCOLLADADataSource *source;
    vsCOLLADAChannel *channel;
    vsCOLLADAAnimation *child;

    // Get lists representing the data in the map of data sources inherited
    // from the parent animation
    idList = new atList();
    sourceList = new atList();
    sources->getSortedList(idList, sourceList);

    // Unref/delete the data sources in the sources map
    sourceID = (atString *)idList->getFirstEntry();
    source = (vsCOLLADADataSource *)sourceList->getFirstEntry();
    while (source != NULL)
    {
        // Remove the entry from the map and the two lists
        sources->removeEntry(sourceID);
        idList->removeCurrentEntry();
        sourceList->removeCurrentEntry();

        // Delete the ID
        delete sourceID;

        // Unref/delete the source
        vsObject::unrefDelete(source);

        // Get the next id/source pair
        sourceID = (atString *)idList->getNextEntry();
        source = (vsCOLLADADataSource *)sourceList->getNextEntry();
    }
    
    // Delete the lists and the sources map
    delete idList;
    delete sourceList;
    delete sources;

    // Unref/delete the channels in the channel list
    channel = (vsCOLLADAChannel *)channels->getFirstEntry();
    while (channel != NULL)
    {
        // Remove the channel from the list
        channels->removeCurrentEntry();

        // Unreference (maybe delete) the channel
        vsObject::unrefDelete(channel);

        // Next channel
        channel = (vsCOLLADAChannel *)channels->getNextEntry();
    }

    // Delete the channel list
    delete channels;

    // Unref/delete the child animations in the list of children
    child = (vsCOLLADAAnimation *)children->getFirstEntry();
    while (child != NULL)
    {
        // Remove the channel from the list
        children->removeCurrentEntry();

        // Unreference (maybe delete) the channel
        vsObject::unrefDelete(child);

        // Next channel
        child = (vsCOLLADAAnimation *)children->getNextEntry();
    }

    // Delete the list of child animations
    delete children;
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
    else
    {
        // Delete the invalid source
        delete source;
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

