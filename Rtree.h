#ifndef RTREE_H
#define RTREE_H

#include "RtreeDef.h"

extern RTreeNode *root;

RTreeNode *createNode();
void insertEntry(Entry entry, RTreeNode *root);
RTreeNode *chooseLeaf(Entry entry, RTreeNode *root);
void printTree(RTreeNode *root);










#endif