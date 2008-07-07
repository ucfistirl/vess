
#include "vsSkinProgramNode.h++"

// ------------------------------------------------------------------------
// Creates a node that encapsulates a vsSkin and GLSL program attribute
// (presumably for adding to a container class)
// ------------------------------------------------------------------------
vsSkinProgramNode::vsSkinProgramNode(vsSkin *theSkin,
                                     vsGLSLProgramAttribute *theProg)
{
    // Reference the skin and program
    skin = theSkin;
    skin->ref();
    program = theProg;
    program->ref();
}

// ------------------------------------------------------------------------
// Cleans the node by unrefDelete'ing the associated objects
// ------------------------------------------------------------------------
vsSkinProgramNode::~vsSkinProgramNode()
{
    // Unreference and delete the skin and program
    vsObject::unrefDelete(skin);
    vsObject::unrefDelete(program);
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsSkinProgramNode::getClassName()
{
    return "vsSkinProgramNode";
}

// ------------------------------------------------------------------------
// Returns the vsSkin associated with this node
// ------------------------------------------------------------------------
vsSkin *vsSkinProgramNode::getSkin()
{
    // Return the skin object
    return skin;
}

// ------------------------------------------------------------------------
// Returns the program attribute associated with this node
// ------------------------------------------------------------------------
vsGLSLProgramAttribute *vsSkinProgramNode::getProgram()
{
    // Return the program attribute
    return program;
}

// ------------------------------------------------------------------------
// Changes the program attribute associated with this node
// ------------------------------------------------------------------------
void vsSkinProgramNode::setProgram(vsGLSLProgramAttribute *newProg)
{
    // Unreference the old program attribute
    vsObject::unrefDelete(program);

    // Set the new program and reference it
    program = newProg;
    program->ref();
}

