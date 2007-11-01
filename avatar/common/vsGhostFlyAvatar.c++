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
//    VESS Module:  vsGhostFlyAvatar.h++
//
//    Description:  Invisible (no geometry) avatar with a vsFlyingMotion
//                  motion model attached. Automatically sets itself to
//                  view the given scene in the given pane.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsGhostFlyAvatar.h++"

#include "vsViewpointAttribute.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsGhostFlyAvatar::vsGhostFlyAvatar(vsPane *targetPane,
    vsScene *targetScene)
{
    // Store the given object pointers
    pane = targetPane;
    scene = targetScene;

    // Initialize the other pointers
    view = NULL;
    ghostKin = NULL;
    flyMotion = NULL;
    windowSystem = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsGhostFlyAvatar::~vsGhostFlyAvatar()
{
    // Delete the object associated with this avatar
    delete flyMotion;
    delete ghostKin;
    delete geometryRoot;
    delete view;
    if (windowSystem)
	delete windowSystem;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsGhostFlyAvatar::getClassName()
{
    return "vsGhostFlyAvatar";
}

// ------------------------------------------------------------------------
// Updates the avatar viewpoint
// ------------------------------------------------------------------------
void vsGhostFlyAvatar::update()
{
    // The window system object is only updated if we created it; if an
    // existing one was obtained instead, then windowSystem will be NULL.
    if (windowSystem)
	windowSystem->update();

    // Update those object that need updating every frame
    flyMotion->update();
    ghostKin->update();
}

// ------------------------------------------------------------------------
// Gets the vsKinematics object for this avatar. Note that this object is
// not created until init() is called on the avatar.
// ------------------------------------------------------------------------
vsKinematics *vsGhostFlyAvatar::getKinematics()
{
    return ghostKin;
}

// ------------------------------------------------------------------------
// Gets the vsFlyingMotion object for this avatar. Note that this object is
// not created until init() is called on the avatar.
// ------------------------------------------------------------------------
vsFlyingMotion *vsGhostFlyAvatar::getFlyingMotion()
{
    return flyMotion;
}

// ------------------------------------------------------------------------
// Sets up this avatar by creating the viewpoint, component, and motion
// model needed by this avatar.
// ------------------------------------------------------------------------
void vsGhostFlyAvatar::setup()
{
    vsViewpointAttribute *viewAttr;
    vsMouse *mouse;
    vsWindow *window;
    vsWindowSystem *wsys;
    atVector boundCenter;
    double boundRadius;
    atQuat initOrient;

    // There shouldn't be any objects in the object arrays; vsGhostFlyAvatar
    // doesn't need (or use) any.
    if (objectCount > 0)
	printf("vsGhostFlyAvatar::setup: Avatar setup does not need any "
	    "configuration objects\n");
	    
    // Create the component for the avatar and attach a viewpoint to it
    geometryRoot = new vsComponent();
    view = new vsView();
    viewAttr = new vsViewpointAttribute(view);
    geometryRoot->addAttribute(viewAttr);

    // Set the viewpoint and scene objects on the avatar's vsPane object
    pane->setView(view);
    pane->setScene(scene);

    // Obtain the mouse object for the given pane by checking its parent
    // window. If there is no window system for that window, create one
    // and set it so that it gets updated with everything else each frame.
    window = pane->getParentWindow();
    wsys = (vsWindowSystem *)
	((vsWindowSystem::getMap())->mapFirstToSecond(window));
    if (!wsys)
    {
	windowSystem = new vsWindowSystem(window);
	wsys = windowSystem;
    }
    mouse = wsys->getMouse();

    // Create the kinematics and motion model
    ghostKin = new vsKinematics(geometryRoot);
    flyMotion = new vsFlyingMotion(mouse, ghostKin);
    
    // Set the kinematics to have a default position outside the bounding
    // sphere of the scene, and a default orientation pointing towards
    // the scene center.
    scene->getBoundSphere(&boundCenter, &boundRadius);
    ghostKin->setPosition(atVector(boundCenter[0] + boundRadius,
	boundCenter[1] + boundRadius, boundCenter[2] + boundRadius));
    initOrient.setVecsRotation(atVector(0.0, 1.0, 0.0),
	atVector(0.0, 0.0, 1.0), atVector(-1.0, -1.0, -1.0),
	atVector(0.0, 0.0, 1.0));
    ghostKin->setOrientation(initOrient);

    // Complete the process by adding the avatar's 'geometry' to the scene
    scene->addChild(geometryRoot);
}
