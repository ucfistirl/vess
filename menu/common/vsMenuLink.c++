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
//    VESS Module:  vsMenuLink.c++
//
//    Description:  The vsMenuLink is an object whose sole purpose is to
//                  navigate to a different location within the menu tree.
//                  This location is represented by a frame and can be
//                  specified in relative or absolute terms.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuLink.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu link with no visual
// representation.
// ------------------------------------------------------------------------
vsMenuLink::vsMenuLink()
{
    // Initialize the component and kinematics objects to null
    menuComponent = NULL;
    menuKinematics = NULL;

    // Set up a new default frame
    destFrame = new vsMenuFrame();
    linkMode = VS_MENU_LINK_MODE_ABSOLUTE;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu link from the
// component and kinematics of an existing vsMenuObject.
// ------------------------------------------------------------------------
vsMenuLink::vsMenuLink(vsMenuObject *object)
{
    // Store the component and kinematics for later use
    menuComponent = (vsComponent *)object->getComponent()->cloneTree();
    menuKinematics = new vsKinematics(menuComponent);

    // Reference the component and kinematics objects
    if (menuComponent)
        menuComponent->ref();
    if (menuKinematics)
        menuKinematics->ref();

    // Set up a new default frame
    destFrame = new vsMenuFrame();
    linkMode = VS_MENU_LINK_MODE_ABSOLUTE;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu link with the given
// menu component and kinematics object. The kinematics object may be null
// if you do not want the component manipulated automatically on updates.
// ------------------------------------------------------------------------
vsMenuLink::vsMenuLink(vsComponent *component, vsKinematics *kinematics)
{
    // Store the component and kinematics for later use
    menuComponent = component;
    menuKinematics = kinematics;

    // Reference the component and kinematics objects
    if (menuComponent)
        menuComponent->ref();
    if (menuKinematics)
        menuKinematics->ref();

    // Set up a new default frame
    destFrame = new vsMenuFrame();
    linkMode = VS_MENU_LINK_MODE_ABSOLUTE;
}

// ------------------------------------------------------------------------
// Destructor - The destructor frees any internal memory
// ------------------------------------------------------------------------
vsMenuLink::~vsMenuLink()
{
    // Delete the externally-created variables
    if (menuKinematics)
        vsObject::unrefDelete(menuKinematics);
    if (menuComponent)
        vsObject::unrefDelete(menuComponent);

    // Delete the internally-created frame
    delete destFrame;
}

// ------------------------------------------------------------------------
// Virtual function
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsMenuLink::getClassName()
{
    return "vsMenuLink";
}

// ------------------------------------------------------------------------
// Virtual function
// Updates the menu object according to the signal it received from the
// indicated menu frame.
// ------------------------------------------------------------------------
void vsMenuLink::update(vsMenuSignal signal, vsMenuFrame *frame)
{
    int i;

    // Decide what to do based on the signal received
    switch (signal)
    {
        // The idle signal updates the kinematics object, if it exists
        case VS_MENU_SIGNAL_IDLE:
        {
            if (menuKinematics)
                menuKinematics->update();
        }
        break;

        // The activate signal navigates to the location specified
        // by the internally-stored destination menu frame
        case VS_MENU_SIGNAL_ACTIVATE:
        {
            // Check whether the link mode is relative or absolute
            if (linkMode == VS_MENU_LINK_MODE_ABSOLUTE)
            {
                // If absolute mode is used, simply set the frame
                frame->setFrame(destFrame);
            }
            else
            {
                // If relative mode is used, modify the current frame
                // according to the indices in the destination frame
                for (i = 0; i < destFrame->getDepth(); i++)
                {
                    // In relative mode, negative indices indicate
                    // upward traversal of the menu tree
                    if (destFrame->getIndex(i) < 0)
                        frame->removeIndex();
                    else
                        frame->appendIndex(destFrame->getIndex(i));
                }
            }
        }
        break;
    }
}

// ------------------------------------------------------------------------
// This method sets the destination frame of this link, indicating whether
// that menu frame is specified in terms of the current frame (relative) or
// in terms of the root node of the tree (absolute). In relative mode,
// negative indices indicate upward traversal of the menu tree.
// ------------------------------------------------------------------------
void vsMenuLink::setTarget(vsMenuFrame *frame, vsMenuLinkMode mode)
{
    // Update the destination frame
    destFrame->setFrame(frame);

    // Store the new link mode
    linkMode = mode;
}

// ------------------------------------------------------------------------
// This method returns the current destination frame of the link
// ------------------------------------------------------------------------
vsMenuFrame *vsMenuLink::getFrame()
{
    return destFrame;
}

// ------------------------------------------------------------------------
// This method returns whether the menu location indicated by this frame is
// specified absolute or relative terms
// ------------------------------------------------------------------------
vsMenuLinkMode vsMenuLink::getMode()
{
    return linkMode;
}

