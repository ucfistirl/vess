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
//    VESS Module:  vsScenePrinter.h++
//
//    Description:  Dumps a textual representation of a scene graph out
//                  to a file
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SCENE_PRINTER_HPP
#define VS_SCENE_PRINTER_HPP

#include <stdio.h>
#include "vsNode.h++"

class vsScenePrinter
{
private:

    void        writeBlanks(FILE *outfile, int count);
    void        writeScene(vsNode *targetNode, FILE *outfile,
                           int treeDepth, int *countArray);

public:

                vsScenePrinter();
    virtual     ~vsScenePrinter();

    void        printScene(vsNode *targetNode, char *outputFileName);
    void        printScene(vsNode *targetNode, FILE *outputFile);
};

#endif
