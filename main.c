#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "Rtree.h"
#define BUFFERLEN 100

void preOrderTraversal(RTreeNode* root, int level);
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
        entry.isEmpty = 0;
        //printf("%d %d\n", entry.point.x, entry.point.y);
        
        insertEntry(entry, root);
       
    }
    preOrderTraversal(root, 0);
    //fflush(stdout);
    //printf("(%d, %d) (%d, %d)\n", root->children[0].child->entries[2].mbr.x2, root->children[0].child->entries[2].mbr.y2, root->children[0].child->entries[2].mbr.x1, root->children[0].child->entries[2].mbr.y1);
    // printf("2D Object is (%d, %d)\n", root->children[0].child->entries[0].point.x, root->children[0].child->entries[0].point.y);
    // printf("2D Object is (%d, %d)\n", root->children[0].child->entries[1].point.x, root->children[0].child->entries[1].point.y);
    // printf("2D Object is (%d, %d)\n", root->children[1].child->entries[0].point.x, root->children[1].child->entries[0].point.y);
    // printf("2D Object is (%d, %d)\n", root->children[1].child->entries[1].point.x, root->children[1].child->entries[1].point.y);
    // printf("2D Object is (%d, %d)\n", root->children[2].child->entries[0].point.x, root->children[2].child->entries[0].point.y);
    // printf("2D Object is (%d, %d)\n", root->children[2].child->entries[1].point.x, root->children[2].child->entries[1].point.y);
    //printf("2D Object is (%d, %d)\n", root->children[1].child->entries[1].point.x, root->children[1].child->entries[1].point.y);
    //printf("2D Object is (%d, %d)\n", root->children[1].child->entries[1].point.x, root->children[1].child->entries[1].point.y);
    //  for (int i = 0; i < M; i++) {
    //     printf("%d val %d %d\n", root->children[1].child->children[0].child->entries[i].isEmpty, root->children[1].child->children[0].child->entries[i].point.x, root->children[1].child->children[0].child->entries[i].point.y);
    //     fflush(stdout);
    //     }
    fclose(fp);
    return 0;
}

void preOrderTraversal(RTreeNode* root, int level)
{
    if(root == NULL) return;

    for(int i = 0; i<M; i++)
    {
        if(root->isLeaf)
        {
            if(root->entries[i].isEmpty == 0)
            {
                for(int k = 0; k<level; k++) printf("\t");
                printf("Level %d: Current Node is leaf with index %d\n", level, i);
                for(int k = 0; k<level; k++) printf("\t");
                printf("2D Object is (%d, %d)\n", root->entries[i].point.x, root->entries[i].point.y);
                // preOrderTraversal(root->children[0].child, level+1);
            }
        }
        else
        {
            if(root->children[i].child != NULL)
            {
                for(int k = 0; k<level; k++) printf("\t");
                printf("Level %d: Current Node is internal node with index %d\n", level, i);
                for(int k = 0; k<level; k++) printf("\t");
                printf("MBR is Top right:(%d, %d) Bottom left:(%d, %d))\n", root->children[i].mbr.x2, root->children[i].mbr.y2, root->children[i].mbr.x1, root->children[i].mbr.y1);
                // if(root->children[i].child == NULL) printf("is null\n");
                preOrderTraversal(root->children[i].child, level+1);

            }
        }
    }
}