#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drzewo.h"

Node* createNode(int key, const char* name) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (NULL != newNode) {
        newNode->key = key;
        strcpy_s(newNode->name, sizeof(newNode->name), name);
        newNode->left = NULL;
        newNode->right = NULL;
    }
    return newNode;

}

void insertNode(Node** root, int key, const char* name) {
    if (*root == NULL) {
        *root = createNode(key, name);
        return;
    }

    if (key < (*root)->key) {
        insertNode(&(*root)->left, key, name);
    }
    else if (key > (*root)->key) {
        insertNode(&(*root)->right, key, name);
    }
}

void removeNode(Node* root) {
    if (root == NULL)
        return;

    removeNode(root->left);
    removeNode(root->right);
    free(root);
}

void writeToFile(Node* root, FILE* file) {
    if (root != NULL) {
        writeToFile(root->right, file);
        fprintf(file, "%s: %d\n", root->name, root->key);
        writeToFile(root->left, file);
    }
}

void readFromFile(Node** root, const char* plik_name) {
    FILE* file;
    if (fopen_s(&file, plik_name, "r") != 0) {
        printf("Nie udalo sie otworzyc pliku.\n");
        return;
    }
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char name[50];
        int key;
        if (sscanf_s(line, "%[^:]: %d", name, sizeof(name), &key) == 2)
            insertNode(root, key, name);
    }
    fclose(file);
}

int drzewo_max(Node* root) {
    while (root && root->right)
        root = root->right;
    return root->key;
}