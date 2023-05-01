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
        if(parent->children->child != NULL) count++;
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
            // leaf->entries[i].isEmpty = 0;
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
            break;
        }
    }
}

MBR calcMbr(RTreeNode* node)
{
    MBR res;
    int len = getEntryCount(node);
    int first_flag = 1;
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
    return res;
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

/* MAIN FUNCTIONS */

RTreeNode *createNode() {
    RTreeNode *node = (RTreeNode *)malloc(sizeof(RTreeNode));
    node->parent = NULL;
    for (int i = 0; i < M; i++) 
    {
        node->entries[i].isEmpty = 1;
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
    int entryCount = getEntryCount(leaf);
    // printf("Entry count is %d\n", entryCount);
    if(entryCount < M)
    {
        leaf->entries[entryCount] = entry;
        // Call AdjustTree() with LL set as NULL
    }
    else // Call & SplitNode()
    {
        // printf("Node became full. Calling SplitNode()\n");
        splitNodes nodeL = splitNode(leaf, entry);

        printf("Old node is: \n");
        for(int j = 0; j<M; j++)
        {
            Entry tempE = nodeL.original->entries[j];
            if(tempE.isEmpty == 0)
            {
                printf("Curr Entry is %d %d\t",  tempE.point.x, tempE.point.y);
            }
        }
        printf("\n");
        printf("New node is: \n");
        for(int j = 0; j<M; j++)
        {
            Entry tempE = nodeL.new->entries[j];
            if(tempE.isEmpty == 0)
            {
                printf("Curr Entry is %d %d\t",  tempE.point.x, tempE.point.y);
            }
        }


        // Call AdjustTree() with LL set as nodeL.new
    }
}

// RTreeNode* compareAreasNode(MBR mbr, RTreeNode* node, RTreeNode* nextNode, int* minArea1)
// {
//     int minArea = *minArea1;
//     int currArea = ((mbr.x2 - mbr.x1)*(mbr.y2-mbr.y1));
//     if(currArea < minArea) // choosing node with min area
//     {
//         *minArea1 = currArea;
//         return node;
//     }
//     else if(currArea == minArea) // Done for balancing ( choose node with smallest entry incase of tie)
//     {
//         int len1 = getChildCount(node); // CHECK THIS
//         int len2 = getChildCount(nextNode); // CHECK THIS
//         if(len1 <= len2) return node;
//         else return nextNode;
//     }
//     else return NULL;
// }


RTreeNode *chooseLeaf(Entry entry, RTreeNode *root)
{
    if(root->isLeaf) return root;

    RTreeNode* nextNode;
    int minEnlarge = 9999999;
    int minArea = 9999999;
    int childCount = getChildCount(root);
    for(int i = 0; i<M; i++)
    {
        childEntry currNodeEntry = root->children[i];
        int x = entry.point.x;
        int y = entry.point.y;
        if(x <= currNodeEntry.mbr.x2 && x >= currNodeEntry.mbr.x1 && y <= currNodeEntry.mbr.y2 && y >= currNodeEntry.mbr.y1) // Point lies inside Rectangle
        {
            RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minArea);
            if(temp != NULL) nextNode = temp;
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
        // printf("x is %d, y is %d\n", tempEntries[i].point.x, tempEntries[i].point.y);
        int ecount1 = getEntryCount(node);
        int ecount2 = getEntryCount(newNode);
        // printf("Count of node is %d\n", ecount1);
        // printf("Count of newNode is %d\n", ecount2);
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
    return res;
}

twinEntry pickSeeds(Entry *pickEntries)
{
    int maxEnlargement = -1;
    int maxi;
    int maxj;
    for (int i = 0; i < M + 1; i++) {
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
    int minEnlarge = 9999999;
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
                if(d1 - d2 > 0) tempNode = newNode;
                else tempNode = node;
            }   
        }
    }

    // tempNode contains the the node with the minEnlargement so add it to that node
    // entryIndex contains the entry which creates maxDiff
    addEntry(tempNode, pickEntries[entryIndex]);
    pickEntries[entryIndex].isEmpty = 1;
}
