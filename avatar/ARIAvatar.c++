// File ARIAvatar.c++

#include "ARIAvatar.h++"

#include "vsSwitchAttribute.h++"

ARIAvatar::ARIAvatar(vsComponent *theScene)
{
    scene = theScene;
}

ARIAvatar::~ARIAvatar()
{
    delete mstar;
    delete unwinder;

    delete root;
    delete head;
    delete r_shoulder;
    delete r_elbow;
    delete r_wrist;

    delete headMotion;
    delete walkMotion;
    delete armMotion;
    delete tFollow;
    delete collide;
}

void ARIAvatar::update()
{
    mstar->update();
    unwinder->update();
    
    headMotion->update();
    walkMotion->update();
    armMotion->update();
    
    tFollow->update();
    collide->update();
    
    root->update();
    head->update();
    r_shoulder->update();
    r_elbow->update();
    r_wrist->update();
}

vsView *ARIAvatar::getLeftEyeView()
{
    return leftEyeView;
}

vsView *ARIAvatar::getRightEyeView()
{
    return rightEyeView;
}

vsKinematics *ARIAvatar::getRootKin()
{
    return root;
}

int ARIAvatar::getButtonPress()
{
    vsJoystick *joy1;
    vsInputButton *but1;
    
    // Check the attached joystick to see if the first button is pressed
    joy1 = unwinder->getJoystick();
    but1 = joy1->getButton(0);
    
    if (but1->isPressed())
	return 1;

    return 0;
}

void *ARIAvatar::makeArmData()
{
    char cfgLine[256];
    char token[256];
    int lineType = 0;
    armData *result;
    double x, y, z;
    
    result = (armData *)calloc(sizeof(armData), 1);
    if (!result)
	return NULL;

    // Obtain the point-offset data for the arm motion model by extracting
    // it from the config file, and fill up the appropriate data structure
    // with it.
    while (lineType != -1)
    {
        lineType = readCfgLine(cfgLine);
        if (lineType != 0)
            continue;

        sscanf(cfgLine, "%s", token);
	
	if (!strcmp(token, "shoulderOffset"))
	{
	    sscanf(cfgLine, "%*s %lf %lf %lf", &x, &y, &z);
	    result->shoulderOffset.set(x, y, z);
	}
	else if (!strcmp(token, "elbowOffset"))
	{
	    sscanf(cfgLine, "%*s %lf %lf %lf", &x, &y, &z);
	    result->elbowOffset.set(x, y, z);
	}
	else if (!strcmp(token, "wristOffset"))
	{
	    sscanf(cfgLine, "%*s %lf %lf %lf", &x, &y, &z);
	    result->wristOffset.set(x, y, z);
	}
    }
    
    return result;
}

void *ARIAvatar::createObject(char *idString)
{
    void *result;
    
    result = vsAvatar::createObject(idString);
    if (result)
	return result;
    
    if (!strcmp(idString, "armData"))
	return makeArmData();
    
    return NULL;
}

void ARIAvatar::setup(vsGrowableArray *objArray, vsGrowableArray *strArray,
                        int objCount)
{
    armData *armOffsets;
    int loop;
    vsNode *avatarNode;
    vsComponent *headComponent;
    vsComponent *leftEyeComp, *rightEyeComp;
    vsTransformAttribute *leftEyeXform, *rightEyeXform;
    vsViewpointAttribute *leftEyeViewAttrib, *rightEyeViewAttrib;
    vsMatrix offsetMat;
    vsSwitchAttribute *headSwitch;
    vsVector colPoint;
    
    mstar = NULL;
    unwinder = NULL;
    armOffsets = NULL;
    geometryRoot = NULL;
    
    // Pull the objects out of the input arrays
    for (loop = 0; loop < objCount; loop++)
    {
	if (!strcmp((char *)((*strArray)[loop]), "mstar"))
	    mstar = (vsEthernetMotionStar *)((*objArray)[loop]);
	else if (!strcmp((char *)((*strArray)[loop]), "unwinder"))
	    unwinder = (vsUnwinder *)((*objArray)[loop]);
	else if (!strcmp((char *)((*strArray)[loop]), "armData"))
	    armOffsets = (armData *)((*objArray)[loop]);
	else if (!strcmp((char *)((*strArray)[loop]), "geometry"))
	    geometryRoot = (vsComponent *)((*objArray)[loop]);
    }
    
    if (!mstar)
    {
	printf("ARIAvatar::setup: No motion star found\n");
	isInitted = 0;
	return;
    }
    if (!unwinder)
    {
	printf("ARIAvatar::setup: No unwinder found\n");
	isInitted = 0;
	return;
    }
    if (!armOffsets)
    {
	printf("ARIAvatar::setup: No arm offset data found\n");
	isInitted = 0;
	return;
    }
    if (!geometryRoot)
    {
	printf("ARIAvatar::setup: No geometry found\n");
	isInitted = 0;
	return;
    }

    // Set up the kinematics objects
    root = new vsKinematics(geometryRoot);

    avatarNode = geometryRoot->findNodeByName("neck");
    headComponent = (vsComponent *)avatarNode;
    head = new vsKinematics(headComponent);

    avatarNode = geometryRoot->findNodeByName("arm");
    r_shoulder = new vsKinematics((vsComponent *)avatarNode);

    avatarNode = geometryRoot->findNodeByName("forearm");
    r_elbow = new vsKinematics((vsComponent *)avatarNode);

    avatarNode = geometryRoot->findNodeByName("hand");
    r_wrist = new vsKinematics((vsComponent *)avatarNode);
    
    // Set up the motion models
    headMotion = new vsHeadMotion(mstar->getTracker(2),
	mstar->getTracker(3), head);

    walkMotion = new vsWalkInPlace(mstar->getTracker(2),
	mstar->getTracker(0), mstar->getTracker(1), root);

    armMotion = new vs3TrackerArm(mstar->getTracker(2), r_shoulder,
        mstar->getTracker(4), r_elbow, mstar->getTracker(5), r_wrist);
    armMotion->setShoulderOffset(armOffsets->shoulderOffset);
    armMotion->setElbowOffset(armOffsets->elbowOffset);
    armMotion->setWristOffset(armOffsets->wristOffset);
    
    // Set up the motion models that use intersections
    geometryRoot->setIntersectValue(VS_AVATAR_LOCAL_ISECT_MASK);

    tFollow = new vsTerrainFollow(root, scene);
    tFollow->setIntersectMask(~VS_AVATAR_LOCAL_ISECT_MASK);

    collide = new vsCollision(root, scene);
    collide->setIntersectMask(~VS_AVATAR_LOCAL_ISECT_MASK);
    collide->setPointCount(1);
    colPoint.set(0.0, 0.0, 1.0);
    collide->setPoint(0, colPoint);
    collide->setCollisionMode(VS_COLLISION_MODE_SLIDE);
    collide->setMargin(0.1);
    
    // Set up the viewpoints and attach them to the geometry
    leftEyeView = new vsView();
    leftEyeView->setClipDistances(0.01, 20000.0);
    leftEyeViewAttrib = new vsViewpointAttribute(leftEyeView);

    leftEyeXform = new vsTransformAttribute();
//    offsetMat.setTranslation(-0.0429, 0.1015, 0.3);
    offsetMat.setTranslation(-0.0143, 0.0338, 0.1);
    leftEyeXform->setPreTransform(offsetMat);

    leftEyeComp = new vsComponent();
    leftEyeComp->addAttribute(leftEyeViewAttrib);
    leftEyeComp->addAttribute(leftEyeXform);
    headComponent->addChild(leftEyeComp); 
    
    rightEyeView = new vsView();
    rightEyeView->setClipDistances(0.01, 20000.0);
    rightEyeViewAttrib = new vsViewpointAttribute(rightEyeView);

    rightEyeXform = new vsTransformAttribute();
//    offsetMat.setTranslation(0.0429, 0.1015, 0.3);
    offsetMat.setTranslation(0.0143, 0.0338, 0.1);
    rightEyeXform->setPreTransform(offsetMat);

    rightEyeComp = new vsComponent();
    rightEyeComp->addAttribute(rightEyeViewAttrib);
    rightEyeComp->addAttribute(rightEyeXform);
    headComponent->addChild(rightEyeComp);
    
    // 'Turn off' the head
    headSwitch = (vsSwitchAttribute *)(headComponent->
	getTypedAttribute(VS_ATTRIBUTE_TYPE_SWITCH, 0));
    if (headSwitch)
	headSwitch->disableAll();

    // Done; clean up
    free(armOffsets);
}
