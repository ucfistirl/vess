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
//    VESS Module:  vsMenuFrame.c++
//
//    Description:  The menu frame class describes a location within a
//                  vsMenuTree.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuFrame.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a vsMenuFrame that points to
// the root node of any given vsMenuTree.
// ------------------------------------------------------------------------
vsMenuFrame::vsMenuFrame()
{
    pathIndices = (int *)malloc(sizeof(int));
    maxDepth = 1;
    pathDepth = 0;
}

// ------------------------------------------------------------------------
// Destructor - The destructor frees any memory used by the index array
// ------------------------------------------------------------------------
vsMenuFrame::~vsMenuFrame()
{
    free(pathIndices);
}

// ------------------------------------------------------------------------
// This method sets the frame to indicate the same location in a tree as
// the specified vsMenuFrame
// ------------------------------------------------------------------------
void vsMenuFrame::setFrame(vsMenuFrame *frame)
{
    int depth;

    // Initialize the path to its empty state
    pathDepth = 0;

    // If the frame exists, copy its data
    if (frame)
    {
        // Walk through the frame, collecting its indices
        for (depth = 0; depth < frame->getDepth(); depth++)
            appendIndex(frame->getIndex(depth));
    }
}

// ------------------------------------------------------------------------
// This method sets the frame to a given set of indices based on the space-
// delimited path indices 
// ------------------------------------------------------------------------
void vsMenuFrame::setFrame(int *indices, int depth)
{
    int i;

    // Initialize the path to its empty state
    pathDepth = 0;

    // Copy the new data into the array
    for (i = 0; i < depth; i++)
        appendIndex(indices[i]);
}

// ------------------------------------------------------------------------
// This method sets the frame to a given set of indices based on the space-
// delimited path indices 
// ------------------------------------------------------------------------
void vsMenuFrame::setFrame(char *path)
{
    char *token;
    int i;

    // Initialize the path to its empty state
    pathDepth = 0;

    // Traverse the string, adding its elements one by one
    token = strtok(path, " \0");
    while (token)
    {
        appendIndex(atoi(token));
        token = strtok(NULL, " \0");
    }
}

// ------------------------------------------------------------------------
// This method adds an index to the end of the frame
// ------------------------------------------------------------------------
void vsMenuFrame::appendIndex(int index)
{
    int *tempIndices;
    int i;

    // If the path is at its maximum length, it needs to be extended
    if (pathDepth == maxDepth)
    {
        // Increase the maximum size of the array
        maxDepth *= 2;
        tempIndices = (int *)malloc(maxDepth * sizeof(int));

        // Copy and free the old data
        memcpy(tempIndices, pathIndices, pathDepth * sizeof(int));
        free(pathIndices);

        // Set the pointer to the new, larger variable
        pathIndices = tempIndices;
    }

    // Copy the new index into the array and increase the path size
    pathIndices[pathDepth] = index;
    pathDepth++;
}

// ------------------------------------------------------------------------
// This method removes the last index from the end of the frame's path
// ------------------------------------------------------------------------
void vsMenuFrame::removeIndex()
{
    if (pathDepth > 0)
        pathDepth--;
}

// ------------------------------------------------------------------------
// This method returns the path index at the specified depth, or -1 if the
// depth is out of range
// ------------------------------------------------------------------------
int vsMenuFrame::getIndex(int depth)
{
    if ((depth >= 0) && (depth < pathDepth))
        return pathIndices[depth];

    return -1;
}

// ------------------------------------------------------------------------
// This method returns the depth of the current path
// ------------------------------------------------------------------------
int vsMenuFrame::getDepth()
{
    return pathDepth;
}

