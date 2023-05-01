#ifndef RTREEDEF_H
#define RTREEDEF_H

#include<stdbool.h>

#define m 2
#define M 4


typedef struct mbr {
    int x1;
    int y1;
    int y2;
    int x2;
}MBR;

typedef struct point{
    int x;
    int y;
}Point;

typedef struct entry {
    MBR mbr;
    Point point;
    int isEmpty;
}Entry;

typedef struct childentry{
    MBR mbr;
    struct rtreeleafnode* child;
}childEntry;

typedef struct rtreeleafnode {
    Entry entries[M]; // Needed in leaf
    struct rtreeleafnode* parent; // Needed in internal node
    bool isLeaf; // Needed in internal node
    childEntry children[M]; 
}RTreeNode;

typedef struct splitnodes {
    RTreeNode* original;
    RTreeNode* new;
}splitNodes;

typedef struct twinentries {
    Entry entry1, entry2;
}twinEntry;

typedef struct twinchildentries {
    childEntry entry1, entry2;
}twinchildEntry;
#endif


