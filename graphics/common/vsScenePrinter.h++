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
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"
#include "vsSkeletonMeshGeometry.h++"

#define VS_PRINTER_ATTRIBUTES        0x00000001
#define VS_PRINTER_ATTRIBUTE_DETAILS 0x00000002
#define VS_PRINTER_GEOMETRY          0x00000004
#define VS_PRINTER_GEOMETRY_BINDINGS 0x00000008
#define VS_PRINTER_GEOMETRY_LISTS    0x00000010
#define VS_PRINTER_NODE_NAMES        0x00000020
#define VS_PRINTER_NODE_ADDRESSES    0x00000040

class VS_GRAPHICS_DLL vsScenePrinter : public vsObject
{
private:

    int         printerMode;

    void        writeGeometryList(vsGeometry *geometry, int dataList,
                                  int treeDepth, FILE *outputFile);
    void        writeDynamicGeometryList(vsDynamicGeometry *geometry, 
                                  int dataList, int treeDepth, 
                                  FILE *outputFile);
    void        writeSkeletonMeshGeometryList(vsSkeletonMeshGeometry *geometry,
                                  int dataList, int treeDepth, 
                                  FILE *outputFile);
    void        writeIndexList(u_int *indexList, int indexListSize,
                               int primitiveCount, int primitiveType,
                               int *primitiveLengths, int treeDepth,
                               FILE *outputFile);
    void        writeGeometry(vsGeometry *geometry, FILE *outFile,
                              int treeDepth);
    void        writeDynamicGeometry(vsDynamicGeometry *geometry, 
                                     FILE *outFile, int treeDepth);
    void        writeSkeletonMeshGeometry(vsSkeletonMeshGeometry *geometry,
                                          FILE *outFile, int treeDepth);
    void        writeBlanks(FILE *outfile, int count);
    void        writeScene(vsNode *targetNode, FILE *outfile,
                           int treeDepth, int *countArray);

public:

                         vsScenePrinter();
    virtual              ~vsScenePrinter();

    void                 setPrinterMode(int newMode);

    virtual const char   *getClassName();

    void                  printScene(vsNode *targetNode, char *outputFileName);
    void                  printScene(vsNode *targetNode, FILE *outputFile);
};

#endif
