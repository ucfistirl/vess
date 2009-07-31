
#include "vsCOLLADAController.h++"


// ------------------------------------------------------------------------
// Create a collada controller that uses the given geometry as its source
// ------------------------------------------------------------------------
vsCOLLADAController::vsCOLLADAController(vsCOLLADAGeometry *source)
{
    // Store the source geometry
    sourceGeometry = source;
    sourceGeometry->ref();

    // Create a map for our data sources (these will be added by descendants)
    dataSources = new atMap();
}

// ------------------------------------------------------------------------
// Cleans up this controller
// ------------------------------------------------------------------------
vsCOLLADAController::~vsCOLLADAController()
{
    // Destroy all of our data sources
    delete dataSources;

    // Unreference the source geometry object
    vsObject::unrefDelete(sourceGeometry);
}

