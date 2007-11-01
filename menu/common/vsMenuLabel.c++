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

#include "vsBackfaceAttribute.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu label with the given
// text builder and text. The string can be null to create a blank label.
// ------------------------------------------------------------------------
vsMenuLabel::vsMenuLabel(vsTextBuilder *newTextBuilder, char *text)
{
    vsBackfaceAttribute *backface;

    // Store and reference the text builder so it is not prematurely deleted
    textBuilder = newTextBuilder;
    if (textBuilder)
        textBuilder->ref();

    // Create a blank menu component and a kinematics to manage it
    menuComponent = new vsComponent();
    menuKinematics = new vsKinematics(menuComponent);

    // Reference the components so they won't be accidentally deleted
    menuComponent->ref();
    menuKinematics->ref();

    // Attach a backface attribute to the menu component
    backface = new vsBackfaceAttribute();
    backface->enable();
    menuComponent->addAttribute(backface);

    // Initialize the label text variable to NULL before attempting to set it
    labelText = NULL;
    textComponent = NULL;

    // Store the desired text locally.
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

    if (menuComponent)
    {
        vsObject::unrefDelete(menuComponent);
        menuComponent = NULL;
    }

    if (menuKinematics)
    {
        vsObject::unrefDelete(menuKinematics);
        menuKinematics = NULL;
    }

    // Attempt to delete the text builder
    if (textBuilder)
        vsObject::unrefDelete(textBuilder);
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
// This method sets a new text builder for the menu label
// ------------------------------------------------------------------------
void vsMenuLabel::setTextBuilder(vsTextBuilder *newTextBuilder)
{
    // Unref the old text builder if it exists
    if (textBuilder)
        vsObject::unrefDelete(textBuilder);

    // Store and ref the new text builder if it exists
    textBuilder = newTextBuilder;
    if (textBuilder)
        textBuilder->ref();

    // Rebuild the text object with the new text builder
    setText(labelText);
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
    atVector centerOfMass;
    double radius;

    // Remove the old text object from this component
    if (textComponent)
    {
        // Remove the old text from the scene
        menuComponent->removeChild(textComponent);

        // Delete the old text
        vsObject::checkDelete(textComponent);
    }

    // If the text object exists, create a component representation of it
    // Otherwise, do nothing until the text is properly set
    if (text)
    {
        // See whether there is any difference between the old data and
        // the new data
        if (text != labelText)
        {
            // Free the old string if it exists
            if (labelText)
                free(labelText);

            // The new string is different from the old one, so free the
            // old string and store the new one
            labelText = (char *)malloc((strlen(text) + 1) * sizeof(char));
            strcpy(labelText, text);
        }

        // See whether the label currently has a text builder object set
        if (textBuilder)
        {
            // Use the text builder to create the text, storing it as the
            // component (since this is a vsMenuObject)
            textComponent = textBuilder->buildText(labelText);
            menuComponent->addChild(textComponent);

            // Set the kinematics object to the center of mass
            textComponent->getBoundSphere(&centerOfMass, &radius);
            menuKinematics->setCenterOfMass(centerOfMass);
        }
        else
        {
            // Label but no component.
            textComponent = NULL;
        }
    }
    else
    {
        // Free the old text variable if it existed
        if (labelText)
            free(labelText);

        // Set these values to NULL as they are no longer valid
        labelText = NULL;
        textComponent = NULL;
    }

/*
    atVector centerOfMass;
    double radius;

    // Remove the old text object from this component
    if (textComponent)
    {
        // Remove the old text from the scene
        menuComponent->removeChild(textComponent);

        // Delete the old text
        vsObject::checkDelete(textComponent);
    }

    // See whether the label currently has a text builder object set
    if (textBuilder)
    {
        // If the text object exists, create a component representation of it
        // Otherwise, do nothing until the text is properly set
        if (text)
        {
            // See whether there is any difference between the old data and
            // the new data
            if (text != labelText)
            {
                // Free the old string if it exists
                if (labelText)
                    free(labelText);

                // The new string is different from the old one, so free the
                // old string and store the new one
                labelText = (char *)malloc((strlen(text) + 1) * sizeof(char));
                strcpy(labelText, text);
            }

            // Use the text builder to create the text, storing it as the
            // component (since this is a vsMenuObject)
            textComponent = textBuilder->buildText(labelText);
            menuComponent->addChild(textComponent);

            // Set the kinematics object to the center of mass
            textComponent->getBoundSphere(&centerOfMass, &radius);
            menuKinematics->setCenterOfMass(centerOfMass);
        }
        else
        {
            // Free the old text variable if it existed
            if (labelText)
                free(labelText);

            // Set these values to NULL as they are no longer valid
            labelText = NULL;
            textComponent = NULL;
        }
    }
    else
    {
        // Free the old text variable if it existed
        if (labelText)
            free(labelText);

        // Set these values to NULL as they are no longer valid
        labelText = NULL;
        textComponent = NULL;

        // Print a notification that the text builder is null
        printf("vsMenuLabel::setText: Cannot set text due to undefined text "
            "builder!\n");
    }
*/
}

// ------------------------------------------------------------------------
// This method simply returns the text on the label
// ------------------------------------------------------------------------
char *vsMenuLabel::getText()
{
    return labelText;
}

