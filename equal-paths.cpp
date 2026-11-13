#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#include <climits>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
void getDepths(Node* root, int depth, int& minDepth, int& maxDepth)
{
    if (root == NULL) {
        return;
    }

    if (root->left == NULL && root->right == NULL) {
        if (depth < minDepth) {
            minDepth = depth;
        }
        if (depth > maxDepth) {
            maxDepth = depth;
        }
        return;
    }

    getDepths(root->left, depth + 1, minDepth, maxDepth);
    getDepths(root->right, depth + 1, minDepth, maxDepth);
}

bool equalPaths(Node* root)
{
    if (root == NULL) {
        return true;
    }

    int minDepth = INT_MAX;
    int maxDepth = 0;

    getDepths(root, 0, minDepth, maxDepth);

    return minDepth == maxDepth;
}
