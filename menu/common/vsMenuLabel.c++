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
//    VESS Module:  vsMenuLabel.c++
//
//    Description:  This is a menu object that is represented by a text
//                  component. It takes a pointer to a text builder
//                  object so it can create and modify its text.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuLabel.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu label with the given
// text builder and text. The string can be null to create a blank label.
// ------------------------------------------------------------------------
vsMenuLabel::vsMenuLabel(vsTextBuilder *newTextBuilder, char *text)
{
    labelText = NULL;
    textBuilder = newTextBuilder;

    menuComponent = NULL;
    menuKinematics = NULL;

    setText(text);
}

// ------------------------------------------------------------------------
// Destructor - The destructor frees the text and text component memory,
// but leaves the text builder object alone, as it was externally created
// ------------------------------------------------------------------------
vsMenuLabel::~vsMenuLabel()
{
    if (labelText)
        free(labelText);

    if (menuKinematics)
        vsObject::unrefDelete(menuKinematics);
    if (menuComponent)
        vsObject::unrefDelete(menuComponent);
}

// ------------------------------------------------------------------------
// Virtual function
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsMenuLabel::getClassName()
{
    return "vsMenuLabel";
}

// ------------------------------------------------------------------------
// Virtual function
// Updates the menu object according to the signal it received from the
// indicated menu frame.
// ------------------------------------------------------------------------
void vsMenuLabel::update(vsMenuSignal signal, vsMenuFrame *frame)
{
    switch (signal)
    {
        case VS_MENU_SIGNAL_IDLE:
        {
            // Update the kinematics object if it exists
            if (menuKinematics)
                menuKinematics->update();
        }
        break;
    }
}

// ------------------------------------------------------------------------
// This method returns a pointer to the current text builder
// ------------------------------------------------------------------------
vsTextBuilder *vsMenuLabel::getTextBuilder()
{
    return textBuilder;
}

// ------------------------------------------------------------------------
// This method sets the text that this label displays and destroys the old
// rendering component
// ------------------------------------------------------------------------
void vsMenuLabel::setText(char *text)
{
    vsBackfaceAttribute *backface;
    vsVector centerOfMass;
    double radius;

    // Free the old label, if it exists
    if (labelText)
        free(labelText);

    // Delete the old kinematics object and rendering component if they are
    // in existence and not being referenced elsewhere
    if (menuKinematics)
        vsObject::unrefDelete(menuKinematics);
    if (menuComponent)
        vsObject::unrefDelete(menuComponent);

    if (textBuilder)
    {
        // If the text object exists, create a component representation of it
        // Otherwise, do nothing until the text is properly set
        if (text)
        {
            // Allocate space to hold the label
            labelText = (char *)malloc((strlen(text) + 1) * sizeof(char));
            strcpy(labelText, text);

            // Use the text builder to create the text, storing it as the
            // component (since this is a vsMenuObject)
            menuComponent = textBuilder->buildText(labelText);
            menuKinematics = new vsKinematics(menuComponent);

            // Set the kinematics object to the center of mass
            menuComponent->getBoundSphere(&centerOfMass, &radius);
            menuKinematics->setCenterOfMass(centerOfMass);

            // Attach a backface attribute to the text geometry
            backface = new vsBackfaceAttribute();
            backface->enable();
            menuComponent->addAttribute(backface);

            // Reference the kinematics and component objects so that they
            // won't be deleted accidentally
            menuKinematics->ref();
            menuComponent->ref();
        }
        else
        {
            labelText = NULL;
            menuComponent = NULL;
            menuKinematics = NULL;
        }
    }
    else
    {
        labelText = NULL;
        menuComponent = NULL;
        menuKinematics = NULL;

        // Print a notification that the text builder is null
        printf("vsMenuLabel::setText: Cannot set text due to undefined text "
            "builder!\n");
    }
}

// ------------------------------------------------------------------------
// This method simply returns the text on the label
// ------------------------------------------------------------------------
char *vsMenuLabel::getText()
{
    return labelText;
}

