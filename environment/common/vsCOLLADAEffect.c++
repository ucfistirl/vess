
#include "vsCOLLADAEffect.h++"

// ------------------------------------------------------------------------
// Constructor, copies the effect identifier and sets up a list to store
// effect parameters
// ------------------------------------------------------------------------
vsCOLLADAEffect::vsCOLLADAEffect(atString id)
{
   // Copy the effect id
   effectID.setString(id);

   // Create the list of effect parameters
   effectParameters = new atList();
}

// ------------------------------------------------------------------------
// Destructor, cleans up the parameters
// ------------------------------------------------------------------------
vsCOLLADAEffect::~vsCOLLADAEffect()
{
    // Clean up the parameters list
    delete effectParameters;
}

// ------------------------------------------------------------------------
// Return this effect's identifier
// ------------------------------------------------------------------------
atString vsCOLLADAEffect::getID()
{
    return effectID;
}

// ------------------------------------------------------------------------
// Adds an effect parameter to our parameters list
// ------------------------------------------------------------------------
void vsCOLLADAEffect::addParameter(vsCOLLADAEffectParameter *param)
{
    // Make sure we don't already have a parameter with this ID
    if (getParameter(param->getName()) == NULL)
    {
        // Add the parameter to our list
        effectParameters->addEntry(param);
    }
    else
    {
        printf("vsCOLLADAEffect::addParameter:  Already have a parameter"
            " named %s\n", param->getName().getString());
    }
}

// ------------------------------------------------------------------------
// Finds the named parameter in our parameter list and returns it (or NULL
// if no such parameter is found)
// ------------------------------------------------------------------------
vsCOLLADAEffectParameter *vsCOLLADAEffect::getParameter(atString name)
{
    vsCOLLADAEffectParameter *param;

    // Look for the parameter with the given name
    param = (vsCOLLADAEffectParameter *)effectParameters->getFirstEntry();
    while (param != NULL)
    {
        // See if the names match
        if (param->getName().equals(&name))
        {
            // This is it, return this parameter
            return param;
        }

        // Try the next parameter
        param = (vsCOLLADAEffectParameter *)effectParameters->getNextEntry();
    }

    // Couldn't find the requested parameter
    return NULL;
}

// ------------------------------------------------------------------------
// Returns the parameter at the given index in the list (or NULL if the
// index is invalid)
// ------------------------------------------------------------------------
vsCOLLADAEffectParameter *vsCOLLADAEffect::getParameter(u_long index)
{
    // Return the requested parameter
    return (vsCOLLADAEffectParameter *)effectParameters->getNthEntry(index);
}

