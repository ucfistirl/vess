// File vsScreen.c++

#include "vsScreen.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the child window list and sets this object
// as a child of its parent pipe
// ------------------------------------------------------------------------
vsScreen::vsScreen(vsPipe *parent)
{
    childWindowList = NULL;

    parentPipe = parent;
    parentPipe->setScreen(this);
}

// ------------------------------------------------------------------------
// Destructor - Deletes all child windows of this screen
// ------------------------------------------------------------------------
vsScreen::~vsScreen()
{
    while (childWindowList)
        delete (childWindowList->data);
}

// ------------------------------------------------------------------------
// Retrieves the parent pipe object of this screen
// ------------------------------------------------------------------------
vsPipe *vsScreen::getParentPipe()
{
    return parentPipe;
}

// ------------------------------------------------------------------------
// Retrieves the number of child windows attached to this screen
// ------------------------------------------------------------------------
int vsScreen::getChildWindowCount()
{
    vsWindowListNode *listNode;
    int result = 0;
    
    for (listNode = childWindowList; listNode; listNode = listNode->next)
        result++;

    return result;
}

// ------------------------------------------------------------------------
// Retrieves one of the windows on this screen, specified by index. The
// index of the first window is 0.
// ------------------------------------------------------------------------
vsWindow *vsScreen::getChildWindow(int index)
{
    vsWindowListNode *node;
    int loop;

    if (index < 0)
    {
        printf("vsScreen::getChildWindow: Bad index value\n");
        return NULL;
    }

    node = childWindowList;
    for (loop = 0; (loop < index) && (node); loop++)
        node = node->next;

    if (!node)
        printf("vsScreen::getChildWindow: Index greater than number of \
children\n");

    return node->data;
}

// ------------------------------------------------------------------------
// Retrieves the pixel size of this screen. NULL pointers may be passed in
// for undesired data values.
// ------------------------------------------------------------------------
void vsScreen::getScreenSize(int *width, int *height)
{
    int x, y;
    
    parentPipe->getBaseLibraryObject()->getSize(&x, &y);
    
    if (width)
        *width = x;
    if (height)
        *height = y;
}

// ------------------------------------------------------------------------
// VESS internal function
// Adds the specified window to this screen's list of child windows
// ------------------------------------------------------------------------
void vsScreen::addWindow(vsWindow *newWindow)
{
    // Add window to screen's internal list
    vsWindowListNode *newNode;
    
    newNode = new vsWindowListNode;
    newNode->data = newWindow;
    newNode->next = childWindowList;
    
    childWindowList = newNode;
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes the specified window from this screen's list of child windows
// ------------------------------------------------------------------------
void vsScreen::removeWindow(vsWindow *targetWindow)
{
    // Remove window from screen's internal list
    vsWindowListNode *thisNode, *prevNode;

    if (childWindowList->data == targetWindow)
    {
        thisNode = childWindowList;
        childWindowList = childWindowList->next;
        delete thisNode;
    }
    else
    {
        prevNode = childWindowList;
        thisNode = prevNode->next;
        while ((thisNode) && (thisNode->data != targetWindow))
        {
            prevNode = thisNode;
            thisNode = thisNode->next;
        }

        if (thisNode)
        {
            prevNode->next = thisNode->next;
            delete thisNode;
        }
        else
            printf("vsScreen::removeWindow: Specified window not part of screen\n");
    }
}
