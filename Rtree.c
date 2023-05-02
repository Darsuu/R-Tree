#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<limits.h> // Library used for LLONG_MIN and LLONG_MAX

#define BUFFERLEN 100
#define m 2
#define M 4

/* STRUCTS */

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
    struct rtreeleafnode* parent;
    bool isLeaf;
    int id;
    childEntry children[M]; // Needed in internal node
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

/* GLOBAL VARIABLES */

RTreeNode *root; // Global Root Node of R-Tree
int id = 0;

/* FUNCTION DECLARATIONS */

RTreeNode *createNode();
void insertEntry(Entry entry, RTreeNode *root);
void AdjustTree(RTreeNode* node, RTreeNode* splitNode);
void preOrderTraversal(RTreeNode* root, int level);

RTreeNode *chooseLeaf(Entry entry, RTreeNode *root);
splitNodes splitNode(RTreeNode *node, Entry entry);
twinEntry pickSeeds(Entry *pickEntries);
void pickNext(Entry *pickEntries, RTreeNode *node, RTreeNode *newNode);

splitNodes splitInternalNode(RTreeNode *node, childEntry entry);
twinchildEntry pickSeedsInternal(childEntry *pickEntries);
void pickNextInternal(childEntry *pickEntries, RTreeNode *node, RTreeNode *newNode);

/* AUXILIARY FUNCTIONS */

int min(int x, int y) // Function to calculate minimum of 2 values
{
    return x > y ? y:x;
}

int max(int x, int y) // Function to calculate maximum of 2 values
{
    return x > y ? x:y;
}

int getChildCount(RTreeNode *parent) // Function to return the number of children in an internal node
{
    int count = 0;
    for(int i = 0; i<M; i++)
    {
        if(parent->children[i].child != NULL) count++;
        else break;
    }
    return count;
}

int getEntryCount(RTreeNode *leaf) // Function to return the number of entries in a leaf node
{
    int entryCount = M;
    for (int i = 0; i < M; i++) 
    {
        if(leaf->entries[i].isEmpty == 1)
        {
            entryCount = i;
            break;
        }
    }
    return entryCount;
}

long long getArea(childEntry currNodeEntry) // Function to return the area of an entry in a leaf node
{   
    long long var = (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1);
    var *= (currNodeEntry.mbr.y2 - currNodeEntry.mbr.y1);
    return var;
}

RTreeNode* compareAreas(childEntry currNodeEntry, RTreeNode* nextNode, long long* minArea1) // Function returns the optimum node (one with minimum area or entries)
{
    long long minArea = *minArea1;
    long long currArea = getArea(currNodeEntry);

    if(currArea < minArea) // Choose node with minimum area
    {
        *minArea1 = currArea;
        return currNodeEntry.child;
    }
    else if(currArea == minArea) // Incase of equal area compare number of entries. (Done for balancing)
    {
        int len1 = getChildCount(currNodeEntry.child);
        int len2 = getChildCount(nextNode);
        if(len1 <= len2) return currNodeEntry.child;
        else return nextNode;
    }
    else return nextNode;
}

void addEntry(RTreeNode* node, Entry entry) // Function adds an entry inside a leaf node
{
    for(int i = 0; i<M; i++)
    {
        if(node->entries[i].isEmpty == 1)
        {
            node->entries[i] = entry;
            node->entries[i].isEmpty = 0;
            node->entries[i].mbr.x1 = entry.point.x;
            node->entries[i].mbr.x2 = entry.point.x;
            node->entries[i].mbr.y1 = entry.point.y;
            node->entries[i].mbr.y2 = entry.point.y;
            break;
        }
    }
}

void addchildEntry(RTreeNode* node, childEntry entry) // Function adds an entry inside an internal node
{
    for(int i = 0; i<M; i++)
    {
        if(node->children[i].child == NULL)
        {
            node->children[i].child = entry.child;
            node->children[i].child->parent = node;
            node->children[i].mbr = entry.mbr;
            break;
        }
    }
}

MBR calcMbr(RTreeNode* node) // Function to calculate MBR of an internal node / a leaf node
{
    MBR res;
    int first_flag = 1;
    if(node->isLeaf)
    {
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
    }
    else
    {
        for(int i = 0; i<M; i++)
        {
            if(node->children[i].child != NULL)
            {
                if(first_flag == 1) // When only 1 point
                {
                    res.x1 = node->children[i].mbr.x1;
                    res.x2 = node->children[i].mbr.x2;
                    res.y1 = node->children[i].mbr.y1;
                    res.y2 = node->children[i].mbr.y2;
                    first_flag = 0;
                }
                else // Successively increase MBR 
                {
                    res.x1 = min(res.x1, node->children[i].mbr.x1);
                    res.x2 = max(res.x2, node->children[i].mbr.x2);
                    res.y1 = min(res.y1, node->children[i].mbr.y1);
                    res.y2 = max(res.y2, node->children[i].mbr.y2);
                }
            }
        }
    }
    return res;
}

MBR encloseMBR(MBR mbr1, MBR mbr2) // Function calculates a new MBR based on 2 previous MBRs
{
    MBR finalmbr; 
    finalmbr.x1 = min(mbr1.x1, mbr2.x1); 
    finalmbr.x2 = max(mbr1.x2, mbr2.x2);
    finalmbr.y1 = min(mbr1.y1, mbr2.y1); 
    finalmbr.y2 = max(mbr1.y2, mbr2.y2);
    return finalmbr;
}

MBR updateMBR(RTreeNode *node) // Function returns a MBR that can cover all the MBRs of the node's children
{  
    if(node == NULL)
    {
        printf("ERROR: Node is null\n");
    }
    int numChildren = getChildCount(node);

    MBR mbr = node->children[0].mbr; // Start with first childentry 

    for (int i = 1; i < numChildren; i++)
    {
        mbr = encloseMBR(mbr,node->children[i].mbr); // Successively changing MBR
    }
    return mbr; 
}

long long pickNextAux(MBR mbr, int x, int y) // Function for calculating minimum enlargement required to fit a point(x,y) into an existing MBR
{
    long long tempArea;
    if(x <= mbr.x2 && x >= mbr.x1)                      
    {                   
        // CASE WHEN:                      
        //                      (X,Y)
        //                 |     .    |
        //                 |__________|
        //                 |    MBR   |
        //                 |__________|

        
        // The below IF condition is                                 
        // for when the point is on the same line as the MBR's edge        
        if( x== mbr.x2 && x == mbr.x1) tempArea = 0;    
        else                                               
        {                                               
            tempArea = min(abs(y - mbr.y2), abs(y - mbr.y1)); 
            tempArea *= (mbr.x2 - mbr.x1);  
        }
    }
    else if(y <= mbr.y2 && y >= mbr.y1)                 
    {                        
        // CASE WHEN:      
        //                 ____________ _ _ _
        //                 |    MBR   |     . (X,Y)
        //                 |__________| _ _ _


        // The below IF condition is                                 
        // for when the point is on the same line as the MBR's edge 
        if( y== mbr.y2 && y == mbr.y1) tempArea = 0;   
        else                                            
        {
            tempArea = min(abs(x - mbr.x2), abs(x - mbr.x1)); 
            tempArea *= (mbr.x2 - mbr.x1);
        }
    }
    else // Take abs                                    
    {     
        // CASE WHEN:   
        //                |          | .(X,Y)
        //                |__________|_ _ _ _ _
        //                |    MBR   |             
        //                |__________|_ _ _ _ _


        long long maxX1 = max(abs(x - mbr.x2), abs(x - mbr.x1));
        long long maxY1 = max(abs(y - mbr.y2), abs(y - mbr.y1));
        tempArea = maxX1*maxY1;
        tempArea -= ((mbr.x2 - mbr.x1)*(mbr.y2-mbr.y1));
    }
    return tempArea;
}

int findindexEntry(RTreeNode *parentNode, RTreeNode *node ) // Function finds the index of a child node in an internal node
{
    int childrenCount = getChildCount(parentNode);
    for (int i = 0; i < childrenCount; i++)
    {
        if(parentNode->children[i].child->id == node->id)  
        {
            return i;    // node matching entry found
        }
    }
    return -1;
}

/* MAIN FUNCTIONS */

int main(void) // Driver Function
{
    root = createNode();
    root->isLeaf = true;
    root->parent = NULL;

    FILE* fp = fopen("data.txt", "r");
    char* token;
    char tempBuffer[BUFFERLEN];

    while(fgets(tempBuffer, BUFFERLEN, fp)!=NULL)
    {
        Entry entry;
        entry.isEmpty = 0;

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

    preOrderTraversal(root, 0);
    fclose(fp);
    return 0;
}

RTreeNode *createNode() // Function to create an empty internal node / leaf node
{
    RTreeNode *node = (RTreeNode *)malloc(sizeof(RTreeNode));
    node->parent = NULL;
    node->id = id++;
    for (int i = 0; i < M; i++) 
    {
        node->entries[i].isEmpty = 1;
        node->children[i].child = NULL;
    }
    return node;
}

void insertEntry(Entry entry, RTreeNode *root) // Function to insert entry into the R-Tree
{
    if(root == NULL)
    {
        printf("ERROR: Root is null\n");
        return;
    }

    RTreeNode* leaf = chooseLeaf(entry, root); // Chooses the node where entry should be added
    if(leaf == NULL) 
    {
        printf("ERROR: Leaf is null\n");
        fflush(stdout);
        return;
    }

    int entryCount = getEntryCount(leaf);
    if(entryCount < M) // If no overflow then just add entry
    {
        leaf->entries[entryCount] = entry;
        AdjustTree(leaf, NULL); // Adjust all the MBRs
    }
    else // If overflow then split the node
    {
        splitNodes nodeL = splitNode(leaf, entry);
        int n1 = getEntryCount(nodeL.original);
        int n2 = getEntryCount(nodeL.new);
        AdjustTree(nodeL.original, nodeL.new); // Adjust all the MBRs and add the new split node
    }
}

RTreeNode *chooseLeaf(Entry entry, RTreeNode *root) // Function returns the node where an entry should be added
{
    if(root->isLeaf) return root; 

    RTreeNode* nextNode;
    long long minEnlarge = LLONG_MAX;
    long long minArea = LLONG_MAX;

    int childCount = getChildCount(root);
    for(int i = 0; i<childCount; i++)
    {
        childEntry currNodeEntry = root->children[i];
        long long x = entry.point.x;
        long long y = entry.point.y;
        if(x <= currNodeEntry.mbr.x2 && x >= currNodeEntry.mbr.x1 && y <= currNodeEntry.mbr.y2 && y >= currNodeEntry.mbr.y1) // Point lies inside Rectangle
        {
            long long tempArea = 0;
            if(minEnlarge > tempArea)
            {
                minEnlarge = tempArea;
                nextNode = currNodeEntry.child;
            }
            else if(minEnlarge == tempArea) // If enlargements equal then use compareAreas() to resolve conflict
            {
                RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge);
                if(temp != NULL) nextNode = temp;
            }
        }
        else // Lies outside Rectangle boundary
        {
            if(x <= currNodeEntry.mbr.x2 && x >= currNodeEntry.mbr.x1)
            {   
                // CASE WHEN:                      
                //                      (X,Y)
                //                 |     .    |
                //                 |__________|
                //                 |    MBR   |
                //                 |__________|

                
                // The below IF condition is                                 
                // for when the point is on the same line as the MBR's edge  
                long long tempArea;
                if( x == currNodeEntry.mbr.x2 && x == currNodeEntry.mbr.x1) tempArea = 0;
                else tempArea = min(abs(y - currNodeEntry.mbr.y2), abs(y - currNodeEntry.mbr.y1)) * (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1);
                if(minEnlarge > tempArea)
                {
                    minEnlarge = tempArea;
                    nextNode = currNodeEntry.child;
                }
                else if(minEnlarge == tempArea)
                {
                    RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge); // If enlargements equal then use compareAreas() to resolve conflict
                    if(temp != NULL) nextNode = temp;
                }
            }
            else if(y <= currNodeEntry.mbr.y2 && y >= currNodeEntry.mbr.y1)
            {
                // CASE WHEN:      
                //                 ____________ _ _ _
                //                 |    MBR   |     . (X,Y)
                //                 |__________| _ _ _


                // The below IF condition is                                 
                // for when the point is on the same line as the MBR's edge 
                long long tempArea;
                if( y== currNodeEntry.mbr.y2 && y == currNodeEntry.mbr.y1) tempArea = 0;
                else tempArea = min(abs(x - currNodeEntry.mbr.x2), abs(x - currNodeEntry.mbr.x1)) * (currNodeEntry.mbr.x2 - currNodeEntry.mbr.x1);
                if(minEnlarge > tempArea)
                {
                    minEnlarge = tempArea;
                    nextNode = currNodeEntry.child;
                }
                else if(minEnlarge == tempArea)
                {
                    RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge); // If enlargements equal then use compareAreas() to resolve conflict
                    if(temp != NULL) nextNode = temp;
                }
            }
            else // Take abs
            {
                // CASE WHEN:   
                //                |          | .(X,Y)
                //                |__________|_ _ _ _ _
                //                |    MBR   |             
                //                |__________|_ _ _ _ _


                long long maxX1 = max(abs(x - currNodeEntry.mbr.x2), abs(x - currNodeEntry.mbr.x1));
                long long maxY1 = max(abs(y - currNodeEntry.mbr.y2), abs(y - currNodeEntry.mbr.y1));
                long long tempArea = maxX1*maxY1;
                tempArea -= getArea(currNodeEntry);
                if(minEnlarge > tempArea)
                {
                    minEnlarge = tempArea;
                    nextNode = currNodeEntry.child;
                }
                else if(minEnlarge == tempArea) // If enlargements equal then use compareAreas() to resolve conflict
                {
                    RTreeNode* temp = compareAreas(currNodeEntry, nextNode, &minEnlarge);
                    if(temp != NULL) nextNode = temp;
                }
            }
        }
    }
    return chooseLeaf(entry, nextNode);
}

splitNodes splitNode(RTreeNode *node, Entry entry) // Function splits the leaf node if an entry needs to be added to a full leaf node
{
    Entry tempEntries[M+1];
    for (int i = 0; i < M; i++) 
    {
        tempEntries[i] = node->entries[i];
        node->entries[i].isEmpty = 1;
    }
    tempEntries[M] = entry; // The entry which caused overflow
    tempEntries[M].isEmpty = 0;

    twinEntry entryPair = pickSeeds(tempEntries); // Forms 2 groups
    RTreeNode *newNode = createNode();
    newNode->isLeaf = true;
    newNode->parent = NULL;

    // Populate first 2 entries into the 2 groups
    addEntry(node, entryPair.entry1);
    addEntry(newNode, entryPair.entry2);


    int entryNumber = M-1; // Here we have M-1 entries in tempEntries

    for(int i = 0; i < M + 1; i++) 
    { 
        int ecount1 = getEntryCount(node);
        int ecount2 = getEntryCount(newNode);
        
        if(m-ecount1 == entryNumber) // When number of entries < m
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[k].isEmpty == 0)
                {
                    addEntry(node, tempEntries[k]);
                    entryNumber--;
                    tempEntries[k].isEmpty = 1;
                }
            }
            break;
        }
        else if(m-ecount2 == entryNumber) // When number of entries < m
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[k].isEmpty == 0)
                {
                    addEntry(newNode, tempEntries[k]);
                    entryNumber--;
                    tempEntries[k].isEmpty = 1;
                }
            }
            break;
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


twinEntry pickSeeds(Entry *pickEntries) // Function to form 2 groups when a split occurs on a leaf node
{
    long long maxEnlargement = LLONG_MIN;
    int maxi;
    int maxj;
    for (int i = 0; i < M + 1; i++) {
        for (int j = 0; j < M + 1; j++) {
            if (i == j) {
                continue;
            }
            long long area = (abs(pickEntries[i].point.x - pickEntries[j].point.x));
            area *= (abs(pickEntries[i].point.y - pickEntries[j].point.y));
            if (area > maxEnlargement) {
                maxEnlargement = area;
                maxi = i;
                maxj = j;
            }
        }
    }
    twinEntry result; // entry1 and entry2 will form the 2 groups
    result.entry1 = pickEntries[maxi];
    result.entry2 = pickEntries[maxj];
    pickEntries[maxi].isEmpty = 1;
    pickEntries[maxj].isEmpty = 1;
    
    return result;
}

void pickNext(Entry *pickEntries, RTreeNode *node, RTreeNode *newNode) // Function to pick the next entry to be added to leaf node after 2 groups formed
{
    long long maxDiff = LLONG_MIN;
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

            long long d1 = pickNextAux(mbrNode, x, y);
            long long d2 = pickNextAux(mbrNewNode, x, y);

            long long diff = abs(d1-d2);
            if(maxDiff < diff)
            {
                maxDiff = diff;
                entryIndex = i;
                
                if(diff > 0)
                {
                    tempNode = newNode;
                } 
                else 
                {
                    tempNode = node;
                }
            }   
        }
    }

    // tempNode contains the the node with the minEnlargement so add it to that node
    // entryIndex contains the entry which creates maxDiff

    addEntry(tempNode, pickEntries[entryIndex]);
    pickEntries[entryIndex].isEmpty = 1;
}

void pickNextInternal(childEntry *pickEntries, RTreeNode *node, RTreeNode *newNode) // Function to pick the next entry to be added to internal node after 2 groups formed
{
    long long maxDiff = LLONG_MIN;
    int entryIndex = -1;
    RTreeNode* tempNode = NULL;
    
    for(int i = 0; i<M+1; i++)
    {
        if(pickEntries[i].child != NULL)
        {

            MBR mbrNode = calcMbr(node);
            MBR mbrNewNode = calcMbr(newNode);
            long long areaChild1 = getArea(pickEntries[i]);

            int x1 = min(pickEntries[i].mbr.x1, mbrNode.x1);
            int x2 = max(pickEntries[i].mbr.x2, mbrNode.x2);
            int y1 = min(pickEntries[i].mbr.y1, mbrNode.y1);
            int y2 = max(pickEntries[i].mbr.y2, mbrNode.y2);

            long long d1 = (abs(x2 - x1)) * (abs(y2 - y1));
            long long tempd1 = (mbrNode.x2-mbrNode.x1)*(mbrNode.y2 - mbrNode.y1);
            d1 -= areaChild1;
            d1 -= tempd1;
            
            int xk1 = min(pickEntries[i].mbr.x1, mbrNewNode.x1);
            int xk2 = max(pickEntries[i].mbr.x2, mbrNewNode.x2);
            int yk1 = min(pickEntries[i].mbr.y1, mbrNewNode.y1);
            int yk2 = max(pickEntries[i].mbr.y2, mbrNewNode.y2);

            long long d2 = (abs(xk2 - xk1)) * (abs(yk2 - yk1));
            long long tempd2 = (mbrNewNode.x2-mbrNewNode.x1)*(mbrNewNode.y2 - mbrNewNode.y1);
            d2 -= areaChild1;
            d2 -= tempd2;

            long long diff = abs(d1-d2);

            if(maxDiff < diff)
            {
                maxDiff = diff;
                entryIndex = i;
                if(diff > 0) tempNode = newNode;
                else tempNode = node;
            }   
        }
    }

    // tempNode contains the the node with the minEnlargement so add it to that node
    // entryIndex contains the entry which creates maxDiff
    int c = getChildCount(tempNode);
    addchildEntry(tempNode, pickEntries[entryIndex]);
    tempNode->children[c].child->parent = tempNode;
    pickEntries[entryIndex].child = NULL;
}


splitNodes splitInternalNode(RTreeNode *node, childEntry entry) // Function splits the leaf node if an entry needs to be added to a full leaf node
{
    childEntry tempEntries[M+1];
    for (int i = 0; i < M; i++) 
    {
        tempEntries[i] = node->children[i];
        node->entries[i].isEmpty = 1;
        node->children[i].child = NULL;
    }
    tempEntries[M] = entry;
    twinchildEntry entryPair = pickSeedsInternal(tempEntries); // Forms 2 groups
    RTreeNode *newNode = createNode();
    newNode->isLeaf = false;
    newNode->parent = NULL;

    // Populate first 2 entries into the 2 groups
    addchildEntry(node, entryPair.entry1);
    addchildEntry(newNode, entryPair.entry2);
 
    int entryNumber = M-1; // Here we have M-1 entries in tempEntries

    for(int i = 0; i < M + 1; i++) 
    {
        int ecount1 = getChildCount(node);
        int ecount2 = getChildCount(newNode);

        if(m-ecount1 == entryNumber) // When number of entries < m
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[k].child != NULL)
                {
                    addchildEntry(node, tempEntries[k]);
                    entryNumber--;
                    tempEntries[k].child = NULL;
                }
            }
            break;
        }
        else if(m-ecount2 == entryNumber) // When number of entries < m
        {
            for(int k = 0; k<M+1; k++)
            {
                if(tempEntries[k].child != NULL)
                {
                    addchildEntry(newNode, tempEntries[k]);
                    entryNumber--;
                    tempEntries[k].child = NULL;
                }
            }
            break;
        }
        else
        {
            pickNextInternal(tempEntries, node, newNode);
            entryNumber--;
        }
    }

    splitNodes res;
    res.new = newNode;
    res.original = node;
    return res;
}

twinchildEntry pickSeedsInternal(childEntry *pickEntries) // Function to form 2 groups when a split occurs on an internal node
{
    long long maxEnlargement = LLONG_MIN;
    int maxi;
    int maxj;
    for (int i = 0; i < M + 1; i++) {
        for (int j = 0; j < M + 1; j++) {
            if (i == j) {
                continue;
            }
            long long areaChild1 = getArea(pickEntries[i]);
            long long areaChild2 = getArea(pickEntries[j]);

            int x1 = min(pickEntries[i].mbr.x1, pickEntries[j].mbr.x1);
            int x2 = max(pickEntries[i].mbr.x2, pickEntries[j].mbr.x2);
            int y1 = min(pickEntries[i].mbr.y1, pickEntries[j].mbr.y1);
            int y2 = max(pickEntries[i].mbr.y2, pickEntries[j].mbr.y2);

            long long areaEnlarge = (abs(x2 - x1)) * (abs(y2 - y1));
            areaEnlarge -= areaChild1;
            areaEnlarge -= areaChild2;
            if (areaEnlarge > maxEnlargement) {
                maxEnlargement = areaEnlarge;
                maxi = i;
                maxj = j;
            }
        }
    }
    twinchildEntry result; // entry1 and entry2 will form the 2 groups
    result.entry1 = pickEntries[maxi];
    result.entry2 = pickEntries[maxj];
    
    pickEntries[maxi].child = NULL;
    pickEntries[maxj].child = NULL;
    
    return result;
}

void AdjustTree(RTreeNode* node, RTreeNode* splitNode) // Function to update all the MBRs and add a split node (if it exists)
{ 
    
    RTreeNode *parentNode = node->parent;
    

    if (node->id == root->id) // If current node is root
    {
        if (splitNode != NULL) 
        {   
            RTreeNode *newRoot = createNode(); // Create a new root
            newRoot->parent = NULL;
            newRoot->isLeaf = false;
            node->parent = newRoot; // newRoot is parent of node and splitNode
            splitNode->parent = newRoot;

            newRoot->children[0].child = node;
            newRoot->children[0].mbr = calcMbr(node);
            newRoot->children[1].child = splitNode;
            newRoot->children[1].mbr =  calcMbr(splitNode);
            root = newRoot;  // Global root set to newRoot 
        }
        return;
        
    }

    // If node is not the root

    // Adjust the MBR in parent entry
    int indexEntry= findindexEntry(parentNode,node);
    if(indexEntry == -1)
    {
        printf("ERROR: Entry not found\n");
        fflush(stdout);
        return;
    }
    parentNode->children[indexEntry].mbr = calcMbr(node);

    // Propagate nodesplit upwards
    if(splitNode != NULL)
    {

        childEntry newEntry;
        newEntry.child = splitNode;
        newEntry.mbr = calcMbr(splitNode);

        int child_count = getChildCount(parentNode);
        if(child_count == M) // If the internal node is full then split that node
        {
            splitNodes tempNodes = splitInternalNode(parentNode, newEntry); // Call splitInternalNode because internal node being split
            AdjustTree(tempNodes.original, tempNodes.new);
        }
        else // Else just add the entry to internal node
        {
            addchildEntry(parentNode, newEntry);
            splitNode->parent = parentNode;
            AdjustTree(parentNode, NULL);
        }
    }
    else // If no splitNode
    {
        AdjustTree(parentNode, NULL);
    }
}

void preOrderTraversal(RTreeNode* root, int level) // Function to print the preOrderTraversal of the R-Tree
{
    if(root == NULL) return;

    for(int i = 0; i<M; i++)
    {
        if(root->isLeaf)
        {
            if(root->entries[i].isEmpty == 0)
            {
                for(int k = 0; k<level; k++) printf("\t");
                printf("\033[1;32mLevel %d: Leaf node | Index: %d\033[0m\n", level, i);
                for(int k = 0; k<level; k++) printf("\t");
                printf("2D Object is (%d, %d)\n", root->entries[i].point.x, root->entries[i].point.y);
            }
        }
        else
        {
            if(root->children[i].child != NULL)
            {
                for(int k = 0; k<level; k++) printf("\t");
                printf("\033[1;31mLevel %d: Internal node | Index %d\033[0m\n", level, i);
                for(int k = 0; k<level; k++) printf("\t");
                printf("MBR = Top right:(%d, %d) Bottom left:(%d, %d))\n", root->children[i].mbr.x2, root->children[i].mbr.y2, root->children[i].mbr.x1, root->children[i].mbr.y1);
                preOrderTraversal(root->children[i].child, level+1);

            }
        }
    }
}
