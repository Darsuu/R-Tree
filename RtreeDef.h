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
}Entry;

typedef struct rtreeleafnode {
    Entry entries[M];
    RTreeLeafNode *parent;
    bool isLeaf;
}RTreeLeafNode;

typedef struct rtreeinteralnode {
    MBR Mbr;
    union children {
        RTreeLeafNode *leafChildren[M];
        RTreeInternalNode *internalChildren[M];
    } Children;
}RTreeInternalNode;

#endif
