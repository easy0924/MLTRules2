#include "GraphNode.h"

Edge::Edge(string word0, int pos0, double prob0, string comment0, GraphNode *target0) {
    word = word0;
    pos = pos0;
    prob = prob0;
    comment = comment0;
    target = target0;
}

Edge::~Edge() {
}

GraphNode::GraphNode() {
    id = -1;
    next.clear();
}

GraphNode::GraphNode(Edge e) {
    id = -1;
    next.clear();
    next.push_back(e);
}

GraphNode::~GraphNode() {
}

vector<GraphNode*> GraphNode::parseSentence(string txt) {
    vector<GraphNode*> res;
    res.clear();
    res.push_back(new GraphNode());

    istringstream in(txt);
    string word;
    int idx = 0;
    while (cin >> word) {
        idx ++;
        res.push_back(new GraphNode());
        res[idx - 1].next.push_back(Edge(word, idx - 1, FULL_PROB, "Monoton", res[idx]));
    }

    return res;
}

void GraphNode::outputLattice(ofstream ofs, GraphNode* start) {
    int cnt = 0;

    queue<GraphNode*> q;
    vector<GraphNode*> lst;
    q.clear();
    lst.clear();
    q.push(start);
    lst.push_back(start);
    while (!q.empty()) {
        GraphNode *h = q.pop();
        if (h -> id == -1) {
            h -> id = cnt ++;
        }
        for (vector<Edge>::iterator i = h -> next.begin(); i != h -> next.end(); ++i) {
            if ((*i).target -> id == -1)
                q.push((*i).target);
                lst.push_back((*i).target);
        }
    }
    for (vector<GraphNode*>::iterator i = lst.begin(); i != lst.end(); ++i) {
        for (vector<Edge>::iterator j = (*i) -> next.begin(); j!= (*i) -> next.end(); ++j) {
            ofs << (*i) -> id << " " << ((*j).target) -> id << " " << (*j).word << " " << (*j).pos << " " << (*j).prob << " # " << (*j).comment << endl;
        }
    }
}

void GraphNode::addBranch(vector<GraphNode*> &mpath, int st, int en, const vector<int> &branch, double prob, pair<string, pair<vector<int>, double> > &rule) {
    GraphNode* prev = mpath[right + 1];
    for (vector<int>::reverse_iterator i = branch.rbegin(); i != branch.rend(); ++i) {
        GraphNode* x = new GraphNode(Edge(mpath[(*x)]->next[0].word, (*x), FULL_PROB, getComment(rule), prev));
        prev = x;
    }
    branch[left]->next.push_back(prev);
}

string GraphNode::getComment(pair<string, pair<vector<int>, double> > &rule) {
    ostringstream res;
    res << "{'MLTRule': '" + rule.first + "#";
    res << rule.second.first[0];
    for (vector<int>::iterator i = rule.second.first.begin(); i != rule.second.first.end(); ++i)
        res << "_" + (*i);
    res << "', 'WEIGHT': " << rule.second.second;
    return res.str();
}

