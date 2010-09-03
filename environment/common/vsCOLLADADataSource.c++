
#include <stdio.h>
#include <string.h>
#include "vsCOLLADADataSource.h++"
#include "vsCOLLADAIDREFArray.h++"
#include "vsCOLLADANameArray.h++"
#include "vsCOLLADABoolArray.h++"
#include "vsCOLLADAFloatArray.h++"
#include "vsCOLLADAIntArray.h++"

// ------------------------------------------------------------------------
// Create a COLLADA data source from the given XML document and node.  The
// given node must be a "source" node as specified in the COLLADA schema
// ------------------------------------------------------------------------
vsCOLLADADataSource::vsCOLLADADataSource(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current)
{
    char *refName;
    int valid;
    vsCOLLADAIDREFArray *idrefArray;
    vsCOLLADANameArray *nameArray;
    vsCOLLADABoolArray *boolArray;
    vsCOLLADAFloatArray *floatArray;
    vsCOLLADAIntArray *intArray;
    int i;

    // Start out with a NULL data array and no data or parameters
    dataArray = NULL;
    dataCount = 0;
    paramCount = 0;

    // Make sure this is a "source" node
    if (strcmp(doc->getNodeName(current), "source") != 0)
    {
        // Print an error and bail
        printf("vsCOLLADADataSource::vsCOLLADADataSource:\n");
        printf("    Specified node ('%s') is not a source node!\n",
            doc->getNodeName(current));
    }
    else
    {
        // Process the source node and its children
        processSource(doc, current);

        // Make sure we have a valid data array source
        refName = dataArrayIDRef.getString();
        if ((refName[0] == '#') && (dataArrayID.getString() != NULL) &&
            (strcmp(&refName[1], dataArrayID.getString()) == 0))
        {
            // The data array and reference is in a supported arrangement,
            // figure out what kind of data the final output will be.
            // There is a very large number of combinations of data that
            // could theoretically be specified, so not all arrangements
            // are supported, just the common ones.  First, make sure we
            // have at least one output parameter
            if (paramCount > 0)
            {
                // The final format will be dictated by the format of
                // the first parameter.  There are no heterogeneous
                // formats allowed (recall we're accessing a single array),
                // so all parameter formats must match
                if (paramFormat[0] == VS_CDS_STRING)
                {
                    // We return a single name per index (if there are
                    // multiple parameters, we don't support this)
                    dataFormat = VS_CDS_STRING;
                    dataSize = 1;
                }
                else if (paramFormat[0] == VS_CDS_BOOL)
                {
                    // We return a single bool per index (if there are
                    // multiple parameters, we don't support this)
                    // TODO:  Consider support for boolean vectors
                    dataFormat = VS_CDS_BOOL;
                    dataSize = 1;
                }
                else if (paramFormat[0] == VS_CDS_INT)
                {
                    // We return a single int per index (if there are
                    // multiple parameters, we don't support this)
                    // TODO:  Consider support for integer vectors
                    dataFormat = VS_CDS_INT;
                    dataSize = 1;
                }
                else if (paramFormat[0] == VS_CDS_FLOAT)
                {
                    // Count the number of valid float parameters and 
                    // set the format to the appropriate type
                    valid = 0;
                    for (i = 0; i < paramCount; i++)
                    {
                        if (paramName[i].getLength() > 0)
                            valid++;
                    }

                    // If there is more than one valid parameter, we
                    // return a vector
                    if (valid > 1)
                    {
                        // We're returning a vector
                        dataFormat = VS_CDS_VECTOR;

                        // Clamp the vector size to 4, just in case
                        if (valid > 4)
                            dataSize = 4;
                        else
                            dataSize = valid;
                    }
                    else
                    {
                        // Just return a single float
                        dataFormat = VS_CDS_FLOAT;
                        dataSize = 1;
                    }
                }
                else if (paramFormat[0] == VS_CDS_VECTOR)
                {
                    // We return a vector per input index (multiple vectors
                    // aren't supported)
                    dataFormat = VS_CDS_VECTOR;
                    dataSize = paramSize[0];
                }
                else if (paramFormat[0] == VS_CDS_MATRIX)
                {
                    // We return a 4x4 matrix per input index (multiple
                    // matrices aren't supported)
                    dataFormat = VS_CDS_MATRIX;
                    dataSize = paramSize[0];
                }

                // As a final check, make sure the data array is of the
                // correct type
                idrefArray = NULL;
                nameArray = NULL;
                boolArray = NULL;
                floatArray = NULL;
                intArray = NULL;
                if (dataFormat == VS_CDS_STRING)
                {
                    // Make sure we have either an IDREF or Name array
                    idrefArray = dynamic_cast<vsCOLLADAIDREFArray *>(dataArray);
                    nameArray = dynamic_cast<vsCOLLADANameArray *>(dataArray);
                }
                else if (dataFormat == VS_CDS_BOOL)
                {
                    // Make sure we have a bool array
                    boolArray = dynamic_cast<vsCOLLADABoolArray *>(dataArray);
                }
                else if ((dataFormat == VS_CDS_FLOAT) ||
                         (dataFormat == VS_CDS_VECTOR) ||
                         (dataFormat == VS_CDS_MATRIX))
                {
                    // Make sure we have a float array
                    floatArray = dynamic_cast<vsCOLLADAFloatArray *>(dataArray);
                }
                else if (dataFormat == VS_CDS_INT)
                {
                    // Make sure we have a bool array
                    intArray = dynamic_cast<vsCOLLADAIntArray *>(dataArray);
                }

                // If none of the 5 cast arrays is valid, we have a problem
                if ((idrefArray == NULL) && (nameArray == NULL) &&
                    (boolArray == NULL) && (floatArray == NULL) &&
                    (intArray == NULL))
                {
                    // Print an error
                    printf("vsCOLLADADataSource::vsCOLLADADataSource: "
                        "Source data type does not match access parameters\n");

                    // Flush all the data we have accumulated (we can't
                    // make use of it properly)
                    flushData();
                }
            }
        }
        else
        {
            // This data reference isn't local to this source subdocument
            // and this is currently the only kind of reference we
            // support
            // TODO:  Add support for global document references and
            //        external references

            // Get rid of our data, since we won't be using it
            flushData();
        }
    }
}

// ------------------------------------------------------------------------
// Clean up the data source
// ------------------------------------------------------------------------
vsCOLLADADataSource::~vsCOLLADADataSource()
{
   // Get rid of all of our data
   flushData();
}

// ------------------------------------------------------------------------
// Sets all data access variables to default values and flushes any data
// we have created
// ------------------------------------------------------------------------
void vsCOLLADADataSource::flushData()
{
   // If we have a data array, unreference it
   if (dataArray != NULL)
   {
      vsObject::unrefDelete(dataArray);
      dataArray = NULL;
   }

   // Set data and parameter variables to default values
   dataArrayID.setString(NULL);
   dataArrayIDRef.setString(NULL);
   dataCount = 0;
   paramCount = 0;
}

// ------------------------------------------------------------------------
// Parse the "source" XML node and create the data source
// ------------------------------------------------------------------------
void vsCOLLADADataSource::processSource(atXMLDocument *doc,
                                        atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    char *attr;

    // Get the ID attribute
    attr = doc->getNodeAttribute(current, "id");
    if (attr != NULL)
        dataSourceID.setString(attr);

    // Get the first child of the source node
    child = doc->getNextChildNode(current);

    // Get the array and accessor information for the source
    while (child != NULL)
    {
        if (strcmp(doc->getNodeName(child), "IDREF_array") == 0)
        {
            // Create the IDREF array
            dataArray = new vsCOLLADAIDREFArray(doc, child);
            dataArray->ref();

            // Get the array's identifier
            dataArrayID = dataArray->getID();
        }
        else if (strcmp(doc->getNodeName(child), "Name_array") == 0)
        {
            // Create the name array
            dataArray = new vsCOLLADANameArray(doc, child);
            dataArray->ref();

            // Get the array's identifier
            dataArrayID = dataArray->getID();
        }
        else if (strcmp(doc->getNodeName(child), "bool_array") == 0)
        {
            // Create the bool array
            dataArray = new vsCOLLADABoolArray(doc, child);
            dataArray->ref();

            // Get the array's identifier
            dataArrayID = dataArray->getID();
        }
        else if (strcmp(doc->getNodeName(child), "float_array") == 0)
        {
            // Create the float array
            dataArray = new vsCOLLADAFloatArray(doc, child);
            dataArray->ref();

            // Get the array's identifier
            dataArrayID = dataArray->getID();
        }
        else if (strcmp(doc->getNodeName(child), "int_array") == 0)
        {
            // Create the int array
            dataArray = new vsCOLLADAIntArray(doc, child);
            dataArray->ref();

            // Get the array's identifier
            dataArrayID = dataArray->getID();
        }
        else if (strcmp(doc->getNodeName(child), "technique_common") == 0)
        {
            // Process the common technique node (should just be the data
            // array access scheme in this case)
            processTechniqueCommon(doc, child);
        }

        // Try the next child
        child = doc->getNextSiblingNode(child);
    }
}


// ------------------------------------------------------------------------
// Parse the "technique_common" XML node and create the data accessor and
// parameters
// ------------------------------------------------------------------------
void vsCOLLADADataSource::processTechniqueCommon(atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current)
{
    atXMLDocumentNodePtr child;
    atXMLDocumentNodePtr param;
    char *attr;

    // Get the children of the technique_common node (we're
    // looking for an "accessor" node)
    child = doc->getNextChildNode(current);
    while (child != NULL)
    {
        // See if this is the accessor node
        if (strcmp(doc->getNodeName(child), "accessor") == 0)
        {
            // Get the data parameters from the accessor node, first the
            // ID of the data array
            attr = doc->getNodeAttribute(child, "source");
            if (attr != NULL)
                dataArrayIDRef.setString(attr);

            // Next, the offset (the starting index in the data array)
            attr = doc->getNodeAttribute(child, "count");
            if (attr != NULL)
                dataCount = atoi(attr);

            // Next, the offset (the starting index in the data array)
            attr = doc->getNodeAttribute(child, "offset");
            if (attr != NULL)
                dataOffset = atoi(attr);
            else
                dataOffset = 0;

            // Finally, the stride (the number of array elements to consume
            // at a time)
            attr = doc->getNodeAttribute(child, "stride");
            if (attr != NULL)
                dataStride = atoi(attr);
            else
                dataStride = 1;
            
            // Now, we need to count our parameters
            paramCount = 0;

            // Parameters are children of the accessor node
            param = doc->getNextChildNode(child);
            while (param != NULL)
            {
                // If this is a "param" node, get it's attributes
                if (strcmp(doc->getNodeName(param), "param") == 0)
                {
                    // Try to get the parameter's name (a name is required
                    // if there are multiple parameters for this accessor)
                    attr = doc->getNodeAttribute(param, "name");
                    if (attr != NULL)
                        paramName[paramCount].setString(attr);
                    else
                        paramName[paramCount].setString(NULL);

                    // Now, get the parameter type
                    attr = doc->getNodeAttribute(param, "type");
                    if ((strcmp(attr, "Name") == 0) ||
                        (strcmp(attr, "name") == 0))
                    {
                        // String
                        paramFormat[paramCount] = VS_CDS_STRING;
                        paramSize[paramCount] = 1;
                    }
                    else if (strcmp(attr, "IDREF") == 0)
                    {
                        // String
                        paramFormat[paramCount] = VS_CDS_STRING;
                        paramSize[paramCount] = 1;
                    }
                    else if (strcmp(attr, "bool") == 0)
                    {
                        // Single boolean value
                        paramFormat[paramCount] = VS_CDS_BOOL;
                        paramSize[paramCount] = 1;
                    }
                    else if (strcmp(attr, "int") == 0)
                    {
                        // Single integer
                        paramFormat[paramCount] = VS_CDS_INT;
                        paramSize[paramCount] = 1;
                    }
                    else if ((strcmp(attr, "float") == 0) ||
                             (strcmp(attr, "double") == 0))
                    {
                        // Single float
                        paramFormat[paramCount] = VS_CDS_FLOAT;
                        paramSize[paramCount] = 1;
                    }
                    else if ((strcmp(attr, "float2") == 0) ||
                             (strcmp(attr, "double2") == 0))
                    {
                        // 2-element float vector
                        paramFormat[paramCount] = VS_CDS_VECTOR;
                        paramSize[paramCount] = 2;
                    }
                    else if ((strcmp(attr, "float3") == 0) ||
                             (strcmp(attr, "double3") == 0))
                    {
                        // 3-element float vector
                        paramFormat[paramCount] = VS_CDS_VECTOR;
                        paramSize[paramCount] = 3;
                    }
                    else if ((strcmp(attr, "float4") == 0) ||
                             (strcmp(attr, "double4") == 0))
                    {
                        // 4-element float vector
                        paramFormat[paramCount] = VS_CDS_VECTOR;
                        paramSize[paramCount] = 4;
                    }
                    else if ((strcmp(attr, "float4x4") == 0) ||
                             (strcmp(attr, "double4x4") == 0))
                    {
                        // 4x4 matrix
                        paramFormat[paramCount] = VS_CDS_MATRIX;
                        paramSize[paramCount] = 16;
                    }

                    // Increment the parameter count
                    paramCount++;
                }

                // Try the next node
                param = doc->getNextSiblingNode(param);
            }
        }

        // Try the next node
        child = doc->getNextSiblingNode(child);
    }
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADADataSource::getClassName()
{
    return "vsCOLLADADataSource";
}

// ------------------------------------------------------------------------
// Returns the ID of this source
// ------------------------------------------------------------------------
atString vsCOLLADADataSource::getID()
{
    return dataSourceID.clone();
}

// ------------------------------------------------------------------------
// Returns the data array we're using
// ------------------------------------------------------------------------
vsCOLLADADataArray *vsCOLLADADataSource::getDataArray()
{
    return dataArray;
}

// ------------------------------------------------------------------------
// Return the number of data parameters
// ------------------------------------------------------------------------
int vsCOLLADADataSource::getParamCount()
{
    return paramCount;
}

// ------------------------------------------------------------------------
// Return the name of the given parameter (if any)
// ------------------------------------------------------------------------
atString vsCOLLADADataSource::getParamName(int index)
{
    if ((index < 0) || (index > paramCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getParamName:  Invalid index!\n");

        // Return an empty name
        return atString();
    }
    else
        return paramName[index].clone();
}

// ------------------------------------------------------------------------
// Return the number of data elements provided by this data source
// ------------------------------------------------------------------------
int vsCOLLADADataSource::getDataCount()
{
    return dataCount;
}

// ------------------------------------------------------------------------
// Return the format of data elements provided by this data source
// ------------------------------------------------------------------------
vsCOLLADADataSourceFormat vsCOLLADADataSource::getDataFormat()
{
    return dataFormat;
}

// ------------------------------------------------------------------------
// If this data source provides strings, return the given string
// ------------------------------------------------------------------------
atString vsCOLLADADataSource::getString(int index)
{
    vsCOLLADAIDREFArray *idrefArray;
    vsCOLLADANameArray *nameArray;
    int arrayIndex;

    // Make sure the type is correct
    if (dataFormat != VS_CDS_STRING)
    {
        // Print an error
        printf("vsCOLLADADataSource::getString():  This source doesn't "
            "provide strings!\n");

        // Return an empty string
        return atString();
    }

    // Make sure the index is valid
    if ((index < 0) || (index > dataCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getString():  Invalid index!\n");

        // Return an empty string
        return atString();
    }

    // Compute the index into the array we need
    arrayIndex = dataOffset + (index * dataStride);

    // Return the requested string, first see if it's an IDREF
    idrefArray = dynamic_cast<vsCOLLADAIDREFArray *>(dataArray);
    if (idrefArray != NULL)
        return idrefArray->getData(index);

    // Next, try a name
    nameArray = dynamic_cast<vsCOLLADANameArray *>(dataArray);
    if (nameArray != NULL)
        return nameArray->getData(index);

    // We shouldn't get here (the error checks for this were performed at
    // load time).  To keep the compiler happy, we'll just return an empty
    // string here
    return atString();
}

// ------------------------------------------------------------------------
// If this data source provides bools, return the given bool value
// ------------------------------------------------------------------------
bool vsCOLLADADataSource::getBool(int index)
{
    vsCOLLADABoolArray *boolArray;
    int arrayIndex;

    // Make sure the type is correct
    if (dataFormat != VS_CDS_BOOL)
    {
        // Print an error
        printf("vsCOLLADADataSource::getBool():  This source doesn't "
            "provide boolean values!\n");

        // Return false as a default value
        return false;
    }

    // Make sure the index is valid
    if ((index < 0) || (index > dataCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getBool():  Invalid index!\n");

        // Return false as a default value
        return false;
    }

    // Compute the index into the array we need
    arrayIndex = dataOffset + (index * dataStride);

    // Return the requested bool value
    boolArray = dynamic_cast<vsCOLLADABoolArray *>(dataArray);
    if (boolArray)
        return boolArray->getData(index);
    else
        return false;
}

// ------------------------------------------------------------------------
// If this data source provides ints, return the given integer value
// ------------------------------------------------------------------------
int vsCOLLADADataSource::getInt(int index)
{
    vsCOLLADAIntArray *intArray;
    int arrayIndex;

    // Make sure the type is correct
    if (dataFormat != VS_CDS_INT)
    {
        // Print an error
        printf("vsCOLLADADataSource::getInt():  This source doesn't "
            "provide integer values!\n");

        // Return 0 as a default value
        return 0;
    }

    // Make sure the index is valid
    if ((index < 0) || (index > dataCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getInt():  Invalid index!\n");

        // Return 0 as a default value
        return 0;
    }

    // Compute the index into the array we need
    arrayIndex = dataOffset + (index * dataStride);

    // Return the requested integer value
    intArray = dynamic_cast<vsCOLLADAIntArray *>(dataArray);
    if (intArray)
        return intArray->getData(index);
    else
        return 0;
}

// ------------------------------------------------------------------------
// If this data source provides floats, return the given float value
// ------------------------------------------------------------------------
double vsCOLLADADataSource::getFloat(int index)
{
    vsCOLLADAFloatArray *floatArray;
    int arrayIndex;

    // Make sure the type is correct
    if (dataFormat != VS_CDS_FLOAT)
    {
        // Print an error
        printf("vsCOLLADADataSource::getFloat():  This source doesn't "
            "provide float values!\n");

        // Return 0.0 as a default value
        return 0.0;
    }

    // Make sure the index is valid
    if ((index < 0) || (index > dataCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getFloat():  Invalid index!\n");

        // Return 0.0 as a default value
        return 0.0;
    }

    // Compute the index into the array we need
    arrayIndex = dataOffset + (index * dataStride);

    // Return the requested float value
    floatArray = dynamic_cast<vsCOLLADAFloatArray *>(dataArray);
    if (floatArray)
        return floatArray->getData(index);
    else
        return 0.0;
}

// ------------------------------------------------------------------------
// If this data source provides vectors, return the given vectors value
// ------------------------------------------------------------------------
atVector vsCOLLADADataSource::getVector(int index)
{
    vsCOLLADAFloatArray *floatArray;
    int arrayIndex;
    atVector outVec;
    int vecIndex, paramIndex;

    // Make sure the type is correct
    if (dataFormat != VS_CDS_VECTOR)
    {
        // Print an error
        printf("vsCOLLADADataSource::getVector():  This source doesn't "
            "provide vector values!\n");

        // Return a zero vector as a default value
        return atVector(0.0, 0.0, 0.0, 0.0);
    }

    // Make sure the index is valid
    if ((index < 0) || (index > dataCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getFloat():  Invalid index!\n");

        // Return a zero vector as a default value
        return atVector(0.0, 0.0, 0.0, 0.0);
    }

    // Compute the index into the array we need
    arrayIndex = dataOffset + (index * dataStride);

    // Return the requested vector value
    floatArray = dynamic_cast<vsCOLLADAFloatArray *>(dataArray);
    if (floatArray)
    {
        // Set up the output vector with the data we need
        outVec.clear();
        outVec.setSize(dataSize);

        // Iterate over both the output vector elements and the source
        // parameters
        vecIndex = 0;
        paramIndex = 0;
        while ((vecIndex < dataSize) && (paramIndex < paramCount))
        {
            // See if this parameter is valid
            if (paramName[paramIndex].getLength() > 0)
            {
                // Copy the array element corresponding to this parameter
                // to the current element of the output vector
                outVec[vecIndex] = 
                    floatArray->getData(arrayIndex + paramIndex);

                // Increment the vector index
                vecIndex++;
            }

            // Increment the parameter index
            paramIndex++;
        }

        // Finally, return the output vector
        return outVec;
    }
    else
        return atVector(0.0, 0.0, 0.0, 0.0);
}

// ------------------------------------------------------------------------
// If this data source provides vectors, return the given vectors value
// ------------------------------------------------------------------------
atMatrix vsCOLLADADataSource::getMatrix(int index)
{
    vsCOLLADAFloatArray *floatArray;
    int arrayIndex;
    atMatrix outMat;
    int i, j;

    // Make sure the type is correct
    if (dataFormat != VS_CDS_MATRIX)
    {
        // Print an error
        printf("vsCOLLADADataSource::getMatrix():  This source doesn't "
            "provide matrix values!\n");

        // Return a zero matrix as a default value
        return atMatrix();
    }

    // Make sure the index is valid
    if ((index < 0) || (index > dataCount))
    {
        // Print an error
        printf("vsCOLLADADataSource::getFloat():  Invalid index!\n");

        // Return a zero matrix as a default value
        return atMatrix();
    }

    // Compute the index into the array we need
    arrayIndex = dataOffset + (index * dataStride);

    // Return the requested integer value
    floatArray = dynamic_cast<vsCOLLADAFloatArray *>(dataArray);
    if (floatArray)
    {
        // Starting at the array index we computed, iterate over the array to
        // collect the output matrix elements
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                outMat[i][j] = floatArray->getData(arrayIndex + i*4 + j);

        // Finally, return the output matrix
        return outMat;
    }
    else
        return atMatrix();
}
