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
//    VESS Module:  vsMenuSystem.c++
//
//    Description:  The vsMenuSystem is a handler class that manages input
//                  and state changes of a menu structure.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuSystem.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor creates a basic menu system. It has no
// cursor, no notion of intersection testing, and cannot use an existing
// scene for navigation.
// ------------------------------------------------------------------------
vsMenuSystem::vsMenuSystem()
{
    int i;

    // Begin with the pane NULL by default
    menuPane = NULL;

    // Create default scene and view objects
    menuScene = new vsScene();
    menuView = new vsView();

    // Reference the scene and view to keep them from being deleted
    menuScene->ref();
    menuView->ref();

    // Create the component that will hold all of the visualization objects
    menuComponent = new vsComponent();
    menuScene->addChild(menuComponent);

    // This version of the constructor has no cursor at the moment
    hasCursor = false;

    // Generate an object for intersection testing in the scene graph
    isectObject = new vsIntersect();
    isectObject->setSegListSize(1);

    // Initialize all of the buttons to NULL
    for (i = 0; i < VS_MENU_ACTION_COUNT; i++)
    {
        inputButtons[i] = NULL;
        actionValid[i] = false;
        actionRepeatable[i] = false;
    }

    // Initially, set the tree to null
    menuTree = NULL;

    // Begin with a default menu frame
    menuFrame = new vsMenuFrame();
}

// ------------------------------------------------------------------------
// Constructor - This constructor creates a menu system using a keyboard
// and mouse. It displays the menu on a new pane which is placed over the
// provided window.
// ------------------------------------------------------------------------
vsMenuSystem::vsMenuSystem(vsPane *pane, vsWindowSystem *windowSystem)
{
    int i;
    vsComponent *newRoot;

    // Store the pane used to render everything
    menuPane = pane;

    // If the pane already has a scene fetch it, otherwise create a new one
    menuScene = pane->getScene();
    if (menuScene == NULL)
    {
        menuScene = new vsScene();
        menuPane->setScene(menuScene);
    }

    // Reference the scene to keep it from being deleted
    menuScene->ref();

    // If the pane already has a view fetch it, otherwise create a new one
    menuView = pane->getView();
    if (menuView == NULL)
    {
        menuView = new vsView();
        menuPane->setView(menuView);
    }

    // Reference the view to keep it from being deleted
    menuView->ref();

    // Create the component that will hold all of the visualization objects
    menuComponent = new vsComponent();

    // If the scene is empty, add a new component as its child, or otherwise
    // create a new root node that will hold both the old root and the new
    // menu object visualization nodes as children
    if (menuScene->getChildCount() == 0)
    {
        menuScene->addChild(menuComponent);
    }
    else
    {
        // Create the new root node
        newRoot = new vsComponent();

        // Add each of the contending nodes as children of that root
        newRoot->addChild(menuComponent);
        newRoot->addChild(menuScene->getChild(0));

        // Swap the new root node into its place
        menuScene->removeChild(0);
        menuScene->addChild(newRoot);
    }

    // This version of the constructor has a cursor
    hasCursor = true;

    // Extract the necessary axes from the window system
    xAxis = windowSystem->getMouse()->getAxis(0);
    yAxis = windowSystem->getMouse()->getAxis(1);

    // Generate an object for intersection testing in the scene graph
    isectObject = new vsIntersect();
    isectObject->setSegListSize(1);

    // Initialize all of the buttons to NULL
    for (i = 0; i < VS_MENU_ACTION_COUNT; i++)
    {
        inputButtons[i] = NULL;
        actionValid[i] = false;
        actionRepeatable[i] = false;
    }

    // Initially, set the tree to null
    menuTree = NULL;

    // Begin with a default menu frame
    menuFrame = new vsMenuFrame();
}

// ------------------------------------------------------------------------
// Destructor - The destructor simply deletes any internally-created
// variables.
// ------------------------------------------------------------------------
vsMenuSystem::~vsMenuSystem()
{
    // If the pane is set, make sure it doesn't hold references to the scene
    // or view objects, as they need to be deleted
    if (menuPane)
    {
        menuPane->setScene(NULL);
        menuPane->setView(NULL);
    }

    // Delete these objects if it is safe to do so
    vsObject::unrefDelete(menuScene);
    vsObject::unrefDelete(menuView);

    // Delete the intersection object
    delete isectObject;

    // Delete the menu frame
    delete menuFrame;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMenuSystem::getClassName()
{
    return "vsMenuSystem";
}

// ------------------------------------------------------------------------
// Set the pane that this vsMenuSystem renders into (and consequently will
// be using for intersection testing). Note that calling this function will
// automatically set the view and scene of this pane to those stored
// internally.
// ------------------------------------------------------------------------
void vsMenuSystem::setPane(vsPane *pane)
{
    // Make sure the pane passed in is non-null
    if (pane)
    {
        menuPane = pane;
        menuPane->setScene(menuScene);
        menuPane->setView(menuView);
    }
}

// ------------------------------------------------------------------------
// Get the scene displayed by this vsMenuSystem
// ------------------------------------------------------------------------
vsScene *vsMenuSystem::getScene()
{
    return menuScene;
}

// ------------------------------------------------------------------------
// Get the view used to display this vsMenuSystem
// ------------------------------------------------------------------------
vsView *vsMenuSystem::getView()
{
    return menuView;
}

// ------------------------------------------------------------------------
// Set the vsMenuTree that the system will use to display data
// ------------------------------------------------------------------------
void vsMenuSystem::setCursor(vsInputAxis *x, vsInputAxis *y)
{
    // Store the axes
    xAxis = x;
    yAxis = y;

    // The menu system only officially has a cursor if both axes are set
    if (xAxis && yAxis)
    {
        hasCursor = true;
    }
    else
    {
        hasCursor = false;
    }
}

// ------------------------------------------------------------------------
// Set the vsMenuTree that the system will use to display data
// ------------------------------------------------------------------------
void vsMenuSystem::setMenuTree(vsMenuTree *newTree)
{
    // Set the tree for future use
    menuTree = newTree;

    // Begin by displaying at the root menu
    setFrame(NULL);
}

// ------------------------------------------------------------------------
// Update the visualization of the menu system, rebuilding the scene graph
// at the location of the tree currently specified by the menu frame
// ------------------------------------------------------------------------
void vsMenuSystem::rebuildMenu()
{
    vsMenuIterator *menuIter;
    vsNode         *tempNode;
    vsMenuObject   *destObj;

    // Only process if we have a valid menu tree
    if (menuTree)
    {
        // Get an iterator over the children of the current frame
        menuIter = new vsMenuIterator(menuTree, menuFrame);

        // Remove all of the component children of the main node
        while (menuComponent->getChildCount() > 0)
        {
            tempNode = menuComponent->getChild(0);
            menuComponent->removeChild(tempNode);
        }

        // Select the first item in the iterator
        selectedObj = NULL;

        // Build a subgraph containing all of the children of this node
        destObj = menuIter->getObject();
        while (destObj)
        {
            // Add the component of the object as a child
            if (destObj->isEnabled() && destObj->getComponent())
                menuComponent->addChild(destObj->getComponent());

            // Select the first selectable object by default
            if ((selectedObj == NULL) && (destObj->isSelectable()) &&
                (destObj->isEnabled()))
                selectedObj = destObj;

            // Move on to the next child
            menuIter->advance();
            destObj = menuIter->getObject();
        }

        // Free the memory taken up by the menu iterator
        delete menuIter;
    }
}

// ------------------------------------------------------------------------
// Move the menu system to display on a different location in the tree
// ------------------------------------------------------------------------
void vsMenuSystem::setFrame(vsMenuFrame *frame)
{
    // Copy the argument frame
    menuFrame->setFrame(frame);

    // Update the display data based on the new frame
    rebuildMenu();
}

// ------------------------------------------------------------------------
// Returns the current menu frame
// ------------------------------------------------------------------------
vsMenuFrame *vsMenuSystem::getFrame()
{
    return menuFrame;
}

// ------------------------------------------------------------------------
// Gets the currently selected item
// ------------------------------------------------------------------------
vsMenuObject *vsMenuSystem::getSelection()
{
    return selectedObj;
}

// ------------------------------------------------------------------------
// Set the button used to trigger the specified menu action
// ------------------------------------------------------------------------
void vsMenuSystem::setMenuButton(vsMenuAction action, vsInputButton *button)
{
    // Store the button and initialize it to pressed
    inputButtons[action] = button;
    actionValid[action] = actionRepeatable[action];
}

// ------------------------------------------------------------------------
// Sets whether a specific action can be carried out in repeat updates
// without its button being released in between.
// ------------------------------------------------------------------------
void vsMenuSystem::setRepeatable(vsMenuAction action, bool repeatable)
{
    actionRepeatable[action] = repeatable;
}

// ------------------------------------------------------------------------
// Hide the menu system so that it isn't displayed
// ------------------------------------------------------------------------
void vsMenuSystem::hide()
{
    menuPane->hidePane();
}

// ------------------------------------------------------------------------
// Show the menu system so that it will be displayed
// ------------------------------------------------------------------------
void vsMenuSystem::show()
{
    menuPane->showPane();
}

// ------------------------------------------------------------------------
// Process the current input state and adjust the current menu object
// states accordingly.
// ------------------------------------------------------------------------
void vsMenuSystem::update()
{
    vsMenuObject   *prevObj;
    vsMenuObject   *curObj;
    vsMenuIterator *menuIter;
    vsMenuFrame    *curFrame;
    int i;

    // Only update if the menu system is set
    if (menuTree)
    {
        // Get the first child of the tree at the current frame
        menuIter = new vsMenuIterator(menuTree, menuFrame);
        curObj = menuIter->getObject();
    }
    else
    {
        // Initialize the current object to NULL
        curObj = NULL;
    }

    // Initialize the last valid object at the first object
    prevObj = NULL;

    // Create a working frame to use on this update
    curFrame = new vsMenuFrame(menuFrame);

    // For all objects
    while (curObj)
    {
        // Tell each object to update its internal states, including any
        // visual effects
        curObj->update(VS_MENU_SIGNAL_IDLE, curFrame);

        // If the window system uses a cursor, intersect with this object
        if (hasCursor && curObj->getComponent() && curObj->isSelectable())
        {
            isectObject->setPickSeg(0, menuPane, xAxis->getPosition(),
                yAxis->getPosition());

            isectObject->intersect(curObj->getComponent());

            if (isectObject->getIsectValid(0))
            {
                // Make the highlighted item the selected item
                selectedObj = curObj;

                // If the activate button is pressed, activate the item
                if (processAction(VS_MENU_ACTION_CURSOR))
                {
                    // Send the activation signal to the object
                    curObj->update(VS_MENU_SIGNAL_ACTIVATE, curFrame);
                }
            }
        }

        // Check the accelerator of this object
        if (curObj->getAccelerator())
        {
            // If the accelerator is pressed down, activate the object
            if (curObj->getAccelerator()->isPressed())
                curObj->update(VS_MENU_SIGNAL_ACTIVATE, curFrame);
        }

        // Handle selecting the previous item
        if (processAction(VS_MENU_ACTION_PREVIOUS))
        {
            // If the current item is already the selected object, move the
            // selected object back to the previous item
            if ((curObj == selectedObj) && (prevObj != NULL))
            {
                selectedObj = prevObj;

                // Make sure the movement only happens once
                actionValid[VS_MENU_ACTION_PREVIOUS] = false;
            }
        }

        // Handle selecting the next item
        if (processAction(VS_MENU_ACTION_NEXT) && curObj)
        {
            // If the old current item is selected and the next one exists,
            // move the selected object indicator forward to the next item
            if ((prevObj == selectedObj) && (curObj->isSelectable()))
            {
                selectedObj = curObj;

                // Make sure the movement only happens once
                actionValid[VS_MENU_ACTION_NEXT] = false;
            }
        }

        // Store the last valid selectable object at each movement
        if (curObj->isSelectable())
            prevObj = curObj;

        // Move on to the next child
        menuIter->advance();
//        while (menuIter->getObject() && !menuIter->getObject()->isEnabled())
//            menuIter->advance();
        curObj = menuIter->getObject();
    }

    // Make sure there is an object selected before sending any signals
    if (selectedObj)
    {
        // Check the activation button
        if (processAction(VS_MENU_ACTION_ACTIVATE))
        {
            // Send the activation signal to the object
            selectedObj->update(VS_MENU_SIGNAL_ACTIVATE, curFrame);
        }

        // Check the increase button
        if (processAction(VS_MENU_ACTION_INCREASE))
        {
            // Send an increase signal to the object
            selectedObj->update(VS_MENU_SIGNAL_INCREASE, curFrame);
        }

        // Check the decrease button
        if (processAction(VS_MENU_ACTION_DECREASE))
        {
            // Send a decrease signal to the object
            selectedObj->update(VS_MENU_SIGNAL_DECREASE, curFrame);
        }
    }

    // If the frame data has changed during this update, rebuild the menus
    if (!curFrame->isEqual(menuFrame))
    {
        // Copy the new data into the existing frame, deleting the working copy
        // This slightly slower method is used to preserve the validity of any
        // frame pointers in use outside of this class
        menuFrame->setFrame(curFrame);
        delete curFrame;

        // Rebuild the menu system in the new location
        rebuildMenu();
    }

    // Update all of the button press states for the next frame
    for (i = 0; i < VS_MENU_ACTION_COUNT; i++)
    {
        if (inputButtons[i])
        {
            if (!inputButtons[i]->isPressed() || actionRepeatable[i])
                actionValid[i] = true;
            else
                actionValid[i] = false;
        }
    }
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Convenience function for whether a given action should be performed
// from the state of its assigned input button.
// ------------------------------------------------------------------------
bool vsMenuSystem::processAction(vsMenuAction action)
{
    // Only handle the state if the action has a button assigned
    if (inputButtons[action])
    {
        // Return true if and only if the button is currently pressed but
        // was not pressed on the last update
        return (inputButtons[action]->isPressed() && actionValid[action]);
    }

    // If the action does not have a button assigned, return false by default
    return false;
}
