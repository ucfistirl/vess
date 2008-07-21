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
                         atArray *animations)
{
    vsGLSLProgramAttribute *skinProgram;
    vsComponent *skinRoot;

    // Create the lists for skeletons, kinematics, and skins
    characterSkeletons = new atList();
    skeletonKinematics = new atList();
    characterSkins = new atList();

    // Check the skeleton to see if it's valid
    if (skeleton != NULL)
    {
        // Add the lone skeleton to the list and reference it
        characterSkeletons->addEntry(skeleton);
        skeleton->ref();
    }
    else
        printf("vsCharacter::vsCharacter: Skeleton is not valid!\n");

    // Check the skeleton kinematics to see if it's valid
    if (skelKin != NULL)
    {
        // Add the lone kinematics to the list and reference it
        skeletonKinematics->addEntry(skelKin);
        skelKin->ref();
    }
    else
        printf("vsCharacter::vsCharacter: Skeleton kinematics is not "
            "valid!\n");

    // Check the skin to see if it's valid
    if (skin != NULL)
    {
        // Add the lone skin to the list and reference it
        characterSkins->addEntry(skin);
        skin->ref();
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
        characterAnimations = new atArray();
   
    // Initialize the current animation to NULL
    currentAnimation = NULL;

    // Set the flag to indicate whether or not the character is valid
    if ((characterSkeletons->getNumEntries() > 0) &&
        (skeletonKinematics->getNumEntries() > 0) &&
        (characterSkins->getNumEntries() > 0) &&
        (characterMesh != NULL))
        validFlag = true;

    // Create a list to associate each skin with the GLSL program that
    // handles it
    skinProgramList = new atList();

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

        // If we didn't find a program, create a default one
        if (skinProgram == NULL) 
            skinProgram = createDefaultSkinProgram();

        // Set the new skin program
        if (setSkinProgram(skin, skinProgram))
        {
            // We're now set up for hardware skinning, so try to enable it
            enableHardwareSkinning();
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
vsCharacter::vsCharacter(atList *skeletons, atList *skelKins,
                         atList *skins, atArray *animationNames,
                         atArray *animations)
{
    atList *submeshList;
    vsSkeleton *skeleton;
    vsSkeletonKinematics *skelKin;
    vsSkin *skin;
    vsComponent *lca;
    vsGLSLProgramAttribute *skinProgram;

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
        characterSkeletons = new atList();
    }
    if (characterSkeletons->getNumEntries() == 0)
    {
        // The skeleton list is empty
        printf("vsCharacter::vsCharacter:  No skeleton found!\n");
    }

    // Reference all skeletons in the list
    skeleton = (vsSkeleton *)characterSkeletons->getFirstEntry();
    while (skeleton != NULL)
    {
        // Reference the skeleton
        skeleton->ref();

        // Get the next skeleton
        skeleton = (vsSkeleton *)characterSkeletons->getNextEntry();
    }

    // Make sure we have at least one skeleton kinematics
    if (skeletonKinematics == NULL)
    {
        // The skeleton kinematics list doesn't exist, so create it
        skeletonKinematics = new atList();
    }
    if (skeletonKinematics->getNumEntries() == 0)
    {
        // The kinematics list is empty
        printf("vsCharacter::vsCharacter:  No kinematics found!\n");
    }

    // Reference all skeleton kinematics in the list
    skelKin = (vsSkeletonKinematics *)skeletonKinematics->getFirstEntry();
    while (skelKin != NULL)
    {
        // Reference the skeleton
        skelKin->ref();

        // Get the next skeleton
        skelKin = (vsSkeletonKinematics *)skeletonKinematics->getNextEntry();
    }

    // Make sure we have at least one skin
    if (characterSkins == NULL)
    {
        // The skeleton list doesn't exist, so create it
        characterSkins = new atList();
    }
    if (characterSkins->getNumEntries() == 0)
    {
        // The skin list is empty
        printf("vsCharacter::vsCharacter:  No skin found!\n");
    }

    // Reference all skins in the list
    skin = (vsSkin *)characterSkins->getFirstEntry();
    while (skin != NULL)
    {
        // Reference the skin
        skin->ref();

        // Get the next skin
        skin = (vsSkin *)characterSkins->getNextEntry();
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
        characterAnimations = new atArray();
   
    // Initialize the current animation to NULL
    currentAnimation = NULL;

    // Set the flag to indicate whether or not the character is valid
    if ((characterSkeletons->getNumEntries() > 0) &&
        (skeletonKinematics->getNumEntries() > 0) &&
        (characterSkins->getNumEntries() > 0) &&
        (characterMesh != NULL))
        validFlag = true;

    // Create a list that will store a skin program for each skin in
    // our character (the programs must be distinct, because there will
    // be different sets of skin matrices for each skin)
    skinProgramList = new atList();

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

            // If we didn't find a program, create a default one
            if (skinProgram == NULL) 
                skinProgram = createDefaultSkinProgram();

            // Set the new skin program
            skinProgram = createDefaultSkinProgram();
            if (!setSkinProgram(skin, skinProgram))
            {
                printf("vsCharacter::vsCharacter:  Failed to set skin "
                    "program on skin!\n");
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
    vsObject *obj;

    // Clean up the skinning program and uniforms
    disableHardwareSkinning();
    delete skinProgramList;

    // Clean up the animations
    if (characterAnimationNames != NULL)
        delete characterAnimationNames;
    if (characterAnimations != NULL)
        delete characterAnimations;

    // Clean up the various character components, starting with the
    // kinematics
    obj = (vsObject *)skeletonKinematics->getFirstEntry();
    while (obj != NULL)
    {
        // Remove the entry from the list
        skeletonKinematics->removeCurrentEntry();

        // Unreference the entry
        vsObject::unrefDelete(obj);

        // Get the new head of the list
        obj = (vsObject *)skeletonKinematics->getFirstEntry();
    }

    // Next, the skins
    obj = (vsObject *)characterSkins->getFirstEntry();
    while (obj != NULL)
    {
        // Remove the entry from the list
        characterSkins->removeCurrentEntry();

        // Unreference the entry
        vsObject::unrefDelete(obj);

        // Get the new head of the list
        obj = (vsObject *)characterSkins->getFirstEntry();
    }

    // Then, the skeletons
    obj = (vsObject *)characterSkeletons->getFirstEntry();
    while (obj != NULL)
    {
        // Remove the entry from the list
        characterSkeletons->removeCurrentEntry();

        // Unreference the entry
        vsObject::unrefDelete(obj);

        // Get the new head of the list
        obj = (vsObject *)characterSkeletons->getFirstEntry();
    }

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
        "    vec3  half;\n"
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
        "        half = normalize(gl_LightSource[i].halfVector.xyz);\n"
        "\n"
        "        // Compute the specular component\n"
        "        nDotH = max(0.0, dot(normal, half));\n"
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
    atList *newSkeletonList;
    atMatrix scale;
    atMatrix offset;
    vsSkeleton *skeleton;
    vsSkeleton *newSkeleton;
    atList *newSkelKinList;
    vsSkeletonKinematics *skelKin;
    vsSkeletonKinematics *newSkelKin;
    vsKinematics *kin;
    vsKinematics *newKin;
    vsSkin *skin;
    atList *newSkinList;
    vsSkin *newSkin;
    vsComponent *newSkinRoot;
    vsGLSLProgramAttribute *newSkinProg;
    vsSkeleton *temp;
    u_long skeletonIndex;
    int i;
    atArray *newAnimationNames;
    atString *name;
    atString *newName;
    atArray *newAnimations;
    vsPathMotionManager *animation;
    vsPathMotionManager *newAnimation;

    // If the current character isn't valid, return nothing
    if (!isValid())
        return NULL;

    // Create a new list for skeletons, and also create a list for
    // skeleton kinematics objects
    newSkeletonList = new atList();
    newSkelKinList = new atList();

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
    newSkinList = new atList();

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
            newSkinRoot->removeAttribute(newSkinProg);

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
        newSkin = (vsSkin *)characterSkins->getNextEntry();
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
    newAnimations = new atArray();
    for (i = 0; i < characterAnimations->getNumEntries(); i++)
    {
        // Get the i'th name
        animation = (vsPathMotionManager *)characterAnimations->getEntry(i);

        // See if the name is valid
        if (name != NULL)
        {
            // Clone the name string, and add it to the new array in the
            // same position
            newAnimation = new vsPathMotionManager(animation);
            newAnimations->setEntry(i, newAnimation);
        }
    }
    
    // Create a character using the skeletons, skins, kinematics, and
    // animations that we just finished creating
    // TODO:  Animations
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
vsSkeleton * vsCharacter::getSkeleton(int index)
{
    return (vsSkeleton *)characterSkeletons->getNthEntry(index);
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
vsSkin * vsCharacter::getSkin(int index)
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
// Enables the animation at the given position in the animation list
// ------------------------------------------------------------------------
void vsCharacter::switchAnimation(int index)
{
   atString * currentName;
   vsSkeleton *skeleton;
   vsSkeletonKinematics *kin;
   vsSkin *skin;

   // Don't do anything if the character isn't valid
   if (!validFlag)
      return;

   // Deactivate the previous animation
   if (currentAnimation != NULL)
       currentAnimation->stop();

   // Set the new animation (if the index is negative, this indicates we
   // should return to the default pose)
   if (index >= 0)
       currentAnimation = (vsPathMotionManager *)
           characterAnimations->getEntry(index);
   else
       currentAnimation = NULL;

   // Make sure the new animation is valid (if not, we just won't animate
   // the character until we're switched back to a valid one)
   if (currentAnimation != NULL)
   {
      // Get the name of the next animation
      currentName = (atString *)
         characterAnimationNames->getEntry(index);

      // Activate the animation
      currentAnimation->setCycleMode(VS_PATH_CYCLE_CLOSED_LOOP);
      currentAnimation->setCycleCount(VS_PATH_CYCLE_FOREVER);
      currentAnimation->stop();
      currentAnimation->startResume();
   }
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
}

// ------------------------------------------------------------------------
// Enables the animation with the given name
// ------------------------------------------------------------------------
void vsCharacter::switchAnimation(atString name)
{
    atString * currentName;
    u_long index;

    // Special case: if the name is "DefaultPose", switch to the default
    // skeleton pose (by switching to animation number -1)
    if (strcmp(name.getString(), "DefaultPose") == 0)
    {
        // Switch to the default pose using the other switchAnimation method
        switchAnimation(-1);

        // Nothing more to do
        return;
    }

    // Find the animation with the given name in the list
    index = 0;
    currentName = (atString *)characterAnimationNames->getEntry(index);
    while ((index < characterAnimationNames->getNumEntries()) &&
           (!currentName->equals(&name)))
    {
        // Try the next one
        currentName = (atString *)characterAnimationNames->getEntry(index);
        index++;
    }

    // See if we found what we were looking for
    if (index < characterAnimationNames->getNumEntries())
    {
        // Activate the given animation
        switchAnimation(index);
    }
    else
    {
        // Print a warning that we couldn't find the specified animation
        notify(AT_WARN, "Animation named \"%s\" does not exist\n",
            name.getString());
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
    vsGLSLProgramAttribute *oldProgram;
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

    // Find the program that goes with the given skin (if any)
    node = (vsSkinProgramNode *)skinProgramList->getFirstEntry();
    while ((node != NULL) && (node->getSkin() != skin))
        node = (vsSkinProgramNode *)skinProgramList->getNextEntry();
    if (node != NULL)
    {
        // Remove the program from the mesh if we're currently using
        // hardware skinning
        oldProgram = node->getProgram();
        if ((oldProgram != NULL) && (hardwareSkinning))
            skin->getRootComponent()->removeAttribute(oldProgram);

        // Set and reference the new program and uniform, if the program
        // is no longer referenced by any other node, this will also
        // delete the program
        node->setProgram(prog);
    }
    else
    {
        // Create a new skin program node with the given skin and program
        node = new vsSkinProgramNode(skin, prog);
        skinProgramList->addEntry(node);
    }

    // Attach the program to the mesh, if we're currently using hardware
    // skinning
    if (hardwareSkinning)
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

    // Update the animation with the time value
    if (currentAnimation != NULL)
        currentAnimation->update(deltaTime);

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
                    skinMatrix = skin->getSkinMatrix(i);
                    matrixList->setEntry(i, skinMatrix);
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

