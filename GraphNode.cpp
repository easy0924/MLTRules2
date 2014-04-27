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
    while (in >> word) {
        idx ++;
        res.push_back(new GraphNode());
        res[idx - 1]->next.push_back(Edge(word, idx - 1, FULL_PROB, "Monoton", res[idx]));
    }

    return res;
}

void GraphNode::outputLattice(ostream &out, GraphNode* start) {
    int cnt = 0;

    queue<GraphNode*> q;
    vector<GraphNode*> lst;
    lst.clear();
    start->id = cnt++;
    q.push(start);
    lst.push_back(start);
    while (!q.empty()) {
        GraphNode *h = q.front();
        q.pop();
        for (vector<Edge>::iterator i = h -> next.begin(); i != h -> next.end(); ++i) {
            if ((*i).target -> id == -1) {
                (*i).target->id = cnt ++;
                q.push((*i).target);
                lst.push_back((*i).target);
            }
        }
    }
    for (vector<GraphNode*>::iterator i = lst.begin(); i != lst.end(); ++i) {
        for (vector<Edge>::iterator j = (*i) -> next.begin(); j!= (*i) -> next.end(); ++j) {
            out << ((*i) -> id) << " " << (((*j).target) -> id) << " " << (*j).word << " " << (*j).pos + 1 << " " << (*j).prob << (((*j).comment == "")?"": " # " + (*j).comment) << endl;
        }
    }
}

void GraphNode::addBranch(vector<GraphNode*> &mpath, int startp, int endp, const vector<int> &branch, double prob,const RULE &rule) {
//    for (int i = 0; i + 1< mpath.size(); ++i)
//        cerr << mpath[i]->next[0].word << " ";
//    cerr << endl;
//    cerr << branch.size();
//    for (int i = 0; i < branch.size(); ++i)
//        cerr << branch[i] << " " << endl;
//    cerr << endl;
    GraphNode* prev = mpath[endp + 1];
    for (vector<int>::const_reverse_iterator i = branch.rbegin(); i + 1 != branch.rend(); ++i) {
        GraphNode* x = new GraphNode(Edge(mpath[(*i)]->next[0].word, (*i), FULL_PROB, "", prev));
        prev = x;
    }
    mpath[startp]->next.push_back(Edge(mpath[branch[0]]->next[0].word, branch[0], prob, getComment(rule), prev));
    mpath[startp]->next[0].prob = max(MINIMUM_PROB, mpath[startp]->next[0].prob - prob);
}

string GraphNode::getComment(const RULE &rule) {
    ostringstream res;
    res << "{'MLTRule': '" + rule.first + "#";
    res << rule.second.first[0];
    for (vector<int>::const_iterator i = rule.second.first.begin() + 1; i != rule.second.first.end(); ++i)
        res << "_" << (*i);
    res << "', 'WEIGHT': " << rule.second.second << "}";
    return res.str();
}

