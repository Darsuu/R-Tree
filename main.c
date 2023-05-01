#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "Rtree.h"
#define BUFFERLEN 100

RTreeNode *root; // Global Root Node of R-Tree
int main(void)
{
    root = createNode();
    root->isLeaf = true;
    FILE* fp = fopen("data.txt", "r");
    char* token;
    char tempBuffer[BUFFERLEN];

    while(fgets(tempBuffer, BUFFERLEN, fp)!=NULL)
    {
        Entry entry;
        token = strtok(tempBuffer," ");
        entry.point.x = atoi(token);
        entry.mbr.x1 = atoi(token);
        entry.mbr.x2 = atoi(token);
        token = strtok(NULL, " ");
        entry.point.y = atoi(token);
        entry.mbr.y1 = atoi(token);
        entry.mbr.y2 = atoi(token);
        insertEntry(entry, root);
    }
    

    fclose(fp);
    return 0;
}

// void preOrderTraversal(RTreeNode* root, int level)
// {
//     if(root == NULL) return;

//     for(int i = 0; i<M; i++)
//     {
//         if(root->entries[i].isEmpty == 0)
//         {
//             if(root->isLeaf)
//             {
//                 printf("Level %d: Current Node is leaf\n", level);
//             }
//             else
//             {
//                 printf("Level %d: Current Node is internal node\n", level);
//             }
//         }
//     }
// }

