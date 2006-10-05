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

#include "vsScenePrinter.h++"

#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsBackfaceAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsTransformAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsLightAttribute.h++"
#include "vsShaderAttribute.h++"
#include "vsLODAttribute.h++"
#include "vsGLSLProgramAttribute.h++"
#include "vsGLSLShader.h++"
#include "vsGLSLUniform.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsScenePrinter::vsScenePrinter()
{
    // Set the printer to print attributes and geometry, but no details
    // for either.  Also include node names and addresses.
    printerMode = VS_PRINTER_ATTRIBUTES | VS_PRINTER_GEOMETRY | 
        VS_PRINTER_NODE_NAMES | VS_PRINTER_NODE_ADDRESSES;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsScenePrinter::~vsScenePrinter()
{
}

// ------------------------------------------------------------------------
// Set the printer mode to the new given mode.  The mode controls the
// verbosity of the output.  Input to this method should be a combination
// of the printer bitmasks defined in the header file.
// ------------------------------------------------------------------------
void vsScenePrinter::setPrinterMode(int newMode)
{
    // Set the new mode
    printerMode = newMode;

    // Some modes imply other modes are on as well.  
    // VS_PRINTER_GEOMETRY_BINDINGS implies that VS_PRINTER_GEOMETRY is on.
    // Likewise, VS_PRINTER_GEOMETRY_LISTS implies VS_PRINTER_GEOMETRY_BINDINGS
    // which implies VS_PRINTER_GEOMETRY.  VS_PRINTER_ATTRIBUTE_DETAILS
    // implies VS_PRINTER_ATTRIBUTES.  The code below handles these 
    // implied modes

    // GEOMETRY_BINDINGS implies GEOMETRY
    if (printerMode & VS_PRINTER_GEOMETRY_BINDINGS)
        printerMode |= VS_PRINTER_GEOMETRY;

    // GEOMETRY_LISTS implies GEOMETRY and GEOMETRY_BINDINGS
    if (printerMode & VS_PRINTER_GEOMETRY_LISTS)
        printerMode |= VS_PRINTER_GEOMETRY | VS_PRINTER_GEOMETRY_BINDINGS;
    
    // ATTRIBUTE_DETAILS implies ATTRIBUTES
    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
        printerMode |= VS_PRINTER_ATTRIBUTES;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsScenePrinter::getClassName()
{
    return "vsScenePrinter";
}

// ------------------------------------------------------------------------
// Writes a textual representation of the scene rooted at the given node
// out to the file specified by the given filename.
// ------------------------------------------------------------------------
void vsScenePrinter::printScene(vsNode *targetNode, char *outputFileName)
{
    FILE *outfile;
    int counts[256];

    // Open the output file
    outfile = fopen(outputFileName, "w");
    if (!outfile)
    {
        printf("vsScenePrinter::printScene: Unable to open output file '%s'\n",
            outputFileName);
        return;
    }

    // Print the scene to our new file
    writeScene(targetNode, outfile,  0, counts);

    // Close the file now that we're done
    fclose(outfile);
}

// ------------------------------------------------------------------------
// Writes a textual representation of the scene rooted at the given node
// out to the specified file.
// ------------------------------------------------------------------------
void vsScenePrinter::printScene(vsNode *targetNode, FILE *outputFile)
{
    int counts[256];
    
    // Print the scene to the file
    writeScene(targetNode, outputFile,  0, counts);

    // Make sure we've written everything to disk
    fflush(outputFile);
}

// ------------------------------------------------------------------------
// Prints the given data list from the given geometry at the correct
// indentation level
// ------------------------------------------------------------------------
void vsScenePrinter::writeGeometryList(vsGeometry *geometry, int dataList,
                                       int treeDepth, FILE *outputFile)
{
    int size;
    int loop;
    vsVector geoVec;

    // Get the length of the list
    size = geometry->getDataListSize(dataList);

    // If the list is larger than zero...
    if (size > 0)
    {
        // Start a new output level for the list info
        writeBlanks(outputFile, (treeDepth * 2) + 3);
        fprintf(outputFile, "{\n");

        // Write each list element out on its own line
        for (loop = 0; loop < size; loop++)
        {
            writeBlanks(outputFile, (treeDepth * 2) + 5);
            geoVec = geometry->getData(dataList, loop);
            geoVec.print(outputFile);
            fprintf(outputFile, "\n");
        }

        // Finish the list output
        writeBlanks(outputFile, (treeDepth * 2) + 3);
        fprintf(outputFile, "}\n");
    }
}

// ------------------------------------------------------------------------
// Prints the given data list from the given geometry at the correct
// indentation level
// ------------------------------------------------------------------------
void vsScenePrinter::writeDynamicGeometryList(vsDynamicGeometry *geometry, 
                                       int dataList, int treeDepth, 
                                       FILE *outputFile)
{
    int size;
    int loop;
    vsVector geoVec;

    // Get the length of the list
    size = geometry->getDataListSize(dataList);

    // If the list is larger than zero...
    if (size > 0)
    {
        // Start a new output level for the list info
        writeBlanks(outputFile, (treeDepth * 2) + 3);
        fprintf(outputFile, "{\n");

        // Write each list element out on its own line
        for (loop = 0; loop < size; loop++)
        {
            writeBlanks(outputFile, (treeDepth * 2) + 5);
            geoVec = geometry->getData(dataList, loop);
            geoVec.print(outputFile);
            fprintf(outputFile, "\n");
        }

        // Finish the list output
        writeBlanks(outputFile, (treeDepth * 2) + 3);
        fprintf(outputFile, "}\n");
    }
}

// ------------------------------------------------------------------------
// Prints the given data list from the given geometry at the correct
// indentation level
// ------------------------------------------------------------------------
void vsScenePrinter::writeSkeletonMeshGeometryList(
                                          vsSkeletonMeshGeometry *geometry, 
                                          int dataList, int treeDepth, 
                                          FILE *outputFile)
{
    int size;
    int loop;
    vsVector geoVec;

    // Get the length of the list
    size = geometry->getDataListSize(dataList);

    // If the list is larger than zero...
    if (size > 0)
    {
        // Start a new output level for the list info
        writeBlanks(outputFile, (treeDepth * 2) + 3);
        fprintf(outputFile, "{\n");

        // Write each list element out on its own line
        for (loop = 0; loop < size; loop++)
        {
            writeBlanks(outputFile, (treeDepth * 2) + 5);
            geoVec = geometry->getData(dataList, loop);
            geoVec.print(outputFile);
            fprintf(outputFile, "\n");
        }

        // Finish the list output
        writeBlanks(outputFile, (treeDepth * 2) + 3);
        fprintf(outputFile, "}\n");
    }
}

// ------------------------------------------------------------------------
// Prints the information for the given geometry
// ------------------------------------------------------------------------
void vsScenePrinter::writeGeometry(vsGeometry *geometry, FILE *outfile, 
                                   int treeDepth)
{
    int geoType, geoCount;
    int geoBinding;
    vsVector geoVec;
    int size, length;
    int loop;
    int textureUnit;

    // Print the primitive type and count
    writeBlanks(outfile, (treeDepth * 2) + 1);
    geoType = geometry->getPrimitiveType();
    geoCount = geometry->getPrimitiveCount();
    fprintf(outfile, "%d ", geoCount);
    switch (geoType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            fprintf(outfile, "POINTS");
            break;
        case VS_GEOMETRY_TYPE_LINES:
            fprintf(outfile, "LINES");
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            fprintf(outfile, "LINE STRIPS");
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            fprintf(outfile, "LINE LOOPS");
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            fprintf(outfile, "TRIS");
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            fprintf(outfile, "TRI STRIPS");
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            fprintf(outfile, "TRI FANS");
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            fprintf(outfile, "QUADS");
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            fprintf(outfile, "QUAD STRIPS");
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            fprintf(outfile, "POLYS");
            break;
        default:
            fprintf(outfile, "?");
            break;
    }

    // Now, print the binding
    fprintf(outfile, "  Vertex binding: ");
    geoBinding = geometry->getBinding(VS_GEOMETRY_VERTEX_COORDS);
    switch (geoBinding)
    {
        case VS_GEOMETRY_BIND_NONE:
            fprintf(outfile, "NONE");
            break;
        case VS_GEOMETRY_BIND_OVERALL:
            fprintf(outfile, "OVERALL");
            break;
        case VS_GEOMETRY_BIND_PER_PRIMITIVE:
            fprintf(outfile, "PER PRIMITIVE");
            break;
        case VS_GEOMETRY_BIND_PER_VERTEX:
            fprintf(outfile, "PER VERTEX");
            break;
    }
    fprintf(outfile, "\n");

    // Print vertex coordinates (if configured and there is geometry
    // to print)
    if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) && (geoCount > 0))
    {
        writeGeometryList(geometry, VS_GEOMETRY_VERTEX_COORDS, 
            treeDepth, outfile);

        // If primitive type is variable length, print the lengths array
        if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) &&
            ((geoType == VS_GEOMETRY_TYPE_LINE_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_LINE_LOOPS) ||
             (geoType == VS_GEOMETRY_TYPE_TRI_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_TRI_FANS) ||
             (geoType == VS_GEOMETRY_TYPE_QUAD_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_POLYS)))
        {
            // Start a new list output level
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "LENGTHS\n");
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "{\n");

            // Print each primitive's length on its own line
            size = geometry->getPrimitiveCount();
            for (loop = 0; loop < size; loop++)
            {
                writeBlanks(outfile, (treeDepth * 2) + 5);
                length = geometry->getPrimitiveLength(loop);
                fprintf(outfile, "%d\n", length);
            }

            // Finish up the list
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "}\n");
        }
    }
    
    // Print geometry bindings (if configured)
    if (printerMode & VS_PRINTER_GEOMETRY_BINDINGS)
    {
        // Only print binding info for normals, colors, and texture
        // coords, because vertex coords are always per-vertex
        for (loop = 0; loop < 10; loop++)
        {
            // Don't write the spaces for indenting if this data list
            // isn't supported
            if (loop - 2 < VS_MAXIMUM_TEXTURE_UNITS)
                writeBlanks(outfile, (treeDepth * 2) + 1);

            // Print the list type
            switch (loop)
            {
                case 0:
                    fprintf(outfile, "NORMALS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_NORMALS));
                    geoType = VS_GEOMETRY_NORMALS;
                    break;

                case 1:
                    fprintf(outfile, "COLORS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_COLORS));
                    geoType = VS_GEOMETRY_COLORS;
                    break;

                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    // Attributes 2 - 9 are texture coordinates.  One set
                    // corresponds to each texture unit.  First, figure out
                    // which texture unit we're working with.
                    textureUnit = loop - 2;

                    // Print the information for this texture unit, if it
                    // is supported by the graphics library.
                    if (textureUnit < VS_MAXIMUM_TEXTURE_UNITS)
                    {
                        fprintf(outfile, "TEXCOORDS (unit %d) (%d): ",
                            textureUnit, 
                            geometry->getDataListSize(
                                VS_GEOMETRY_TEXTURE0_COORDS + textureUnit));
                        geoType = VS_GEOMETRY_TEXTURE0_COORDS + textureUnit;
                    }
                    break;
            }

            // Now, print the binding if the current list is supported
            // by the graphics library
            if ((loop < 2) ||
                (loop >= 2) && (textureUnit < VS_MAXIMUM_TEXTURE_UNITS))
            {
                geoBinding = geometry->getBinding(geoType);
                switch (geoBinding)
                {
                    case VS_GEOMETRY_BIND_NONE:
                        fprintf(outfile, "NONE");
                        break;
                    case VS_GEOMETRY_BIND_OVERALL:
                        fprintf(outfile, "OVERALL");
                        break;
                    case VS_GEOMETRY_BIND_PER_PRIMITIVE:
                        fprintf(outfile, "PER PRIMITIVE");
                        break;
                    case VS_GEOMETRY_BIND_PER_VERTEX:
                        fprintf(outfile, "PER VERTEX");
                        break;
                }
                fprintf(outfile, "\n");
            }

            // Print out the current data list (normals, colors, or
            // texture coordinates) if configured to do so
            if (printerMode & VS_PRINTER_GEOMETRY_LISTS)
            {
                switch (loop)
                {
                    case 0:
                        // Print normal data
                        writeGeometryList(geometry, VS_GEOMETRY_NORMALS,
                            treeDepth, outfile);
                        break;

                    case 1:
                        // Print color data
                        writeGeometryList(geometry, VS_GEOMETRY_COLORS,
                            treeDepth, outfile);
                        break;

                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        // As above, print texture coordinate data 
                        // corresponding to the proper texture unit
                        textureUnit = loop - 2;

                        // Skip this texture unit's data if it isn't supported
                        // by the graphics library.
                        if (textureUnit < VS_MAXIMUM_TEXTURE_UNITS)
                        {
                            writeGeometryList(geometry, 
                                VS_GEOMETRY_TEXTURE_COORDS + textureUnit,
                                treeDepth, outfile);
                        }
                        break;
                }
            }
        }
    }
} 
// ------------------------------------------------------------------------
// Prints the information for the given dynamic geometry
// ------------------------------------------------------------------------
void vsScenePrinter::writeDynamicGeometry(vsDynamicGeometry *geometry, 
                                          FILE *outfile, 
                                          int treeDepth)
{
    int geoType, geoCount;
    int geoBinding;
    vsVector geoVec;
    int size, length;
    int loop;
    int textureUnit;

    // Print the primitive type and count
    writeBlanks(outfile, (treeDepth * 2) + 1);
    geoType = geometry->getPrimitiveType();
    geoCount = geometry->getPrimitiveCount();
    fprintf(outfile, "%d ", geoCount);
    switch (geoType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            fprintf(outfile, "POINTS");
            break;
        case VS_GEOMETRY_TYPE_LINES:
            fprintf(outfile, "LINES");
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            fprintf(outfile, "LINE STRIPS");
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            fprintf(outfile, "LINE LOOPS");
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            fprintf(outfile, "TRIS");
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            fprintf(outfile, "TRI STRIPS");
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            fprintf(outfile, "TRI FANS");
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            fprintf(outfile, "QUADS");
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            fprintf(outfile, "QUAD STRIPS");
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            fprintf(outfile, "POLYS");
            break;
        default:
            fprintf(outfile, "?");
            break;
    }
    fprintf(outfile, "\n");

    // Print vertex coordinates (if configured and there is geometry
    // to print)
    if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) && (geoCount > 0))
    {
        writeDynamicGeometryList(geometry, VS_GEOMETRY_VERTEX_COORDS,
            treeDepth, outfile);

        // If primitive type is variable length, print the lengths array
        if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) &&
            ((geoType == VS_GEOMETRY_TYPE_LINE_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_LINE_LOOPS) ||
             (geoType == VS_GEOMETRY_TYPE_TRI_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_TRI_FANS) ||
             (geoType == VS_GEOMETRY_TYPE_QUAD_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_POLYS)))
        {
            // Start a new list output level
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "LENGTHS\n");
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "{\n");

            // Print each primitive's length on its own line
            size = geometry->getPrimitiveCount();
            for (loop = 0; loop < size; loop++)
            {
                writeBlanks(outfile, (treeDepth * 2) + 5);
                length = geometry->getPrimitiveLength(loop);
                fprintf(outfile, "%d\n", length);
            }

            // Finish up the list
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "}\n");
        }
    }
    
    // Print geometry bindings (if configured)
    if (printerMode & VS_PRINTER_GEOMETRY_BINDINGS)
    {
        // Only print binding info for normals, colors, and texture
        // coords, because vertex coords are always per-vertex
        for (loop = 0; loop < 10; loop++)
        {
            // First, print the list type
            writeBlanks(outfile, (treeDepth * 2) + 1);
            switch (loop)
            {
                case 0:
                    fprintf(outfile, "NORMALS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_NORMALS));
                    geoType = VS_GEOMETRY_NORMALS;
                    break;

                case 1:
                    fprintf(outfile, "COLORS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_COLORS));
                    geoType = VS_GEOMETRY_COLORS;
                    break;

                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    // Attributes 2 - 9 are texture coordinates.  One set
                    // corresponds to each texture unit.  First, figure out
                    // which texture unit we're working with.
                    textureUnit = loop - 2;

                    // Print the information for this texture unit, if it
                    // is supported by the graphics library.
                    if (textureUnit < VS_MAXIMUM_TEXTURE_UNITS)
                    {
                        fprintf(outfile, "TEXCOORDS (unit %d) (%d): ",
                            textureUnit, 
                            geometry->getDataListSize(
                                VS_GEOMETRY_TEXTURE0_COORDS + textureUnit));
                        geoType = VS_GEOMETRY_TEXTURE0_COORDS + textureUnit;
                    }
                    break;
            }

            // Now, print the binding
            geoBinding = geometry->getBinding(geoType);
            switch (geoBinding)
            {
                case VS_GEOMETRY_BIND_NONE:
                    fprintf(outfile, "NONE");
                    break;
                case VS_GEOMETRY_BIND_OVERALL:
                    fprintf(outfile, "OVERALL");
                    break;
                case VS_GEOMETRY_BIND_PER_PRIMITIVE:
                    fprintf(outfile, "PER PRIMITIVE");
                    break;
                case VS_GEOMETRY_BIND_PER_VERTEX:
                    fprintf(outfile, "PER VERTEX");
                    break;
            }
            fprintf(outfile, "\n");

            // Print out the current data list (normals, colors, or
            // texture coordinates) if configured to do so
            if (printerMode & VS_PRINTER_GEOMETRY_LISTS)
            {
                switch (loop)
                {
                    case 0:
                        // Print normal data
                        writeDynamicGeometryList(geometry, VS_GEOMETRY_NORMALS,
                            treeDepth, outfile);
                        break;

                    case 1:
                        // Print color data
                        writeDynamicGeometryList(geometry, VS_GEOMETRY_COLORS,
                            treeDepth, outfile);
                        break;

                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        // As above, print texture coordinate data 
                        // corresponding to the proper texture unit
                        textureUnit = loop - 2;

                        // Skip this texture unit's data if it isn't supported
                        // by the graphics library.
                        if (textureUnit < VS_MAXIMUM_TEXTURE_UNITS)
                        {
                            writeDynamicGeometryList(geometry, 
                                VS_GEOMETRY_TEXTURE_COORDS + textureUnit,
                                treeDepth, outfile);
                        }
                }
            }
        }
    }
}

// ------------------------------------------------------------------------
// Prints the information for the given dynamic geometry
// ------------------------------------------------------------------------
void vsScenePrinter::writeSkeletonMeshGeometry(
                                          vsSkeletonMeshGeometry *geometry, 
                                          FILE *outfile, int treeDepth)
{
    int geoType, geoCount;
    int geoBinding;
    vsVector geoVec;
    int size, length;
    int loop;
    int textureUnit;

    // Print the primitive type and count
    writeBlanks(outfile, (treeDepth * 2) + 1);
    geoType = geometry->getPrimitiveType();
    geoCount = geometry->getPrimitiveCount();
    fprintf(outfile, "%d ", geoCount);
    switch (geoType)
    {
        case VS_GEOMETRY_TYPE_POINTS:
            fprintf(outfile, "POINTS");
            break;
        case VS_GEOMETRY_TYPE_LINES:
            fprintf(outfile, "LINES");
            break;
        case VS_GEOMETRY_TYPE_LINE_STRIPS:
            fprintf(outfile, "LINE STRIPS");
            break;
        case VS_GEOMETRY_TYPE_LINE_LOOPS:
            fprintf(outfile, "LINE LOOPS");
            break;
        case VS_GEOMETRY_TYPE_TRIS:
            fprintf(outfile, "TRIS");
            break;
        case VS_GEOMETRY_TYPE_TRI_STRIPS:
            fprintf(outfile, "TRI STRIPS");
            break;
        case VS_GEOMETRY_TYPE_TRI_FANS:
            fprintf(outfile, "TRI FANS");
            break;
        case VS_GEOMETRY_TYPE_QUADS:
            fprintf(outfile, "QUADS");
            break;
        case VS_GEOMETRY_TYPE_QUAD_STRIPS:
            fprintf(outfile, "QUAD STRIPS");
            break;
        case VS_GEOMETRY_TYPE_POLYS:
            fprintf(outfile, "POLYS");
            break;
        default:
            fprintf(outfile, "?");
            break;
    }
    fprintf(outfile, "\n");

    // Print vertex coordinates (if configured and there is geometry
    // to print)
    if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) && (geoCount > 0))
    {
        writeSkeletonMeshGeometryList(geometry, VS_GEOMETRY_VERTEX_COORDS,
            treeDepth, outfile);

        // If primitive type is variable length, print the lengths array
        if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) &&
            ((geoType == VS_GEOMETRY_TYPE_LINE_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_LINE_LOOPS) ||
             (geoType == VS_GEOMETRY_TYPE_TRI_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_TRI_FANS) ||
             (geoType == VS_GEOMETRY_TYPE_QUAD_STRIPS) ||
             (geoType == VS_GEOMETRY_TYPE_POLYS)))
        {
            // Start a new list output level
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "LENGTHS\n");
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "{\n");

            // Print each primitive's length on its own line
            size = geometry->getPrimitiveCount();
            for (loop = 0; loop < size; loop++)
            {
                writeBlanks(outfile, (treeDepth * 2) + 5);
                length = geometry->getPrimitiveLength(loop);
                fprintf(outfile, "%d\n", length);
            }

            // Finish up the list
            writeBlanks(outfile, (treeDepth * 2) + 3);
            fprintf(outfile, "}\n");
        }
    }
    
    // Print geometry bindings (if configured)
    if (printerMode & VS_PRINTER_GEOMETRY_BINDINGS)
    {
        // Only print binding info for normals, colors, and texture
        // coords, because vertex coords are always per-vertex
        for (loop = 0; loop < 14; loop++)
        {
            // First, print the geometry data list type
            geoType = -1;
            switch (loop)
            {
                case 0:
                    // This is the list of normals
                    writeBlanks(outfile, (treeDepth * 2) + 1);
                    fprintf(outfile, "NORMALS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_NORMALS));
                    geoType = VS_GEOMETRY_NORMALS;
                    break;

                case 1:
                    // This is the list of colors
                    writeBlanks(outfile, (treeDepth * 2) + 1);
                    fprintf(outfile, "COLORS (%d): ",
                        geometry->getDataListSize(VS_GEOMETRY_COLORS));
                    geoType = VS_GEOMETRY_COLORS;
                    break;

                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    // Attributes 2 - 9 are texture coordinates.  One set
                    // corresponds to each texture unit.  First, figure out
                    // which texture unit we're working with.
                    textureUnit = loop - 2;

                    // Print the information for this texture unit, if it
                    // is supported by the graphics library.
                    if (textureUnit < VS_MAXIMUM_TEXTURE_UNITS)
                    {
                        writeBlanks(outfile, (treeDepth * 2) + 1);
                        fprintf(outfile, "TEXCOORDS (unit %d) (%d): ",
                            textureUnit, 
                            geometry->getDataListSize(
                                VS_GEOMETRY_TEXTURE0_COORDS + textureUnit));
                        geoType = VS_GEOMETRY_TEXTURE0_COORDS + textureUnit;
                    }
                    break;

                case 10:
                    // This is the list of reference vertex coordinates used
                    // for vertex skinning.
                    writeBlanks(outfile, (treeDepth * 2) + 1);
                    fprintf(outfile, "SKIN VERTEX COORDS (%d): ",
                        geometry->
                            getDataListSize(VS_GEOMETRY_SKIN_VERTEX_COORDS));
                    geoType = VS_GEOMETRY_SKIN_VERTEX_COORDS;
                    break;

                case 11:
                    // This is the list of reference normals used for vertex 
                    // skinning.
                    writeBlanks(outfile, (treeDepth * 2) + 1);
                    fprintf(outfile, "SKIN NORMALS (%d): ",
                        geometry->
                            getDataListSize(VS_GEOMETRY_SKIN_NORMALS));
                    geoType = VS_GEOMETRY_SKIN_NORMALS;
                    break;

                case 12:
                    // This is the list of vertex weights used for vertex
                    // skinning.
                    writeBlanks(outfile, (treeDepth * 2) + 1);
                    fprintf(outfile, "VERTEX WEIGHTS (%d): ",
                        geometry->
                            getDataListSize(VS_GEOMETRY_VERTEX_WEIGHTS));
                    geoType = VS_GEOMETRY_VERTEX_WEIGHTS;
                    break;

                case 13:
                    // This is the list of bone indices used for vertex
                    // skinning.
                    writeBlanks(outfile, (treeDepth * 2) + 1);
                    fprintf(outfile, "BONE INDICES (%d): ",
                        geometry->
                            getDataListSize(VS_GEOMETRY_BONE_INDICES));
                    geoType = VS_GEOMETRY_BONE_INDICES;
                    break;
            }

            // Now, print the binding, don't print anything if the geometry 
            // type isn't valid (e.g.: an unsupported texture unit's texture
            // coordinates)
            if (geoType >= 0)
            {
                geoBinding = geometry->getBinding(geoType);
                switch (geoBinding)
                {
                    case VS_GEOMETRY_BIND_NONE:
                        fprintf(outfile, "NONE");
                        break;
                    case VS_GEOMETRY_BIND_OVERALL:
                        fprintf(outfile, "OVERALL");
                        break;
                    case VS_GEOMETRY_BIND_PER_PRIMITIVE:
                        fprintf(outfile, "PER PRIMITIVE");
                        break;
                    case VS_GEOMETRY_BIND_PER_VERTEX:
                        fprintf(outfile, "PER VERTEX");
                        break;
                }
                fprintf(outfile, "\n");
            }

            // Print out the current data list (normals, colors, or
            // texture coordinates) if configured to do so
            if (printerMode & VS_PRINTER_GEOMETRY_LISTS)
            {
                switch (loop)
                {
                    case 0:
                        // Print normal data
                        writeSkeletonMeshGeometryList(geometry,
                            VS_GEOMETRY_NORMALS, treeDepth, outfile);
                        break;

                    case 1:
                        // Print color data
                        writeSkeletonMeshGeometryList(geometry,
                            VS_GEOMETRY_COLORS, treeDepth, outfile);
                        break;

                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        // As above, print out the texture coordinate data
                        // for the proper texture unit
                        textureUnit = loop - 2;

                        // Skip this texture unit's data if it isn't supported
                        // by the graphics library.
                        if (textureUnit < VS_MAXIMUM_TEXTURE_UNITS)
                        {
                            writeSkeletonMeshGeometryList(geometry,
                                VS_GEOMETRY_TEXTURE0_COORDS + textureUnit, 
                                treeDepth, outfile);
                        }
                        break;

                    case 10:
                        // Print the skinning reference vertex coordinates
                        writeSkeletonMeshGeometryList(geometry,
                            VS_GEOMETRY_SKIN_VERTEX_COORDS, treeDepth, 
                            outfile);
                        break;

                    case 11:
                        // Print the skinning reference normals
                        writeSkeletonMeshGeometryList(geometry,
                                VS_GEOMETRY_SKIN_NORMALS, treeDepth, outfile);
                        break;

                    case 12:
                        // Print the vertex weights
                        writeSkeletonMeshGeometryList(geometry,
                            VS_GEOMETRY_VERTEX_WEIGHTS, treeDepth, outfile);
                        break;

                    case 13:
                        // Print the bone indices
                        writeSkeletonMeshGeometryList(geometry,
                            VS_GEOMETRY_BONE_INDICES, treeDepth, outfile);
                        break;
                }
            }
        }
    }
}

// ------------------------------------------------------------------------
// Private static utility function
// Writes the specified number of space characters to the given file
// ------------------------------------------------------------------------
void vsScenePrinter::writeBlanks(FILE *outfile, int count)
{
    // Print the given number of spaces to the output file
    for (int loop = 0; loop < count; loop++)
        fprintf(outfile, " ");
}

// ------------------------------------------------------------------------
// Private static function
// Recursive function that writes the specified scene to the given (open)
// file. The countArray contains the current child number at each depth
// level of the VESS tree.
// ------------------------------------------------------------------------
void vsScenePrinter::writeScene(vsNode *targetNode, FILE *outfile,
    int treeDepth, int *countArray)
{
    vsGeometry *geometry;
    vsDynamicGeometry *dynamicGeometry;
    vsSkeletonMeshGeometry *smGeometry;
    vsAttribute *attribute;
    int loop, sloop;
    vsMatrix mat;
    double r, g, b, a;
    double nearFog, farFog;
    int mode;
    int shadingData;
    bool attrData;
    double quadratic, linear, constant;
    double x, y, z;
    double exponent, angle;
    
    // Print which node type
    switch (targetNode->getNodeType())
    {
        case VS_NODE_TYPE_GEOMETRY:
            fprintf(outfile, "Geometry: ");
            break;
        case VS_NODE_TYPE_DYNAMIC_GEOMETRY:
            fprintf(outfile, "Dynamic Geometry: ");
            break;
        case VS_NODE_TYPE_COMPONENT:
            fprintf(outfile, "Component: ");
            break;
        case VS_NODE_TYPE_SCENE:
            fprintf(outfile, "Scene: ");
            break;
        case VS_NODE_TYPE_SKELETON_MESH_GEOMETRY:
            fprintf(outfile, "Skeleton Mesh Geometry: ");
            break;
        case VS_NODE_TYPE_UNMANAGED:
            fprintf(outfile, "Unmanaged Node: ");
            break;
    }
    
    // Print the node's name (if configured)
    if ((VS_PRINTER_NODE_NAMES) && (strlen(targetNode->getName()) > 0))
        fprintf(outfile, "\"%s\" ", targetNode->getName());

    // Print it's address (if configured)
    if (VS_PRINTER_NODE_ADDRESSES)
        fprintf(outfile, "address %p ", targetNode);

    // Indicate if the node is instanced
    if (targetNode->getParentCount() > 1)
        fprintf(outfile, "(instanced) ");

    fprintf(outfile, "\n");

    // If the node is a vsGeometry, write out all of the primitive and
    // binding info
    if ((printerMode & VS_PRINTER_GEOMETRY) && 
        (targetNode->getNodeType() == VS_NODE_TYPE_GEOMETRY))
    {
        // Cast to geometry and write out the geometry data
        geometry = (vsGeometry *)targetNode;
        writeGeometry(geometry, outfile, treeDepth);
    }

    // If the node is a vsDynamicGeometry, write out all of the primitive and
    // binding info
    if ((printerMode & VS_PRINTER_GEOMETRY) && 
        (targetNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        // Cast to geometry and write out the geometry data
        dynamicGeometry = (vsDynamicGeometry *)targetNode;
        writeDynamicGeometry(dynamicGeometry, outfile, treeDepth);
    }

    // If the node is a vsGeometry, write out all of the primitive and
    // binding info
    if ((printerMode & VS_PRINTER_GEOMETRY) && 
        (targetNode->getNodeType() == VS_NODE_TYPE_SKELETON_MESH_GEOMETRY))
    {
        // Cast to geometry and write out the geometry data
        smGeometry = (vsSkeletonMeshGeometry *)targetNode;
        writeSkeletonMeshGeometry(smGeometry, outfile, treeDepth);
    }

    // Print any attached attributes (if configured)
    if (printerMode & VS_PRINTER_ATTRIBUTES)
    {
        // Loop over all attached attributes
        for (loop = 0; loop < targetNode->getAttributeCount(); loop++)
        {
            // Get the next attribute, and print the basic information
            // for it (address, reference count, and type)
            attribute = targetNode->getAttribute(loop);
            writeBlanks(outfile, (treeDepth * 2) + 1);
            fprintf(outfile, "Attribute: address %p, references %d, type ",
                attribute, attribute->isAttached());

            // Figure out the attribute type and print it out
            switch (attribute->getAttributeType())
            {
                case VS_ATTRIBUTE_TYPE_TRANSFORM:
                    fprintf(outfile, "TRANSFORM\n");
                    
                    // Print out the data in the transform attribute's 
                    // three matrices, if configured to do so
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Pre-transform static matrix
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        mat = ((vsTransformAttribute *)attribute)->
                            getPreTransform();
                        fprintf(outfile, "Pretransform:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(0, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(1, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(2, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(3, outfile);
                        fprintf(outfile, "\n");

                        // Dynamic transform matrix
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        mat = ((vsTransformAttribute *)attribute)->
                            getDynamicTransform();
                        fprintf(outfile, "Dynamic transform:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(0, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(1, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(2, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(3, outfile);
                        fprintf(outfile, "\n");

                        // Post-transform static matrix
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        mat = ((vsTransformAttribute *)attribute)->
                            getPostTransform();
                        fprintf(outfile, "Posttransform:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(0, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(1, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(2, outfile);
                        fprintf(outfile, "\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        mat.printRow(3, outfile);
                        fprintf(outfile, "\n");
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_SWITCH:
                    fprintf(outfile, "SWITCH\n");
                    break;

                case VS_ATTRIBUTE_TYPE_SEQUENCE:
                    fprintf(outfile, "SEQUENCE\n");
                    break;

                case VS_ATTRIBUTE_TYPE_LOD:
                    fprintf(outfile, "LOD\n");

                    // Print the LOD ranges, if we're configured to provide
                    // attribute details
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Iterate over the levels and print the maximum
                        // range for that level
                        for (loop = 0; loop < targetNode->getChildCount(); 
                             loop++)
                        {
                            writeBlanks(outfile, (treeDepth * 2) + 3);
                            fprintf(outfile, "Level %d:  %0.3lf\n",
                                loop, ((vsLODAttribute *)attribute)->
                                getRangeEnd(loop));
                        }
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_LIGHT:
                    attrData = ((vsLightAttribute *)attribute)->isOn();
                    if (attrData)
                        fprintf(outfile, "LIGHT (on)\n");
                    else
                        fprintf(outfile, "LIGHT (off)\n");

                    // Print the light details, if we're configured to
                    // do so
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Scope
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        mode = ((vsLightAttribute *)attribute)->getScope();
                        if (mode == VS_LIGHT_MODE_GLOBAL)
                            fprintf(outfile, "Scope:     GLOBAL\n");
                        else
                            fprintf(outfile, "Scope:     LOCAL\n");

                        // Ambient light
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Ambient:  ");
                        ((vsLightAttribute *)attribute)->
                            getAmbientColor(&r, &g, &b);
                        fprintf(outfile, " %0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Diffuse light
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Diffuse:  ");
                        ((vsLightAttribute *)attribute)->
                            getDiffuseColor(&r, &g, &b);
                        fprintf(outfile, " %0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Specular material (front and back)
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Specular:  ");
                        ((vsLightAttribute *)attribute)->
                            getSpecularColor(&r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Attenuation
                        ((vsLightAttribute *)attribute)->
                            getAttenuationVals(&quadratic, &linear, &constant);
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Attenuation:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Quadratic:  %0.4lf\n", quadratic);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Linear:     %0.4lf\n", linear);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Constant:   %0.4lf\n", constant);

                        // Spotlight configuration
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Spotlight:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        ((vsLightAttribute *)attribute)->
                            getSpotlightDirection(&x, &y, &z);
                        fprintf(outfile, "Direction:    ");
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", x, y, z);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Spread:\n");
                        ((vsLightAttribute *)attribute)->
                            getSpotlightValues(&exponent, &angle);
                        writeBlanks(outfile, (treeDepth * 2) + 7);
                        fprintf(outfile,"Exponent:      %0.2lf\n", exponent);
                        writeBlanks(outfile, (treeDepth * 2) + 7);
                        fprintf(outfile,"Cutoff angle:  %0.2lf\n", angle);
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_FOG:
                    fprintf(outfile, "FOG\n");

                    // Print the fog details, if we're configured to
                    // do so
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Equation type
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Equation type: ");
                        mode = ((vsFogAttribute *)attribute)->
                            getEquationType();
                        switch (mode)
                        {
                            case VS_FOG_EQTYPE_LINEAR:
                                fprintf(outfile, "LINEAR\n");
                                break;
                            case VS_FOG_EQTYPE_EXP:
                                fprintf(outfile, "EXP\n");
                                break;
                            case VS_FOG_EQTYPE_EXP2:
                                fprintf(outfile, "EXP2\n");
                                break;
                        }

                        // Color
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Color: ");
                        ((vsFogAttribute *)attribute)->getColor(&r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b); 

                        // Ranges
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Ranges: ");
                        ((vsFogAttribute *)attribute)->getRanges(&nearFog, &farFog);
                        fprintf(outfile, "near: %0.2lf far: %0.2lf\n", 
                            nearFog, farFog);
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_MATERIAL:
                    fprintf(outfile, "MATERIAL\n");

                    // Print the material details, if we're configured to
                    // do so
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Ambient material (front and back)
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Ambient:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Front:  ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_FRONT, 
                                VS_MATERIAL_COLOR_AMBIENT, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Back:   ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_BACK, 
                                VS_MATERIAL_COLOR_AMBIENT, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Diffuse material (front and back)
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Diffuse:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Front:  ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_FRONT, 
                                VS_MATERIAL_COLOR_DIFFUSE, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Back:   ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_BACK, 
                                VS_MATERIAL_COLOR_DIFFUSE, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Specular material (front and back)
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Specular:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Front:  ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_FRONT, 
                                VS_MATERIAL_COLOR_SPECULAR, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Back:   ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_BACK, 
                                VS_MATERIAL_COLOR_SPECULAR, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Emissive material (front and back)
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Emissive:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Front:  ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_FRONT, 
                                VS_MATERIAL_COLOR_EMISSIVE, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Back:   ");
                        ((vsMaterialAttribute *)attribute)->
                            getColor(VS_MATERIAL_SIDE_BACK, 
                                VS_MATERIAL_COLOR_EMISSIVE, &r, &g, &b);
                        fprintf(outfile, "%0.2lf %0.2lf %0.2lf\n", r, g, b);

                        // Alpha (front and back)
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Alpha:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Front:  ");
                        a = ((vsMaterialAttribute *)attribute)->
                            getAlpha(VS_MATERIAL_SIDE_FRONT);
                        fprintf(outfile, "%0.2lf\n", a);
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Back:   ");
                        a = ((vsMaterialAttribute *)attribute)->
                            getAlpha(VS_MATERIAL_SIDE_BACK);
                        fprintf(outfile, "%0.2lf\n", a);

                        // Front material color-tracking mode
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Color Mode:\n");
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Front:  ");
                        mode = ((vsMaterialAttribute *)attribute)->
                            getColorMode(VS_MATERIAL_SIDE_FRONT);
                        switch (mode)
                        {
                            case VS_MATERIAL_CMODE_AMBIENT:
                                fprintf(outfile, "AMBIENT\n");
                                break;
                            case VS_MATERIAL_CMODE_DIFFUSE:
                                fprintf(outfile, "DIFFUSE\n");
                                break;
                            case VS_MATERIAL_CMODE_SPECULAR:
                                fprintf(outfile, "SPECULAR\n");
                                break;
                            case VS_MATERIAL_CMODE_EMISSIVE:
                                fprintf(outfile, "EMISSIVE\n");
                                break;
                            case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
                                fprintf(outfile, "AMBIENT_DIFFUSE\n");
                                break;
                            case VS_MATERIAL_CMODE_NONE:
                                fprintf(outfile, "NONE\n");
                                break;
                        }

                        // Back material color-tracking mode
                        writeBlanks(outfile, (treeDepth * 2) + 5);
                        fprintf(outfile, "Back:   ");
                        mode = ((vsMaterialAttribute *)attribute)->
                            getColorMode(VS_MATERIAL_SIDE_BACK);
                        switch (mode)
                        {
                            case VS_MATERIAL_CMODE_AMBIENT:
                                fprintf(outfile, "AMBIENT\n");
                                break;
                            case VS_MATERIAL_CMODE_DIFFUSE:
                                fprintf(outfile, "DIFFUSE\n");
                                break;
                            case VS_MATERIAL_CMODE_SPECULAR:
                                fprintf(outfile, "SPECULAR\n");
                                break;
                            case VS_MATERIAL_CMODE_EMISSIVE:
                                fprintf(outfile, "EMISSIVE\n");
                                break;
                            case VS_MATERIAL_CMODE_AMBIENT_DIFFUSE:
                                fprintf(outfile, "AMBIENT_DIFFUSE\n");
                                break;
                            case VS_MATERIAL_CMODE_NONE:
                                fprintf(outfile, "NONE\n");
                                break;
                        }
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_TEXTURE:
                    fprintf(outfile, "TEXTURE\n");

                    // Print out the texture data, if configured to do so
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Texture unit
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Texture Unit:  %d\n",
                            ((vsTextureAttribute *)attribute)->
                            getTextureUnit());
                     
                        // Texture application mode
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Apply Mode:    ");
                        switch (((vsTextureAttribute *)attribute)->
                            getApplyMode())
                        {
                            case VS_TEXTURE_APPLY_DECAL:
                                fprintf(outfile, "DECAL\n");
                                break;
                            case VS_TEXTURE_APPLY_MODULATE:
                                fprintf(outfile, "MODULATE\n");
                                break;
                            case VS_TEXTURE_APPLY_REPLACE:
                                fprintf(outfile, "REPLACE\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }

                        // Texture coordinate generation mode
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "TexGen Mode:   ");
                        switch (((vsTextureAttribute *)attribute)->
                            getGenMode())
                        {
                            case VS_TEXTURE_GEN_OBJECT_LINEAR:
                                fprintf(outfile, "OBJECT_LINEAR\n");
                                break;
                            case VS_TEXTURE_GEN_EYE_LINEAR:
                                fprintf(outfile, "EYE_LINEAR\n");
                                break;
                            case VS_TEXTURE_GEN_SPHERE_MAP:
                                fprintf(outfile, "SPHERE_MAP\n");
                                break;
                            case VS_TEXTURE_GEN_NORMAL_MAP:
                                fprintf(outfile, "NORMAL_MAP\n");
                                break;
                            case VS_TEXTURE_GEN_REFLECTION_MAP:
                                fprintf(outfile, "REFLECTION_MAP\n");
                                break;
                            case VS_TEXTURE_GEN_OFF:
                                fprintf(outfile, "OFF\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }
                    
                        // Magnification filter
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Mag Filter:    ");
                        switch (((vsTextureAttribute *)attribute)->
                            getMagFilter())
                        {
                            case VS_TEXTURE_MAGFILTER_NEAREST:
                                fprintf(outfile, "NEAREST\n");
                                break;
                            case VS_TEXTURE_MAGFILTER_LINEAR:
                                fprintf(outfile, "LINEAR\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }

                        // Minification filter
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Min Filter:    ");
                        switch (((vsTextureAttribute *)attribute)->
                            getMinFilter())
                        {
                            case VS_TEXTURE_MINFILTER_NEAREST:
                                fprintf(outfile, "NEAREST\n");
                                break;
                            case VS_TEXTURE_MINFILTER_LINEAR:
                                fprintf(outfile, "LINEAR\n");
                                break;
                            case VS_TEXTURE_MINFILTER_MIPMAP_NEAREST:
                                fprintf(outfile, "MIPMAP NEAREST\n");
                                break;
                            case VS_TEXTURE_MINFILTER_MIPMAP_LINEAR:
                                fprintf(outfile, "MIPMAP LINEAR\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_TEXTURE_CUBE:
                    fprintf(outfile, "TEXTURE_CUBE\n");

                    // Print out the texture data, if configured to do so
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        // Texture unit
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Texture Unit:  %d\n",
                            ((vsTextureCubeAttribute *)attribute)->
                            getTextureUnit());
                     
                        // Texture application mode
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Apply Mode:    ");
                        switch (((vsTextureCubeAttribute *)attribute)->
                            getApplyMode())
                        {
                            case VS_TEXTURE_APPLY_DECAL:
                                fprintf(outfile, "DECAL\n");
                                break;
                            case VS_TEXTURE_APPLY_MODULATE:
                                fprintf(outfile, "MODULATE\n");
                                break;
                            case VS_TEXTURE_APPLY_REPLACE:
                                fprintf(outfile, "REPLACE\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }

                        // Texture coordinate generation mode
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "TexGen Mode:   ");
                        switch (((vsTextureCubeAttribute *)attribute)->
                            getGenMode())
                        {
                            case VS_TEXTURE_GEN_OBJECT_LINEAR:
                                fprintf(outfile, "OBJECT_LINEAR\n");
                                break;
                            case VS_TEXTURE_GEN_EYE_LINEAR:
                                fprintf(outfile, "EYE_LINEAR\n");
                                break;
                            case VS_TEXTURE_GEN_SPHERE_MAP:
                                fprintf(outfile, "SPHERE_MAP\n");
                                break;
                            case VS_TEXTURE_GEN_NORMAL_MAP:
                                fprintf(outfile, "NORMAL_MAP\n");
                                break;
                            case VS_TEXTURE_GEN_REFLECTION_MAP:
                                fprintf(outfile, "REFLECTION_MAP\n");
                                break;
                            case VS_TEXTURE_GEN_OFF:
                                fprintf(outfile, "OFF\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }

                        // Magnification filter
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Mag Filter:    ");
                        switch (((vsTextureCubeAttribute *)attribute)->
                            getMagFilter())
                        {
                            case VS_TEXTURE_MAGFILTER_NEAREST:
                                fprintf(outfile, "NEAREST\n");
                                break;
                            case VS_TEXTURE_MAGFILTER_LINEAR:
                                fprintf(outfile, "LINEAR\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }

                        // Minification filter
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Min Filter:    ");
                        switch (((vsTextureCubeAttribute *)attribute)->
                            getMinFilter())
                        {
                            case VS_TEXTURE_MINFILTER_NEAREST:
                                fprintf(outfile, "NEAREST\n");
                                break;
                            case VS_TEXTURE_MINFILTER_LINEAR:
                                fprintf(outfile, "LINEAR\n");
                                break;
                            case VS_TEXTURE_MINFILTER_MIPMAP_NEAREST:
                                fprintf(outfile, "MIPMAP NEAREST\n");
                                break;
                            case VS_TEXTURE_MINFILTER_MIPMAP_LINEAR:
                                fprintf(outfile, "MIPMAP LINEAR\n");
                                break;
                            default:
                                fprintf(outfile, "(Unknown Mode)\n");
                                break;
                        }
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_TRANSPARENCY:
                    attrData = ((vsTransparencyAttribute *)attribute)->
                        isEnabled();
                    if (attrData)
                        fprintf(outfile, "TRANSPARENCY (on)\n");
                    else
                        fprintf(outfile, "TRANSPARENCY (off)\n");
                    break;

                case VS_ATTRIBUTE_TYPE_BILLBOARD:
                    fprintf(outfile, "BILLBOARD\n");
                    break;

                case VS_ATTRIBUTE_TYPE_VIEWPOINT:
                    fprintf(outfile, "VIEWPOINT\n");
                    break;

                case VS_ATTRIBUTE_TYPE_BACKFACE:
                    attrData = ((vsBackfaceAttribute *)attribute)->isEnabled();
                    if (attrData)
                        fprintf(outfile, "BACKFACE (on)\n");
                    else
                        fprintf(outfile, "BACKFACE (off)\n");
                    break;

                case VS_ATTRIBUTE_TYPE_DECAL:
                    fprintf(outfile, "DECAL\n");
                    break;

                case VS_ATTRIBUTE_TYPE_SHADING:
                    shadingData = ((vsShadingAttribute *)attribute)->getShading();
                    if (shadingData == VS_SHADING_FLAT)
                        fprintf(outfile, "SHADING (flat)\n");
                    else
                        fprintf(outfile, "SHADING (gouraud)\n");
                    break;

                case VS_ATTRIBUTE_TYPE_SHADER:
                    fprintf(outfile, "SHADER\n");
                    
                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        char *shaderProgram;
                        char *shaderCode, *shaderLine;
                        
                        // Print the vertex program
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Vertex Program:   ");
                       
                        // Get the program from the attribute
                        shaderProgram = ((vsShaderAttribute *)attribute)->
                            getVertexSource();
                        if (shaderProgram == NULL)
                            fprintf(outfile, "None\n");
                        else
                        {
                            // End the current printer line
                            fprintf(outfile, "\n");

                            // Copy the shader code so we can tokenize it
                            shaderCode = 
                                (char *)malloc(strlen(shaderProgram) + 1);
                            strcpy(shaderCode, shaderProgram);

                            // Print out each shader line
                            shaderLine = strtok(shaderCode, "\n");
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            fprintf(outfile, "%s", shaderLine);
                            while((shaderLine = strtok(NULL, "\n")) != NULL)
                            {
                                writeBlanks(outfile, (treeDepth * 2) + 5);
                                fprintf(outfile, "%s\n", shaderLine);
                            }

                            // Release the duplicate string memory
                            free(shaderCode);
                        }

                        // Print the fragment program
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Fragment Program:   ");
                       
                        // Get the program from the attribute
                        shaderProgram = ((vsShaderAttribute *)attribute)->
                            getFragmentSource();
                        if (shaderProgram == NULL)
                            fprintf(outfile, "None\n");
                        else
                        {
                            // End the current printer line
                            fprintf(outfile, "\n");

                            // Copy the shader code so we can tokenize it
                            shaderCode = 
                                (char *)malloc(strlen(shaderProgram) + 1);
                            strcpy(shaderCode, shaderProgram);

                            // Print out each shader line
                            shaderLine = strtok(shaderCode, "\n");
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            fprintf(outfile, "%s", shaderLine);
                            while((shaderLine = strtok(NULL, "\n")) != NULL)
                            {
                                writeBlanks(outfile, (treeDepth * 2) + 5);
                                fprintf(outfile, "%s\n", shaderLine);
                            }

                            // Release the shader code memory
                            free(shaderCode);
                        }
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_GLSL_PROGRAM:
                    fprintf(outfile, "GLSL_PROGRAM\n");

                    if (printerMode & VS_PRINTER_ATTRIBUTE_DETAILS)
                    {
                        int i;
                        int numShaders;
                        vsGLSLShader *shader;
                        int numUniforms;
                        vsGLSLUniform *uniform;
                        
                        // Print the shaders attached to the program
                        numShaders = ((vsGLSLProgramAttribute *)attribute)->
                            getNumShaders();
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Shaders (%d):\n", numShaders);
                        for (i = 0; i < numShaders; i++)
                        {
                            shader = ((vsGLSLProgramAttribute *)attribute)->
                                getShader(i);
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            fprintf(outfile, "%p  type = ", shader);
                            if (shader->getShaderType() == 
                                VS_GLSL_VERTEX_SHADER)
                                fprintf(outfile, "VERTEX_SHADER\n");
                            else if (shader->getShaderType() == 
                                VS_GLSL_FRAGMENT_SHADER)
                                fprintf(outfile, "FRAGMENT_SHADER\n");
                            else
                                fprintf(outfile, "UNDEFINED_SHADER\n");
                        }

                        // Print the uniforms attached to the program
                        numUniforms = ((vsGLSLProgramAttribute *)attribute)->
                            getNumUniforms();
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Uniforms (%d):\n", numUniforms);
                        for (i = 0; i < numUniforms; i++)
                        {
                            uniform = ((vsGLSLProgramAttribute *)attribute)->
                                getUniform(i);
                            writeBlanks(outfile, (treeDepth * 2) + 5);
                            fprintf(outfile, "%p  name = %s  type = ", uniform, 
                                uniform->getName());
                            switch (uniform->getType())
                            {
                                case VS_UNIFORM_FLOAT:
                                    fprintf(outfile, "FLOAT\n");
                                    break;
                                case VS_UNIFORM_FLOAT_VEC2:
                                    fprintf(outfile, "FLOAT_VEC2\n");
                                    break;
                                case VS_UNIFORM_FLOAT_VEC3:
                                    fprintf(outfile, "FLOAT_VEC3\n");
                                    break;
                                case VS_UNIFORM_FLOAT_VEC4:
                                    fprintf(outfile, "FLOAT_VEC4\n");
                                    break;
                                case VS_UNIFORM_INT:
                                    fprintf(outfile, "INT\n");
                                    break;
                                case VS_UNIFORM_INT_VEC2:
                                    fprintf(outfile, "INT_VEC2\n");
                                    break;
                                case VS_UNIFORM_INT_VEC3:
                                    fprintf(outfile, "INT_VEC3\n");
                                    break;
                                case VS_UNIFORM_INT_VEC4:
                                    fprintf(outfile, "INT_VEC4\n");
                                    break;
                                case VS_UNIFORM_BOOL:
                                    fprintf(outfile, "BOOL\n");
                                    break;
                                case VS_UNIFORM_BOOL_VEC2:
                                    fprintf(outfile, "BOOL_VEC2\n");
                                    break;
                                case VS_UNIFORM_BOOL_VEC3:
                                    fprintf(outfile, "BOOL_VEC3\n");
                                    break;
                                case VS_UNIFORM_BOOL_VEC4:
                                    fprintf(outfile, "BOOL_VEC4\n");
                                    break;
                                case VS_UNIFORM_FLOAT_MAT2:
                                    fprintf(outfile, "FLOAT_MAT2\n");
                                    break;
                                case VS_UNIFORM_FLOAT_MAT3:
                                    fprintf(outfile, "FLOAT_MAT3\n");
                                    break;
                                case VS_UNIFORM_FLOAT_MAT4:
                                    fprintf(outfile, "FLOAT_MAT4\n");
                                    break;
                                case VS_UNIFORM_SAMPLER_1D:
                                    fprintf(outfile, "SAMPLER_1D\n");
                                    break;
                                case VS_UNIFORM_SAMPLER_2D:
                                    fprintf(outfile, "SAMPLER_2D\n");
                                    break;
                                case VS_UNIFORM_SAMPLER_3D:
                                    fprintf(outfile, "SAMPLER_3D\n");
                                    break;
                                case VS_UNIFORM_SAMPLER_1D_SHADOW:
                                    fprintf(outfile, "SAMPLER_1D_SHADOW\n");
                                    break;
                                case VS_UNIFORM_SAMPLER_2D_SHADOW:
                                    fprintf(outfile, "SAMPLER_2D_SHADOW\n");
                                    break;
                                default:
                                    fprintf(outfile, "UNDEFINED\n");
                                    break;
                            }
                        }
                    }
                    break;

                case VS_ATTRIBUTE_TYPE_SOUND_SOURCE:
                    fprintf(outfile, "SOUND_SOURCE\n");
                    break;

                case VS_ATTRIBUTE_TYPE_SOUND_LISTENER:
                    fprintf(outfile, "SOUND_LISTENER\n");
                    break;

                case VS_ATTRIBUTE_TYPE_WIREFRAME:
                    attrData = ((vsWireframeAttribute *)attribute)->isEnabled();
                    if (attrData)
                        fprintf(outfile, "WIREFRAME (on)\n");
                    else
                        fprintf(outfile, "WIREFRAME (off)\n");
                    break;

                default:
                    fprintf(outfile, "<unknown type>\n");
                    break;
            }
        }
    }
    
    // If the node has children, take care of them
    if ((targetNode->getNodeType() == VS_NODE_TYPE_COMPONENT) ||
        (targetNode->getNodeType() == VS_NODE_TYPE_SCENE))
    {
        writeBlanks(outfile, treeDepth * 2);
        if (targetNode->getChildCount() == 1)
            fprintf(outfile, "1 child:\n");
        else
            fprintf(outfile, "%d children:\n", targetNode->getChildCount());
        
        // For each child, call this function again
        for (loop = 0; loop < targetNode->getChildCount(); loop++)
        {
            // Print out the node tree location specifier
            countArray[treeDepth] = loop+1;
            writeBlanks(outfile, (treeDepth + 1) * 2);
            for (sloop = 0; sloop <= treeDepth; sloop++)
            {
                if (sloop != 0)
                    fprintf(outfile, ".");
                fprintf(outfile, "%d", countArray[sloop]);
            }
            fprintf(outfile, ") ");

            // Recurse on the child node
            writeScene(targetNode->getChild(loop), outfile, treeDepth+1,
                countArray);
        }
    }
}
