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

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsGhostFlyAvatar::vsGhostFlyAvatar(vsPane *targetPane,
    vsComponent *targetScene)
{
    pane = targetPane;
    scene = targetScene;

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
    delete flyMotion;
    delete ghostKin;
    delete geometryRoot;
    delete view;
    if (windowSystem)
	delete windowSystem;
}

// ------------------------------------------------------------------------
// Updates the avatar viewpoint
// ------------------------------------------------------------------------
void vsGhostFlyAvatar::update()
{
    if (windowSystem)
	windowSystem->update();

    flyMotion->update();
    ghostKin->update();
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
    vsVector boundCenter;
    double boundRadius;
    vsQuat initOrient;

    if (objectCount > 0)
	printf("vsGhostFlyAvatar::setup: Avatar setup does not need any "
	    "configuration objects\n");
	    
    // Create the component for the avatar and attach a viewpoint to it
    geometryRoot = new vsComponent();
    view = new vsView();
    viewAttr = new vsViewpointAttribute(view);
    geometryRoot->addAttribute(viewAttr);

    pane->setView(view);
    pane->setScene(scene);

    // Obtain the mouse object for the given pane by checking its parent
    // window. If there is no window system for that window, create one
    // and set it so that its gets updated with everything else each frame.
    window = pane->getParentWindow();
    wsys = window->getWSystem();
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
    ghostKin->setPosition(vsVector(boundCenter[0] + boundRadius,
	boundCenter[1] + boundRadius, boundCenter[2] + boundRadius));
    initOrient.setVecsRotation(vsVector(0.0, 1.0, 0.0),
	vsVector(0.0, 0.0, 1.0), vsVector(-1.0, -1.0, -1.0),
	vsVector(0.0, 0.0, 1.0));
    ghostKin->setOrientation(initOrient);

    // Complete the process by adding the avatar's 'geometry' to the scene
    scene->addChild(geometryRoot);
}
