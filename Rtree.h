#ifndef RTREE_H
#define RTREE_H

#include "RtreeDef.h"

RTreeNode *createNode();
RTreeNode *chooseLeaf(Entry e);
void printTree(RTreeNode *root);









#endif