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
//    VESS Module:  vsBoundingVolume.c++
//
//    Description:  A vsBoundingVolume is the primary class used for
//                  collision detection. Each vsBoundingVolume is a
//                  composite of one or more vsBoundingSurfaces. This
//                  version uses the built-in ODE collision engine.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsBoundingVolume.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsBoundingVolume::vsBoundingVolume()
{
    // Begin with an empty atList.
    surfaceList = new atList();

    // Create an ODE space to hold the bounding volume.
    odeSpaceID = dSimpleSpaceCreate(0);

    // Begin with no offset.
    volumeOffset.set(0.0, 0.0, 0.0);

    // By default, the volume is free to be assigned.
    volumeLocked = false;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsBoundingVolume::~vsBoundingVolume()
{
    // Clear out all of the geometries in this bounding volume.
    clear();

    // Destroy the space that had contained the geometries.
    dSpaceDestroy(odeSpaceID);

    // Free up the storage. Note that the previous clear call will already
    // have emptied this list.
    delete surfaceList;
}

// ------------------------------------------------------------------------
// Include the new bounding surface in this volume.
// ------------------------------------------------------------------------
void vsBoundingVolume::addSurface(vsBoundingSurface *surface)
{
    // Add the geometry of the new bounding surface to this bounding volume.
    dSpaceAdd(odeSpaceID, surface->getODEGeomID());

    // Store the new vsBoundingSurface in the list.
    surfaceList->addEntry(surface);

    // Account for this new possession by adding a reference to the object.
    surface->ref();
}

// ------------------------------------------------------------------------
// Return the number of surfaces currently contained in this volume.
// ------------------------------------------------------------------------
int vsBoundingVolume::getSurfaceCount()
{
    return surfaceList->getNumEntries();
}

// ------------------------------------------------------------------------
// Return the surface corresponding to the provided index, or NULL if the
// index is invalid.
// ------------------------------------------------------------------------
vsBoundingSurface *vsBoundingVolume::getSurface(int index)
{
    // Make sure the index is valid before proceeding.
    if ((index >= 0) && (index < surfaceList->getNumEntries()))
    {
        return (vsBoundingSurface *)surfaceList->getNthEntry(index);
    }

    // Return NULL by default.
    return NULL;
}

// ------------------------------------------------------------------------
// Clear the entire bounding space of the object.
// ------------------------------------------------------------------------
void vsBoundingVolume::clear()
{
    vsBoundingSurface *surface;
    int i;

    // Iterate through the space removing bounding surfaces from the space.
    surface = (vsBoundingSurface *)surfaceList->getFirstEntry();
    while (surface)
    {
        // Remove the surface ID from the space.
        dSpaceRemove(odeSpaceID, surface->getODEGeomID());

        // Lose the reference to this vsBoundingSurface.
        vsObject::unrefDelete(surface);

        // Move on to the next surface.
        surface = (vsBoundingSurface *)surfaceList->getNextEntry();
    }
}

// ------------------------------------------------------------------------
// Attempt to collide the two bounding volumes, returning a structure
// describing the result of the attempt. This structure will not be
// cleaned up automatically; it must be freed by the calling method. This
// method will generate a maximum of VS_BOUNDING_VOLUME_MAX_COLLISIONS
// results.
// ------------------------------------------------------------------------
vsCollisionResult *vsBoundingVolume::collide(vsBoundingVolume *target)
{
    // Pass the call along.
    return collide(target, VS_BOUNDING_VOLUME_MAX_COLLISIONS);
}

// ------------------------------------------------------------------------
// Attempt to collide the two bounding volumes, returning a structure
// describing the result of the attempt. This structure will not be
// cleaned up automatically; it must be freed by the calling method. This
// method will generate a maximum number of results equal to the second
// argument.
// ------------------------------------------------------------------------
vsCollisionResult *vsBoundingVolume::collide(vsBoundingVolume *target, int max)
{
    vsCollisionProgress *progress;
    vsCollisionResult *result;
    int i;

    // Allocate the structure to be passed into the collision function.
    progress = (vsCollisionProgress *)malloc(sizeof(vsCollisionProgress));

    // Allocate memory to hold the maximum number of contact geom results.
    progress->contactGeoms =
        (dContactGeom *)malloc(max * sizeof(dContactGeom));
    progress->maxCollisions = max;
    progress->curCollisions = 0;

    // Pass the structure into the collision function, which will fill
    // in the progress and result structures.
    dSpaceCollide2((dGeomID)odeSpaceID, (dGeomID)target->getODESpaceID(),
        progress, vsBoundingVolume::nearCallback);

    // Prepare the structure for passing by allocating the maximum number
    // of collisions.
    result = (vsCollisionResult *)malloc(sizeof(vsCollisionResult));
    result->contactCount = progress->curCollisions;

    // Handle the case where no collisions occurred seperately.
    if (result->contactCount == 0)
    {
        result->contactPoints = NULL;
    }
    else
    {
        result->contactPoints = (vsContactPoint **)
            malloc(progress->curCollisions * sizeof(vsContactPoint *));

        // Create contact points from the resulting collision geometries.
        for (i = 0; i < result->contactCount; i++)
        {
            result->contactPoints[i] =
                new vsContactPoint(progress->contactGeoms[i]);
        }
    }

    // Free up the progress structure and the contact geoms, as they are no
    // longer necessary.
    free(progress->contactGeoms);
    free(progress);

    // Return the result structure to the user.
    return result;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Attempts to lock the bounding volume, preventing any other dynamics from
// claiming it for collision purposes. If the volume is already locked, it
// will return false indicating failure, otherwise it will return true.
// ------------------------------------------------------------------------
bool vsBoundingVolume::lock()
{
    // If the volume is already locked, return false to indicate failure.
    if (volumeLocked)
        return false;

    // Lock the volume and indicate success.
    volumeLocked = true;
    return true;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Unlocks the bounding volume, allowing other dynamics objects to claim it
// for collision purposes.
// ------------------------------------------------------------------------
void vsBoundingVolume::unlock()
{
    volumeLocked = false;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Returns whether this bounding volume may be claimed by a dynamics object
// for collision purposes.
// ------------------------------------------------------------------------
bool vsBoundingVolume::isLocked()
{
    return volumeLocked;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Returns the ODE space ID containing all of the geometry of this
// vsBoundingVolume.
// ------------------------------------------------------------------------
dSpaceID vsBoundingVolume::getODESpaceID()
{
    return odeSpaceID;
}

// ------------------------------------------------------------------------
// Private Static Function
// This method is the workhorse of the collision process.
// ------------------------------------------------------------------------
void vsBoundingVolume::nearCallback(void *data, dGeomID geomA, dGeomID geomB)
{
    vsCollisionProgress *progress;
    int remainingContacts;

    // Store the progress value in a more accessible form.
    progress = (vsCollisionProgress *)data;

    // See how many contact points may still be generated.
    remainingContacts = progress->maxCollisions - progress->curCollisions;

    // Make sure further collisions may be performed.
    if (remainingContacts > 0)
    {
        // Attempt the collision, using the two geometries, attempting the
        // maximum remaining number of collisions. Store how many contacts
        // were generated using the newResults value.
        progress->curCollisions += dCollide(geomA, geomB, remainingContacts,
            &(progress->contactGeoms[progress->curCollisions]),
            sizeof(dContactGeom));
    }
}

// ------------------------------------------------------------------------
// VESS Internal Function
// Because ODE will automatically adjust the center of mass to be at the
// origin of its dynamic unit, any changes to the center of mass (specified
// in model space) may alter the relative position of model origin and
// center of mass. This requires an offset between the two be maintained,
// because both the mass distribution and bounding surfaces are specified
// in model coordinates but applied by ODE in dynamic unit coordinates.
// ------------------------------------------------------------------------
void vsBoundingVolume::setSurfaceOffset(const atVector &offset)
{
    atVector offsetDifference;
    vsBoundingSurface *surface;
    int i;

    // Determine the distance between the previous offset and the new one.
    offsetDifference = offset.getDifference(volumeOffset);

    // Loop through the bounding surfaces and modify the translation of the
    // non-planar surfaces with the new offset.
    surface = (vsBoundingSurface *)surfaceList->getFirstEntry();
    while (surface)
    {
        // Fetch the surface pointer and remove it from the space.
        surface->modifyOffset(offsetDifference);

        // Move on to the next surface.
        surface = (vsBoundingSurface *)surfaceList->getNextEntry();
    }

    // Store the new offset.
    volumeOffset.clearCopy(offset);
}

