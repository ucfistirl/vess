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
//    VESS Module:  vsCallbackList.c++
//
//    Description:  This class manages a list of callback functions used
//                  by a Performer process
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsCallbackList.h++"

// ------------------------------------------------------------------------
// Constructor - Saves the channel for future use, makes callback lists for
// the default cull and draw callbacks, and sets their traversal functions
// ------------------------------------------------------------------------
vsCallbackList::vsCallbackList(pfChannel *callbackChannel)
{
    vsCallbackNode *defaultDrawNode;

    // Store the callback channel
    channel = callbackChannel;

    // Set up the default clear mask, which will be passed along as user data
    glClearMask = (int *)pfMalloc(sizeof(int), pfGetSharedArena());
    *glClearMask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;

    // The first member of the list is a dummy node. This is necessary because
    // changes made to the first node are not reflected within the draw process
    // so the first node must always point to the same location.
    callbackList = (vsCallbackNode *)pfMalloc(sizeof(vsCallbackNode),
        pfGetSharedArena());
    callbackList->prev = NULL;
    callbackList->data = NULL;
    callbackList->func = NULL;

    // The list must contain the default callback as a member. Create it and
    // initialize its fields
    defaultDrawNode = (vsCallbackNode *)pfMalloc(sizeof(vsCallbackNode),
        pfGetSharedArena());
    defaultDrawNode->next = NULL;
    defaultDrawNode->data = glClearMask;
    defaultDrawNode->func = vsCallbackList::drawCallback;

    // Connect the list nodes to one another
    callbackList->next = defaultDrawNode;
    defaultDrawNode->prev = callbackList;

    // Set the traversal function for the channel to our custom traversal
    channel->setTravFunc(PFTRAV_DRAW, vsCallbackList::traverseCallbacks);

    // Initialize a pointer variable that holds the address of the pointer to
    // the callback list. This, too, is necessary because this data cannot be
    // changed from within the draw process
    callbackListAddress = (vsCallbackNode **)pfMalloc(sizeof(vsCallbackNode *),
        pfGetSharedArena());

    // Set the value of the callback list address pointer to the address of
    // the callback list
    *callbackListAddress = callbackList;

    // Set the data for the channel to the address of the default node
    channel->setChanData(callbackListAddress, sizeof(vsCallbackNode *));

    // Pass the channel data along
    channel->passChanData();

    // Declare a semaphore to protect the main list node
    listSemaphore = usnewsema(pfGetSemaArena(), 1);

    // Set the semaphore of the dummy node to the semaphore of the entire list
    // so that it can be retrieved later by the traversal function
    callbackList->sema = listSemaphore;

    // Declare a semaphore to protect the GL clear mask
    maskSemaphore = usnewsema(pfGetSemaArena(), 1);

    // Set the semaphore of the default node to the mask semaphore so that it
    // can be retrieved later by the traversal function
    defaultDrawNode->sema = maskSemaphore;
}

// ------------------------------------------------------------------------
// Destructor - Frees all of the memory used by the callback nodes, but
// *DOES NOT* free the shared memory
// ------------------------------------------------------------------------
vsCallbackList::~vsCallbackList()
{
    vsCallbackNode *traversalNode;
    vsCallbackNode *currentNode;

    // Start at the beginning of the callback list
    traversalNode = callbackList;

    // Move through the list deleting nodes
    while (traversalNode != NULL)
    {
        // Store the node about to be deleted
        currentNode = traversalNode;

        // Move to the next node before deleting the current one to keep
        // from losing the location of the list
        traversalNode = traversalNode->next;

        // See if this node has an associated semaphore
        if (currentNode->sema != NULL)
        {
            // Acquire the semaphore to make sure it is safe to delete
            uspsema(currentNode->sema);

            // Free the semaphore of the current node
            usfreesema(currentNode->sema, pfGetSemaArena());
        }

        // Deallocate the current node
        pfFree(currentNode);
    }

    // Free the callback list address pointer
    pfFree(callbackListAddress);

    // Free the GL clear mask variable
    pfFree(glClearMask);
}

// ------------------------------------------------------------------------
// Set the value of the mask used for clearing during rendering
// ------------------------------------------------------------------------
void vsCallbackList::setGLClearMask(int clearMask)
{
    // Acquire the semaphore for the mask
    uspsema(maskSemaphore);

    // Safely update the value
    *glClearMask = clearMask;

    // Release the semaphore
    usvsema(maskSemaphore);
}

// ------------------------------------------------------------------------
// Get the value of the mask used for clearing during rendering
// ------------------------------------------------------------------------
int vsCallbackList::getGLClearMask()
{
    int clearMask;

    // Acquire the semaphore for the mask
    uspsema(maskSemaphore);

    // Safely retrieve the value
    clearMask = *glClearMask;

    // Release the semaphore
    usvsema(maskSemaphore);

    // Return the value for the mask
    return clearMask;
}

// ------------------------------------------------------------------------
// Adds a function to the start of the list of callbacks executed during
// the draw process. This will return a pointer to shared memory (of the
// size specified by the argument) that will be acted on by Performer.
// ------------------------------------------------------------------------
void *vsCallbackList::prependCallback(pfChanFuncType callback,
                                      int sharedMemorySize)
{
    void *sharedMemory;

    if (sharedMemorySize > 0)
    {
        // Declare shared memory to be acted upon by the callbacks
        sharedMemory = pfMalloc(sharedMemorySize, pfGetSharedArena());
    }
    else
    {
        // The callback does not need any shared memory
        sharedMemory = NULL;
    }

    // Add a callback acting on this shared memory to the list
    prependCallback(callback, sharedMemory);

    // Return the pointer to the shared memory
    return sharedMemory;
}

// ------------------------------------------------------------------------
// Adds a function to the start of the list of callbacks executed during
// the draw process. The callback will act upon the shared memory at the
// location specified by the argument.
// ------------------------------------------------------------------------
void vsCallbackList::prependCallback(pfChanFuncType callback,
                                     void *sharedMemory)
{
    vsCallbackNode *newNode;
    vsCallbackNode *traversalNode;

    // Acquire the semaphore for the list
    uspsema(listSemaphore);

    // Allocate a new node in shared memory for the main channel
    newNode = (vsCallbackNode *)pfMalloc(sizeof(vsCallbackNode),
        pfGetSharedArena());

    // Attach the new node to the beginning of this draw callback list,
    // skipping the dummy node that is at the beginning of every list
    newNode->prev = callbackList;
    newNode->next = callbackList->next;
    callbackList->next->prev = newNode;
    callbackList->next = newNode;

    // Store the function pointer in the new node
    newNode->func = callback;

    // Store the pointer to the shared memory
    newNode->data = sharedMemory;

    // Start the new node with a NULL semaphore for now
    newNode->sema = NULL;

    // Only perform the following actions if the node needs a semaphore
    if (newNode->data)
    {
        // Start at the beginning of the list
        traversalNode = callbackList;

        // Traverse the list, searching for a semaphore to use for this node
        while (traversalNode->next != NULL)
        {
            // If the node has data and its data matches that of the current
            // node, use the semaphore of the existing node rather than
            // creating a new one
            if (traversalNode->data == newNode->data)
            {
                newNode->sema = traversalNode->sema;
            }

            // Move to the next node
            traversalNode = traversalNode->next;
        }

        // See whether the node acts upon any shared memory
        if ((newNode->sema == NULL) && ( newNode->data != NULL))
        {
            // Create a semaphore to handle user data in the node itself
            newNode->sema = usnewsema(pfGetSemaArena(), 0);
        }
    }

    // Release the semaphore for the list
    usvsema(listSemaphore);
}

// ------------------------------------------------------------------------
// Adds a function to the end of the list of callbacks executed during the
// draw process. This will return a pointer to shared memory (of the size
// specified by the argument) that will be acted on by Performer.
// ------------------------------------------------------------------------
void *vsCallbackList::appendCallback(pfChanFuncType callback,
                                     int sharedMemorySize)
{
    void *sharedMemory;

    if (sharedMemorySize > 0)
    {
        // Declare shared memory to be acted upon by the callbacks
        sharedMemory = pfMalloc(sharedMemorySize, pfGetSharedArena());
    }
    else
    {
        // The callback does not need any shared memory
        sharedMemory = NULL;
    }

    // Add a callback acting on this shared memory to the list
    appendCallback(callback, sharedMemory);

    // Return the pointer to the shared memory
    return sharedMemory;
}

// ------------------------------------------------------------------------
// Adds a function to the end of the list of callbacks executed during the
// draw process. The callback will act upon the shared memory at the
// location specified by the argument.
// ------------------------------------------------------------------------
void vsCallbackList::appendCallback(pfChanFuncType callback,
                                    void *sharedMemory)
{
    vsCallbackNode *newNode;
    vsCallbackNode *traversalNode;

    // Acquire the semaphore for the list
    uspsema(listSemaphore);

    // Allocate a new node in shared memory for the main channel
    newNode = (vsCallbackNode *)pfMalloc(sizeof(vsCallbackNode),
        pfGetSharedArena());

    // Store the function pointer in the new node
    newNode->func = callback;

    // Store the pointer to the shared memory
    newNode->data = sharedMemory;

    // Start the new node with a NULL semaphore for now
    newNode->sema = NULL;

    // Start at the beginning of the list
    traversalNode = callbackList;

    // Traverse to the end of the list
    while (traversalNode->next != NULL)
    {
        // If the node has data and its data matches that of the current
        // node, use the semaphore of the existing node rather than
        // creating a new one
        if ((newNode->data) && (traversalNode->data == newNode->data))
        {
            newNode->sema = traversalNode->sema;
        }

        // Move to the next node
        traversalNode = traversalNode->next;
    }

    // Attach the new node to the end of the linked list
    traversalNode->next = newNode;
    newNode->prev = traversalNode;
    newNode->next = NULL;

    // See if the node needs a semaphore and still doesn't have one
    if ((newNode->sema == NULL) && (newNode->data != NULL))
    {
        // Create a semaphore to handle user data in the node itself
        newNode->sema = usnewsema(pfGetSemaArena(), 0);
    }

    // Release the semaphore for the list
    usvsema(listSemaphore);
}

// ------------------------------------------------------------------------
// Attempts to remove from the list of draw process callbacks the function
// that matches the callback parameter and acts upon the memory specified
// by the sharedMemory parameter. If the callback is found, the function
// will attempt to deallocate the shared memory it acts upon. NOTE: A null
// callback cannot be removed in this manner.
// ------------------------------------------------------------------------
void vsCallbackList::removeCallback(pfChanFuncType callback,
                                    void *sharedMemory)
{
    vsCallbackNode *traversalNode;

    // Acquire the semaphore for the list
    uspsema(listSemaphore);

    // Make sure no attempt is made to remove the null callback of the list or
    // the default callback that actually executes the draw function
    if ((callback != NULL) && (callback != vsCallbackList::drawCallback))
    {
        // Start at the first callback node
        traversalNode = callbackList;

        // Traverse the callback list
        while (traversalNode != NULL)
        {
            // Compare the traversal callback function and shared memory
            // pointers to that of the node that is being searched for
            if ( (traversalNode->func == callback) &&
                 (traversalNode->data == sharedMemory) )
            {
                // The data matches, so find it in the list and remove it
                // NOTE: This naive removal method works only because the list
                // will always contain two members when this point is reached:
                // the default node and the one being removed.
                if (traversalNode->next == NULL)
                {
                    // Remove the node from the end of the list
                    traversalNode->prev->next = NULL;
                }
                else
                {
                    // Remove the node from the middle of the list
                    traversalNode->next->prev = traversalNode->prev;
                    traversalNode->prev->next = traversalNode->next;
                }

                // Deallocate the shared memory pointed to by the node
                pfFree(traversalNode);

                // Set the node to null to break out of the loop
                traversalNode = NULL;
            }
            else
            {
                // Move to the next node
                traversalNode = traversalNode->next;
            }
        }
    }

    // Release the semaphore
    usvsema(listSemaphore);
}

// ------------------------------------------------------------------------
// Returns a pointer to the data of the specified callback node, for use by
// any user-defined callback functions that need to act on shared memory.
// ------------------------------------------------------------------------
void *vsCallbackList::getData(void *nodeData)
{
    return ((vsCallbackNode *)nodeData)->data;
}

// ------------------------------------------------------------------------
// Removes the specified callback node from its traversal. This function is
// for use by any user-defined callback functions that need to remove
// themselves during the draw process. Note that this function *DOES NOT*
// deallocate the shared memory used by the callback.
// ------------------------------------------------------------------------
void vsCallbackList::nodeRemove(void *nodeData)
{
    vsCallbackNode *callbackNode;

    // Grab the callback node from the user data
    callbackNode = (vsCallbackNode *)nodeData;

    // Make sure the current node is not null
    if (callbackNode != NULL)
    {
        // Check where in the callback list the current node resides
        // NOTE: The naive removal method works only because the list will
        // always contain two members when this point is reached: the default
        // node and the one being removed.
        if (callbackNode->next == NULL)
        {
            // Remove the node from the end of the list
            callbackNode->prev->next = NULL;
        }
        else
        {
            // Remove the node from the middle of the list
            callbackNode->prev->next = callbackNode->next;
            callbackNode->next->prev = callbackNode->prev;
        }

        // The semaphore will be null if the node has no shared memory
        if (callbackNode->sema != NULL)
        {
            // Free the semaphore of the current node
            usfreesema(callbackNode->sema, pfGetSemaArena());
        }

        // Deallocate the memory used by the node
        pfFree(callbackNode);
    }
}

// ------------------------------------------------------------------------
// Attempt to acquire the semaphore of the data in this callback node,
// indicating to the application process that the data is being modified
// and should not be used. If the data is acquired, the function will
// return true, but if it is in use by the application or otherwise
// unavailable the function returns false. If the semaphore is NULL,
// the true is returned by default.
// ------------------------------------------------------------------------
bool vsCallbackList::nodeAcquireData(void *nodeData)
{
    vsCallbackNode *callbackNode;

    // Get the data in its proper callback node form
    callbackNode = (vsCallbackNode *)nodeData;

    // Make sure the semaphore exists
    if (callbackNode->sema)
    {
        // See if the data is unavailable
        if (ustestsema(callbackNode->sema) == 0)
        {
            // Indicate to the callback function that the data could
            // not be acquired
            return false;
        }
        else
        {
            // Acquire the semaphore of this data node
            uspsema(callbackNode->sema);

            // Indicate to the callback function that the data was acquired
            return true;
        }
    }
    else
    {
        // Indicate to the application that the data is NULL and does not
        // need to be acquired
        return true;
    }
}

// ------------------------------------------------------------------------
// Release the semaphore of the data in this callback node, indicating to
// the application process that the data is stable and safe to reference.
// ------------------------------------------------------------------------
void vsCallbackList::nodeReleaseData(void *nodeData)
{
    vsCallbackNode *callbackNode;

    // Get the data in its proper callback node form
    callbackNode = (vsCallbackNode *)nodeData;

    // The node will not have a semaphore if it has no user data
    if (callbackNode->sema)
    {
        // Release the semaphore of this data node
        usvsema(callbackNode->sema);
    }
}

// ------------------------------------------------------------------------
// Search the callback list for any nodes that act upon the shared memory
// indicated by the argument, acquiring the semaphore of that data if it
// can be found. true is returned if the data cannot be found or if it
// is found and its semaphore is acquired, indicating to the draw process
// that the data is unstable and should not be modified. False is
// returned if the data is located but its semaphore cannot be acquired.
// ------------------------------------------------------------------------
bool vsCallbackList::acquireData(void *sharedMemory)
{
    vsCallbackNode *traversalNode;

    if (sharedMemory)
    {
        // Acquire the semaphore for the list itself
        uspsema(listSemaphore);

        // Start at the head of the list
        traversalNode = callbackList;

        // Search the list node by node
        while (traversalNode != NULL)
        {
            // See whether the data pointers match
            if (sharedMemory == traversalNode->data)
            {
                // Test whether the semaphore is available
                if (ustestsema(traversalNode->sema) >= 1)
                {
                    // Acquire the semaphore of the user data
                    uspsema(traversalNode->sema);

                    // Release the semaphore of the entire list
                    uspsema(listSemaphore);

                    // Return true to indicate that the data is safe to modify
                    return true;
                }
                else
                {
                    // Release the list semaphore
                    usvsema(listSemaphore);

                    // Return false to indicate that the data is unstable and
                    // should not be modified
                    return false;
                }
            }

            // Move to the next node
            traversalNode = traversalNode->next;
        }

        // Release the list semaphore
        usvsema(listSemaphore);
    }

    // Return true because the data was not found in the callback list or
    // was NULL (this sort of data is always available by default)
    return true;
}

// ------------------------------------------------------------------------
// Search the callback list for any nodes that act upon the shared memory
// indicated by the argument and release that data if it was acquired,
// indicating to the draw process that the memory is stable and can safely
// be referenced.
// ------------------------------------------------------------------------
void vsCallbackList::releaseData(void *sharedMemory)
{
    vsCallbackNode *traversalNode;

    if (sharedMemory)
    {
        // Acquire the semaphore for the list itself
        uspsema(listSemaphore);

        // Start at the head of the list
        traversalNode = callbackList;

        // Search the list node by node
        while (traversalNode != NULL)
        {
            // See whether the data pointers match
            if (sharedMemory == traversalNode->data)
            {
                // Test whether the semaphore is acquired by the application
                if (ustestsema(traversalNode->sema) == 0)
                {
                    // Free the semaphore of the data
                    usvsema(traversalNode->sema);
                }

                // Release the list semaphore
                usvsema(listSemaphore);

                // The data was found and manipulated in the list
                return;
            }

            // Move to the next node
            traversalNode = traversalNode->next;
        }

        // Release the list semaphore
        usvsema(listSemaphore);
    }
}

// ------------------------------------------------------------------------
// static private function - Performer callback
// This is the only draw callback that should be set to a Performer
// channel. It traverses the linked list of callback functions and calls
// each in turn on their specified user data.
// ------------------------------------------------------------------------
void vsCallbackList::traverseCallbacks(pfChannel *chan, void *userData)
{
    vsCallbackNode *currentNode;
    usema_t        *semaphore;

    // Grab the callback list class from user data
    currentNode = *((vsCallbackNode **)userData);

    // Pick up the pointer for the semaphore from the dummy node at the
    // head of the list
    semaphore = currentNode->sema;

    // Acquire the semaphore for the list
    uspsema(semaphore);

    // The first callback node is a dummy, so move to the second one
    currentNode = currentNode->next;

    // Traverse the list of nodes, executing each callback in turn
    while (currentNode != NULL)
    {
        // Process the current callback function
        (currentNode->func)(chan, currentNode);

        // Move forwards to the next callback node
        currentNode = currentNode->next;
    }

    // Release the semaphore
    usvsema(semaphore);
}

// ------------------------------------------------------------------------
// static private function - Performer callback
// This is the draw callback that will always exist somewhere in
// the draw callback list. It performs the same function as Performer's
// ------------------------------------------------------------------------
void vsCallbackList::drawCallback(pfChannel *chan, void *userData)
{
    vsCallbackNode *currentNode;
    int            clearMask;

    // See if pfEarthSky is enabled on this pane
    if (chan->getESky()->getMode(PFES_BUFFER_CLEAR) == PFES_SKY_GRND)
    {
        // The entire channel must be cleared for EarthSky to function properly
        chan->clear();
    }
    else
    {
        // Grab the pointer to the current callback node
        currentNode = (vsCallbackNode *)userData;

        // Acquire the semaphore for the clear mask
        uspsema(currentNode->sema);

        // Get the value of the clear mask
        clearMask = *((int *)currentNode->data);

        // Release the clear mask semaphore
        usvsema(currentNode->sema);

        // Clear the channel according to the user data clear mask
        glClear(clearMask);
    }

    // Draw the scene
    pfDraw();
}

