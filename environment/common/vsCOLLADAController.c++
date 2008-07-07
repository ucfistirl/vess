
#include "vsCOLLADAController.h++"


// ------------------------------------------------------------------------
// Create a collada controller that uses the given geometry as its source
// ------------------------------------------------------------------------
vsCOLLADAController::vsCOLLADAController(vsCOLLADAGeometry *source)
{
    // Store the source geometry
    sourceGeometry = source;
    sourceGeometry->ref();

    // Create a list for our data sources
    dataSources = new atMap();
}

// ------------------------------------------------------------------------
// Cleans up this controller
// ------------------------------------------------------------------------
vsCOLLADAController::~vsCOLLADAController()
{
    // Destroy all of our data sources
    delete dataSources;

    // Unreference the source geometry object (don't delete it since it's
    // owned by the main loader's geometry library map)
    sourceGeometry->unref();
}
