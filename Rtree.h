#ifndef RTREE_H
#define RTREE_H

#include "RtreeDef.h"

extern RTreeNode *root;

RTreeNode *createNode();
void insertEntry(Entry entry, RTreeNode *root);
RTreeNode *chooseLeaf(Entry entry, RTreeNode *root);
void printTree(RTreeNode *root);
splitNodes splitNode(RTreeNode *node, Entry entry);
twinEntry pickSeeds(Entry *pickEntries);
void pickNext(Entry *pickEntries, RTreeNode *node, RTreeNode *newNode);
void pickNextInternal(childEntry *pickEntries, RTreeNode *node, RTreeNode *newNode);
splitNodes splitInternalNode(RTreeNode *node, childEntry entry);
twinchildEntry pickSeedsInternal(childEntry *pickEntries);
void AdjustTree(RTreeNode* node, RTreeNode* splitNode);







#endif