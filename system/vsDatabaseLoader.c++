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
//    VESS Module:  vsDatabaseLoader.c++
//
//    Description:  Object for loading scene databases from files
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsDatabaseLoader.h++"

#include <stdlib.h>
#include <string.h>
#include <Performer/pf/pfGeode.h>
#include <Performer/pf/pfBillboard.h>
#include <Performer/pf/pfDCS.h>
#include <Performer/pfdu.h>
#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructor - Adds the given file extension as the first in the loader's
// list of file extensions. Initializes the list of important node names.
// ------------------------------------------------------------------------
vsDatabaseLoader::vsDatabaseLoader() : nodeNames(0, 50)
{
    // Important name list starts empty
    nodeNameCount = 0;
    
    // Default unit mode is meters
    unitMode = VS_DATABASE_UNITS_METERS;
    
    // 'Transforms are important' mode defaults to off
    importantXformMode = VS_FALSE;
    
    // System is not initialized yet
    inittedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDatabaseLoader::~vsDatabaseLoader()
{
    // Delete all important names
    clearNames();
}

// ------------------------------------------------------------------------
// Initializes the internal database loader corresponding to the given
// filename extension. Can only be called before the vsSystem object is
// initialized.
// ------------------------------------------------------------------------
void vsDatabaseLoader::initExtension(char *fileExtension)
{
    fltRegisterNodeT callbackPtr;
    void *callbackHandle;

    // Can't initialize an extension after the system object finishes
    // its initialization
    if (inittedFlag)
    {
        printf("vsDatabaseLoader::initExtension: Can't initialize extensions "
	    "after initialization of vsSystem object\n");
        return;
    }

    // Attempt the initialization of the specified loader
    if (!pfdInitConverter(fileExtension))
    {
        printf("vsDatabaseLoader::initExtension: Unable to initialize '%s' "
	    "loader\n", fileExtension);
        return;
    }

    // Perform extra initialization based on the particular loader
    if (classifyExtension(fileExtension) == VS_DATABASE_TYPE_FLT)
    {
        // OpenFlight specific stuff: custom callback, flatten and
	// clean modes disabled
        callbackPtr = fltLoaderCallback;
        callbackHandle = &callbackPtr;
        pfdConverterAttr_flt(PFFLT_REGISTER_NODE, callbackHandle);
        pfdConverterMode_flt(PFFLT_FLATTEN, PF_OFF);
        pfdConverterMode_flt(PFFLT_CLEAN, PF_OFF);
    }
}

// ------------------------------------------------------------------------
// Adds the given node name to the loader's list of "important" node names.
// Nodes with names appearing in the loader's name list are given special
// attention during the database loading process.
// ------------------------------------------------------------------------
void vsDatabaseLoader::addImportantNodeName(char *newName)
{
    // Allocate memory for the new name and copy the name to the new memory
    nodeNames[nodeNameCount] = strdup(newName);

    // Verify that the allocation was successful; increment the list size
    // if so.
    if (!(nodeNames[nodeNameCount]))
        printf("vsDatabaseLoader::addImportantNodeName: Error allocating "
            "space for node name string\n");
    else
        nodeNameCount++;
}

// ------------------------------------------------------------------------
// Completely erases the loader's list of "important" node names.
// ------------------------------------------------------------------------
void vsDatabaseLoader::clearNames()
{
    int loop;
    
    // Delete all names in the list
    for (loop = 0; loop < nodeNameCount; loop++)
        free(nodeNames[loop]);

    // Set the list size to empty
    nodeNameCount = 0;
}

// ------------------------------------------------------------------------
// Sets the unit translation scale used when loading in a database. Only
// has an effect if set before the database is loaded.
// ------------------------------------------------------------------------
void vsDatabaseLoader::setUnits(int databaseUnit)
{
    unitMode = databaseUnit;
}

// ------------------------------------------------------------------------
// Adds the given directory path to the search path list for loading new
// databases
// ------------------------------------------------------------------------
void vsDatabaseLoader::addPath(char *filePath)
{
    const char *performerPath;
    char *fullPath;
    
    // Get the current file path from Performer
    performerPath = pfGetFilePath();

    // Determine if the current path is empty or not
    if (!performerPath)
    {
	// No existing path; create a new one that contains just a '.'
	fullPath = (char *)(malloc(strlen(filePath) + 5));
	if (!fullPath)
	{
	    printf("vsDatabaseLoader::addPath: Memory allocation error\n");
	    return;
	}
        strcpy(fullPath, ".");
    }
    else
    {
	// Path already exists; copy what's there into our new buffer
	fullPath = (char *)
	    (malloc(strlen(filePath) + strlen(pfGetFilePath()) + 5));
	if (!fullPath)
	{
	    printf("vsDatabaseLoader::addPath: Memory allocation error\n");
	    return;
	}
        strcpy(fullPath, pfGetFilePath());
    }

    // Append the new path
    strcat(fullPath, ":");
    strcat(fullPath, filePath);
    
    // Set the Performer path to our newly calculated path
    pfFilePath(fullPath);
    printf("Path is: %s\n", fullPath);
    
    // Delete the memory needed by the path; Performer should have copied
    // the path into to its own data area by now.
    free(fullPath);
}

// ------------------------------------------------------------------------
// Clears the directory path
// ------------------------------------------------------------------------
void vsDatabaseLoader::clearPath()
{
    // Set the current Performer path to the application directory only
    pfFilePath(".");
}

// ------------------------------------------------------------------------
// Sets the specified loader mode to the given value
// ------------------------------------------------------------------------
void vsDatabaseLoader::setLoaderMode(int whichMode, int modeVal)
{
    // Interpret the whichMode constant
    switch (whichMode)
    {
        case VS_DATABASE_MODE_NAME_XFORM:
            importantXformMode = modeVal;
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the value of the specified loader mode
// ------------------------------------------------------------------------
int vsDatabaseLoader::getLoaderMode(int whichMode)
{
    // Interpret the whichMode constant
    switch (whichMode)
    {
        case VS_DATABASE_MODE_NAME_XFORM:
            return importantXformMode;
    }
    
    // If the constant is unrecognized, return a default value
    return 0;
}

// ------------------------------------------------------------------------
// Creates a VESS scene graph from the geometric data stored within the
// given named database file. The database file must have an extension
// that was specified as a valid extension to the loader before the
// vsSystem object was created.
// ------------------------------------------------------------------------
vsComponent *vsDatabaseLoader::loadDatabase(char *databaseFilename)
{
    vsNode *dbRoot;
    vsComponent *result;
    pfNode *performerGraph;

    // Verify that the system object has been intialized
    if (!inittedFlag)
    {
        printf("vsDatabaseLoader::loadDatabase: Can't load database until "
            "vsSystem has been initialized\n");
        return NULL;
    }

    // Do loader specific work here
    if (classifyExtension(databaseFilename) == VS_DATABASE_TYPE_FLT)
    {
        // Is an OpenFlight file...  set the database units
        switch (unitMode)
        {
            case VS_DATABASE_UNITS_METERS:
                pfdConverterMode_flt(PFFLT_USEUNITS, FLT_METERS);
                break;
            case VS_DATABASE_UNITS_FEET:
                pfdConverterMode_flt(PFFLT_USEUNITS, FLT_FEET);
                break;
            case VS_DATABASE_UNITS_KILOMETERS:
                pfdConverterMode_flt(PFFLT_USEUNITS, FLT_KILOMETERS);
                break;
        }
    }

    // Attempt to load the specified file
    performerGraph = pfdLoadFile(databaseFilename);
    if (!performerGraph)
    {
        printf("vsDatabaseLoader::loadDatabase: Load of '%s' failed\n",
	    databaseFilename);
        return NULL;
    }

    // Do loader specific work here
    if (classifyExtension(databaseFilename) == VS_DATABASE_TYPE_FLT)
    {
        // Is an OpenFlight file...  fix the DOF/DCS nodes
        fixPerformerFltDOF(performerGraph);
    }

    // Separate each geoset into a different geode. This may slow things down
    // slightly but ultimately the scene graph is easier to handle this way.
    fixGeodes(performerGraph);

    // Translate the Performer graph into a VESS tree
    if (performerGraph->isOfType(pfGroup::getClassType()))
        dbRoot = new vsComponent((pfGroup *)performerGraph, this);
    else if (performerGraph->isOfType(pfGeode::getClassType()))
        dbRoot = new vsGeometry((pfGeode *)performerGraph);
    else
    {
        printf("vsDatabaseLoader::loadDatabase: No geometry found\n");
        pfDelete(performerGraph);
        dbRoot = NULL;
    }

    // * Replace all pfBillboards in the Performer scene with pfGeodes; the
    // database loader should have extracted all of the relavant information
    // into vsBillboardAttributes by now, so the pfBillboards should be
    // removes so they don't get in the way.
    replaceBillboards(performerGraph);

    // Package the resulting database into its own component and return
    result = new vsComponent();
    result->addChild(dbRoot);

    // Return the loaded scene graph
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Attempts to match the given filename extension (either a full filename
// or just the extension by itself) to the object's list of file types
// that need special attention during the loading process. Returns a
// constant specifying the type of the extension, or a generic constant
// if the extension is not recognized.
// ------------------------------------------------------------------------
int vsDatabaseLoader::classifyExtension(char *name)
{
    char *fileExtension;

    // Search for the last . in the extension name
    fileExtension = strrchr(name, '.');

    // OpenFlight
    if (!strcmp(name, "flt") ||
        (fileExtension && !strcmp(fileExtension, ".flt")))
        return VS_DATABASE_TYPE_FLT;

    // Default
    return VS_DATABASE_TYPE_DEFAULT;
}

// ------------------------------------------------------------------------
// Private function
// "Fixes" Performer DCS nodes generated by the OpenFlight loader when
// reading in DOF beads. The fix involves creating a pair of SCS nodes
// around the DCS to allow for correct off-origin rotations.
// ------------------------------------------------------------------------
void vsDatabaseLoader::fixPerformerFltDOF(pfNode *node)
{
    int loop;
    int childCount;
    pfNode *childNode;
    pfGroup *groupNode, *parentGroup;
    pfDCS *DCSNode;
    pfSCS *aboveSCS, *belowSCS;
    vsdbMatrixBlock *myData;

    // The Performer OpenFlight loader doesn't handle DOF beads correctly.
    // Specifically, it ignores the put matrices in the DOF that allow the
    // object to rotate around some point other than the origin. To get
    // around this, we have a loader callback that extracts these matrices
    // from the DOFs during the load process, and stores them in the user
    // data block of the pfDCS node associated with the DOF. This function
    // searches the Performer scene for pfDCSs with these matrix data
    // blocks attached to them, extracts the matrices, creates a pair of
    // pfSCS nodes bracketing the pfDCS that utilize the matrix data, and
    // gets rid of the data block when it's done.

    // First, if this node is a group, recurse down through the scene
    if (node->isOfType(pfGroup::getClassType()))
    {
        // Type cast
        groupNode = (pfGroup *)node;

        // Call this function on each child of the group
        childCount = groupNode->getNumChildren();
        for (loop = 0; loop < childCount; loop++)
            fixPerformerFltDOF(groupNode->getChild(loop));
    }

    // Second, if this node is a DCS, apply the fix
    if (node->isOfType(pfDCS::getClassType()))
    {
        // Type cast
        DCSNode = (pfDCS *)node;

        // Get the matrix data block from the pfDCS
        myData = (vsdbMatrixBlock *)(DCSNode->getUserData());
        if (!myData || (strcmp(myData->magicString, "DOF")))
        {
            // Error: loader matrix data not available
            printf("vsDatabaseLoader::fixPerformerFltDOF: Unable to obtain "
                "DOF data\n");
            return;
        }

        // Create two new pfSCS groups from the matrix data in the block
        aboveSCS = new pfSCS(myData->aboveMatrix);
        belowSCS = new pfSCS(myData->belowMatrix);

        // Move all of the children of the DCS to the 'below' SCS
        while (DCSNode->getNumChildren() > 0)
        {
            // Get the first child of the pfDCS, remove it from the
	    // DCS, and add it to the 'below' SCS
            childNode = DCSNode->getChild(0);
            DCSNode->removeChild(childNode);
            belowSCS->addChild(childNode);
        }

	// Put the new pfSCS nodes into place by setting the parent of
	// the DCS to point to the 'above' SCS instead, setting the
	// DCS as the child of the 'above' SCS, and setting the 'below'
	// SCS as a child of the DCS.
        parentGroup = DCSNode->getParent(0);
        parentGroup->replaceChild(DCSNode, aboveSCS);
        aboveSCS->addChild(DCSNode);
        DCSNode->addChild(belowSCS);

        // Clean up; delete the data and reset the user data pointer
        DCSNode->setUserData(NULL);
        pfMemory::free(myData);
    }
}

// ------------------------------------------------------------------------
// Private function
// "Fixes" Performer geodes by splitting the geosets within the geode
// into separate geodes. This action makes the VESS scene graph easier to
// manipulate.
// ------------------------------------------------------------------------
void vsDatabaseLoader::fixGeodes(pfNode *targetGraph)
{
    int loop, childCount;
    pfGeode *oldGeode, *newGeode;
    pfBillboard *oldBillboard, *newBillboard;
    pfVec3 data;
    pfGroup *newMasterGroup, *parentGroup;
    pfGeoSet *tempGeoset;
    
    // Determine the type of the Performer node
    if (targetGraph->isOfType(pfGroup::getClassType()))
    {
	// Recurse on the group's children
        childCount = ((pfGroup *)targetGraph)->getNumChildren();
        for (loop = 0; loop < childCount; loop++)
            fixGeodes(((pfGroup *)targetGraph)->getChild(loop));
    }
    else if (targetGraph->isOfType(pfGeode::getClassType()))
    {
        // Store the source geode
        oldGeode = (pfGeode *)targetGraph;

	// Nothing to do if there's only one geoset on this geode
        if (oldGeode->getNumGSets() > 1)
        {
	    // Create a new group that will hold all of the new geodes
            newMasterGroup = new pfGroup();

            // Set each parent of the source geode to point to our
	    // new group instead
            while (oldGeode->getNumParents() > 0)
            {
                parentGroup = oldGeode->getParent(0);
                parentGroup->replaceChild(oldGeode, newMasterGroup);
            }
            
            // 'Fix' the geode by creating one geode for every geoset,
	    // and adding those geodes to the new group
            while (oldGeode->getNumGSets() > 0)
            {
                // Get the first geoset on the geode
                tempGeoset = oldGeode->getGSet(0);

                // Check to see if this geode is actually a pfBillboard
                if (oldGeode->isOfType(pfBillboard::getClassType()))
                {
		    // Create a new pfBillboard object and copy all of
		    // the billboard parameters to it
                    oldBillboard = (pfBillboard *)oldGeode;
                    newBillboard = new pfBillboard();
                    oldBillboard->getPos(0, data);
                    newBillboard->setPos(0, data);
                    newBillboard->setMode(PFBB_ROT,
                        oldBillboard->getMode(PFBB_ROT));
                    oldBillboard->getAxis(data);
                    newBillboard->setAxis(data);
                    newGeode = newBillboard;
                }
                else
                {
                    // Create a new pfGeode object
                    newGeode = new pfGeode();
                }

                // Move the target geoset from the original geode
		// to its very own geode: the one we just created. Also
		// add the new geode to our master group.
                oldGeode->removeGSet(tempGeoset);
                newGeode->addGSet(tempGeoset);
                newMasterGroup->addChild(newGeode);
            }

            // By now the source geode should be empty and is no
	    // longer needed
            pfDelete(oldGeode);
        }
    }
}

// ------------------------------------------------------------------------
// Private function
// Replaces every billboard in the Performer scene graph with an ordinary
// geode. This is done after the billboard data has been collected by the
// VESS scene graph construction pass so that the Performer billboards no
// longer have any effect; their responsibility is taken up by the
// vsBillboardAttribute objects.
// ------------------------------------------------------------------------
void vsDatabaseLoader::replaceBillboards(pfNode *targetGraph)
{
    int childCount, loop;
    pfBillboard *oldBillboard;
    pfGeode *newGeode;
    pfGeoSet *geoset;
    vsGeometry *boundGeom;
    pfGroup *parentGroup;
    vsObjectMap *nodeMap;

    // Determine the type of the Performer node
    if (targetGraph->isOfType(pfGroup::getClassType()))
    {
	// Recurse on the group's children
        childCount = ((pfGroup *)targetGraph)->getNumChildren();
        for (loop = 0; loop < childCount; loop++)
            fixGeodes(((pfGroup *)targetGraph)->getChild(loop));
    }
    else if (targetGraph->isOfType(pfBillboard::getClassType()))
    {
        // Replace every encountered pfBillboard with a pfGeode.
        oldBillboard = (pfBillboard *)targetGraph;
        newGeode = new pfGeode();
        
        // Move all of the geosets from the billboard to the geode
        while (oldBillboard->getNumGSets() > 0)
        {
            geoset = oldBillboard->getGSet(0);
            oldBillboard->removeGSet(geoset);
            newGeode->addGSet(geoset);
        }
        
        // Replace the billboard with the geode for each parent of the
        // billboard
        while (oldBillboard->getNumParents() > 0)
        {
            parentGroup = oldBillboard->getParent(0);
            parentGroup->replaceChild(oldBillboard, newGeode);
        }
        
        // Replace the billboard with the geode in the global object map
        nodeMap = (vsSystem::systemObject)->getNodeMap();
        boundGeom = (vsGeometry *)
            (nodeMap->mapSecondToFirst(oldBillboard));
        if (boundGeom)
        {
            nodeMap->removeLink(boundGeom, VS_OBJMAP_FIRST_LIST);
            nodeMap->registerLink(boundGeom, newGeode);
        }
        
        // Clean up
        pfDelete(oldBillboard);
    }
}

// ------------------------------------------------------------------------
// VESS internal function
// Signals to the loader that the system object is being initialized and
// that it should not accept any more requests to initialize file
// extensions. This is done because Performer requires loader
// initializations to be completed before any processes fork.
// ------------------------------------------------------------------------
void vsDatabaseLoader::init()
{
    // Take note that the system object has been initialized
    inittedFlag = 1;
}

// ------------------------------------------------------------------------
// static VESS internal function - Callback given to the OpenFlight loader
// When loading in an OpenFlight format file, this loader callback is used
// to prepare DOF beads to be 'fixed'; the standard translation from DOFs
// to Performer DCSs is incomplete. The function stores matrix data taken
// from the OpenFlight loader and stores it in the DCS's user data field,
// to be later retrieved by the fixPerformerFltDOF function.
// ------------------------------------------------------------------------
void vsDatabaseLoader::fltLoaderCallback(pfNode *node, int mgOp, int *cbs,
                                         COMMENTcb *comment, void *userData)
{
    // For each DCS node, pull the pre/post static transform matricies out
    // of the Flight loader data block and stuff them into the DCS's
    // user data field for later retrieval.

    DOFcb *loaderDOFBlock;
    pfDCS *currentDCS;
    vsdbMatrixBlock *myData;
    
    switch (mgOp)
    {
        case CB_DOF:
            loaderDOFBlock = (DOFcb *)cbs;
            currentDCS = (pfDCS *)node;
            myData = (vsdbMatrixBlock *)
                (pfMemory::malloc(sizeof(vsdbMatrixBlock)));
            if (!myData)
            {
                printf("vsDatabaseLoader::fltLoaderCallback: Memory "
                    "allocation failure\n");
                return;
            }

            strcpy(myData->magicString, "DOF");
            myData->aboveMatrix.copy(loaderDOFBlock->putinvmat);
            myData->belowMatrix.copy(loaderDOFBlock->putmat);

            currentDCS->setUserData(myData);

            pfDelete(cbs);
            if (comment)
                pfDelete(comment);

            break;

        case CB_CLEANNODE:
            *cbs = TRUE;

        case CB_CLONE:
            break;
        
        default:
            if (cbs)
                pfDelete(cbs);
            if (comment)
                pfDelete(comment);
            break;
    }
}

// ------------------------------------------------------------------------
// VESS internal function
// Checks to see if the given node's name is part of the loader's list of
// 'important' node names, or if the node is a DCS and the user has
// specified that DCS's are automatically important. The name check is case
// sensitive.
// ------------------------------------------------------------------------
int vsDatabaseLoader::importanceCheck(pfNode *targetNode)
{
    int loop;
    const char *targetName;
    
    // Get the name of the node
    targetName = targetNode->getName();

    // Compare the node's name to each important name; return true if
    // there's a match
    for (loop = 0; loop < nodeNameCount; loop++)
        if (!strcmp((char *)(nodeNames[loop]), targetName))
            return VS_TRUE;

    // Check the type of the node; if the node is a DCS, and the
    // 'transforms are important' mode is on, then the node is important
    if (importantXformMode && targetNode->isOfType(pfDCS::getClassType()))
        return VS_TRUE;

    // If we got this far, then the node must not be important
    return VS_FALSE;
}
