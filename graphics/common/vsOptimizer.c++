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
//    VESS Module:  vsOptimizer.c++
//
//    Description:  Class for reorganizing a VESS scene graph in order to
//                  increase rendering speed
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsOptimizer.h++"
#include "vsStateAttribute.h++"
#include "vsDecalAttribute.h++"
#include "vsLODAttribute.h++"

// ------------------------------------------------------------------------
// Constructor - Turns all optimizations on
// ------------------------------------------------------------------------
vsOptimizer::vsOptimizer()
{
    // Default optimizations to perform are all of them
    passMask = VS_OPTIMIZER_ALL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsOptimizer::~vsOptimizer()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsOptimizer::getClassName()
{
    return "vsOptimizer";
}

// ------------------------------------------------------------------------
// Start optimizations on the scene rooted at the given node
// ------------------------------------------------------------------------
void vsOptimizer::optimize(vsNode *rootNode)
{
    // Call the recursive optimization function, starting at the
    // given scene root node
    optimizeNode(rootNode);
}

// ------------------------------------------------------------------------
// Sets a bit mask indicating which optimizations are to be performed
// ------------------------------------------------------------------------
void vsOptimizer::setOptimizations(int mask)
{
    passMask = mask;
}

// ------------------------------------------------------------------------
// Returns a bit mask indicating which optimizations are to be performed
// ------------------------------------------------------------------------
int vsOptimizer::getOptimizations()
{
    return passMask;
}

// ------------------------------------------------------------------------
// Recursive function - runs optimizations on the given node, and calls
// this function again for each child of the given node
// ------------------------------------------------------------------------
void vsOptimizer::optimizeNode(vsNode *node)
{
    int loop;
    vsGeometry *geometryNode;
    vsComponent *componentNode;

    // Select optimizations based on node type. We don't do any sort of
    // optimization on vsScenes or vsDynamicGeometries.
    if (node->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        // Geometry type cast, for convenience
        geometryNode = (vsGeometry *)node;

        // Data compression optimization (colors and normals)
        if (passMask & VS_OPTIMIZER_CONDENSE_COLORS)
            condenseGeoData(geometryNode, VS_GEOMETRY_COLORS);
        if (passMask & VS_OPTIMIZER_CONDENSE_NORMALS)
            condenseGeoData(geometryNode, VS_GEOMETRY_NORMALS);
    }
    else if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        // Component type cast, for convenience
        componentNode = (vsComponent *)node;

        // Clean tree optimization
        if (passMask & VS_OPTIMIZER_CLEAN_TREE)
            cleanChildren(componentNode);

        // Merge decals optimization
        if (passMask & VS_OPTIMIZER_MERGE_DECALS)
            mergeDecals(componentNode);

        if (passMask & VS_OPTIMIZER_MERGE_LODS)
            mergeLODs(componentNode);

        // Recurse on the child component
        for (loop = 0; loop < componentNode->getChildCount(); loop++)
            optimizeNode(componentNode->getChild(loop));

        // Clean tree optimization (again, in case merging decals
        // or the child traversal consolidated some nodes)
        if (passMask & VS_OPTIMIZER_CLEAN_TREE)
            cleanChildren(componentNode);

        // State attribute promotion optimization
        if (passMask & VS_OPTIMIZER_PROMOTE_ATTRIBUTES)
        {
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_BACKFACE);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_FOG);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_MATERIAL);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_SHADING);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_TEXTURE);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_TRANSPARENCY);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_WIREFRAME);
        }

        // Geometry merging optimization
        if (passMask & VS_OPTIMIZER_MERGE_GEOMETRY)
            mergeGeometry(componentNode);

        // Clean tree optimization (yet again, in case merging geometry
        // made some components have only one child)
        if (passMask & VS_OPTIMIZER_CLEAN_TREE)
            cleanChildren(componentNode);

        // Priority of attributes is (texutre, material, shading).
        // Sort in reverse order so that the highest priority sort
        // (texture) gets performed last and so has the most effect.
        if (passMask & VS_OPTIMIZER_SORT_CHILDREN)
        {
            sortByAttribute(componentNode, VS_ATTRIBUTE_TYPE_SHADING);
            sortByAttribute(componentNode, VS_ATTRIBUTE_TYPE_MATERIAL);
            sortByAttribute(componentNode, VS_ATTRIBUTE_TYPE_TEXTURE);
        }
    }
    
}

// ------------------------------------------------------------------------
// For each child of this component, check to see if that child is also
// a component, and if so, if that component has zero or one children of
// its own. If so, then that component isn't really needed and is a
// candidate to be removed.
// ------------------------------------------------------------------------
void vsOptimizer::cleanChildren(vsComponent *componentNode)
{
    vsComponent *targetComponent;
    vsNode *childNode;
    int loop;
    
    // Loop through all the node's children
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        // Get the node's loop'th child
        childNode = componentNode->getChild(loop);

        // Check if this child is a vsComponent
        if (childNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        {
            // Type cast
            targetComponent = (vsComponent *)childNode;

            // If the component has more than one child, has any attributes,
            // or has a name, then we don't want to remove it.
            if (targetComponent->getChildCount() > 1)
                continue;
            if (targetComponent->getAttributeCount() > 0)
                continue;
            if (strlen(targetComponent->getName()) > 0)
                continue;

            // Additionally, if the component has no children, then removing
            // this component would change the ordering of the parent's
            // children. If the parent has a GROUPING category attribute, then
            // this change is unacceptable.
            if ( (targetComponent->getChildCount() == 0) &&
                 (componentNode->getCategoryAttribute(
                    VS_ATTRIBUTE_CATEGORY_GROUPING, 0)) )
                continue;

            // If we've made it this far, then it should be okay to remove
            // the component
            zapComponent(targetComponent);
        }
    }
}

// ------------------------------------------------------------------------
// Remove this component from the scene, assigning the child of this
// component to each of the component's parents instead. Assumes that the
// component to be removed has no more than one child. Also deletes the
// component when finished.
// ------------------------------------------------------------------------
void vsOptimizer::zapComponent(vsComponent *targetComponent)
{
    vsNode *parentNode;
    vsNode *childNode;
    
    // Check how many children the component has
    if (targetComponent->getChildCount() == 0)
    {
        // No children; simply remove this component from each parent
        while (targetComponent->getParentCount() > 0)
        {
            parentNode = targetComponent->getParent(0);
            parentNode->removeChild(targetComponent);
        }
    }
    else
    {
        // One child; assign the child of this component to each of the
        // component's parents, and then delete the component
        childNode = targetComponent->getChild(0);
        targetComponent->removeChild(childNode);

        // For each parent of the component, replace this component with
        // this component's child
        while (targetComponent->getParentCount() > 0)
        {
            parentNode = targetComponent->getParent(0);
            parentNode->replaceChild(targetComponent, childNode);
        }
    }
    
    // With all links to this component gone, it should be safe to delete it
    vsObject::checkDelete(targetComponent);
}

// ------------------------------------------------------------------------
// Attempts to merge geometry under components with decal attributes that
// are children of this component
// ------------------------------------------------------------------------
void vsOptimizer::mergeDecals(vsComponent *componentNode)
{
    int loop;
    int count;
    vsNode *childNode, *decalChild;
    vsComponent *decalNode, *childComponent;
    
    // If there's a grouping category attribute on this component, then it's
    // not safe to rearrange the component's children, as would be needed
    // for a decal merge. Abort.
    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;
    
    // Count the number of children that have decal attributes; if there are
    // two or more, then this operation is worth the effort.
    count = 0;
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        // Get the loop'th child of this component
        childNode = componentNode->getChild(loop);

        // If the child has a vsDecalAttribute, that attribute is its
        // only attribute, and the child only parent is this component,
        // then the child is a candidate for merging
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_DECAL, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
            count++;
    }
    
    // If there aren't enough decalled children to warrant a merge,
    // then abort.
    if (count < 2)
        return;

    // Create a new decal component
    decalNode = new vsComponent;
    decalNode->addAttribute(new vsDecalAttribute);

    // Attempt to add each decal under this node to the new 
    // component instead
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        // Get the loop'th child of the component
        childNode = componentNode->getChild(loop);

        // Check if the child has a vsDecalAttribute, that attribute is its
        // only attribute, and the child only parent is the component
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_DECAL, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
        {
            // Candidate for merging: Transfer the children of this decal
            // component into the new component.
            childComponent = (vsComponent *)childNode;

            // First, make sure there are at least as many children of
            // the new component as there are of the target component
            while (decalNode->getChildCount() < childComponent->getChildCount())
                decalNode->addChild(new vsComponent);

            // Then, start moving the children of the target component over
            // to the new component
            count = 0;
            while (childComponent->getChildCount() > 0)
            {
                // Get the first child of the target component
                decalChild = childComponent->getChild(0);

                // Move that child from the target component to the
                // corresponding group on the new component
                childComponent->removeChild(decalChild);
                ((vsComponent *)(decalNode->getChild(count)))->
                    addChild(decalChild);

                // Move to the next group
                count++;
            }

            // Finally, remove the depleted decal component from the parent
            // component and reset the loop counter
            componentNode->removeChild(childNode);
            loop--;

            // The target node is now unneeded; delete it.
            vsObject::checkDelete(childNode);
        }
    }
    
    // Last step: add the new merged decal component back into the
    // parent component
    componentNode->addChild(decalNode);
}

// ------------------------------------------------------------------------
// Attempts to merge geometry under components with LOD attributes that
// are children of this component
// ------------------------------------------------------------------------
void vsOptimizer::mergeLODs(vsComponent *componentNode)
{
    int loop, sloop;
    int count, idx;
    bool flag;
    vsNode *childNode, *lodChild, *newChild;
    double *rangeList;
    int rangeListSize;
    int lodChildCount;
    vsLODAttribute *lodAttr, *newLODAttr;
    double tempDouble, midpoint;
    vsComponent *newLODComponent;
    double rangeStart, rangeEnd;

    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;

    // Determine how many children of this component have LOD attributes.
    // If there aren't at least two, then there's no work to do.
    // (While we're at it, compute the number of children of each eligible
    // node as well, so we know how many range values we will need.)
    count = 0;
    lodChildCount = 0;
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        childNode = componentNode->getChild(loop);
        // Check for the presence of a vsLODAttribute, as well as no
        // other attributes and only one parent. (The last two shouldn't
        // really be needed, but if they were otherwise they would make
        // the merging process too complicated.)
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_LOD, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
        {
            count++;
            lodChildCount += childNode->getChildCount();
        }
    }
    if (count < 2)
        return;

    // Create the ranges list and fill it with values from the LODs. The
    // list starts with a zero value to act as a convenient lower bound.
    rangeList = (double *)(malloc(sizeof(double) * lodChildCount));
    rangeListSize = 0;
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        childNode = componentNode->getChild(loop);
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_LOD, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
        {
            lodAttr = (vsLODAttribute *)
                (childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_LOD, 0));
            for (sloop = 0; sloop < childNode->getChildCount(); sloop++)
                rangeList[rangeListSize++] = lodAttr->getRangeEnd(sloop);
        }
    }
    
    // Sort the list of ranges, removing duplicates at the same time
    flag = true;
    while (flag)
    {
        flag = false;
        
        for (loop = 0; loop < rangeListSize-1; loop++)
        {
            if (VS_EQUAL(rangeList[loop], rangeList[loop+1]))
            {
                // Delete one of the equal range values by copying
                // the last range value over it; the sorting process
                // will take care of putting the ranges back into order.
                rangeList[loop] = rangeList[rangeListSize - 1];
                rangeListSize--;
                flag = true;
            }
            else if (rangeList[loop] > rangeList[loop+1])
            {
                tempDouble = rangeList[loop];
                rangeList[loop] = rangeList[loop+1];
                rangeList[loop+1] = tempDouble;
                flag = true;
            }
        }
    }
    
    // Create a new component to hold an LOD attribute, and create a number
    // of children on it equal to the number of range values
    newLODComponent = new vsComponent();
    for (loop = 0; loop < rangeListSize; loop++)
        newLODComponent->addChild(new vsComponent());
    newLODAttr = new vsLODAttribute();
    newLODComponent->addAttribute(newLODAttr);
    for (loop = 0; loop < rangeListSize; loop++)
        newLODAttr->setRangeEnd(loop, rangeList[loop]);

    // Free up the memory allocated for the rangelist
    free(rangeList);
    
    // For each LOD on componentNode, remove all of the LOD's children
    // and add them to the children of the new LOD component, taking the
    // range values into account
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        childNode = componentNode->getChild(loop);
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_LOD, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
        {
            lodAttr = (vsLODAttribute *)
                (childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_LOD, 0));

            // Run through all of the children of the LOD component in
            // reverse order, removing them and adding them to the new
            // tree as we go.
            while (childNode->getChildCount() > 0)
            {
                // Get the child and its ranges
                idx = childNode->getChildCount() - 1;
                lodChild = childNode->getChild(idx);
                if (idx == 0)
                    rangeStart = 0.0;
                else
                    rangeStart = lodAttr->getRangeEnd(idx - 1);
                rangeEnd = lodAttr->getRangeEnd(idx);

                childNode->removeChild(lodChild);
                
                // Add the LOD child to the new tree, as many times
                // as dictated by its range values
                for (sloop = 0; sloop < newLODComponent->getChildCount();
                    sloop++)
                {
                    newChild = newLODComponent->getChild(sloop);
                
                    // Calculate the midpoint of the new child's range
                    if (sloop == 0)
                        midpoint = newLODAttr->getRangeEnd(0) / 2.0;
                    else
                        midpoint = (newLODAttr->getRangeEnd(sloop - 1) +
                            newLODAttr->getRangeEnd(sloop)) / 2.0;
                
                    // If the new child's midpoint is in the LOD child's
                    // range, then add the LOD child to the new child
                    if ((rangeStart <= midpoint) && (midpoint <= rangeEnd))
                    {
                        // Check if the child can't have any more parents;
                        // if so, add a clone instead.
                        if ((lodChild->getNodeType() == VS_NODE_TYPE_COMPONENT)
                            && (lodChild->getParentCount() > 0))
                            newChild->addChild(lodChild->cloneTree());
                        else
                            newChild->addChild(lodChild);
                    }
                }
            }
            
            // Finally, remove the depleted LOD component from the old
            // scene and discard it.
            componentNode->removeChild(childNode);
            vsObject::checkDelete(childNode);
        }
    }

    // Last step is to add the new tree as a child of componentNode
    componentNode->addChild(newLODComponent);
}

// ------------------------------------------------------------------------
// Attempts to merge multiple geometry objects that are children of this
// component
// ------------------------------------------------------------------------
void vsOptimizer::mergeGeometry(vsComponent *componentNode)
{
    int loop, sloop;
    vsNode *firstNode, *secondNode;
    vsGeometry *firstGeo, *secondGeo;
    vsNode *parent;

    // If there's a grouping category attribute on this component, then it's
    // not safe to rearrange the component's children, as would be needed
    // for a geometry merge. Abort.
    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;

    // Compare each pair of children for merge compatability
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
        for (sloop = loop+1; sloop < componentNode->getChildCount(); sloop++)
        {
            // Pick two children of the component
            firstNode = componentNode->getChild(loop);
            secondNode = componentNode->getChild(sloop);

            // Two children are compatable if they are both geometry
            // nodes and they contain the same type of geometry
            if ((firstNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) &&
                (secondNode->getNodeType() == VS_NODE_TYPE_GEOMETRY))
            {
                // Type cast
                firstGeo = (vsGeometry *)firstNode;
                secondGeo = (vsGeometry *)secondNode;

                // Determine if the two vsGeometry objects are compatible,
                // and merge them if they are
                if (isSimilarGeometry(firstGeo, secondGeo))
                {
                    // Remove the second geometry object from its parents,
                    // and add its geometry to the first geometry object
                    while (secondGeo->getParentCount() > 0)
                    {
                        parent = secondGeo->getParent(0);
                        parent->removeChild(secondGeo);
                    }
                    addGeometry(firstGeo, secondGeo);

                    // The second geometry object is now unneeded; get rid
                    // of it.
                    vsObject::checkDelete(secondGeo);

                    // Back up one child, so that we don't skip over the
                    // child that was just moved into the spot that
                    // the second geometry object vacated.
                    sloop--;
                }
            }
        }
}

// ------------------------------------------------------------------------
// Compares two geometry objects for similarity; used by the geometry merge
// routine to determine when two geometries can be merged. If the two
// geometry pointers are the same, false is returned.
// ------------------------------------------------------------------------
bool vsOptimizer::isSimilarGeometry(vsGeometry *firstGeo, vsGeometry *secondGeo)
{
    int firstVal, secondVal;
    vsVector firstVec, secondVec;
    int loop, firstType;
    vsAttribute *firstAttr, *secondAttr;
    vsStateAttribute *stateAttr;
    int sloop;
    bool matchFlag;

    // If somehow they're the same geometry object, then we don't want the
    // caller to get the bright idea of trying to merge the object with
    // itself. Return false in this case.
    if (firstGeo == secondGeo)
        return false;

    // If either geometry node is named, return false
    if (strlen(firstGeo->getName()) > 0)
        return false;
    if (strlen(secondGeo->getName()) > 0)
        return false;

    // Compare primitive types
    firstVal = firstGeo->getPrimitiveType();
    secondVal = secondGeo->getPrimitiveType();
    if (firstVal != secondVal)
        return false;

    // Compare attribute counts
    firstVal = firstGeo->getAttributeCount();
    secondVal = secondGeo->getAttributeCount();
    if (firstVal != secondVal)
        return false;

    // Check to make sure that both geometry nodes have the same parent(s)
    if (firstGeo->getParentCount() != secondGeo->getParentCount())
        return false;
    for (loop = 0; loop < firstGeo->getParentCount(); loop++)
    {
        // Start with no match
        matchFlag = false;

        // Search the second parent list for a component with the same
        // address as the parent from the first parent list
        for (sloop = 0; sloop < secondGeo->getParentCount(); sloop++)
            if (firstGeo->getParent(loop) == secondGeo->getParent(sloop))
            {
                // Mark that we've found a match
                matchFlag = true;
                break;
            }
        
        // If there wasn't a match, then the geometeries aren't compatible
        if (!matchFlag)
            return false;
    }

    // Compare the two geometries' attributes
    for (loop = 0; loop < firstVal; loop++)
    {
        // Get the loop'th attribute from the first geometry
        firstAttr = firstGeo->getAttribute(loop);

        // Only graphics state attributes can be merged in this way
        if (firstAttr->getAttributeCategory() != VS_ATTRIBUTE_CATEGORY_STATE)
            return false;

        // For each attribute in the first geometry, the second geometry
        // must have a corresponding attribute of the same type
        firstType = firstAttr->getAttributeType();
        secondAttr = secondGeo->getTypedAttribute(firstType, 0);
        if (!secondAttr)
            return false;

        // Consult the state attribute's isEquivalent() function to
        // determine if the two attributes are the same
        stateAttr = (vsStateAttribute *)firstAttr;
        if (!(stateAttr->isEquivalent(secondAttr)))
            return false;
    }

    // * Compare geometric data bindings
    // Normal binding
    firstVal = firstGeo->getBinding(VS_GEOMETRY_NORMALS);
    secondVal = secondGeo->getBinding(VS_GEOMETRY_NORMALS);
    if (firstVal != secondVal)
        return false;
    if (firstVal == VS_GEOMETRY_BIND_OVERALL)
    {
        // If normals have overall binding, then we need to verify that
        // the actual normal data matches up
        firstVec = firstGeo->getData(VS_GEOMETRY_NORMALS, 0);
        secondVec = secondGeo->getData(VS_GEOMETRY_NORMALS, 0);

        // Compare for equality
        if (!(firstVec == secondVec))
            return false;
    }

    // Color binding
    firstVal = firstGeo->getBinding(VS_GEOMETRY_COLORS);
    secondVal = secondGeo->getBinding(VS_GEOMETRY_COLORS);
    if (firstVal != secondVal)
        return false;
    if (firstVal == VS_GEOMETRY_BIND_OVERALL)
    {
        // If colors have overall binding, then we need to verify that
        // the actual color data matches up
        firstVec = firstGeo->getData(VS_GEOMETRY_COLORS, 0);
        secondVec = secondGeo->getData(VS_GEOMETRY_COLORS, 0);

        // Compare for equality
        if (!(firstVec == secondVec))
            return false;
    }

    // Texture coordinate binding
    firstVal = firstGeo->getBinding(VS_GEOMETRY_TEXTURE_COORDS);
    secondVal = secondGeo->getBinding(VS_GEOMETRY_TEXTURE_COORDS);
    if (firstVal != secondVal)
        return false;

    // If we've gotten this far, then the geometries should be compatible
    return true;
}

// ------------------------------------------------------------------------
// Adds the geometry within the second geometry object to the first one.
// The second geometry object is unchanged.
// ------------------------------------------------------------------------
void vsOptimizer::addGeometry(vsGeometry *destGeo, vsGeometry *srcGeo)
{
    int loop;
    int srcPrimCount, destPrimCount;
    int srcVertCount, destVertCount;
    vsVector tempData;
    int tempLength;
    
    // Don't trust the vertex data list size values; determine the actual
    // (used) vertex counts by summing together the lengths of the primitives
    // of each geometry.
    srcPrimCount = srcGeo->getPrimitiveCount();
    destPrimCount = destGeo->getPrimitiveCount();
    srcVertCount = 0;
    for (loop = 0; loop < srcPrimCount; loop++)
        srcVertCount += srcGeo->getPrimitiveLength(loop);
    destVertCount = 0;
    for (loop = 0; loop < destPrimCount; loop++)
        destVertCount += destGeo->getPrimitiveLength(loop);

    // * Copy vertex coordinates
    // Set the vertex list size to the sum of both geometry's
    // vertex counts
    destGeo->setDataListSize(VS_GEOMETRY_VERTEX_COORDS,
        srcVertCount + destVertCount);

    // Copy the vertex coordinates
    for (loop = 0; loop < srcVertCount; loop++)
    {
        tempData = srcGeo->getData(VS_GEOMETRY_VERTEX_COORDS, loop);
        destGeo->setData(VS_GEOMETRY_VERTEX_COORDS, loop + destVertCount,
            tempData);
    }
    
    // * Copy normals
    // Check the binding value
    if (destGeo->getBinding(VS_GEOMETRY_NORMALS) ==
        VS_GEOMETRY_BIND_PER_PRIMITIVE)
    {
        // Per-primitive normal binding

        // Set the normal list size to the sum of both geometry's
        // primitive counts
        destGeo->setDataListSize(VS_GEOMETRY_NORMALS,
            srcPrimCount + destPrimCount);

        // Copy the primitive normals
        for (loop = 0; loop < srcPrimCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_NORMALS, loop);
            destGeo->setData(VS_GEOMETRY_NORMALS, loop + destPrimCount,
                tempData);
        }
    }
    else if (destGeo->getBinding(VS_GEOMETRY_NORMALS) ==
        VS_GEOMETRY_BIND_PER_VERTEX)
    {
        // Per-vertex normal binding

        // Set the normal list size to the sum of both geometry's
        // vertex counts
        destGeo->setDataListSize(VS_GEOMETRY_NORMALS,
            srcVertCount + destVertCount);

        // Copy the vertex normals
        for (loop = 0; loop < srcVertCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_NORMALS, loop);
            destGeo->setData(VS_GEOMETRY_NORMALS, loop + destVertCount,
                tempData);
        }
    }
    
    // * Copy colors
    // Check the binding value
    if (destGeo->getBinding(VS_GEOMETRY_COLORS) ==
        VS_GEOMETRY_BIND_PER_PRIMITIVE)
    {
        // Per-primitive color binding

        // Set the color list size to the sum of both geometry's
        // primitive counts
        destGeo->setDataListSize(VS_GEOMETRY_COLORS,
            srcPrimCount + destPrimCount);

        // Copy the primitive colors
        for (loop = 0; loop < srcPrimCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_COLORS, loop);
            destGeo->setData(VS_GEOMETRY_COLORS, loop + destPrimCount,
                tempData);
        }
    }
    else if (destGeo->getBinding(VS_GEOMETRY_COLORS) ==
        VS_GEOMETRY_BIND_PER_VERTEX)
    {
        // Per-vertex color binding

        // Set the color list size to the sum of both geometry's
        // vertex counts
        destGeo->setDataListSize(VS_GEOMETRY_COLORS,
            srcVertCount + destVertCount);

        // Copy the vertex colors
        for (loop = 0; loop < srcVertCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_COLORS, loop);
            destGeo->setData(VS_GEOMETRY_COLORS, loop + destVertCount,
                tempData);
        }
    }
    
    // * Copy texture coordinates
    // Check the binding value (can only be per-vertex or off)
    if (destGeo->getBinding(VS_GEOMETRY_TEXTURE_COORDS) ==
        VS_GEOMETRY_BIND_PER_VERTEX)
    {
        // Per-vertex texture coordinate binding

        // Set the texture coordinate list size to the sum of both geometry's
        // vertex counts
        destGeo->setDataListSize(VS_GEOMETRY_TEXTURE_COORDS,
            srcVertCount + destVertCount);

        // Copy the vertex texture coordinates
        for (loop = 0; loop < srcVertCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_TEXTURE_COORDS, loop);
            destGeo->setData(VS_GEOMETRY_TEXTURE_COORDS, loop + destVertCount,
                tempData);
        }
    }
    
    // * Copy primitive counts/lengths
    // Set the primitive list size to the sum of both geometry's
    // primitive counts
    destGeo->setPrimitiveCount(destPrimCount + srcPrimCount);
    // Only need to copy the actual primitive length data if the type is
    // not one of the fixed-length types
    if ((destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_POINTS) &&
        (destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_LINES) &&
        (destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_TRIS) &&
        (destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_QUADS))
    {
        // Copy the primitive lengths
        for (loop = 0; loop < srcPrimCount; loop++)
        {
            tempLength = srcGeo->getPrimitiveLength(loop);
            destGeo->setPrimitiveLength(loop + destPrimCount, tempLength);
        }
    }

}

// ------------------------------------------------------------------------
// Goes through the specified data list (which must be colors or normals)
// and determines of all of the entries have the same data. If they do,
// then all but one are removed, and the binding for that data is set
// to OVERALL.
// ------------------------------------------------------------------------
void vsOptimizer::condenseGeoData(vsGeometry *geometry, int whichData)
{
    vsVector keyValue;
    int dataListSize;
    bool allSame;
    int loop;

    // 'whichData' must be COLORS or NORMALS; vertices (and texture
    // vertices) should never be condensed in this way.
    if ((whichData != VS_GEOMETRY_COLORS) && (whichData != VS_GEOMETRY_NORMALS))
    {
        printf("vsOptimizer::condenseGeoData: Bad data type\n");
        return;
    }

    dataListSize = geometry->getDataListSize(whichData);

    // If there's zero or one entry in the data list, then there's no
    // work to do on this geometry
    if (dataListSize < 2)
        return;

    keyValue = geometry->getData(whichData, 0);

    // Compare every entry in the data list for equality
    allSame = true;
    for (loop = 1; loop < dataListSize; loop++)
        if (!(keyValue == geometry->getData(whichData, loop)))
        {
            allSame = false;
            break;
        }

    // If all of the entries in the list are the same, then we can compress
    // the list by setting it to OVERALL binding
    if (allSame)
    {
        geometry->setDataListSize(whichData, 1);
        geometry->setData(whichData, 0, keyValue);
        geometry->setBinding(whichData, VS_GEOMETRY_BIND_OVERALL);
    }
}

// ------------------------------------------------------------------------
// Attempts to 'promote' attributes by determining which attribute of the
// given type is most prominently used among the children of the given
// component; this attribute is added to the component. Then those same
// attributes are removed from the children nodes if they match the parent
// component's attribute.
// ------------------------------------------------------------------------
void vsOptimizer::optimizeAttributes(vsComponent *componentNode,
    int attributeType)
{
    int loop, sloop;
    vsNode *childNode;
    vsNode *parentNode;
    vsAttribute *attrArray[1000];
    int attrHitCounts[1000];
    int attrCount;
    bool emptyFlag, matchFlag;
    vsAttribute *parentAttr;
    vsStateAttribute *childAttr;
    
    // First, if the parent node does not already have an attribute of the
    // indicated type, attempt to create one by examining the child
    // nodes' attributes.
    if (!(componentNode->getTypedAttribute(attributeType, 0)))
    {
        // * Create a list of the frequencies of the various attributes
        // of the specified type. Two attributes that are similar are
        // considered to be the same attribute with a frequency of two
        // (or more if more are similar). The attribute that is the
        // most frequent is the one to get promoted.
    
        // Initialize attributes found and NULLs found to zero
        attrCount = 0;
        emptyFlag = false;

        // Check each child for an attribute of the given type
        for (loop = 0; loop < componentNode->getChildCount(); loop++)
        {
            // Examine the loop'th child
            childNode = componentNode->getChild(loop);
            childAttr = (vsStateAttribute *)
                (childNode->getTypedAttribute(attributeType, 0));

            // If _any_ child doesn't have an attribute of the specified
            // type, then we can't promote this type of attribute. (An
            // attribute on the parent node would incorrectly affect
            // this node.)  Mark that we've failed and abort.
            if (!childAttr)
            {
                emptyFlag = true;
                break;
            }
            
            // Determine if we've seen a similar attribute before; either mark
            // the similar one if we have or add this one in as new if we
            // haven't. The attributes of instanced nodes aren't considered.
            if (childNode->getParentCount() == 1)
            {
                matchFlag = false;

                // Search the distinct-attributes list to see if one
                // of them is similar to the attribute we're currently
                // examining
                for (sloop = 0; sloop < attrCount; sloop++)
                    if (childAttr->isEquivalent(attrArray[sloop]))
                    {
                        // Mark that this attribute is a lot like
                        // a previously-seen one
                        attrHitCounts[sloop]++;
                        matchFlag = true;
                        break;
                    }

                // If the current attribute is completely unlike anything
                // in our list, then add it to the list
                if (!matchFlag)
                {
                    attrArray[attrCount] = childAttr;
                    attrHitCounts[attrCount] = 1;
                    attrCount++;
                }
            }
        }
        
        // Sort the attributes by frequency, and then add the most common
        // attribute to the parent component
        if (attrCount && !emptyFlag)
        {
            sortLists(attrHitCounts, attrArray, attrCount);
            componentNode->addAttribute(attrArray[0]);
        }
    }

    // Second, if the parent node now has the attribute, check each child
    // node to see if its parent(s) have the same attribute that the child
    // does; remove the child's attribute if so.
    if (componentNode->getTypedAttribute(attributeType, 0))
    {
        for (loop = 0; loop < componentNode->getChildCount(); loop++)
        {
            // Get the loop'th child and it's attribute
            childNode = componentNode->getChild(loop);
            childAttr = (vsStateAttribute *)
                (childNode->getTypedAttribute(attributeType, 0));

            // Compare the parents' attributes to the child's one, if
            // it has one
            if (childAttr)
            {
                // Check each parent of the child to see if it has an
                // equivalent attribute
                matchFlag = true;
                for (sloop = 0; sloop < childNode->getParentCount(); sloop++)
                {
                    // Get the sloop'th parent and its attribute
                    parentNode = childNode->getParent(sloop);
                    parentAttr = parentNode->getTypedAttribute(attributeType, 0);

                    // Compare the child's attribute and the parent's one,
                    // if the parent has one
                    if (!parentAttr || !(childAttr->isEquivalent(parentAttr)))
                    {
                        // Mismatch; can't promote
                        matchFlag = false;
                        break;
                    }
                }
            }
            else
            {
                // No child attribute; can't promote
                matchFlag = false;
            }
            
            // If promotion is indicated
            if (matchFlag)
            {
                // The parent(s) of the child have the same attribute;
                // the one on the child is unnecessary, so remove it.
                childNode->removeAttribute(childAttr);
                vsObject::checkDelete(childAttr);
            }
        }
    }
}

// ------------------------------------------------------------------------
// This function sorts two parallel lists; the first list contains
// attribute counts, and the second the attributes themselves. Used by
// the optimizeAttributes function for determining the most common
// attribute at a particular component.
// ------------------------------------------------------------------------
void vsOptimizer::sortLists(int *countArray, vsAttribute **attrArray,
    int listSize)
{
    int loop;
    bool flag;
    int tempCount;
    vsAttribute *tempAttr;
    
    // A list of size zero or one is already sorted
    if (listSize < 2)
        return;
    
    // Perform a bubble-sort on the parallel arrays, sorting by
    // the values in the count array
    flag = true;
    while (flag)
    {
        // No activity yet
        flag = false;
        
        // Sort pass
        for (loop = 0; loop < listSize-1; loop++)
        {
            // Compare
            if (countArray[loop] < countArray[loop+1])
            {
                // Swap
                tempCount = countArray[loop];
                countArray[loop] = countArray[loop+1];
                countArray[loop+1] = tempCount;
                tempAttr = attrArray[loop];
                attrArray[loop] = attrArray[loop+1];
                attrArray[loop+1] = tempAttr;

                // Keep sorting as long as there's activity
                flag = true;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Attempts to sort the children of the given component by what attributes
// they posess. This is done in order to try to reduce the number of state
// changes between adjacent drawn objects. Components with attributes that
// require children to be in a specific order (such as vsSwitchAttributes)
// are not altered.
// ------------------------------------------------------------------------
void vsOptimizer::sortByAttribute(vsComponent *componentNode, int attributeType)
{
    vsNode *firstNode, *secondNode;
    vsAttribute *firstAttr, *secondAttr;
    int loop;
    bool flag;

    // Components with grouping category attributes can't have their
    // children moved
    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;

    // Bubble-sort the children of the component
    flag = true;
    while (flag)
    {
        // No activity yet
        flag = false;

        // Sort pass
        for (loop = 0; loop < (componentNode->getChildCount() - 1); loop++)
        {
            // Get two adjacent children, and their attributes
            firstNode = componentNode->getChild(loop);
            firstAttr = firstNode->getTypedAttribute(attributeType, 0);
            secondNode = componentNode->getChild(loop+1);
            secondAttr = secondNode->getTypedAttribute(attributeType, 0);

            // Plain old pointer comparison; equivalency tests would be
            // too slow, and there's no sort order for them anyway. This
            // also means that components without the specified attribute
            // get moved to the front. (Because their pointer values are
            // NULL.)
            if (firstAttr > secondAttr)
            {
                // Swap the two children
                componentNode->removeChild(secondNode);
                componentNode->insertChild(secondNode, loop);

                // Keep sorting as long as there's activity
                flag = true;
            }
        }
    }
}
