#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
// #include<math.h>
#include "Rtree.h"


RTreeNode *createNode() {
    RTreeNode *node = (RTreeNode *)malloc(sizeof(RTreeNode));
    node->parent = NULL;
    for (int i = 0; i < M; i++) 
    {
        node->entries[i].init = 1;
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
    int entryCount;
    for (int i = 0; i < M; i++) 
    {
        if (leaf->entries[i].init == 1) {
            entryCount = i;
            leaf->entries[i].init = 0;
            break;
        }
    }
    
    if(entryCount < M)
    {
        leaf->entries[entryCount] = entry;
        printf("Entry Count is %d\n", entryCount);
        printf("Curr Entry is %d %d\n",  entry.point.x, entry.point.y);
        return;
    }
    else // Call & SplitNode()
    {
        printf("Node full\n");
        // return;
    }

    // Call AdjustTree()
}

int getArea(childEntry currNodeEntry)
{   
    return (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1)*(currNodeEntry.mbr.y2 - currNodeEntry.mbr.y1);
}

int min(int x, int y)
{
    return x > y ? y:x;
}

int max(int x, int y)
{
    return x > y ? x:y;
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
        int len1 = sizeof(currNodeEntry.child->children)/sizeof(currNodeEntry.child->children[0]);
        int len2 = sizeof(nextNode->children)/sizeof(nextNode->children[0]);
        if(len1 < len2) return currNodeEntry.child;
    }
    else return NULL;
 }


RTreeNode *chooseLeaf(Entry entry, RTreeNode *root)
{
    if(root->isLeaf) return root;

    RTreeNode* nextNode;
    int minEnlarge = 9999999;
    int minArea = 9999999;
    int childCount = sizeof(root->children)/sizeof(root->children[0]);
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
