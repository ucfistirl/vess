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
//                  and state changes of a menu structure. It requires a
//                  window and an input system, creating a pane over the
//                  existing window for output and extracting devices for
//                  input.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuSystem.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor creates a menu system using a keyboard
// and mouse. It displays the menu on a new pane which is placed over the
// provided window.
// ------------------------------------------------------------------------
vsMenuSystem::vsMenuSystem(vsWindow *window, vsWindowSystem *windowSystem)
{
    int b;

    // Store the parent window
    parentWindow = window;

    // Create the objects used to view the menu system
    menuPane = new vsPane(parentWindow);
    menuScene = new vsScene();
    menuView = new vsView();
    menuComponent = new vsComponent();
    menuScene->addChild(menuComponent);

    // Set up the viewing objects
    menuPane->setScene(menuScene);
    menuPane->setView(menuView);
//    menuPane->setGLClearMask(GL_DEPTH_BUFFER_BIT);

    // This version of the constructor has a cursor
    hasCursor = true;

    // Extract the necessary axes from the window system
    xAxis = windowSystem->getMouse()->getAxis(0);
    yAxis = windowSystem->getMouse()->getAxis(1);

    // Initialize all of the buttons to NULL
    for (b = 0; b < VS_MENU_ACTION_COUNT; b++)
        setMenuButton((vsMenuAction)b, NULL);

    // Generate an object for intersection testing in the scene graph
    isectObject = new vsIntersect();
    isectObject->setSegListSize(1);

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
    delete isectObject;
    delete menuPane;
    delete menuScene;
    delete menuComponent;
    delete menuView;
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
void vsMenuSystem::setMenuTree(vsMenuTree *newTree)
{
    // Set the tree for future use
    menuTree = newTree;

    // Begin by displaying at the root menu
    setFrame(NULL);
}

// ------------------------------------------------------------------------
// Set the button used to trigger the specified menu action
// ------------------------------------------------------------------------
void vsMenuSystem::setMenuButton(vsMenuAction action, vsInputButton *button)
{
    // Store the button and initialize it to unpressed
    inputButtons[action] = button;
    pressed[action] = false;
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
// Update the input devices
// ------------------------------------------------------------------------
void vsMenuSystem::update()
{
    vsMenuObject   *prevObj;
    vsMenuObject   *currentObj;
    vsMenuIterator *menuIter;
    int b;

    // Get the first child of the tree at the current frame
    menuIter = new vsMenuIterator(menuTree, menuFrame);
    currentObj = menuIter->getObject();

    // For all objects
    while (currentObj)
    {
        // Tell each object to update its internal states, including any
        // visual effects
        currentObj->update(VS_MENU_SIGNAL_IDLE, menuFrame);

        // If the window system uses a cursor, intersect with this object
        if (hasCursor && currentObj->getComponent())
        {
            isectObject->setPickSeg(0, menuPane, xAxis->getPosition(),
                yAxis->getPosition());

            isectObject->intersect(currentObj->getComponent());

            if (isectObject->getIsectValid(0))
            {
                // Make the highlighted item the selected item
                selectedObj = currentObj;

                // If the activate button is pressed, activate the item
                if (processAction(VS_MENU_ACTION_CURSOR))
                {
                    // Send the activation signal to the object
                    currentObj->update(VS_MENU_SIGNAL_ACTIVATE, menuFrame);
                }
            }
        }

        // Check the accelerator of this object
        if (currentObj->getAccelerator())
        {
            // If the accelerator is pressed down, ensure that this object
            // is selected
            if (currentObj->getAccelerator()->isPressed())
                selectedObj = currentObj;
        }

        // Move on to the next child
        menuIter->advance();

        // Handle selecting the previous item
        if (processAction(VS_MENU_ACTION_PREVIOUS))
        {
            // If the next item is already the selected object, move the
            // selected object back to the current item
            if (menuIter->getObject() == selectedObj)
            {
                selectedObj = currentObj;

                pressed[VS_MENU_ACTION_PREVIOUS] = true;
            }
        }

        // Handle selecting the next item
        if (processAction(VS_MENU_ACTION_NEXT))
        {
            // If the old current item is selected and the next one exists,
            // move the selected object indicator forward to the next item
            if ((currentObj == selectedObj) && (menuIter->getObject()))
            {
                selectedObj = menuIter->getObject();
                pressed[VS_MENU_ACTION_NEXT] = true;
            }
        }

        // Update the current item before moving on in the loop
        currentObj = menuIter->getObject();
    }

    // Check the activation button on the selected object
    if (processAction(VS_MENU_ACTION_ACTIVATE))
    {
        // Send the activation signal to the object
        selectedObj->update(VS_MENU_SIGNAL_ACTIVATE, menuFrame);
    }

    // Update all of the button press states for the next frame
    for (b = 0; b < VS_MENU_ACTION_COUNT; b++)
    {
        if (inputButtons[b])
            pressed[b] = inputButtons[b]->isPressed();
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
        return (inputButtons[action]->isPressed() && !pressed[action]);
    }

    // If the action does not have a button assigned, return false by default
    return false;
}

// ------------------------------------------------------------------------
// VESS Internal Function
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
        // Get an iterator over the children of the current
        menuIter = new vsMenuIterator(menuTree, menuFrame);

        // Remove all of the component children of the main node
        while (menuComponent->getChildCount() > 0)
        {
            tempNode = menuComponent->getChild(0);
            menuComponent->removeChild(tempNode);
        }

        // Select the first item in the iterator
        selectedObj = menuIter->getObject();

        // Build a subgraph containing all of the children of this node
        destObj = menuIter->getObject();
        while (destObj)
        {
            // Add the component of the object as a child
            if (destObj->getComponent())
                menuComponent->addChild(destObj->getComponent());

            // Move on to the next child
            menuIter->advance();
            destObj = menuIter->getObject();
        }

        // Free the memory taken up by the menu iterator
        delete menuIter;
    }
}
