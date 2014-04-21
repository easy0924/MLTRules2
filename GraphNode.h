#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <utility>
#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <queue>
#include <deque>
#include <vector>
#include <algorithm>

#define MAX_LEN 100000000
#define FULL_PROB 1.000000
#define MIN_MONO_PROB 0.500000

using namespace std;

class GraphNode;

class Edge {
public:
    string word;
    int pos;
    double prob;
    string comment;
    GraphNode *target;

    Edge();
    ~Edge();
};

class GraphNode {
public:
    int id;
    vector<Edge> next;
    GraphNode();
    GraphNode(Edge);
    ~GraphNode();

    static vector<GraphNode*> parseSentence(string txt);
    static void addBranch(vector<GraphNode*> &mpath, int, int, const vector<int> &branch, double prob, pair<string, pair<vector<int>, double> > &rule);
    static void outputLattice(ofstream ofs, GraphNode* start);
    static string getComment(pair<string, pair<vector<int>, double> > &rule);
};

#endif
