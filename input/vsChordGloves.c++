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
//    VESS Module:  vsChordGloves.c++
//
//    Description:  Device to keep track of the state of a pair of VR
//                  chord gloves.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsChordGloves.h++"
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------------------------
// Constructor, creates the state matrix;
// ------------------------------------------------------------------------
vsChordGloves::vsChordGloves()
{
    int i, j;

    // Initialize the contact matrix
    memset(contactMatrix, 0, sizeof(contactMatrix));

    // Create a matrix of vsInputButtons in the upper section of the matrix
    // (above the diagonal), since self-contacting digits are impossible, and
    // the contact pairs are symmetric (4,7 is the same as 7,4)
    for (i = 0; i < VS_CG_MAX_DIGITS - 1; i++)
    {
        for (j = i+1; j < VS_CG_MAX_DIGITS; j++)
        {
            contactMatrix[i][j] = new vsInputButton();
        }
    }
}

// ------------------------------------------------------------------------
// Destructor, does nothing.
// ------------------------------------------------------------------------
vsChordGloves::~vsChordGloves()
{
    int i, j;

    // Delete the input buttons in the contact matrix
    for (i = 0; i < VS_CG_MAX_DIGITS - 1; i++)
    {
        for (j = i+1; j < VS_CG_MAX_DIGITS; j++)
            delete contactMatrix[i][j];
    }
}

// ------------------------------------------------------------------------
// VESS Internal function.  Sets the two given digits as connected.
// ------------------------------------------------------------------------
void vsChordGloves::connect(int first, int second)
{
    // Validate the parameters, complain if invalid
    if (((first > VS_CG_MAX_DIGITS) || (second > VS_CG_MAX_DIGITS)) ||
         (first == second))
    {
        printf("vsChordGloves::connect:  Invalid digit pair specified "
               "(%d and %d)\n", first, second);

        return;
    }
  
    // Only return contacts in the cells above the matrix diagonal
    // (the matrix is symmetric)
    if (first > second)
        contactMatrix[second][first]->setPressed();
    else
        contactMatrix[first][second]->setPressed();
}

// ------------------------------------------------------------------------
// VESS Internal function.  Sets the two given digits as not connected.
// ------------------------------------------------------------------------
void vsChordGloves::disconnect(int first, int second)
{
    // Validate the parameters, complain if invalid
    if (((first > VS_CG_MAX_DIGITS) || (second > VS_CG_MAX_DIGITS)) ||
         (first == second))
    {
        printf("vsChordGloves::disconnect:  Invalid digit pair specified "
               "(%d and %d)\n", first, second);

        return;
    }
  
    // Only return contacts in the cells above the matrix diagonal
    // (the matrix is symmetric)
    if (first > second)
        contactMatrix[second][first]->setReleased();
    else
        contactMatrix[first][second]->setReleased();
}

// ------------------------------------------------------------------------
// VESS Internal function.  Clears the contact matrix of all contacts.
// ------------------------------------------------------------------------
void vsChordGloves::clearContacts()
{
    int i, j;

    // Set all the contacts to disconnected
    for (i = 0; i < VS_CG_MAX_DIGITS - 1; i++)
    {
        for (j = i+1; j < VS_CG_MAX_DIGITS; j++)
            contactMatrix[i][j]->setReleased();
    }
}

// ------------------------------------------------------------------------
// Returns the number of input axes (zero in this case)
// ------------------------------------------------------------------------
int vsChordGloves::getNumAxes()
{
    // NEEDS COMMENT: WHY ALWAYS ZERO?
    return 0;
}

// ------------------------------------------------------------------------
// Returns the number of input buttons
// ------------------------------------------------------------------------
int vsChordGloves::getNumButtons()
{
    // Compute and return the number of buttons in the matrix
    return (2*VS_CG_MAX_DIGITS - 1);
}

// ------------------------------------------------------------------------
// Returns the given input axis (always NULL in this case)
// ------------------------------------------------------------------------
vsInputAxis *vsChordGloves::getAxis(int index)
{
    // No axes on chord gloves
    return NULL;
}

// ------------------------------------------------------------------------
// Returns the given input button.  This function isn't particularly
// meaningful in this class, but it is provided to comply with the object
// hierarchy.  For the purposes of this function, the vsInputButton objects
// are enumerated in row-major order and only the cells above the matrix
// diagonal are counted.
// ------------------------------------------------------------------------
vsInputButton *vsChordGloves::getButton(int index)
{
    int row, thisRowStart, nextRowStart;
    int col;

    // Check if this is a valid index
    if (index > (2 * VS_CG_MAX_DIGITS - 1))
        return NULL;

    // Determine the row and column of the given index
    row = 0;
    thisRowStart = 0;
    nextRowStart = VS_CG_MAX_DIGITS - (row+1);

    // Count down the matrix to find the correct row
    while (index >= nextRowStart)
    {
        row++;
        thisRowStart = nextRowStart;
        nextRowStart += VS_CG_MAX_DIGITS - (row+1);
    }

    // Compute the column from the row and the difference between
    // the given button index and the starting index of the row
    col = row + (index - thisRowStart) + 1;

    // Call the alternative getButton() with the row and column
    return getButton(row, col);
}

// ------------------------------------------------------------------------
// Returns the "input button" corresponding to the given pair of digits
// ------------------------------------------------------------------------
vsInputButton *vsChordGloves::getButton(int first, int second)
{
    // Only return contacts in the cells above the matrix diagonal
    // (the matrix is symmetric)
    if (first > second)
        return contactMatrix[second][first];
    else
        return contactMatrix[first][second];
}

// ------------------------------------------------------------------------
// Lists the current contact pairs in the given array.  Each pair is an
// even/odd neighbor in the array (i.e.: pairs[0] and pairs[1] are a 
// contact pair, as are pairs[8] and pairs[9]).  
//
// Returns the total number of contacts (even if there is not enough room
// to list all contacts in the array).
// ------------------------------------------------------------------------
int vsChordGloves::getContactPairs(int *pairs, int maxSize)
{
    int numPairs;
    int i, j;

    // Make sure the pairs array is valid
    if (pairs == NULL)
    {
        printf("vsChordGloves::getContactPairs:  pairs array is NULL!\n");

        return 0;
    }

    // Initialize the number of contact pairs
    numPairs = 0;

    // Scan the contactMatrix looking for VS_TRUE's
    for (i = 0; i < VS_CG_MAX_DIGITS-1; i++)
    {
        for (j = i + 1; j < VS_CG_MAX_DIGITS; j++)
        {
            // Check button i,j to see if it is pressed
            if (getButton(i, j)->isPressed())
            {
                // Found a contact pair, add it to the array if there's room
                if (((2*numPairs + 1) * sizeof(int)) <= maxSize)
                {
                    pairs[2*numPairs]       = i;
                    pairs[(2*numPairs) + 1] = j;
                }

                // Increment the pair count
                numPairs++;
            }
        }
    }

    // Return the number of pairs we found
    return numPairs;
}
