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
//    VESS Module:  vsCharacter.c++
//
//    Description:  Class to encapsulate a virtual character, including
//                  one or more skeletons, skeleton kinematics, skins,
//                  and associated geometry.  Animations for the character
//                  are also managed.  Hardware and software skinning are
//                  both supported, and can be enabled or disabled on the
//                  fly.  A default GLSL program is generated for each
//                  skin, but the user can elect to specify a custom
//                  program.
//                   
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsCharacter.h++"
#include "vsSkinProgramNode.h++"

// ------------------------------------------------------------------------
// Constructor for a simple character with a single skeleton, kinematics,
// and skin, and a set of animations
// ------------------------------------------------------------------------
vsCharacter::vsCharacter(vsSkeleton *skeleton, vsSkeletonKinematics *skelKin,
                         vsSkin *skin, atArray *animationNames,
                         vsArray *animations)
{
    vsGLSLProgramAttribute *skinProgram;
    vsComponent *skinRoot;
    int i;
    vsPathMotionManager *animation;

    // Create the lists for skeletons, kinematics, and skins
    characterSkeletons = new vsList();
    skeletonKinematics = new vsList();
    characterSkins = new vsList();

    // Check the skeleton to see if it's valid
    if (skeleton != NULL)
    {
        // Add the lone skeleton to the list
        characterSkeletons->addEntry(skeleton);
    }
    else
        printf("vsCharacter::vsCharacter: Skeleton is not valid!\n");

    // Check the skeleton kinematics to see if it's valid
    if (skelKin != NULL)
    {
        // Add the lone kinematics to the list
        skeletonKinematics->addEntry(skelKin);
    }
    else
        printf("vsCharacter::vsCharacter: Skeleton kinematics is not "
            "valid!\n");

    // Check the skin to see if it's valid
    if (skin != NULL)
    {
        // Add the lone skin to the list
        characterSkins->addEntry(skin);
    }
    else
        printf("vsCharacter::vsCharacter: Skin is not valid!\n");

    // Create a common component for all the skin meshes
    characterMesh = new vsComponent();
    characterMesh->ref();
    
    // See if we were given a skin
    if (skin != NULL)
    {
        // Get the root component from the skin, and add it to the
        // mesh component
        skinRoot = skin->getRootComponent();
        if (skinRoot != NULL)
            characterMesh->addChild(skinRoot);
    }

    // Disable culling on the mesh
    characterMesh->disableCull();

    // Enable lighting on the mesh
    characterMesh->enableLighting();

    // Get the animations (if any)
    characterAnimationNames = animationNames;
    characterAnimations = animations;

    // Create the arrays (leaving them empty) if we were passed NULL values
    if (characterAnimationNames == NULL)
        characterAnimationNames = new atArray();
    if (characterAnimations == NULL)
        characterAnimations = new vsArray();

    // Initialize the animation state
    currentAnimation = NULL;
    defaultAnimation = NULL;
    loopingAnimation = NULL;
    loopStarted = false;
    oneTimeAnimation = NULL;
    oneTimeStarted = false;
    finalStarted = false;
    transitionAnimation = NULL;

    // Set the flag to indicate whether or not the character is valid
    if ((characterSkeletons->getNumEntries() > 0) &&
        (skeletonKinematics->getNumEntries() > 0) &&
        (characterSkins->getNumEntries() > 0) &&
        (characterMesh != NULL))
        validFlag = true;

    // Create a list to associate each skin with the GLSL program that
    // handles it
    skinProgramList = new vsList();

    // Start out assuming we're not hardware skinning
    hardwareSkinning = false;

    // See if we've got a valid character
    if (validFlag)
    {
        // Now, prepare the necessary players for hardware skinning.
        // We need a separate skinning program for each skin, first, see if
        // the skin already has one (if so, we assume it's taking care of
        // the skinning)
        skinProgram = (vsGLSLProgramAttribute *)skin->getRootComponent()->
            getTypedAttribute(VS_ATTRIBUTE_TYPE_GLSL_PROGRAM, 0);
        if (skinProgram == NULL) 
        {
            // Create a default skin program
            skinProgram = createDefaultSkinProgram();

            // Set the new skin program
            if (setSkinProgram(skin, skinProgram))
            {
                // We're now set up for hardware skinning, so try to enable it
                enableHardwareSkinning();
            }
        }
    }
    else
    {
        // No skinning program, so disable hardware skinning
        hardwareSkinning = false;
    }
} 

// ------------------------------------------------------------------------
// Constructor for a complex character consisting of one or more skeletons,
// kinematics, and skins, along with a set of animations
// ------------------------------------------------------------------------
vsCharacter::vsCharacter(vsList *skeletons, vsList *skelKins,
                         vsList *skins, atArray *animationNames,
                         vsArray *animations)
{
    vsSkeleton *skeleton;
    vsSkeletonKinematics *skelKin;
    vsSkin *skin;
    vsComponent *lca;
    vsGLSLProgramAttribute *skinProgram;
    int i;
    vsPathMotionManager *animation;

    // Assume ownership of all five containers of character pieces
    characterSkeletons = skeletons;
    skeletonKinematics = skelKins;
    characterSkins = skins;
    characterAnimationNames = animationNames;
    characterAnimations = animations;

    // Make sure we have at least one skeleton
    if (characterSkeletons == NULL)
    {
        // The skeleton list doesn't exist, so create it
        characterSkeletons = new vsList();
    }
    if (characterSkeletons->getNumEntries() == 0)
    {
        // The skeleton list is empty
        printf("vsCharacter::vsCharacter:  No skeleton found!\n");
    }

    // Make sure we have at least one skeleton kinematics
    if (skeletonKinematics == NULL)
    {
        // The skeleton kinematics list doesn't exist, so create it
        skeletonKinematics = new vsList();
    }
    if (skeletonKinematics->getNumEntries() == 0)
    {
        // The kinematics list is empty
        printf("vsCharacter::vsCharacter:  No kinematics found!\n");
    }

    // Make sure we have at least one skin
    if (characterSkins == NULL)
    {
        // The skeleton list doesn't exist, so create it
        characterSkins = new vsList();
    }
    if (characterSkins->getNumEntries() == 0)
    {
        // The skin list is empty
        printf("vsCharacter::vsCharacter:  No skin found!\n");
    }

    // Create a common component for all the skin meshes
    characterMesh = new vsComponent();
    characterMesh->ref();
    
    // Iterate over all the skins and add each skin's root component to the
    // characterMesh component
    skin = (vsSkin *)characterSkins->getFirstEntry();
    while (skin != NULL)
    {
        // Add the root component of the skin to the list
        characterMesh->addChild(skin->getRootComponent());

        // Get the next skin
        skin = (vsSkin *)characterSkins->getNextEntry();
    }

    // Disable culling on the mesh
    characterMesh->disableCull();

    // Enable lighting on the mesh
    characterMesh->enableLighting();

    // Get the animations (if any)
    characterAnimationNames = animationNames;
    characterAnimations = animations;

    // Create the arrays (leaving them empty) if we were passed NULL values
    if (characterAnimationNames == NULL)
        characterAnimationNames = new atArray();
    if (characterAnimations == NULL)
        characterAnimations = new vsArray();
   
    // Initialize the animation state
    currentAnimation = NULL;
    defaultAnimation = NULL;
    loopingAnimation = NULL;
    loopStarted = false;
    oneTimeAnimation = NULL;
    oneTimeStarted = false;
    finalStarted = false;
    transitionAnimation = NULL;

    // Set the flag to indicate whether or not the character is valid
    if ((characterSkeletons->getNumEntries() > 0) &&
        (skeletonKinematics->getNumEntries() > 0) &&
        (characterSkins->getNumEntries() > 0) &&
        (characterMesh != NULL))
        validFlag = true;

    // Create a list that will store a skin program for each skin in
    // our character (the programs must be distinct, because there will
    // be different sets of skin matrices for each skin)
    skinProgramList = new vsList();

    // Start out assuming we're not hardware skinning
    hardwareSkinning = false;

    // See if we've got a valid character
    if (validFlag)
    {
        // Iterate over the skins in our list
        skin = (vsSkin *)characterSkins->getFirstEntry();
        while (skin != NULL)
        {
            // Now, prepare the necessary players for hardware skinning.
            // We need a separate skinning program for each skin, first, see if
            // the skin already has one (if so, we assume it's taking care of
            // the skinning)
            skinProgram = (vsGLSLProgramAttribute *)skin->getRootComponent()->
                getTypedAttribute(VS_ATTRIBUTE_TYPE_GLSL_PROGRAM, 0);
            if (skinProgram == NULL) 
            {
                // Create a default skin program
                skinProgram = createDefaultSkinProgram();

                // Set the new skin program
                if (!setSkinProgram(skin, skinProgram))
                {
                    printf("vsCharacter::vsCharacter:  Failed to set skin "
                        "program on skin!\n");
                }
            }

            // Next skin
            skin = (vsSkin *)characterSkins->getNextEntry();
        }

        // We're now set up for hardware skinning, so try to enable it
        enableHardwareSkinning();
    }
    else
    {
        // The character isn't valid, so disable hardware skinning
        hardwareSkinning = false;
    }
}

// ------------------------------------------------------------------------
// Destructor.  Cleans up anything created or referenced by this character
// ------------------------------------------------------------------------
vsCharacter::~vsCharacter()
{
    // Clean up the skinning program and uniforms
    disableHardwareSkinning();
    delete skinProgramList;

    // Clean up the animations names array
    if (characterAnimationNames != NULL)
        delete characterAnimationNames;

    // Clean up the animations array
    if (characterAnimations != NULL)
        delete characterAnimations;

    // Clean up the other character components (skeleton kinematics,
    // skins, and skeletons)
    delete skeletonKinematics;
    delete characterSkins;
    delete characterSkeletons;

    // Finally, clean up the mesh
    if (characterMesh != NULL)
        vsObject::unrefDelete(characterMesh);
}

// ------------------------------------------------------------------------
// Creates a default GLSL program to handle the rendering of this
// character.  The default shaders handle the vertex skinning as well as
// all basic transforms, directional lighting (single light source), and
// texturing (single diffuse map)
// ------------------------------------------------------------------------
vsGLSLProgramAttribute *vsCharacter::createDefaultSkinProgram()
{
    vsGLSLProgramAttribute *program;
    vsGLSLShader *vertexShader;
    vsGLSLShader *fragmentShader;
    vsGLSLUniform *boneUniform;
    vsGLSLUniform *textureUniform;

    const char *vertexSource = 
        "\n"
        "attribute vec4 weight;\n"
        "attribute vec4 boneIndex;\n"
        "\n"
        "uniform mat4 matrixList[36];\n"
        "\n"
        "void calcDirectionalLight(in int i, in vec3 normal,\n"
        "                          inout vec4 ambient, inout vec4 diffuse,\n"
        "                          inout vec4 specular)\n"
        "{\n"
        "    vec3  light;\n"
        "    float nDotL;\n"
        "    vec3  halfAngle;\n"
        "    float nDotH;\n"
        "    float powerFactor;\n"
        "\n"
        "    // Transform the normal to world space\n"
        "    normal = normalize(gl_NormalMatrix * normal);\n"
        "\n"
        "    // Normalize the light vector\n"
        "    light = normalize(vec3(gl_LightSource[i].position));\n"
        "\n"
        "    // Diffuse component\n"
        "    nDotL = max(0.0, dot(normal, light));\n"
        "\n"
        "    // See if we need to compute a specular component\n"
        "    if (nDotL > 0.0)\n"
        "    {\n"
        "        // Normalize the half angle vector\n"
        "        halfAngle = normalize(gl_LightSource[i].halfVector.xyz);\n"
        "\n"
        "        // Compute the specular component\n"
        "        nDotH = max(0.0, dot(normal, halfAngle));\n"
        "\n"
        "        // Exponentiate the specular component by the material\n"
        "        // shininess\n"
        "        powerFactor = pow(nDotH, gl_FrontMaterial.shininess);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        // If the light is on the wrong side of the surface, there\n"
        "        // can be no specular component\n"
        "        powerFactor = 1.0;\n"
        "    }\n"
        "\n"
        "    // Output the lighting components\n"
        "    ambient += gl_LightSource[i].ambient * gl_FrontMaterial.ambient;\n"
        "    diffuse += gl_LightSource[i].diffuse * nDotL *\n"
        "        gl_FrontMaterial.diffuse;\n"
        "    specular += gl_LightSource[i].specular * powerFactor *\n"
        "        gl_FrontMaterial.specular;\n"
        "}\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    mat4 boneMatrix;\n"
        "    mat3 rotate;\n"
        "    int bone;\n"
        "    vec3 bonePosition;\n"
        "    vec3 boneNormal;\n"
        "    vec3 finalPosition;\n"
        "    vec3 finalNormal;\n"
        "    mat4 finalMatrix;\n"
        "    vec4 ambient, diffuse, specular;\n"
        "\n"
        "    // Initialize the final position, normal, and matrix\n"
        "    finalPosition = vec3(0.0, 0.0, 0.0);\n"
        "    finalNormal = vec3(0.0, 0.0, 0.0);\n"
        "    finalMatrix = mat4(0.0, 0.0, 0.0, 0.0,\n"
        "                       0.0, 0.0, 0.0, 0.0,\n"
        "                       0.0, 0.0, 0.0, 0.0,\n"
        "                       0.0, 0.0, 0.0, 0.0);\n"
        "\n"
        "    // Accumulate the vertex's influences by weighted sum into a\n"
        "    // final matrix\n"
        "    for (int i = 0; i < 4; i++)\n"
        "    {\n"
        "       // Get this bone index\n"
        "       bone = int(boneIndex[i]);\n"
        "\n"
        "       // Get the bone's weighted matrix and add it to the final\n"
        "       // matrix\n"
        "       boneMatrix = matrixList[bone];\n"
        "       finalMatrix += boneMatrix * weight[i];\n"
        "    }\n"
        "\n"
        "    // Calculate the final position\n"
        "    finalPosition = (finalMatrix * gl_Vertex).xyz;\n"
        "\n"
        "    // Get the upper 3x3 of the final matrix and calculate the\n"
        "    // final normal\n"
        "    rotate = mat3(finalMatrix[0].xyz,\n"
        "                  finalMatrix[1].xyz,\n"
        "                  finalMatrix[2].xyz);\n"
        "    finalNormal = rotate * gl_Normal;\n"
        "\n"
        "    // Normalize the normal\n"
        "    finalNormal = normalize(finalNormal);\n"
        "\n"
        "    // Do lighting calculations (assuming only one directional\n"
        "    // light)\n"
        "    ambient = diffuse = specular = vec4(0.0);\n"
        "    calcDirectionalLight(0, finalNormal, ambient, diffuse,\n"
        "                         specular);\n"
        "\n"
        "    // Output the final position, color, and normal\n"
        "    gl_Position = gl_ModelViewProjectionMatrix *\n"
        "        vec4(finalPosition, 1.0);\n"
        "    gl_FrontColor.rgb = gl_Color.rgb * (ambient.rgb + diffuse.rgb +\n"
        "        specular.rgb);\n"
        "    gl_FrontColor.a = gl_Color.a * gl_FrontMaterial.ambient.a *\n"
        "        gl_FrontMaterial.diffuse.a * gl_FrontMaterial.specular.a;\n"
        "    gl_TexCoord[0] = vec4(gl_MultiTexCoord0.st, 0.0, 0.0);\n"
        "}";

    const char *fragmentSource = 
        "\n"
        "uniform sampler2D tex;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    // Modulate the single texture with the final fragment color\n"
        "    gl_FragColor = gl_Color * texture2D(tex, gl_TexCoord[0].st);\n"
        "}";

    // Create the program attribute
    program = new vsGLSLProgramAttribute();

    // Create vertex and fragment shaders
    vertexShader = new vsGLSLShader(VS_GLSL_VERTEX_SHADER);
    vertexShader->setSource(vertexSource);
    fragmentShader = new vsGLSLShader(VS_GLSL_FRAGMENT_SHADER);
    fragmentShader->setSource(fragmentSource);

    // Add the shaders to the program
    program->addShader(vertexShader);
    program->addShader(fragmentShader);

    // Bind the vertex attributes for the weights and bone indices
    program->bindVertexAttr("weight", 1);
    program->bindVertexAttr("boneIndex", 7);

    // Create a uniform parameter for the bone matrix list
    boneUniform = new vsGLSLUniform("matrixList", VS_UNIFORM_FLOAT_MAT4,
        VS_CHAR_MAX_BONES);
    program->addUniform(boneUniform);

    // Create another uniform for the texture sampler (set it to use texture
    // unit 0)
    textureUniform = new vsGLSLUniform("tex", VS_UNIFORM_SAMPLER_2D);
    textureUniform->set(0);
    program->addUniform(textureUniform);

    // Return the new program
    return program;
}

// ------------------------------------------------------------------------
// Create a transition animation from the 
// ------------------------------------------------------------------------
void vsCharacter::transitionToAnimation(vsPathMotionManager *target,
                                        double transitionTime)
{
    vsPathMotion *transPath;
    vsPathMotion *sourcePath;
    vsPathMotion *targetPath;
    int index;
    int targetIndex;
    vsKinematics *sourceKin;
    vsKinematics *targetKin;

    // If the target animation is NULL, do nothing
    if (target == NULL)
    {
        printf("vsCharacter::transitionToAnimation:  "
            "NULL animation specified!\n");
        return;
    }

    // If the current animation is NULL, start the target animation
    // immediately
    if (currentAnimation == NULL)
    {
        // Nothing to start from, so just activate the final animation
        // now
        currentAnimation = target;

        // Configure the target animation based on state
        if (oneTimeStarted)
        {
            // One-time and final animations play once from start to end
            currentAnimation->setCycleMode(VS_PATH_CYCLE_RESTART);
            currentAnimation->setCycleCount(1);
        }
        else
        {
            // Default and looping animations loop forever
            currentAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
            currentAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);
        }

        // Start the new animation
        currentAnimation->stop();
        currentAnimation->startResume();

        return;
    }

    // Create the vsPathMotionManager representing the transition animation
    transitionAnimation = new vsPathMotionManager();

    // Loop through the path motions until we've hit the end of one of the
    // managers
    index = 0;
    while (index < currentAnimation->getPathMotionCount() &&
           index < target->getPathMotionCount())
    {
        // Get the path motion at the index from the source animation
        sourcePath = currentAnimation->getPathMotion(index);

        // Get the source's kinematics
        sourceKin = sourcePath->getKinematics();

        // See if the target path motion is at the same index as the source
        // path motion (this is often the case, but we have to check to be
        // sure)
        targetPath = target->getPathMotion(index);
        targetKin = targetPath->getKinematics();
        if (targetKin != sourceKin)
        {
            // Search the target animation for the path motion corresponding
            // to the source path motion (they both should be targeting the
            // same kinematics object)
            targetIndex = 0;
            targetPath = target->getPathMotion(targetIndex);
            targetKin = targetPath->getKinematics();
            while ((targetPath != NULL) && (targetKin != sourceKin))
            {
                targetIndex++;
                targetPath = target->getPathMotion(targetIndex);
                if (targetPath == NULL)
                    targetKin = NULL;
                else
                    targetKin = targetPath->getKinematics();
            }
        }

        // Make sure we found the target kinematics (otherwise, we'll skip
        // this channel in the transition animation)
        if (targetKin != NULL)
        {
            // Create a new path motion using the same kinematics
            transPath = new vsPathMotion(sourceKin);

            // There are two points in this path motion - the current position
            // of the current animation, and the starting position of the
            // target animation
            transPath->setPointListSize(2);

            // Set the position/orientation data for the first point
            transPath->setPosition(0, sourcePath->getCurrentPosition());
            transPath->setOrientation(0, sourcePath->getCurrentOrientation());

            // Get the path to the end of this animation. As we are going
            // forward, the second point should be the beginning of the new
            // animation.
            transPath->setPosition(1, targetPath->getPosition(0));
            transPath->setOrientation(1, targetPath->getOrientation(0));

            // Set the time to be equal to the specified transition time
            transPath->setTime(0, transitionTime);

            // Use linear interpolation for the transition
            transPath->setPositionMode(VS_PATH_POS_IMODE_LINEAR);
            transPath->setOrientationMode(VS_PATH_ORI_IMODE_SLERP);

            // Add this path motion (corresponding only to a single bone in the
            // skeleton) to the overall animation
            transitionAnimation->addPathMotion(transPath);
        }

        // Next bone
        index++;
    }

    // Stop the current animation
    currentAnimation->stop();

    // Set the transition path's properties and start it up.
    transitionAnimation->setCycleMode(VS_PATH_CYCLE_RESTART);
    transitionAnimation->setCycleCount(1);
    transitionAnimation->stop();
    transitionAnimation->startResume();
    currentAnimation = transitionAnimation;
    transitioning = true;
}

// ------------------------------------------------------------------------
// After an animation transition is complete, this method cleans up the
// transition animation and figures out which animation to start playing
// next
// ------------------------------------------------------------------------
void vsCharacter::finishTransition()
{
    // Check the animation state to see what animation to play next
    if (oneTimeStarted)
    {
        // Make the one-time animation current
        currentAnimation = oneTimeAnimation;
    }
    else if (loopStarted)
    {
        // Make the looping animation current
        currentAnimation = loopingAnimation;
    }
    else
    {
        // Make the default animation current
        currentAnimation = defaultAnimation;
    }

    // Start the new animation
    currentAnimation->stop();
    currentAnimation->startResume();

    // Make sure we actually have a transition to finish
    if (transitionAnimation != NULL)
    {
        // Delete the transition animation
        delete transitionAnimation;
        transitionAnimation = NULL;
    }

    // Mark that we're not transitioning
    transitioning = false;
}

// --------------------------------------------------------------------------
// Return the name of this class
// --------------------------------------------------------------------------
const char *vsCharacter::getClassName()
{
    return "vsCharacter.h++";
}

// ------------------------------------------------------------------------
// Returns a clone of this character object
// ------------------------------------------------------------------------
vsCharacter *vsCharacter::clone()
{
    vsCharacter * character;
    vsList *newSkeletonList;
    atMatrix scale;
    atMatrix offset;
    vsSkeleton *skeleton;
    vsSkeleton *newSkeleton;
    vsList *newSkelKinList;
    vsSkeletonKinematics *skelKin;
    vsSkeletonKinematics *newSkelKin;
    vsKinematics *kin;
    vsKinematics *newKin;
    vsSkin *skin;
    vsList *newSkinList;
    vsSkin *newSkin;
    vsComponent *newSkinRoot;
    vsGLSLProgramAttribute *newSkinProg;
    vsSkeleton *temp;
    u_long skeletonIndex;
    int i, j;
    atArray *newAnimationNames;
    atString *name;
    atString *newName;
    vsArray *newAnimations;
    vsPathMotionManager *animation;
    vsPathMotionManager *newAnimation;
    int boneID;
    int skelKinIndex;
    vsPathMotion *pathMotion;
    vsPathMotion *newPathMotion;

    // If the current character isn't valid, return nothing
    if (!isValid())
        return NULL;

    // Create a new list for skeletons, and also create a list for
    // skeleton kinematics objects
    newSkeletonList = new vsList();
    newSkelKinList = new vsList();

    // Iterate over the list of skeletons
    skeleton = (vsSkeleton *)characterSkeletons->getFirstEntry();
    skelKin = (vsSkeletonKinematics *)skeletonKinematics->getFirstEntry();
    while (skeleton != NULL)
    {
        // Clone the skeleton and add it to the new list
        newSkeleton = new vsSkeleton(skeleton);
        newSkeletonList->addEntry(newSkeleton);

        // Create a skeleton kinematics for the new skeleton and
        // add it to its list
        newSkelKin = new vsSkeletonKinematics(newSkeleton);
        newSkelKinList->addEntry(newSkelKin);

        // Copy the bone poses from the original skeleton
        for (i = 0; i < skeleton->getBoneCount(); i++)
        {
            // Get the kinematics for the i'th bone from each
            // skeleton
            kin = skelKin->getBoneKinematics(i);
            newKin = newSkelKin->getBoneKinematics(i);

            // Make sure both kinematics are valid
            if ((kin != NULL) && (newKin != NULL))
            {
                // Copy the original kinematic pose to the newly
                // created bone
                newKin->setPosition(kin->getPosition());
                newKin->setOrientation(kin->getOrientation());
            }
        }

        // Move on to the next skeleton and kinematics
        skeleton = (vsSkeleton *)characterSkeletons->getNextEntry();
        skelKin = (vsSkeletonKinematics *)skeletonKinematics->getNextEntry();
    }

    // Create a new list for skins
    newSkinList = new vsList();

    // Iterate over the list of skins
    skin = (vsSkin *)characterSkins->getFirstEntry();
    while (skin != NULL)
    {
        // Clone the skin and add it to the new list
        newSkin = new vsSkin(skin);
        newSkinList->addEntry(newSkin);

        // Remove the GLSL program from the cloned skin.  The cloned program
        // uses the same shaders and uniforms as the original program, but
        // we need our own copy in this case.  The new vsCharacter object
        // will create new GLSL program attributes for all of the cloned
        // skins.
        // JPD:  What if this character has a custom GLSL program?  The
        //       new one will only get the default program.  We should look
        //       into a way to do a deep-clone of a GLSL program.
        newSkinRoot = newSkin->getRootComponent();
        newSkinProg = (vsGLSLProgramAttribute *)newSkinRoot->
            getTypedAttribute(VS_ATTRIBUTE_TYPE_GLSL_PROGRAM, 0);
        if (newSkinProg != NULL)
        {
            newSkinRoot->removeAttribute(newSkinProg);
            vsObject::checkDelete(newSkinProg);
        }

        // Move on to the next skin
        skin = (vsSkin *)characterSkins->getNextEntry();
    }

    // Iterate over the list of cloned skins and replace each skin's
    // original skeleton with the corresponding clone that we created
    // earlier
    newSkin = (vsSkin *)newSkinList->getFirstEntry();
    while (newSkin != NULL)
    {
        // Get the skeleton attached to this skin
        skeleton = newSkin->getSkeleton();

        // Find the position of this skeleton in the original skeleton
        // list
        temp = (vsSkeleton *)characterSkeletons->getFirstEntry();
        skeletonIndex = 0;
        while (temp != skeleton)
        {
            temp = (vsSkeleton *)characterSkeletons->getNextEntry();
            skeletonIndex++;
        }

        // Get the corresponding skeleton in the list of clones
        newSkeleton = (vsSkeleton *)
            newSkeletonList->getNthEntry(skeletonIndex);

        // Set the cloned skeleton on the cloned skin
        newSkin->setSkeleton(newSkeleton);

        // Move on to the next skin
        newSkin = (vsSkin *)newSkinList->getNextEntry();
    }

    // Clone the array of animation names
    newAnimationNames = new atArray();
    for (i = 0; i < characterAnimationNames->getNumEntries(); i++)
    {
        // Get the i'th name
        name = (atString *)characterAnimationNames->getEntry(i);

        // See if the name is valid
        if (name != NULL)
        {
            // Clone the name string, and add it to the new array in the
            // same position
            newName = new atString(*name);
            newAnimationNames->setEntry(i, newName);
        }
    }

    // Clone the array of animations
    newAnimations = new vsArray();
    for (i = 0; i < characterAnimations->getNumEntries(); i++)
    {
        // Get the i'th name
        animation = (vsPathMotionManager *)characterAnimations->getEntry(i);

        // See if the name is valid
        if (name != NULL)
        {
            // Clone the path motion manager, and add it to the new array in
            // the same position
            newAnimation = new vsPathMotionManager(animation);
            newAnimations->setEntry(i, newAnimation);

            // Switch the kinematics on the cloned path motion manager
            // to use the newly cloned kinematics
            for (j = 0; j < animation->getPathMotionCount(); j++)
            {
                // Get the j'th path motion from the old animation
                pathMotion = animation->getPathMotion(j);

                // Get the kinematics from the old path motion
                kin = pathMotion->getKinematics();

                // Find the old skeleton kinematics and bone ID corresponding
                // to this individual kinematics object
                boneID = -1;
                skelKinIndex = 0;
                skelKin = (vsSkeletonKinematics *)
                    skeletonKinematics->getFirstEntry();
                while ((skelKin != NULL) && (boneID < 0))
                {
                    // Look for the kinematics in this skeleton kinematics
                    // object
                    boneID = skelKin->getBoneIDForKinematics(kin);

                    // Try the next kinematics if we didn't find it
                    if (boneID < 0)
                    {
                        skelKinIndex++;
                        skelKin = (vsSkeletonKinematics *)
                            skeletonKinematics->getNextEntry();
                    }
                }

                // Make sure we found it
                if ((skelKin != NULL) && (boneID >= 0))
                {
                    // Get the j'th path motion from the new animation
                    newPathMotion = newAnimation->getPathMotion(j);

                    // Get the kinematics from the corresponding new
                    // skeleton kinematics object and bone ID
                    newSkelKin = (vsSkeletonKinematics *)
                        newSkelKinList->getNthEntry(skelKinIndex);
                    newKin = newSkelKin->getBoneKinematics(boneID);

                    // Set the kinematics from the corresponding new skeleton
                    // kinematics and bone ID on the new path motion
                    newPathMotion->setKinematics(newKin);
                }
            }
        }
    }
    
    // Create a character using the skeletons, skins, kinematics, and
    // animations that we just finished creating.  The character takes
    // ownership of these lists directly, so we don't need to do anything
    // else with them
    character = new vsCharacter(newSkeletonList, newSkelKinList,
        newSkinList, newAnimationNames, newAnimations);

    // Return the new character
    return character;
}

// ------------------------------------------------------------------------
// Return whether or not this character is valid.  A character is valid
// if it has at least one skeleton, skeleton kinematics, and skin.  Also,
// the skin must have a valid root mesh.
// ------------------------------------------------------------------------
bool vsCharacter::isValid()
{
    return validFlag;
}

// ------------------------------------------------------------------------
// Returns the root component of all of the character's skin meshes
// ------------------------------------------------------------------------
vsComponent *vsCharacter::getMesh()
{
    return characterMesh;
}

// ------------------------------------------------------------------------
// Returns the number of skeletons in this character
// ------------------------------------------------------------------------
int vsCharacter::getNumSkeletons()
{
    return characterSkeletons->getNumEntries();
}

// ------------------------------------------------------------------------
// Returns a skeleton based on the given index
// ------------------------------------------------------------------------
vsSkeleton *vsCharacter::getSkeleton(int index)
{
    return (vsSkeleton *)characterSkeletons->getNthEntry(index);
}

// ------------------------------------------------------------------------
// Returns the number of skeleton kinematics in this character
// ------------------------------------------------------------------------
int vsCharacter::getNumSkeletonKinematics()
{
    return skeletonKinematics->getNumEntries();
}

// ------------------------------------------------------------------------
// Returns a skeleton kinematics object based on the given index
// ------------------------------------------------------------------------
vsSkeletonKinematics *vsCharacter::getSkeletonKinematics(int index)
{
    return (vsSkeletonKinematics *)skeletonKinematics->getNthEntry(index);
}

// ------------------------------------------------------------------------
// Returns the number of skins in this character
// ------------------------------------------------------------------------
int vsCharacter::getNumSkins()
{
    return characterSkins->getNumEntries();
}

// ------------------------------------------------------------------------
// Returns a skin based on the given index
// ------------------------------------------------------------------------
vsSkin *vsCharacter::getSkin(int index)
{
    return (vsSkin *)characterSkins->getNthEntry(index);
}

// ------------------------------------------------------------------------
// Enable the use of GLSL programs to skin and render this character
// ------------------------------------------------------------------------
void vsCharacter::enableHardwareSkinning()
{
    vsSkin *skin;
    vsGLSLProgramAttribute *prog;

    // Don't do anything if the character isn't valid
    if (!validFlag)
        return;

    // Make sure hardware skinning isn't already on
    if (!hardwareSkinning)
    {
        // Iterate over the skins
        skin = (vsSkin *)characterSkins->getFirstEntry();
        while (skin != NULL)
        {
            // Find the skin in the skin program list
            prog = getSkinProgram(skin);
            
            // Attach the skin's shader program to the mesh
            if (prog != NULL)
                skin->getRootComponent()->addAttribute(prog);

            // Reset the skin, so the shader starts with the correct vertex
            // and normal values
            skin->reset();
            skin = (vsSkin *)characterSkins->getNextEntry();
        }
    }

    // Update the hardware skinning flag
    hardwareSkinning = true;
}

// ------------------------------------------------------------------------
// Disable the use of GLSL programs to skin and render this character.
// In this case, skinning is done by the CPU, and fixed-function rendering
// is used
// ------------------------------------------------------------------------
void vsCharacter::disableHardwareSkinning()
{
    vsSkin *skin;
    vsGLSLProgramAttribute *prog;

    // Don't do anything if the character isn't valid
    if (!validFlag)
        return;

    // See if we're currently hardware skinning
    if (hardwareSkinning)
    {
        // Iterate over the skins
        skin = (vsSkin *)characterSkins->getFirstEntry();
        while (skin != NULL)
        {
            // Find the skin in the skin program list
            prog = getSkinProgram(skin);
            
            // Detach the skinning shader program from the mesh
            if (prog != NULL)
                skin->getRootComponent()->removeAttribute(prog); 

            // Get the next skin
            skin = (vsSkin *)characterSkins->getNextEntry();
        }
    }

    // Update the hardware skinning flag
    hardwareSkinning = false;
}

// ------------------------------------------------------------------------
// Returns whether or not we're using hardware skinning
// ------------------------------------------------------------------------
bool vsCharacter::isHardwareSkinning()
{
    return hardwareSkinning;
}

// ------------------------------------------------------------------------
// Returns the number of animations available for the character
// ------------------------------------------------------------------------
int vsCharacter::getNumAnimations()
{
    return characterAnimations->getNumEntries();
}

// ------------------------------------------------------------------------
// Returns the name of the animation at the given position in the animation
// list
// ------------------------------------------------------------------------
atString vsCharacter::getAnimationName(int index)
{
    atString * name;

    name = (atString *)characterAnimationNames->getEntry(index);
    if (name != NULL)
        return name->clone();
    else
        return atString();
}

// ------------------------------------------------------------------------
// Returns the vsPathMotionManager representing the current animation
// ------------------------------------------------------------------------
vsPathMotionManager *vsCharacter::getCurrentAnimation()
{
   return currentAnimation;
}

// ------------------------------------------------------------------------
// Returns the vsPathMotionManager representing the animation with the
// given name, or NULL if the animation isn't found
// ------------------------------------------------------------------------
vsPathMotionManager *vsCharacter::getAnimation(atString name)
{
    int index;
    atString *currentName;

    // Find the animation with the given name in the list
    index = 0;
    currentName = (atString *)characterAnimationNames->getEntry(index);
    while ((index < characterAnimationNames->getNumEntries()) &&
           (!currentName->equals(&name)))
    {
        // Try the next one
        index++;
        currentName = (atString *)characterAnimationNames->getEntry(index);
    }

    // See if we found what we were looking for
    if (index < characterAnimationNames->getNumEntries())
    {
        // Return the given animation
        return (vsPathMotionManager *)characterAnimations->getEntry(index);
    }
    else
    {
        // Return NULL, as we didn't find the requested animation
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns the vsPathMotionManager representing the animation at the
// given index, or NULL if the animation isn't found
// ------------------------------------------------------------------------
vsPathMotionManager *vsCharacter::getAnimation(int index)
{
    // Return the requested animation
    return (vsPathMotionManager *)characterAnimations->getEntry(index);
}

// ------------------------------------------------------------------------
// Enables the animation at the given position in the animation list
// ------------------------------------------------------------------------
void vsCharacter::switchAnimation(int index)
{
    vsPathMotionManager *newAnimation;

    // Don't do anything if the character isn't valid
    if (!validFlag)
        return;

    // Get the new animation
    newAnimation = getAnimation(index);

    // Make sure the new animation is valid (if not, we just won't animate
    // the character until we're switched back to a valid one)
    if (newAnimation != NULL)
    {
        // Set the new animation as the default animation
        defaultAnimation = newAnimation;
        defaultAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
        defaultAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);

        // Cancel any other animations and start the (new) default animation
        restartAnimation();
    }

/*
      JPD:
      COLLADA skeletons seem to be set up in the document's scene graph (as
      opposed to CAL 3D skeletons, which have an independent set of bone
      transforms, while the scene transforms start out as identity).  Because
      of this, we don't have a good way of "resetting" the skeleton in a
      COLLADA file (trying to reset the skeleton kinematics to identity will
      effectively collapse the skeleton in on itself).

      Instead of resetting the skeleton, we'll just stop the current
      animation and leave it as is.  I'm leaving the code behind in case
      a solution presents itself someday.

    vsSkeleton *skeleton;
    vsSkeletonKinematics *kin;
    vsSkin *skin;

    else
    {
        // A negative index means the default pose (all bones in the skeleton
        // set to identity), first reset the kinematics
        kin = (vsSkeletonKinematics *)skeletonKinematics->getFirstEntry();
        while (kin != NULL)
        {
            kin->reset();
            kin = (vsSkeletonKinematics *)skeletonKinematics->getNextEntry();
        }

        // Next, update the skeletons
        skeleton = (vsSkeleton *)characterSkeletons->getFirstEntry();
        while (kin != NULL)
        {
            skeleton->update();
            skeleton = (vsSkeleton *)characterSkeletons->getNextEntry();
        }

        // Finally, update and reset the skins
        skin = (vsSkin *)characterSkins->getFirstEntry();
        while (skin != NULL)
        {
            skin->update();
            skin->reset();
            skin = (vsSkin *)characterSkins->getNextEntry();
        }
    }
*/
}

// ------------------------------------------------------------------------
// Enables the animation with the given name
// ------------------------------------------------------------------------
void vsCharacter::switchAnimation(atString name)
{
    vsPathMotionManager *newAnimation;

    // Don't do anything if the character isn't valid
    if (!validFlag)
        return;

    // Get the new animation
    newAnimation = getAnimation(name);

    // Make sure the new animation is valid (if not, we just won't animate
    // the character until we're switched back to a valid one)
    if (newAnimation != NULL)
    {
        // Set the new animation as the default animation
        defaultAnimation = newAnimation;
        defaultAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
        defaultAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);

        // Cancel any other animations, and start the (new) default animation
        restartAnimation();
    }
    else
    {
        // Print a warning that we couldn't find the specified animation
        notify(AT_WARN, "Animation named \"%s\" does not exist\n",
            name.getString());
    }
}

// ------------------------------------------------------------------------
// Sets the currently active animation to the given animation.  This
// allows for temporary animations to be specified by the user
// ------------------------------------------------------------------------
void vsCharacter::setCurrentAnimation(vsPathMotionManager *anim)
{
    vsSkeletonKinematics *kin;
    vsSkeleton *skeleton;
    vsSkin *skin;

    // Don't do anything if the character isn't valid
    if (!validFlag)
        return;

    // Deactivate the previous animation
    if (currentAnimation != NULL)
        currentAnimation->stop();

    // Set the new animation
    currentAnimation = anim;
    
    // Make sure the new animation is valid (if not, we just won't animate
    // the character until we're switched back to a valid one)
    if (currentAnimation != NULL)
    {
        // Activate the animation
        currentAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
        currentAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);
        currentAnimation->stop();
        currentAnimation->startResume();
    }
}

// ------------------------------------------------------------------------
// Sets the animation that will play if no other animation is specified.
// If the default animation is currently playing, it will transition to
// the specified animation
// ------------------------------------------------------------------------
void vsCharacter::setDefaultAnimation(atString name, double transitionTime)
{
    vsPathMotionManager *defaultAnim;

    // Look up the requested animation
    defaultAnim = getAnimation(name);
    if (defaultAnim == NULL)
    {
        // We didn't find the requested animation
        printf("vsCharacter::setDefaultAnimation:  Animation %s not found\n",
            name.getString());
    }
    else
    {
        // If we're currently playing the default animation, transition
        // to the new one now
        if (!loopStarted && !oneTimeStarted)
            transitionToAnimation(defaultAnim, transitionTime);

        // Set the new default animation
        defaultAnimation = defaultAnim;
        defaultAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
        defaultAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);
    }
}

// ------------------------------------------------------------------------
// Transitions from the current animation to the specified animation.  The
// given animation will loop endlessly until finishLoopingAnimation() is
// called or another animation is selected
// ------------------------------------------------------------------------
void vsCharacter::startLoopingAnimation(atString name, double transitionTime)
{
    vsPathMotionManager *newLoop;

    // Look up the new animation
    newLoop = getAnimation(name);
    if (newLoop == NULL)
    {
        // We didn't find the requested animation
        printf("vsCharacter::startLoopingAnimation:  Animation %s not found\n",
            name.getString());
    }
    else
    {
        // If we're currently playing the default animation or another looping
        // animation (not a one-time animation), transition to the new
        // looping animation now
        if (!oneTimeStarted)
            transitionToAnimation(newLoop, transitionTime);

        // Set the new looping animation
        loopingAnimation = newLoop;
        loopingAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
        loopingAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);
        loopStarted = true;
    }
}

// ------------------------------------------------------------------------
// Transitions from the current looping animation back to the default
// animation.  If a one-time animation has been started, this will simply
// clear the current looping animation so that the one-time animation will
// transition back to the default animation instead
// ------------------------------------------------------------------------
void vsCharacter::finishLoopingAnimation(double transitionTime)
{
    // First, make sure we're actually running a looping animation
    if ((loopingAnimation != NULL) && (loopStarted))
    {
        // See if there is a one-time animation playing now
        if (!oneTimeStarted)
        {
            // See if there is a default animation to transition to
            if (defaultAnimation != NULL)
            {
                // Transition to the default animation
                transitionToAnimation(defaultAnimation, transitionTime);
            }
            else
            {
                // Just stop the current animation
                currentAnimation->stop();
            }
        }

        // Remove the looping animation and indicate that it is no longer
        // running
        loopingAnimation = NULL;
        loopStarted = false;
    }
    else
    {
        // We're not running a looping animation now, so we can't finish it
        printf("vsCharacter::finishLoopingAnimation:  No looping animation "
            "currently running.\n");
    }
}

// ------------------------------------------------------------------------
// Transitions from the default or current looping animation to the
// specified animation.  The animation will play once, and then transition
// back to the previous animation (default or looping)
// ------------------------------------------------------------------------
void vsCharacter::startOneTimeAnimation(atString name, double transInTime,
                                        double transOutTime)
{
    vsPathMotionManager *oneTime;

    // Find the requested animation
    oneTime = getAnimation(name);
    if (oneTime != NULL)
    {
        // Make use of the new animation as long as the animation currently
        // playing isn't a final animation
        if (!finalStarted)
        {
            // Transition to the new animation
            transitionToAnimation(oneTime, transInTime);

            // Save the "transition out" time (the amount of time to take
            // when transitioning back from this one-time animation
            oneTimeTransOutTime = transOutTime;

            // Set up the animation for one-time play and mark it as started
            oneTimeAnimation = oneTime;
            oneTimeAnimation->setCycleMode(VS_PATH_CYCLE_RESTART);
            oneTimeAnimation->setCycleCount(1);
            oneTimeStarted = true;
        }
    }
    else
    {
        // Couldn't find the requested animation
        printf("vsCharacter::startOneTimeAnimation:  Animation %s not found",
            name.getString());
    }
}

// ------------------------------------------------------------------------
// Transitions from the current animation to the specified animation.  The
// animation will play once and then stop (it will not transition to any
// other animation)
// ------------------------------------------------------------------------
void vsCharacter::startFinalAnimation(atString name, double transitionTime)
{
    vsPathMotionManager *finalAnim;

    // Make sure we haven't already started a final animation
    if (!finalStarted)
    {
        // Look up the animation
        finalAnim = getAnimation(name);
        if ((finalAnim != NULL) && (!finalStarted))
        {
            // Transition to the final animation
            transitionToAnimation(finalAnim, transitionTime);

            // Set up this animation for one-time play and mark it as
            // started and final
            oneTimeAnimation = finalAnim;
            oneTimeAnimation->setCycleMode(VS_PATH_CYCLE_RESTART);
            oneTimeAnimation->setCycleCount(1);
            oneTimeStarted = true;
            finalStarted = true;
        }
    }
}

// ------------------------------------------------------------------------
// Returns whether or not a final animation has started playing, and no
// further animations will be played (unless a restart is given)
// ------------------------------------------------------------------------
bool vsCharacter::isAnimationFinal()
{
    // Return whether or not a final animation has been started
    return finalStarted;
}

// ------------------------------------------------------------------------
// Restarts the default animation and clears any other animations
// (including the final animation)
// ------------------------------------------------------------------------
void vsCharacter::restartAnimation()
{
    // Reinitialize all animation state
    loopingAnimation = NULL;
    loopStarted = false;
    oneTimeAnimation = NULL;
    oneTimeStarted = false;
    finalStarted = false;

    // Start the default animation
    currentAnimation = defaultAnimation;
    if (currentAnimation != NULL)
    {
        // Set up the animation to loop forever and start it
        currentAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
        currentAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);
        currentAnimation->stop();
        currentAnimation->startResume();
    }
}

// ------------------------------------------------------------------------
// Returns the GLSL program used by the given skin object
// ------------------------------------------------------------------------
vsGLSLProgramAttribute *vsCharacter::getSkinProgram(vsSkin *skin)
{
    vsSkinProgramNode *node;

    // Find the program that goes with the given skin
    node = (vsSkinProgramNode *)skinProgramList->getFirstEntry();
    while ((node != NULL) && (node->getSkin() != skin))
        node = (vsSkinProgramNode *)skinProgramList->getNextEntry();

    // Return the program if we found the skin, or NULL if not
    if (node != NULL)
        return node->getProgram();
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Changes the GLSL program used by the given skin object.  Returns whether
// or not the program is acceptable (if the new program is not acceptable,
// the old program is retained).  In order for a program to be accepted,
// it must contain a uniform parameter named "matrixList", which will be
// used to pass the latest set of skin matrices to the vertex shader.
// ------------------------------------------------------------------------
bool vsCharacter::setSkinProgram(vsSkin *skin, vsGLSLProgramAttribute *prog)
{
    vsGLSLUniform *boneList;
    vsGLSLProgramAttribute *currentProgram;
    vsSkinProgramNode *node;

    // Don't bother doing anything if the character isn't valid
    if (!validFlag)
        return false;

    // Try to get the "matrixList" uniform from the new program (we need this
    // to exist in order to be able to update the bone matrices)
    boneList = prog->getUniform("matrixList");

    // See if we found it
    if (boneList == NULL)
    {
        // Print a warning message indicating we can't use this program
        printf("vsCharacter::setSkinProgram:  No \"matrixList\" uniform "
            "found.\n");
        printf("   Unable to use this program for skinning.\n");

        // We can't effectively use this program, so bail out now
        return false;
    }

    // Get the program that's currently attached to the skin (if any)
    currentProgram = (vsGLSLProgramAttribute *) skin->getRootComponent()->
        getTypedAttribute(VS_ATTRIBUTE_TYPE_GLSL_PROGRAM, 0);

    // Find the program that goes with the given skin (if any)
    node = (vsSkinProgramNode *)skinProgramList->getFirstEntry();
    while ((node != NULL) && (node->getSkin() != skin))
        node = (vsSkinProgramNode *)skinProgramList->getNextEntry();
    if (node != NULL)
    {
        // See if the new program matches the program currently attached to
        // the skin (don't do anything if so)
        if (prog != currentProgram)
        {
            // Remove the program from the mesh if we're currently using
            // hardware skinning
            if (currentProgram != NULL)
                skin->getRootComponent()->removeAttribute(currentProgram);
        }

        // Set and reference the new program and uniform, if the program
        // is no longer referenced by any other node, this will also
        // delete the program
        node->setProgram(prog);
    }
    else
    {
        // If there is an program currently attached, remove it, and delete
        // it if it's unreferenced
        if (currentProgram != NULL)
        {
            skin->getRootComponent()->removeAttribute(currentProgram);
            vsObject::checkDelete(currentProgram);
        }

        // Create a new skin program node with the given skin and program
        node = new vsSkinProgramNode(skin, prog);
        skinProgramList->addEntry(node);
    }

    // Attach the program to the mesh if we're currently using hardware
    // skinning
    if ((hardwareSkinning) && (prog != currentProgram))
        skin->getRootComponent()->addAttribute(prog);

    // Return true to indicate the new program is ready to go
    return true;
}

// ------------------------------------------------------------------------
// Updates the character based on the previous frame's time interval
// ------------------------------------------------------------------------
void vsCharacter::update()
{
    update(vsTimer::getSystemTimer()->getInterval());
}


// ------------------------------------------------------------------------
// Updates the character using the given deltaTime.  The animation will be 
// updated and the new poses applied to the skeleton kinematics.  The 
// skeletons will be traversed to accumulate the new set of world-to-bone 
// matrices, and the skins will be updated to generate the final skin 
// matrices.  If hardware skinning is enabled, the skin matrices will be 
// passed to the skin program. If not, the new skin matrices will be 
// applied to the skin geometry immediately.
// ------------------------------------------------------------------------
void vsCharacter::update(double deltaTime)
{
    atMatrix skinMatrix;
    vsSkeletonKinematics *kin;
    vsSkeleton *skeleton;
    vsSkin *skin;
    vsGLSLProgramAttribute *prog;
    vsGLSLUniform *matrixList;
    int i;

    // Make sure we have a character to update
    if (!validFlag)
        return;

    // Update the animation state
    if (currentAnimation != NULL)
    {
        // See if we're currently transitioning between animations
        if (transitioning)
        {
            // Update the transition animation (the current animation is
            // the transition animation in this case)
            currentAnimation->update(deltaTime);

            // If the transition is complete, finish it up and move on to
            // the next animation
            if (currentAnimation->isDone())
                finishTransition();
        }

        // If we're not transitioning (or not _still_ transitioning)
        // update the current animation.  In the case where we just completed
        // a transition, we allow this double-update to "prime" the new
        // animation
        if (!transitioning)
        {
            // Update the current animation
            currentAnimation->update(deltaTime);

            // See if we need to switch animations
            if (currentAnimation->isDone())
            {
                // See if a one time-animation is playing
                if (oneTimeStarted)
                {
                    // We're done with this one-time animation, so
                    // clean up the state
                    oneTimeAnimation = NULL;
                    oneTimeStarted = false;

                    // See if this is a final animation
                    if (finalStarted)
                    {
                        // No more animations will be applied, set the
                        // current animation to NULL to avoid unnecessary
                        // checks on the next update
                        currentAnimation = NULL;
                    }
                    else
                    {
                        // We need to transition back to another animation
                        if (loopStarted)
                        {
                            // There is a looping animation defined,
                            // transition back to that
                            transitionToAnimation(loopingAnimation,
                                                  oneTimeTransOutTime);
                        }
                        else
                        {
                            // Transition back to the default animation
                            transitionToAnimation(defaultAnimation,
                                                  oneTimeTransOutTime);
                        }
                    }
                }
            }
        }
    }

    // Update all kinematics and skeletons with the time value
    kin = (vsSkeletonKinematics *)skeletonKinematics->getFirstEntry();
    while (kin != NULL)
    {
        kin->update(deltaTime);
        kin = (vsSkeletonKinematics *)skeletonKinematics->getNextEntry();
    }
    skeleton = (vsSkeleton *)characterSkeletons->getFirstEntry();
    while (skeleton != NULL)
    {
        skeleton->update();
        skeleton = (vsSkeleton *)characterSkeletons->getNextEntry();
    }

    // Update the character's skins to generate the new skin matrices
    skin = (vsSkin *)characterSkins->getFirstEntry();
    while (skin != NULL)
    {
        // Update the skin to generate the final skinning matrices
        skin->update();

        // See if we're doing skinning on the CPU, or if we're letting the
        // GPU take care of it
        if (hardwareSkinning)
        {
            // Get the skin program and matrix list
            prog = getSkinProgram(skin);
            if (prog != NULL)
                matrixList = prog->getUniform("matrixList");
            else
                matrixList = NULL;

            // Make sure we have a matrix list to update
            if (matrixList != NULL)
            {
                // Update the shader's uniform parameters with the new skin
                // poses 
                for (i = 0; i < skin->getSkeleton()->getBoneCount(); i++)
                {
                    // Don't bother updating the matrix for this bone if
                    // the skin doesn't use it
                    if (skin->usesBone(i))
                    {
                        skinMatrix = skin->getSkinMatrix(i);
                        matrixList->setEntry(i, skinMatrix);
                    }
                }
            }
        }
        else
        {
            // Apply the skin in software to the mesh geometries
            skin->applySkin();
        }

        // Next skin
        skin = (vsSkin *)characterSkins->getNextEntry();
    }
}

