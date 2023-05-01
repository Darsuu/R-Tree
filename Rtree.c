#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
// #include<math.h>
#include "Rtree.h"


/* AUXILIARY FUNCTIONS */

int min(int x, int y)
{
    return x > y ? y:x;
}

int max(int x, int y)
{
    return x > y ? x:y;
}

int getChildCount(RTreeNode *parent)
{
    int count = 0;
    for(int i = 0; i<M; i++)
    {
        if(parent->children[i].child != NULL) count++;
        else break;
    }
    return count;
}

int getEntryCount(RTreeNode *leaf) {
    int entryCount = M;
    for (int i = 0; i < M; i++) 
    {
        if(leaf->entries[i].isEmpty == 1)
        {
            entryCount = i;
            break;
        }
    }
    return entryCount;
}

int getArea(childEntry currNodeEntry)
{   
    return (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1)*(currNodeEntry.mbr.y2 - currNodeEntry.mbr.y1);
}

RTreeNode* compareAreas(childEntry currNodeEntry, RTreeNode* nextNode, int* minArea1)
{
    int minArea = *minArea1;
    int currArea = getArea(currNodeEntry);

    if(currArea < minArea) // choosing node with min area
    {
        *minArea1 = currArea;
        return currNodeEntry.child;
    }
    else if(currArea == minArea) // Done for balancing ( choose node with smallest entry incase of tie)
    {
        int len1 = getChildCount(currNodeEntry.child); // CHECK THIS
        int len2 = getChildCount(nextNode); // CHECK THIS
        if(len1 <= len2) return currNodeEntry.child;
        else return nextNode;
    }
    else return NULL;
}

void addEntry(RTreeNode* node, Entry entry)
{
    for(int i = 0; i<M; i++)
    {
        if(node->entries[i].isEmpty == 1)
        {
            node->entries[i] = entry;
            node->entries[i].isEmpty = 0;
            node->entries[i].mbr.x1 = entry.point.x;
            node->entries[i].mbr.x2 = entry.point.x;
            node->entries[i].mbr.y1 = entry.point.y;
            node->entries[i].mbr.y2 = entry.point.y;
            break;
        }
    }
}

void addchildEntry(RTreeNode* node, childEntry entry)
{
    for(int i = 0; i<M; i++)
    {
        if(node->children[i].child == NULL)
        {
            node->children[i].child = entry.child;
            node->children[i].mbr = entry.mbr;
            break;
        }
    }
}

MBR calcMbr(RTreeNode* node)
{
    MBR res;
    int first_flag = 1;
    if(node->isLeaf)
    {
        for(int i = 0; i<M; i++)
        {
            if(node->entries[i].isEmpty == 0)
            {
                if(first_flag == 1) // When only 1 point
                {
                    res.x1 = node->entries[i].point.x;
                    res.x2 = node->entries[i].point.x;
                    res.y1 = node->entries[i].point.y;
                    res.y2 = node->entries[i].point.y;
                    first_flag = 0;
                }
                else // Successively increase MBR 
                {
                    res.x1 = min(res.x1, node->entries[i].point.x);
                    res.x2 = max(res.x2, node->entries[i].point.x);
                    res.y1 = min(res.y1, node->entries[i].point.y);
                    res.y2 = max(res.y2, node->entries[i].point.y);
                }
            }
        }
    }
    else
    {
        for(int i = 0; i<M; i++)
        {
            if(node->children[i].child != NULL)
            {
                if(first_flag == 1) // When only 1 point
                {
                    res.x1 = node->children[i].mbr.x1;
                    res.x2 = node->children[i].mbr.x2;
                    res.y1 = node->children[i].mbr.y1;
                    res.y2 = node->children[i].mbr.y2;
                    first_flag = 0;
                }
                else // Successively increase MBR 
                {
                    res.x1 = min(res.x1, node->children[i].mbr.x1);
                    res.x2 = max(res.x2, node->children[i].mbr.x2);
                    res.y1 = min(res.y1, node->children[i].mbr.y1);
                    res.y2 = max(res.y2, node->children[i].mbr.y2);
                }
            }
        }
    }
    return res;
}

MBR encloseMBR(MBR mbr1, MBR mbr2) //finalmbr encloses both mbrs
{
    MBR finalmbr; 
    finalmbr.x1 = min(mbr1.x1, mbr2.x1); 
    finalmbr.x2 = max(mbr1.x2, mbr2.x2);
    finalmbr.y1 = min(mbr1.y1, mbr2.y1); 
    finalmbr.y2 = max(mbr1.y2, mbr2.y2);
    return finalmbr;
}

MBR updateMBR(RTreeNode *node)
{  
    if(node == NULL)
    {
        printf("Node is null\n");
    }
    int numChildren = getChildCount(node);

    MBR mbr = node->children[0].mbr; //start with first childentry 

    for (int i = 1; i < numChildren; i++)
    {
        mbr = encloseMBR(mbr,node->children[i].mbr);
    }
    return mbr; // ?
}

int pickNextAux(MBR mbr, int x, int y)
{
    int tempArea;
    if(x <= mbr.x2 && x >= mbr.x1)
    {   
        tempArea = min(abs(y - mbr.y2), abs(y - mbr.y1)) * (mbr.x2 - mbr.x1);
    }
    else if(y <= mbr.y2 && y >= mbr.y1)
    {
        tempArea = min(abs(x - mbr.x2), abs(x - mbr.x1)) * (mbr.x2 - mbr.x1);
    }
    else // Take abs
    {
        int maxX1 = max(abs(x - mbr.x2), abs(x - mbr.x1));
        int maxY1 = max(abs(y - mbr.y2), abs(y - mbr.y1));
        tempArea = maxX1*maxY1 - ((mbr.x2 - mbr.x1)*(mbr.y2-mbr.y1));
    }
    return tempArea;
}

int findindexEntry(RTreeNode *parentNode, RTreeNode *node )
{
    int childrenCount = getChildCount(parentNode);
    for (int i = 0; i < childrenCount; i++)
    {
        if(parentNode->children[i].child == node)  
        {
            return i;    // node matching entry found
        }
    }
    return -1;

}

/* MAIN FUNCTIONS */

RTreeNode *createNode() {
    RTreeNode *node = (RTreeNode *)malloc(sizeof(RTreeNode));
    node->parent = NULL;
    for (int i = 0; i < M; i++) 
    {
        node->entries[i].isEmpty = 1;
        node->children[i].child = NULL;
    }
    return node;
}

void insertEntry(Entry entry, RTreeNode *root)
{
    if(root == NULL)
    {
        printf("Error root is null\n");
        return;
    }

    RTreeNode* leaf = chooseLeaf(entry, root);
    if(leaf == NULL) 
    {
        printf("bruh");
        fflush(stdout);
        return;
    }
    int entryCount = getEntryCount(leaf);
    // printf("e1 is %d\n", entryCount);
    
    // for(int i = 0; i<M; i++)
    // {
    //     if(leaf->entries[i].isEmpty == 0)
    //     {
    //         printf("%d %d\n", leaf->entries[i].point.x, leaf->entries[i].point.y);
    //         fflush(stdout);
    //     }
    // }
    // printf("\n");
    //     fflush(stdout);
    if(entryCount < M)
    {
        leaf->entries[entryCount] = entry;
        AdjustTree(leaf, NULL);
    }
    else // Call & SplitNode()
    {
        // printf("Node became full. Calling SplitNode()\n");
        splitNodes nodeL = splitNode(leaf, entry);
        int n1 = getEntryCount(nodeL.original);
        int n2 = getEntryCount(nodeL.new);
        // for (int i = 0; i < n1; i++) {
        //     printf("%d %d\n", nodeL.original->entries[i].point.x, nodeL.original->entries[i].point.y);
        // }
        // printf("\n");
        // for (int i = 0; i < n2; i++) {
        //     printf("%d %d\n", nodeL.new->entries[i].point.x, nodeL.new->entries[i].point.y);
        // }
        // printf("\n");
        AdjustTree(nodeL.original, nodeL.new);


        // Call AdjustTree() with LL set as nodeL.new
    }
}

RTreeNode *chooseLeaf(Entry entry, RTreeNode *root)
{
    if(root->isLeaf) { /*printf("found\n");*/ return root; }

    RTreeNode* nextNode;
    int minEnlarge = 9999999;
    int minArea = 9999999;
    int childCount = getChildCount(root);
    for(int i = 0; i<childCount; i++)
    {
        childEntry currNodeEntry = root->children[i];
        int x = entry.point.x;
        int y = entry.point.y;
        if(x <= currNodeEntry.mbr.x2 && x >= currNodeEntry.mbr.x1 && y <= currNodeEntry.mbr.y2 && y >= currNodeEntry.mbr.y1) // Point lies inside Rectangle
        {
            int tempArea = 0;
            if(minEnlarge > tempArea)
            {
                minEnlarge = tempArea;
                nextNode = currNodeEntry.child;
            }
            else if(minEnlarge == tempArea)
            {
                RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge);
                if(temp != NULL) nextNode = temp;
            }
        }
        else // Lies outside Rectangle boundary
        {
            if(x <= currNodeEntry.mbr.x2 && x >= currNodeEntry.mbr.x1)
            {   
                int tempArea = min(abs(y - currNodeEntry.mbr.y2), abs(y - currNodeEntry.mbr.y1)) * (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1);
                if(minEnlarge > tempArea)
                {
                    minEnlarge = tempArea;
                    nextNode = currNodeEntry.child;
                }
                else if(minEnlarge == tempArea)
                {
                    RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge);
                    if(temp != NULL) nextNode = temp;
                }
            }
            else if(y <= currNodeEntry.mbr.y2 && y >= currNodeEntry.mbr.y1)
            {
                int tempArea = min(abs(x - currNodeEntry.mbr.x2), abs(x - currNodeEntry.mbr.x1)) * (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1);
                if(minEnlarge > tempArea)
                {
                    minEnlarge = tempArea;
                    nextNode = currNodeEntry.child;
                }
                else if(minEnlarge == tempArea)
                {
                    RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge);
                    if(temp != NULL) nextNode = temp;
                }
            }
            else // Take abs
            {
                int maxX1 = max(abs(x - currNodeEntry.mbr.x2), abs(x - currNodeEntry.mbr.x1));
                int maxY1 = max(abs(y - currNodeEntry.mbr.y2), abs(y - currNodeEntry.mbr.y1));
                int tempArea = maxX1*maxY1 - getArea(currNodeEntry);
                if(minEnlarge > tempArea)
                {
                    minEnlarge = tempArea;
                    nextNode = currNodeEntry.child;
                }
                else if(minEnlarge == tempArea)
                {
                    RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge);
                    if(temp != NULL) nextNode = temp;
                }
            }
        }
    }
    return chooseLeaf(entry, nextNode);
}

splitNodes splitNode(RTreeNode *node, Entry entry) {
    Entry tempEntries[M+1];
    for (int i = 0; i < M; i++) 
    {
        tempEntries[i] = node->entries[i];
        node->entries[i].isEmpty = 1;
    }
    tempEntries[M] = entry;
    tempEntries[M].isEmpty = 0;

    twinEntry entryPair = pickSeeds(tempEntries); // Change isEmpty value to 1 (done)
    RTreeNode *newNode = createNode();
    newNode->isLeaf = true;

    // Populated first 2 entries and created 2 groups
    addEntry(node, entryPair.entry1);
    addEntry(newNode, entryPair.entry2);
 
    int entryNumber = M-1; // Here we have M-1 entries in tempEntries

    // Update mbrs so that enlargement can be accurately calculated
    for(int i = 0; i < M + 1; i++) 
    { 
        if (tempEntries[i].isEmpty == 1) {
            continue;
        }

        int ecount1 = getEntryCount(node);
        int ecount2 = getEntryCount(newNode);
        if(m-ecount1 == entryNumber)
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[i].isEmpty == 0)
                {
                    addEntry(node, tempEntries[i]);
                    entryNumber--;
                    tempEntries[i].isEmpty = 1;
                }
            }
        }
        else if(m-ecount2 == entryNumber)
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[i].isEmpty == 0)
                {
                    addEntry(newNode, tempEntries[i]);
                    entryNumber--;
                    tempEntries[i].isEmpty = 1;
                }
            }
        }
        else
        {
            pickNext(tempEntries, node, newNode);
            entryNumber--;
        }
    }

    splitNodes res;
    res.new = newNode;
    res.original = node;
    // int n1 = getEntryCount(node);
    // int n2 = getEntryCount(newNode);
    // for (int i = 0; i < n1; i++) {
    //     printf("%d %d\n", node->entries[i].point.x, node->entries[i].point.y);
    // }
    // printf("\n");
    // for (int i = 0; i < n2; i++) {
    //     printf("%d %d\n", newNode->entries[i].point.x, newNode->entries[i].point.y);
    // }
    // printf("\n");
    return res;
}


twinEntry pickSeeds(Entry *pickEntries)
{
    int maxEnlargement = -1;
    int maxi;
    int maxj;
    for (int i = 0; i < M + 1; i++) {
        // printf("pickseeds: %d %d\n", pickEntries[i].point.x, pickEntries[i].point.y);
        for (int j = 0; j < M + 1; j++) {
            if (i == j) {
                continue;
            }
            int area = (abs(pickEntries[i].point.x - pickEntries[j].point.x)) * (abs(pickEntries[i].point.y - pickEntries[j].point.y));
            if (area > maxEnlargement) {
                maxEnlargement = area;
                maxi = i;
                maxj = j;
            }
        }
    }
    twinEntry result;
    result.entry1 = pickEntries[maxi];
    result.entry2 = pickEntries[maxj];
    pickEntries[maxi].isEmpty = 1;
    pickEntries[maxj].isEmpty = 1;
    
    return result;
}

void pickNext(Entry *pickEntries, RTreeNode *node, RTreeNode *newNode)
{
    int maxDiff = -1;
    int entryIndex = -1;
    RTreeNode* tempNode = NULL;
    for(int i = 0; i<M+1; i++)
    {
        if(pickEntries[i].isEmpty == 0)
        {
            int x = pickEntries[i].point.x;
            int y = pickEntries[i].point.y;

            MBR mbrNode = calcMbr(node);
            MBR mbrNewNode = calcMbr(newNode);

            int d1 = pickNextAux(mbrNode, x, y);
            int d2 = pickNextAux(mbrNewNode, x, y);

            if(maxDiff < abs(d1-d2))
            {
                maxDiff = abs(d1 - d2);
                entryIndex = i;
                if((d1 - d2)> 0)
                {
                    tempNode = newNode;
                } 
                else tempNode = node;
            }   
        }
    }

    // tempNode contains the the node with the minEnlargement so add it to that node
    // entryIndex contains the entry which creates maxDiff
    printf("i %d %d\n", pickEntries[entryIndex].point.x,  pickEntries[entryIndex].point.y );
    addEntry(tempNode, pickEntries[entryIndex]);
    pickEntries[entryIndex].isEmpty = 1;
}

void pickNextInternal(childEntry *pickEntries, RTreeNode *node, RTreeNode *newNode)
{
    int maxDiff = -1;
    int entryIndex = -1;
    RTreeNode* tempNode = NULL;
    for(int i = 0; i<M+1; i++)
    {
        if(pickEntries[i].child != NULL)
        {

            MBR mbrNode = calcMbr(node);
            MBR mbrNewNode = calcMbr(newNode);

            int areaChild1 = getArea(pickEntries[i]);

            int x1 = min(pickEntries[i].mbr.x1, mbrNode.x1);
            int x2 = max(pickEntries[i].mbr.x2, mbrNode.x2);
            int y1 = min(pickEntries[i].mbr.y1, mbrNode.y1);
            int y2 = max(pickEntries[i].mbr.y2, mbrNode.y2);

            int d1 = (abs(x2 - x1)) * (abs(y2 - y1)) - areaChild1 - (mbrNode.x2-mbrNode.x1)*(mbrNode.y2 - mbrNode.y1);
            
            int xk1 = min(pickEntries[i].mbr.x1, mbrNewNode.x1);
            int xk2 = max(pickEntries[i].mbr.x2, mbrNewNode.x2);
            int yk1 = min(pickEntries[i].mbr.y1, mbrNewNode.y1);
            int yk2 = max(pickEntries[i].mbr.y2, mbrNewNode.y2);

            int d2 = (abs(xk2 - xk1)) * (abs(yk2 - yk1)) - areaChild1 - (mbrNewNode.x2-mbrNewNode.x1)*(mbrNewNode.y2 - mbrNewNode.y1);

            if(maxDiff < abs(d1-d2))
            {
                maxDiff = abs(d1 - d2);
                entryIndex = i;
                if(d1 - d2 > 0) tempNode = newNode;
                else tempNode = node;
            }   
        }
    }

    // tempNode contains the the node with the minEnlargement so add it to that node
    // entryIndex contains the entry which creates maxDiff
    addchildEntry(tempNode, pickEntries[entryIndex]);
    pickEntries[entryIndex].child = NULL;
}


splitNodes splitInternalNode(RTreeNode *node, childEntry entry) {
    childEntry tempEntries[M+1];
    for (int i = 0; i < M; i++) 
    {
        tempEntries[i] = node->children[i];
        node->entries[i].isEmpty = 1;
    }
    tempEntries[M] = entry;
    tempEntries[M].child = NULL;

    twinchildEntry entryPair = pickSeedsInternal(tempEntries); // Change isEmpty value to 1 (done)
    RTreeNode *newNode = createNode();
    newNode->isLeaf = false;

    // Populated first 2 entries and created 2 groups
    addchildEntry(node, entryPair.entry1);
    addchildEntry(newNode, entryPair.entry2);
 
    int entryNumber = M-1; // Here we have M-1 entries in tempEntries

    // Update mbrs so that enlargement can be accurately calculated
    for(int i = 0; i < M + 1; i++) 
    { 
        if (tempEntries[i].child == NULL) {
            continue;
        }
        int ecount1 = getChildCount(node);
        int ecount2 = getChildCount(newNode);
        // printf("ec1 %d\n", ecount1);
        // printf("ec2 %d\n", ecount2);
        if(m-ecount1 == entryNumber)
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[i].child != NULL)
                {
                    addchildEntry(node, tempEntries[i]);
                    entryNumber--;
                    tempEntries[i].child = NULL;
                }
            }
        }
        else if(m-ecount2 == entryNumber)
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[i].child != NULL)
                {
                    addchildEntry(newNode, tempEntries[i]);
                    entryNumber--;
                    tempEntries[i].child = NULL;
                }
            }
        }
        else
        {
            pickNextInternal(tempEntries, node, newNode);
            entryNumber--;
        }
    }

    splitNodes res;
    res.new = newNode;
    res.original = node;
    return res;
}

twinchildEntry pickSeedsInternal(childEntry *pickEntries)
{
    int maxEnlargement = -1;
    int maxi;
    int maxj;
    for (int i = 0; i < M + 1; i++) {
        // printf("tr: %d %d\n",pickEntries[i].mbr.x2, pickEntries[i].mbr.y2);
        // printf("bl: %d %d\n", pickEntries[i].mbr.x1, pickEntries[i].mbr.y1);
        for (int j = 0; j < M + 1; j++) {
            if (i == j) {
                continue;
            }
            int areaChild1 = getArea(pickEntries[i]);
            int areaChild2 = getArea(pickEntries[j]);

            int x1 = min(pickEntries[i].mbr.x1, pickEntries[j].mbr.x1);
            int x2 = max(pickEntries[i].mbr.x2, pickEntries[j].mbr.x2);
            int y1 = min(pickEntries[i].mbr.y1, pickEntries[j].mbr.y1);
            int y2 = max(pickEntries[i].mbr.y2, pickEntries[j].mbr.y2);

            int areaEnlarge = (abs(x2 - x1)) * (abs(y2 - y1)) - areaChild1 - areaChild2;
            if (areaEnlarge > maxEnlargement) {
                maxEnlargement = areaEnlarge;
                maxi = i;
                maxj = j;
            }
        }
    }
    twinchildEntry result;
    result.entry1 = pickEntries[maxi];
    result.entry2 = pickEntries[maxj];
    

    // printf("tr: %d %d\n", result.entry2.mbr.x2, result.entry2.mbr.y2);
    // printf("bl: %d %d\n", result.entry2.mbr.x1, result.entry2.mbr.y1);
    pickEntries[maxi].child = NULL;
    pickEntries[maxj].child = NULL;
    
    return result;
}

void AdjustTree(RTreeNode* node, RTreeNode* splitNode)//split took place so we have L and LL(splitnode formed due to insertion)
{ 
    
    RTreeNode *parentNode = node->parent;
    
    //stop if N is the root

    if (node == root) // current node is root node
    {
        if (splitNode != NULL) 
        {
            
            RTreeNode *newRoot = createNode(); 
            newRoot->isLeaf = false;
            node->parent = newRoot; //newRoot as parent of node and splitnode
            splitNode->parent = newRoot;

            newRoot->children[0].child = node;
            newRoot->children[0].mbr = calcMbr(node);
            newRoot->children[1].child = splitNode;
            newRoot->children[1].mbr =  calcMbr(splitNode);
            root = newRoot;  //global root set to newRoot 
        }
        return;
        
    }

    // node not root
    // adjust covering MBR in parent entry
    int indexEntry= findindexEntry(parentNode,node);
    if(indexEntry == -1)
    {
        printf("Entry not found\n");
        return;
    }
    // printf("fr %d\n", indexEntry);
    parentNode->children[indexEntry].mbr = calcMbr(node);
    // printf("tr %d %d\n", parentNode->children[indexEntry].mbr.x2, parentNode->children[indexEntry].mbr.y2);
    // printf("bl %d %d\n", parentNode->children[indexEntry].mbr.x1, parentNode->children[indexEntry].mbr.y1);

    //propagate nodesplit upwards

    if(splitNode != NULL)
    {
        // int n1 = getEntryCount(node);
        // int n2 = getEntryCount(splitNode);
        // for (int i = 0; i < n1; i++) {
        //     printf("%d %d\n", node->entries[i].point.x, node->entries[i].point.y);
        // }
        // printf("\n");
        // for (int i = 0; i < n2; i++) {
        //     printf("%d %d\n", splitNode->entries[i].point.x, splitNode->entries[i].point.y);
        // }

        childEntry newEntry;
        newEntry.child = splitNode;
        newEntry.mbr = calcMbr(splitNode);
        int child_count = getChildCount(parentNode);
        if(child_count == M)
        {
            // for (int i = 0; i < M; i++) {
            //     printf("%d \n", parentNode->entries[i].isEmpty);
            // }
            // printf("here\n");
            splitNodes tempNodes = splitInternalNode(parentNode, newEntry);
            AdjustTree(tempNodes.original, tempNodes.new);
        }
        else
        {
            addchildEntry(parentNode, newEntry);
            splitNode->parent = parentNode;
            AdjustTree(parentNode, NULL);
        }
    }
    else
    {
        AdjustTree(parentNode, NULL);
    }
}
