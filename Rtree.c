#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "Rtree.h"

RTreeNode *createNode() {
    RTreeNode *node = (RTreeNode *)malloc(sizeof(RTreeNode));
    node->parent = NULL;
    
    return node;
}

