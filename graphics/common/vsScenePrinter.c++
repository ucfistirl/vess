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
void vsScenePrinter::writeGeometryList(vsGeometry *geometry,
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
    vsAttribute *attribute;
    int loop, sloop;
    int size, length;
    vsMatrix mat;
    double r, g, b;
    int mode;
    int geoType, geoCount;
    int shadingData;
    int geoBinding;
    vsVector geoVec;
    bool attrData;
    
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
        // Cast to geometry
        geometry = (vsGeometry *)targetNode;

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
            writeGeometryList(geometry, VS_GEOMETRY_VERTEX_COORDS, treeDepth,
                outfile);

            // If primitive type is variable length, print the lengths array
            if ((printerMode & VS_PRINTER_GEOMETRY_LISTS) &&
                ((geoType == VS_GEOMETRY_TYPE_LINE_STRIPS) ||
                 (geoType == VS_GEOMETRY_TYPE_LINE_LOOPS) ||
                 (geoType == VS_GEOMETRY_TYPE_TRI_STRIPS) ||
                 (geoType == VS_GEOMETRY_TYPE_TRI_FANS) ||
                 (geoType == VS_GEOMETRY_TYPE_TRI_FANS) ||
                 (geoType == VS_GEOMETRY_TYPE_TRI_FANS)))
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
            for (loop = 0; loop < 3; loop++)
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
                        fprintf(outfile, "TEXCOORDS (%d): ",
                            geometry->
                                getDataListSize(VS_GEOMETRY_TEXTURE_COORDS));
                        geoType = VS_GEOMETRY_TEXTURE_COORDS;
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
                            writeGeometryList(geometry, VS_GEOMETRY_NORMALS,
                                treeDepth, outfile);
                            break;

                        case 1:
                            // Print color data
                            writeGeometryList(geometry, VS_GEOMETRY_COLORS, 
                                treeDepth, outfile);
                            break;

                        case 2:
                            // Print texture coordinate data
                            writeGeometryList(geometry, 
                                VS_GEOMETRY_TEXTURE_COORDS, treeDepth, 
                                outfile);
                            break;
                    }
                }
            }
        }
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
                    break;

                case VS_ATTRIBUTE_TYPE_LIGHT:
                    fprintf(outfile, "LIGHT\n");
                    break;

                case VS_ATTRIBUTE_TYPE_FOG:
                    fprintf(outfile, "FOG\n");
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
                        // Texture application mode
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Apply Mode: ");
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

                        // Magnification filter
                        writeBlanks(outfile, (treeDepth * 2) + 3);
                        fprintf(outfile, "Mag Filter: ");
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
                        fprintf(outfile, "Min Filter: ");
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

                case VS_ATTRIBUTE_TYPE_SOUND_SOURCE:
                    fprintf(outfile, "SOUND_SOURCE\n");
                    break;

                case VS_ATTRIBUTE_TYPE_SOUND_LISTENER:
                    fprintf(outfile, "SOUND_LISTENER\n");
                    break;

                case VS_ATTRIBUTE_TYPE_WIREFRAME:
                    fprintf(outfile, "WIREFRAME\n");
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
