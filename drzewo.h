#ifndef DRZEWO_H
#define DRZEWO_H

typedef struct Node {
    int key;
    char name[100];
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(int key, const char* name);
void insertNode(Node** root, int key, const char* name);
void removeNode(Node* root);
void writeToFile(Node* root, FILE* file);
void readFromFile(Node** root, const char* filename);
int drzewo_max(Node* root);


#endif
