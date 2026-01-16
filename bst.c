#include <stdlib.h>
#include "bst.h"

#define TRUE 1
#define FALSE 0


BST* createBST(int (*cmp)(void*, void*), void (*print)(void*), void (*freeData)(void*)){
    BST *binarySearchTree;
    BSTNode *treeRoot;
    binarySearchTree = malloc(sizeof(BST));
    treeRoot = malloc(sizeof(BSTNode));

    /* memory error exeption */
    if((binarySearchTree == NULL) || (treeRoot == NULL)){
        printf("we have encountered a memory allocation error when trying to create the tree\n");
        exit(1);
    }

    /* reseting root */
    treeRoot -> data = NULL;
    treeRoot -> left = NULL;
    treeRoot -> right = NULL;

    /* asigning functions */
    binarySearchTree -> compare = cmp;
    binarySearchTree -> print = print;
    binarySearchTree -> freeData = freeData;

    binarySearchTree -> root = treeRoot;
    
    return binarySearchTree;
}


BSTNode* bstInsert(BSTNode* root, void* data, int (*cmp)(void*, void*)){
    BSTNode *curNode, *treeNode;
    treeNode = malloc(sizeof(BSTNode));
    curNode = root;

    treeNode -> left = NULL;
    treeNode -> right = NULL;
    treeNode -> data = data;

    if (curNode -> data == NULL){
        free(treeNode);
        curNode -> data = data;
        return curNode;
    }

    while (TRUE){

        /* in case there is already an object of equel value */
        if (cmp(curNode -> data, data) == 0){
            free(treeNode);
            return NULL;
        }

        /* smaller then current node*/
        if (cmp(curNode -> data, data) > 0){
            if(curNode -> left == NULL){
                curNode -> left = treeNode;
                return treeNode;
            }
            
            curNode = curNode -> left;
            continue;
        }

        /* bigger then current node */
        if (cmp(curNode -> data, data) < 0){
            if(curNode -> right == NULL){
                curNode -> right = treeNode;
                return treeNode;
            }

            curNode = curNode -> right;
            continue;
        }
    }
}


void* bstFind(BSTNode* root, void* data, int (*cmp)(void*, void*)){
    if (isEmpty(root)){
        return NULL;
    }

    if (cmp(root -> data, data) > 0)
        return bstFind(root -> left, data, cmp);

    if (cmp(root -> data, data) < 0)
        return bstFind(root -> right, data, cmp);

    return root;
}


void bstInorder(BSTNode* root, void (*print)(void*)){
    if (isEmpty(root))
        return;

    bstInorder(root -> left, print);

    print(root -> data);

    bstInorder(root -> right, print);
}


void bstPreorder(BSTNode* root, void (*print)(void*)){
    if (isEmpty(root))
        return;

    print(root -> data);

    bstPreorder(root -> left, print);

    bstPreorder(root -> right, print);
}


void bstPostorder(BSTNode* root, void (*print)(void*)){
    if (isEmpty(root))
        return;

    bstPostorder(root -> left, print);

    bstPostorder(root -> right, print);

    print(root -> data); 
}


void bstFree(BSTNode* root, void (*freeData)(void*)){
    if(root == NULL){
        return;
    }

    /* going through children */
    bstFree(root -> left, freeData);

    bstFree(root -> right, freeData);

    /* freeing self */
    freeData(root -> data);
    free(root);
}


int isEmpty(BSTNode* root){
    if (root == NULL)
        return TRUE;

    if(root -> data == NULL)
        return TRUE;

    return FALSE;
}