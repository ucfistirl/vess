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
//    VESS Module:  vsRenderBin.c++
//
//    Description:  Class to represent a render bin, into which elements
//                  of the scene are sorted after culling and before
//                  drawing.  Render bins have numbers to indicate in
//                  which order they are drawn, and they also have a
//                  sorting mode (depth sort or state sort).  By default
//                  all geometry uses render bin 0, and bin 0 defaults to
//                  state sorting.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsRenderBin.h++"

// Global render bin list, and a flag to indicate when changes have been
// made to the sort modes
vsArray vsRenderBin::renderBinList = vsArray();
bool    vsRenderBin::binModesChanged = true;

// ------------------------------------------------------------------------
// Private constructor.  Render bins are meant to be shared, and there
// should only be one instance of a vsRenderBin per bin number.  Bins
// should instead be created and accessed through the getBin() method
// ------------------------------------------------------------------------
vsRenderBin::vsRenderBin(int number)
{
   // Set the bin number and default bin sort mode
   binNumber = number;
   sortMode = VS_RENDER_BIN_SORT_STATE;
}

// ------------------------------------------------------------------------
// Private destructor.  Render bins are shared and reference counted, so
// they shouldn't be deleted explicitly.
// ------------------------------------------------------------------------
vsRenderBin::~vsRenderBin()
{
}

// ------------------------------------------------------------------------
// Get (and possibly create) the render bin corresponding to the given
// bin number
// ------------------------------------------------------------------------
vsRenderBin *vsRenderBin::getBin(int number)
{
    long low, high, mid;
    vsRenderBin *bin;
    vsRenderBin *newBin;
 
    // Scan the bin list for the bin with the given number
    low = 0;
    high = renderBinList.getNumEntries();
    bin = NULL;
    do
    {
        // Compute the next index in the array to try, and fetch the
        // bin
        mid = (low + high) / 2;
        bin = (vsRenderBin *) renderBinList.getEntry(mid);

        // Make sure there is actually a bin at that index
        if (bin != NULL)
        {
            if (bin->getNumber() > number)
            {
                // Number too high, look lower in the list
                high = mid - 1;
            }
            else if (bin->getNumber() < number)
            {
                // Number too low, look higher in the list
                low = mid + 1;
            }
            else
            {
                // Found the bin we want, so return it
                return bin;
            }
        }
    }
    while (low < high);

    // If we get this far, the requested bin doesn't exist (yet), so create
    // it now
    newBin = new vsRenderBin(number);

    // Add it to the bin list in the correct place
    if (renderBinList.getNumEntries() == 0)
    {
        // Add the first entry to the list
        renderBinList.insertEntry(0, newBin);
    }
    else
    {
        // There are already bins in the list.  Compare the last bin
        // number we checked to the new number (the new bin will either
        // go immediately before or after this bin)
        if (bin->getNumber() > number)
        {
            // Insert before the existing bin
            renderBinList.insertEntry(mid, newBin);
        }
        else
        {
            // Insert after the existing bin
            renderBinList.insertEntry(mid+1, newBin);
        }
    }

    // Return the new bin that we created
    return newBin;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsRenderBin::getClassName()
{
   return "vsRenderBin";
}

// ------------------------------------------------------------------------
// Return the number of this render bin
// ------------------------------------------------------------------------
int vsRenderBin::getNumber()
{
   return binNumber;
}

// ------------------------------------------------------------------------
// Return the sorting mode for this render bin
// ------------------------------------------------------------------------
vsRenderBinSortMode vsRenderBin::getSortMode()
{
   return sortMode;
}

// ------------------------------------------------------------------------
// Change the sort mode for this render bin
// ------------------------------------------------------------------------
void vsRenderBin::setSortMode(vsRenderBinSortMode newMode)
{
   // Set the new sort mode
   sortMode = newMode;

   // Set the flag that indicates that a render bin sort mode has changed
   binModesChanged = true;
}

