#ifndef TREENODE_H
#define TREENODE_H

#define MAX_LEN 100000000

#include <utility>
#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <algorithm>
#include "GraphNode.h"

using namespace std;

class TreeNode {
public:
    string label, word;
    int pos, left, right;
    vector<TreeNode*> child;

    static int tcnt;
    static vector<string> seq;
    static int len, n_count;
    static map<int, int> left_, right_;

    static int parse(TreeNode*, int);
    static vector<string> stringToSequence(string);
    static bool cmp(const pair<TreeNode*, int>&, const pair<TreeNode*, int>&);

    void getRules(string&, vector<pair<TreeNode*, int> >&, int&, int);

    TreeNode();
    ~TreeNode();

    static TreeNode* parseSentence(string, const map<int, int>&, const map<int, int>&);

    void extractRules(map<pair<string, string>, int>&, map<string, int>&, int);
    void applyRules(map<string, pair<vector<int>, double> >&, vector<GraphNode*>&, int);
    void print() const;
};

#endif
