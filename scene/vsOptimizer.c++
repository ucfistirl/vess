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

#include "vsOptimizer.h++"

#include "vsGraphicsState.h++"
#include "vsDecalAttribute.h++"

// ------------------------------------------------------------------------
// Constructor - Turns all optimizations on
// ------------------------------------------------------------------------
vsOptimizer::vsOptimizer()
{
    passMask = VS_OPTIMIZER_ALL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsOptimizer::~vsOptimizer()
{
}

// ------------------------------------------------------------------------
// Start optimizations on the scene rooted at the given node
// ------------------------------------------------------------------------
void vsOptimizer::optimize(vsNode *rootNode)
{
    printf("Beginning optimization run...\n");
    optimizeNode(rootNode);
    printf("Optimization completed\n");
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
    vsComponent *componentNode;

    if (node->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        // Component
        componentNode = (vsComponent *)node;

        if (passMask & VS_OPTIMIZER_CLEAN_TREE)
            cleanChildren(componentNode);

        if (passMask & VS_OPTIMIZER_MERGE_DECALS)
            mergeDecals(componentNode);

        // Recurse on the child component
        for (loop = 0; loop < componentNode->getChildCount(); loop++)
            optimizeNode(componentNode->getChild(loop));

        if (passMask & VS_OPTIMIZER_CLEAN_TREE)
            cleanChildren(componentNode);

        if (passMask & VS_OPTIMIZER_PROMOTE_ATTRIBUTES)
        {
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_BACKFACE,
                vsGraphicsState::isSameBackface);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_FOG,
                vsGraphicsState::isSameFog);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_MATERIAL,
                vsGraphicsState::isSameMaterial);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_SHADING,
                vsGraphicsState::isSameShading);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_TEXTURE,
                vsGraphicsState::isSameTexture);
            optimizeAttributes(componentNode, VS_ATTRIBUTE_TYPE_TRANSPARENCY,
                vsGraphicsState::isSameTransparency);
        }

        if (passMask & VS_OPTIMIZER_MERGE_GEOMETRY)
            mergeGeometry(componentNode);

        if (passMask & VS_OPTIMIZER_CLEAN_TREE)
            cleanChildren(componentNode);

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
    
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        childNode = componentNode->getChild(loop);
        if (childNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        {
            targetComponent = (vsComponent *)childNode;

            // If the component has more than one child, has any attributes,
            // or has a name, then we don't want to remove it.
            if (targetComponent->getChildCount() > 1)
                continue;
            if (targetComponent->getAttributeCount() > 0)
                continue;
            if (strlen(targetComponent->getName()) > 0)
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
    vsComponent *parentComponent;
    vsNode *childNode;
    
    if (targetComponent->getChildCount() == 0)
    {
        while (targetComponent->getParentCount() > 0)
        {
            parentComponent = targetComponent->getParent(0);
            parentComponent->removeChild(targetComponent);
        }
    }
    else
    {
        childNode = targetComponent->getChild(0);
        targetComponent->removeChild(childNode);

        while (targetComponent->getParentCount() > 0)
        {
            parentComponent = targetComponent->getParent(0);
            parentComponent->replaceChild(targetComponent, childNode);
        }
    }
    
    delete targetComponent;
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
    
    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;
    
    count = 0;
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        childNode = componentNode->getChild(loop);
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_DECAL, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
            count++;
    }
    
    if (count < 2)
        return;

    // Create a new decal component
    decalNode = new vsComponent;
    decalNode->addAttribute(new vsDecalAttribute);

    // Attempt to add each decal under this node to the new 
    // component instead
    for (loop = 0; loop < componentNode->getChildCount(); loop++)
    {
        childNode = componentNode->getChild(loop);
        if ((childNode->getTypedAttribute(VS_ATTRIBUTE_TYPE_DECAL, 0)) &&
            (childNode->getAttributeCount() == 1) &&
            (childNode->getParentCount() == 1))
        {
            // Candidate for merging: Transfer the children of this decal
            // component into the new component.
            childComponent = (vsComponent *)childNode;

            // First, make sure there are enough groups in the new component
            while (decalNode->getChildCount() < childComponent->getChildCount())
                decalNode->addChild(new vsComponent);

            // Then, start moving the child nodes over
            count = 0;
            while (childComponent->getChildCount() > 0)
            {
                decalChild = childComponent->getChild(0);
                childComponent->removeChild(decalChild);
                ((vsComponent *)(decalNode->getChild(count)))->
                    addChild(decalChild);
                count++;
            }

            // Finally, remove the depleted decal component from the parent
            // component and reset the loop counter
            componentNode->removeChild(childNode);
            loop--;
            delete childNode;
        }
    }
    
    // Last step: add the new merged decal component back into the
    // parent component
    componentNode->addChild(decalNode);
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
    vsComponent *parent;

    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;

    for (loop = 0; loop < componentNode->getChildCount(); loop++)
        for (sloop = loop+1; sloop < componentNode->getChildCount(); sloop++)
        {
            firstNode = componentNode->getChild(loop);
            secondNode = componentNode->getChild(sloop);

            if ((firstNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) &&
                (secondNode->getNodeType() == VS_NODE_TYPE_GEOMETRY))
            {
                firstGeo = (vsGeometry *)firstNode;
                secondGeo = (vsGeometry *)secondNode;
                if (isSimilarGeometry(firstGeo, secondGeo))
                {
                    while (secondGeo->getParentCount() > 0)
                    {
                        parent = secondGeo->getParent(0);
                        parent->removeChild(secondGeo);
                    }
                    addGeometry(firstGeo, secondGeo);
                    delete secondGeo;
                    sloop--;
                }
            }
        }
}

// ------------------------------------------------------------------------
// Compares two geometry objects for similarity; used by the geometry merge
// routine to determine when two geometries can be merged. If the two
// geometry pointers are the same, VS_FALSE is returned.
// ------------------------------------------------------------------------
int vsOptimizer::isSimilarGeometry(vsGeometry *firstGeo, vsGeometry *secondGeo)
{
    int firstVal, secondVal;
    vsVector firstVec, secondVec;
    int loop, firstType;
    vsAttribute *firstAttr, *secondAttr;
    int sloop, matchFlag;

    // If somehow they're the same geometry object, then we don't want the
    // caller to get the bright idea of trying to merge the object with
    // itself. Return false in this case.
    if (firstGeo == secondGeo)
        return VS_FALSE;

    // If either geometry node is named, return false
    if (strlen(firstGeo->getName()) > 0)
        return VS_FALSE;
    if (strlen(secondGeo->getName()) > 0)
        return VS_FALSE;

    // Compare primitive types
    firstVal = firstGeo->getPrimitiveType();
    secondVal = secondGeo->getPrimitiveType();
    if (firstVal != secondVal)
        return VS_FALSE;

    // Compare attribute counts
    firstVal = firstGeo->getAttributeCount();
    secondVal = secondGeo->getAttributeCount();
    if (firstVal != secondVal)
        return VS_FALSE;

    // Check to make sure that both geometry nodes have the same parent(s)
    if (firstGeo->getParentCount() != secondGeo->getParentCount())
        return VS_FALSE;
    for (loop = 0; loop < firstGeo->getParentCount(); loop++)
    {
        matchFlag = 0;
        for (sloop = 0; sloop < secondGeo->getParentCount(); sloop++)
            if (firstGeo->getParent(loop) == secondGeo->getParent(sloop))
            {
                matchFlag = 1;
                break;
            }
        
        if (!matchFlag)
            return VS_FALSE;
    }

    // Compare attributes
    for (loop = 0; loop < firstVal; loop++)
    {
        firstAttr = firstGeo->getAttribute(loop);

        // Only graphics state attributes can be merged in this way
        if (firstAttr->getAttributeCategory() != VS_ATTRIBUTE_CATEGORY_STATE)
            return VS_FALSE;

        // For each attribute in the first geometry, the second geometry
        // must have a corresponding attribute
        firstType = firstAttr->getAttributeType();
        secondAttr = secondGeo->getTypedAttribute(firstType, 0);
        if (!secondAttr)
            return VS_FALSE;

        switch (firstType)
        {
            case VS_ATTRIBUTE_TYPE_BACKFACE:
                if (!(vsGraphicsState::isSameBackface(firstAttr, secondAttr)))
                    return VS_FALSE;
                break;
            case VS_ATTRIBUTE_TYPE_FOG:
                if (!(vsGraphicsState::isSameFog(firstAttr, secondAttr)))
                    return VS_FALSE;
                break;
            case VS_ATTRIBUTE_TYPE_MATERIAL:
                if (!(vsGraphicsState::isSameMaterial(firstAttr, secondAttr)))
                    return VS_FALSE;
                break;
            case VS_ATTRIBUTE_TYPE_SHADING:
                if (!(vsGraphicsState::isSameShading(firstAttr, secondAttr)))
                    return VS_FALSE;
                break;
            case VS_ATTRIBUTE_TYPE_TEXTURE:
                if (!(vsGraphicsState::isSameTexture(firstAttr, secondAttr)))
                    return VS_FALSE;
                break;
            case VS_ATTRIBUTE_TYPE_TRANSPARENCY:
                if (!(vsGraphicsState::isSameTransparency(firstAttr,
                    secondAttr)))
                    return VS_FALSE;
                break;
            default:
                return VS_FALSE;
        }
    }

    // Compare attribute bindings
    firstVal = firstGeo->getBinding(VS_GEOMETRY_NORMALS);
    secondVal = secondGeo->getBinding(VS_GEOMETRY_NORMALS);
    if (firstVal != secondVal)
        return VS_FALSE;
    if (firstVal == VS_GEOMETRY_BIND_OVERALL)
    {
        firstVec = firstGeo->getData(VS_GEOMETRY_NORMALS, 0);
        secondVec = secondGeo->getData(VS_GEOMETRY_NORMALS, 0);
        if (!(firstVec == secondVec))
            return VS_FALSE;
    }

    firstVal = firstGeo->getBinding(VS_GEOMETRY_COLORS);
    secondVal = secondGeo->getBinding(VS_GEOMETRY_COLORS);
    if (firstVal != secondVal)
        return VS_FALSE;
    if (firstVal == VS_GEOMETRY_BIND_OVERALL)
    {
        firstVec = firstGeo->getData(VS_GEOMETRY_COLORS, 0);
        secondVec = secondGeo->getData(VS_GEOMETRY_COLORS, 0);
        if (!(firstVec == secondVec))
            return VS_FALSE;
    }

    firstVal = firstGeo->getBinding(VS_GEOMETRY_TEXTURE_COORDS);
    secondVal = secondGeo->getBinding(VS_GEOMETRY_TEXTURE_COORDS);
    if (firstVal != secondVal)
        return VS_FALSE;
    if (firstVal == VS_GEOMETRY_BIND_OVERALL)
    {
        firstVec = firstGeo->getData(VS_GEOMETRY_TEXTURE_COORDS, 0);
        secondVec = secondGeo->getData(VS_GEOMETRY_TEXTURE_COORDS, 0);
        if (!(firstVec == secondVec))
            return VS_FALSE;
    }

    return VS_TRUE;
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
    
    // Compute the vertex counts
    srcPrimCount = srcGeo->getPrimitiveCount();
    destPrimCount = destGeo->getPrimitiveCount();
    srcVertCount = 0;
    for (loop = 0; loop < srcPrimCount; loop++)
        srcVertCount += srcGeo->getPrimitiveLength(loop);
    destVertCount = 0;
    for (loop = 0; loop < destPrimCount; loop++)
        destVertCount += destGeo->getPrimitiveLength(loop);

    // Vertex coordinates (always per-vertex)
    destGeo->setDataListSize(VS_GEOMETRY_VERTEX_COORDS,
        srcVertCount + destVertCount);
    for (loop = 0; loop < srcVertCount; loop++)
    {
        tempData = srcGeo->getData(VS_GEOMETRY_VERTEX_COORDS, loop);
        destGeo->setData(VS_GEOMETRY_VERTEX_COORDS, loop + destVertCount,
            tempData);
    }
    
    // Normals
    if (destGeo->getBinding(VS_GEOMETRY_NORMALS) ==
        VS_GEOMETRY_BIND_PER_PRIMITIVE)
    {
        destGeo->setDataListSize(VS_GEOMETRY_NORMALS,
            srcPrimCount + destPrimCount);
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
        destGeo->setDataListSize(VS_GEOMETRY_NORMALS,
            srcVertCount + destVertCount);
        for (loop = 0; loop < srcVertCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_NORMALS, loop);
            destGeo->setData(VS_GEOMETRY_NORMALS, loop + destVertCount,
                tempData);
        }
    }
    
    // Colors
    if (destGeo->getBinding(VS_GEOMETRY_COLORS) ==
        VS_GEOMETRY_BIND_PER_PRIMITIVE)
    {
        destGeo->setDataListSize(VS_GEOMETRY_COLORS,
            srcPrimCount + destPrimCount);
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
        destGeo->setDataListSize(VS_GEOMETRY_COLORS,
            srcVertCount + destVertCount);
        for (loop = 0; loop < srcVertCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_COLORS, loop);
            destGeo->setData(VS_GEOMETRY_COLORS, loop + destVertCount,
                tempData);
        }
    }
    
    // Texture coordinates
    if (destGeo->getBinding(VS_GEOMETRY_TEXTURE_COORDS) ==
        VS_GEOMETRY_BIND_PER_VERTEX)
    {
        destGeo->setDataListSize(VS_GEOMETRY_TEXTURE_COORDS,
            srcVertCount + destVertCount);
        for (loop = 0; loop < srcVertCount; loop++)
        {
            tempData = srcGeo->getData(VS_GEOMETRY_TEXTURE_COORDS, loop);
            destGeo->setData(VS_GEOMETRY_TEXTURE_COORDS, loop + destVertCount,
                tempData);
        }
    }
    
    // Primitive Counts/lengths
    destGeo->setPrimitiveCount(destPrimCount + srcPrimCount);
    if ((destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_POINTS) &&
        (destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_LINES) &&
        (destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_TRIS) &&
        (destGeo->getPrimitiveType() != VS_GEOMETRY_TYPE_QUADS))
    {
        for (loop = 0; loop < srcPrimCount; loop++)
        {
            tempLength = srcGeo->getPrimitiveLength(loop);
            destGeo->setPrimitiveLength(loop + destPrimCount, tempLength);
        }
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
    int attributeType, int (*cmpFunc)(vsAttribute *, vsAttribute *))
{
    int loop, sloop;
    vsNode *childNode;
    vsComponent *parentNode;
    vsAttribute *attrArray[1000];
    int attrHitCounts[1000];
    int attrCount;
    int emptyFlag, matchFlag;
    vsAttribute *childAttr, *parentAttr;
    
    // First, if the parent node does not already have an attribute of the
    // indicated type, attempt to create one by examining the child
    // nodes' attributes.
    if (!(componentNode->getTypedAttribute(attributeType, 0)))
    {
        attrCount = 0;
        emptyFlag = 0;

        // Check each child for an attribute of the given type
        for (loop = 0; loop < componentNode->getChildCount(); loop++)
        {
            childNode = componentNode->getChild(loop);
            childAttr = childNode->getTypedAttribute(attributeType, 0);
            if (!childAttr)
            {
                emptyFlag = 1;
                break;
            }
            
            // Determine if we've seen a similar attribute before; either
            // mark the similar one if we have or add this one in as new if
            // we haven't. The attributes of instanced nodes don't count.
            if (childNode->getParentCount() < 2)
            {
                matchFlag = 0;
                for (sloop = 0; sloop < attrCount; sloop++)
                    if (cmpFunc(childAttr, attrArray[sloop]))
                    {
                        attrHitCounts[sloop]++;
                        matchFlag = 1;
                        break;
                    }
                if (!matchFlag)
                {
                    attrArray[attrCount] = childAttr;
                    attrHitCounts[attrCount] = 1;
                    attrCount++;
                }
            }
        }
        
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
            childNode = componentNode->getChild(loop);
            childAttr = childNode->getTypedAttribute(attributeType, 0);

            if (childAttr)
            {
                matchFlag = 1;
                for (sloop = 0; sloop < childNode->getParentCount(); sloop++)
                {
                    parentNode = childNode->getParent(sloop);
                    parentAttr = parentNode->getTypedAttribute(attributeType,
                        0);
                    if (!parentAttr || !(cmpFunc(childAttr, parentAttr)))
                    {
                        matchFlag = 0;
                        break;
                    }
                }
            }
            else
                matchFlag = 0;
            
            if (matchFlag)
            {
                childNode->removeAttribute(childAttr);
                if (!(childAttr->isAttached()))
                    delete childAttr;
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
    int loop, flag;
    int tempCount;
    vsAttribute *tempAttr;
    
    if (listSize < 2)
        return;
    
    flag = 1;
    while (flag)
    {
        flag = 0;
        
        for (loop = 0; loop < listSize-1; loop++)
            if (countArray[loop] < countArray[loop+1])
            {
                tempCount = countArray[loop];
                countArray[loop] = countArray[loop+1];
                countArray[loop+1] = tempCount;
                tempAttr = attrArray[loop];
                attrArray[loop] = attrArray[loop+1];
                attrArray[loop+1] = tempAttr;
                flag = 1;
            }
    }
}

// ------------------------------------------------------------------------
// Attempts to sort the children of the given component by what attributes
// they posess. This is done in a manner as to attempt to reduce the number
// of state changes between adjacent drawn objects. Components with
// attributes that require children to be in a specific order (such as
// vsSwitchAttributes) are not altered.
// ------------------------------------------------------------------------
void vsOptimizer::sortByAttribute(vsComponent *componentNode, int attributeType)
{
    vsNode *firstNode, *secondNode;
    vsAttribute *firstAttr, *secondAttr;
    int loop, flag;

    if (componentNode->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;

    flag = 1;
    while (flag)
    {
        flag = 0;
        for (loop = 0; loop < (componentNode->getChildCount() - 1); loop++)
        {
            firstNode = componentNode->getChild(loop);
            firstAttr = firstNode->getTypedAttribute(attributeType, 0);
            secondNode = componentNode->getChild(loop+1);
            secondAttr = secondNode->getTypedAttribute(attributeType, 0);
            if (firstAttr > secondAttr)
            {
                componentNode->removeChild(secondNode);
                componentNode->insertChild(secondNode, loop);
                flag = 1;
            }
        }
    }
}
