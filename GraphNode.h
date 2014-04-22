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

using namespace std;

#define MAX_LEN 100000000
#define FULL_PROB 1.000000
#define MINIMUM_PROB 0.500000

typedef map<string, map<vector<int>, double> > RULES;
typedef pair<string, pair<vector<int>, double> > RULE;
typedef map<vector<int>, double> RIGHT_RULES;

class GraphNode;

class Edge {
public:
    string word;
    int pos;
    double prob;
    string comment;
    GraphNode *target;

    Edge(string word, int pos, double prob, string comment, GraphNode *target);
    ~Edge();
};

class GraphNode {
public:
    int id;
    vector<Edge> next;

    GraphNode();
    GraphNode(Edge);
    ~GraphNode();

    static string getComment(const RULE &rule);
    static vector<GraphNode*> parseSentence(string txt);
    static void addBranch(vector<GraphNode*> &mpath, int startp, int endp, const vector<int> &branch, double prob, const RULE &rule);
    static void outputLattice(ostream &out, GraphNode* start);

};

#endif

