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
//    VESS Module:  vsScentManager.c++
//
//    Description:  Singleton class to watch over all olfactory
//                  operations.  This class keeps track of all scents
//                  currently being provided by hardware, as well as
//                  the various scent sources in the scene.  Each update
//                  call will adjust the strength of all scents 
//                  appropriately, according to the scent sources and
//                  scent detector.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsScentManager.h++"

// Static instance variable
vsScentManager *vsScentManager::instance = NULL;

// ------------------------------------------------------------------------
// Constructor for the vsScentManager object.  This is not called directly,
// but by the getInstance() singleton accessor method.  The scents and
// scentSources growable arrays are created initially with size zero, so
// that little space is taken if the application doesn't require any
// olfactory functionality.
// ------------------------------------------------------------------------
vsScentManager::vsScentManager()
{
    // Initialize the scentDetector pointer to NULL
    scentDetector = NULL;

    // Inintialize the scent and scent source counters
    numScents = 0;
    numScentSources = 0;
}

// ------------------------------------------------------------------------
// Return the index of the given vsScent in the scents array
// ------------------------------------------------------------------------
int vsScentManager::getScentIndex(vsScent *scent)
{
    int i;

    // Search the scents array for the given scent
    for (i = 0; i < numScents; i++)
    {
        // Return the current index if the scents match
        if (scents[i] == scent)
            return i;
    }

    // Scent not found
    return -1;
}

// ------------------------------------------------------------------------
// Destrutor.  This is an internal method, as the single instance of
// vsScentManager will be destroyed by the vsSystem object.
// ------------------------------------------------------------------------
vsScentManager::~vsScentManager()
{
    instance = NULL;
}

// ------------------------------------------------------------------------
// Registers a vsScent with the manager so it can be manipulated
// ------------------------------------------------------------------------
void vsScentManager::addScent(vsScent *scent)
{
    // Add the scent to the scents array
    scents[numScents] = scent;

    // Increment the number of scents
    numScents++;
}

// ------------------------------------------------------------------------
// Removes a vsScent from the manager 
// ------------------------------------------------------------------------
void vsScentManager::removeScent(vsScent *scent)
{
    int scentIndex, i;

    // Find the scent in the array
    scentIndex = 0;
    while ((scentIndex < numScents) && 
        (scents[scentIndex] != scent))
    {
        scentIndex++;
    }

    // If we found the scent, slide the remaining scents down
    // into its place
    if (scentIndex < numScents)
    {
        // Decrement the number of scents
        numScents--;

        for (i = scentIndex; i < numScents; i++)
            scents[i] = scents[i+1];
    }
}

// ------------------------------------------------------------------------
// Registers a vsScentSourceAttribute with the manager so it can be updated
// ------------------------------------------------------------------------
void vsScentManager::addScentSource(vsScentSourceAttribute *attr)
{
    // Add the scent source to the sources array
    scentSources[numScentSources] = attr;

    // Increment the number of sources
    numScentSources++;
}

// ------------------------------------------------------------------------
// Removes a vsScentSourceAttribute from the manager
// ------------------------------------------------------------------------
void vsScentManager::removeScentSource(vsScentSourceAttribute *attr)
{
    int attrIndex, i;

    // Find the scent source in the array
    attrIndex = 0;
    while ((attrIndex < numScentSources) && 
        (scentSources[attrIndex] != attr))
    {
        attrIndex++;
    }

    // If we found the attribute, slide the remaining attributes down
    // into its place
    if (attrIndex < numScentSources)
    {
        // Decrement the number of sources
        numScentSources--;

        for (i = attrIndex; i < numScentSources; i++)
            scentSources[i] = scentSources[i+1];
    }
}

// ------------------------------------------------------------------------
// Register the scent detector object with the manager so it can be updated
// ------------------------------------------------------------------------
void vsScentManager::setScentDetector(vsScentDetectorAttribute *attr)
{
    // Ignore the request if there is already a scent detector present
    if (scentDetector != NULL)
    {
        printf("vsScentManager::setScentDetector:  A scent detector already "
            "exists!\n");
        return;
    }

    // Set the scent detector pointer to the given attribute
    scentDetector = attr;
}

// ------------------------------------------------------------------------
// Remove the scent detector object from the manager
// ------------------------------------------------------------------------
void vsScentManager::removeScentDetector(vsScentDetectorAttribute *attr)
{
    // Ignore the request if there is no scent detector registered
    if (scentDetector == NULL)
    {
        printf("vsScentManager::removeScentDetector:  No scent detector "
            "registered!\n");
        return;
    }

    // Also ignore the request if the registered detector attribute doesn't
    // match the given attribute
    if (scentDetector != attr)
    {
        printf("vsScentManager::removeScentDetector:  Registered scent "
            "detector does not match given scent detector!\n");
        return;
    }

    // Set the scent detector pointer to NULL
    scentDetector = NULL;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsScentManager::getClassName()
{
    return "vsScentManager";
}

// ------------------------------------------------------------------------
// Return the current instance of this class, creating one if necessary
// ------------------------------------------------------------------------
vsScentManager *vsScentManager::getInstance()
{
    // Check to see if an instance exists, and create one if not
    if (instance == NULL)
    {
        instance = new vsScentManager();
    }

    // Return the singleton instance of this class
    return instance;
}

// ------------------------------------------------------------------------
// Update the strengths of all available scents to match the current
// situation in the scene.
// ------------------------------------------------------------------------
void vsScentManager::update()
{
    int i;
    int index;
    double strength;
    vsVector scentVec;
    double distance;
    double currentStrengths[VS_SM_MAX_SCENTS];
    vsScent *currentScent;
    double scale, reference, maxDist, rolloff, minStr, maxStr, sensitivity;

    // If there are no scents, scent sources, or scent detector, no
    // scents should be present.  Just make sure that all scents (if
    // any) have their strength set to zero.
    if ((numScents == 0) || (numScentSources == 0) || 
        (scentDetector == NULL))
    {
        // Set all scents (if any) to zero strength
        for (i = 0; i < numScents; i++)
            scents[i]->setStrength(0.0);

        return;
    }

    // Update all scent sources
    for (i = 0; i < numScentSources; i++)
    {
        scentSources[i]->update();
    }

    // Update the scent detector
    scentDetector->update();

    // Initialize the currentStrengths array
    memset(currentStrengths, 0, sizeof(currentStrengths));

    // Get the effective strength of each scent source, and set the
    // corresponding scent to the calculated strength.  If the same
    // scent is used by more than one source, use the greatest
    // strength.
    for (i = 0; i < numScentSources; i++)
    {
        // Get the vsScent object from the source
        currentScent = scentSources[i]->getScent();

        // Get the index of the scent in the scents array
        index = getScentIndex(currentScent);

        // If this scent is already at full strength (due to some other
        // scent source) we don't need to process this source
        if (currentStrengths[index] < 1.0)
        {
            // Calculate the distance between the source and the
            // detector
            scentVec = scentSources[i]->getPosition() - 
                scentDetector->getPosition();
            distance = scentVec.getMagnitude();

            // Get the scent attenuation parameters
            scale = scentSources[i]->getStrengthScale();
            reference = scentSources[i]->getReferenceDistance();
            maxDist = scentSources[i]->getMaxDistance();
            rolloff = scentSources[i]->getRolloffFactor();
            minStr = scentSources[i]->getMinStrength();
            maxStr = scentSources[i]->getMaxStrength();
            sensitivity = scentDetector->getSensitivity();

            // Mute the scent if the distance is greater than the
            // maximum distance
            if (distance > maxDist)
            {
                strength = 0.0;
            }
            else
            {
                // Clamp the distance to the given reference and maximum
                // parameters.  A scent will never be stronger than it is
                // at the reference distance
                if (distance < reference)
                    distance = reference;

                // Calculate the strength of the scent, accounting for
                // the gain adjustments and distance.
                if (fabs(rolloff) < 1.0e-6)
                {
                    // Cheap optimization, if the rolloff factor is very small,
                    // then no distance attenuation occurs
                    strength = scale;
                }
                else
                {
                    // Calculate the effective strength normally.  This uses 
                    // the same linear gain distance attenuation equation 
                    // used by OpenAL.  I'm hoping that scent attenuates 
                    // with distance in a manner similar to sound.
                    strength = scale / 
                        (1 + rolloff * ((distance - reference) / reference));
                }

                // Clamp the strength to the given minimum and maximum 
                // strength parameters.  A scent will never be stronger 
                // than its maximum strength, nor weaker than its minimum 
                // strength.
                if (strength < minStr)
                    strength = minStr;
                if (strength > maxStr)
                    strength = maxStr;

                // Adjust for the sensitivity of the detector
                strength *= sensitivity;

                // Finally, clamp the strength to the interval [0.0, 1.0]
                if (strength < 0.0)
                    strength = 0.0;
                if (strength > 1.0)
                    strength = 1.0;
	    }

            // See if this source is forcing the scent to be stronger
            // than it already is.
            if (strength > currentStrengths[index])
                currentStrengths[index] = strength;
        }
    }

    // Now that we know the strength of each scent, update each vsScent
    // object accordingly
    for (i = 0; i < numScents; i++)
    {
        scents[i]->setStrength(currentStrengths[i]);
    }
}
