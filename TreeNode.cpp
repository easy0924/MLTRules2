#include "TreeNode.h"

vector<string> TreeNode::seq;
int TreeNode::len, TreeNode::n_count;
map<int, int> TreeNode::left_, TreeNode::right_;

int TreeNode::tcnt = 0;

TreeNode::TreeNode() {
    tcnt ++;
    label = "";
    word = "";
    child.clear();
    pos = right = -1;
    left = MAX_LEN;
}

TreeNode::~TreeNode() {
}

bool TreeNode::cmp(const pair<TreeNode*, int> &x, const pair<TreeNode*, int> &y) {
    return (x.first -> left < y.first -> left) || ((x.first -> left == y.first -> left) && (x.second < y.second));
}

int TreeNode::parse(TreeNode* r, int idx) {
    while (idx < len) {
        if (seq[idx] == "(") {
            r -> child.push_back(new TreeNode());
            idx = parse(r -> child.back(), idx + 1);
            r -> left = min(r -> left, r -> child.back() -> left);
            r -> right = max(r -> right, r -> child.back() -> right);
        } else if (seq[idx] == ")")
            return idx;
        else if (r -> label == "")
            r -> label = seq[idx];
        else {
            r -> word = seq[idx];
            r -> left = left_.find(n_count) == left_.end()? 0: left_[n_count];
            r -> right = right_.find(n_count) == right_.end()? MAX_LEN: right_[n_count];
            r -> pos = n_count ++;
        }
        idx += 1;
    }
}

int snt_cnt = 0;

TreeNode* TreeNode::parseSentence(string s, const map<int, int> &left, const map<int, int> &right) {
    snt_cnt++;
    if (snt_cnt % 5000 == 0) {
        cerr << "Processing sentence " << snt_cnt << endl;
    }

//    if (snt_cnt == 518) {
//        for (map<int, int>::const_iterator i = left.begin(); i != left.end(); ++i)
//            cerr << "left: " << (*i).first << "-" << (*i).second << endl;
//        for (map<int, int>::const_iterator i = right.begin(); i != right.end(); ++i)
//            cerr << "right: " << (*i).first << "-" << (*i).second << endl;
//
//
//    }
    TreeNode* root = new TreeNode();
    seq = stringToSequence(s);
    len = (int) seq.size();
    n_count = 0;
    left_ = left;
    right_ = right;
    int tmp = parse(root, 1);
//    cerr << s << endl;
//    for (map<int, int>::const_iterator i = left.begin(); i != left.end(); ++i) {
//        cerr << "L range " << i -> first << " " << i -> second << endl;
//        cerr << "R range " << i -> first << " " << i -> second << endl;
//    }

    if (tmp + 1 != len)
        cerr << "Sentence not parsed completely. Error at sentence position " << tmp << len << endl;
    return root;
}

vector<string> TreeNode::stringToSequence(string s) {
    vector<string> res;
    string ns = "";

    res.clear();
    int l = s.size();
    for (int i = 0; i < l; ++i)
        if (s[i] == '(')
            ns += " ( ";
        else if (s[i] == ')')
            ns += " ) ";
        else
            ns += s[i];

    istringstream in(ns);
    string seg;
    while (in >> seg)
        res.push_back(seg);

    return res;
}

void TreeNode::extractRules(map<pair<string, string>, int> &rules, map<string, int> &lefthand, int layer) {
//    if (snt_cnt == 518) {
//        cerr << label << " " << word << " " << left << " " << right << endl;
//    }

    for (vector<TreeNode*>::iterator i = child.begin(); i != child.end(); ++i)
        if ((*i) -> word == "")
            (*i) -> extractRules(rules, lefthand, layer);
    if (child.size() < 2)
        return;
    string lh = "";
    ostringstream out("");
    vector<pair<TreeNode*, int> > rh;
    rh.clear();
    int xcnt = 0;
    getRules(lh, rh, xcnt, layer);
    sort(rh.begin(), rh.end(), TreeNode::cmp);

    //bool unaligned = false;
    //for (vector<pair<TreeNode*, int> >::iterator i = rh.begin(); i != rh.end(); ++i) {
//    cerr << "----------" << endl;
    for (int i = 0; i < rh.size(); ++i) {
        //cerr << (*i).first -> label << " " <<
        //if ((*i).first -> right == -1) {
            //unaligned = true;
            //break;
//            cerr << "Warning: word " << (*i).first -> word << " at snt " << snt_cnt << " loc " << (*i).first -> pos << " is not aligned." << endl;
        //}
 //       cerr << rh[i].first -> label << " " << rh[i].first -> word << " " << rh[i].first -> left << " " << rh[i].first -> right << endl;
    }
//    if (unaligned)
//        cerr << "Warning: sentence " << snt_cnt << " unaligned." << endl;

    bool ordered = false;
    bool crossed = (rh[0].first -> left > rh[0].first -> right);
    out << rh[0].second;

    //for (vector<pair<TreeNode*, int> >::iterator i = rh.begin() + 1; i != rh.end(); ++i) {
    int rhlen = rh.size();
    for (int i = 1; i < rhlen; ++i) {
        out << " " << rh[i].second;
        ordered |= (rh[i-1].second > rh[i].second);
        if ((rh[i-1].first -> right > rh[i].first -> left) || (rh[i].first -> left > rh[i].first -> right)) {
            crossed = true;
        }
    }


//    cerr << flag << " " << ordered << endl;
//    cerr << lh << " ---- "u << out.str() << endl;

//    if (lh == "NP( NP PP)") {
//        cerr << "tracking: " << lh << " " << out.str() << " " << left << " " << right << " " <<flag << " " << ordered << endl;
//    }

//    if (lh.find("ADJP , ADJP") != string::npos)
//        cerr << "tracking: " << lh << " " << out.str() << " " << left << " " << right << " " <<flag << " " << ordered << endl;

    pair<string, string> rule = make_pair(lh, out.str());
//     if (lh == "X( X NP)") {
//            cerr << snt_cnt << endl;
//            for (int i = 0 ; i < rh.size(); ++i)
//                cerr << rh[i].first -> label << " " << rh[i].first -> word << " " << rh[i].first -> left << " " << rh[i].first -> right << " " << rh[i].second << endl;
//        }
    if (!crossed){  // && rh.size() > 1) {


//        if (lh == "ADJP ( $ CD )") {
//            cerr << "-------------" << endl;
//            for (int i = 0; i < rh.size(); ++i)
//                cerr << rh[i].first -> label << " " << rh[i].first -> word << " " << rh[i].first -> left << " " << rh[i].first -> right << endl;
//        }

        if (lefthand.find(lh) != lefthand.end())
            lefthand[lh] += 1;
        else
            lefthand[lh] = 1;
        if (ordered) {
            if (rules.find(rule) != rules.end())
                rules[rule] += 1;
            else
                rules[rule] = 1;
        }
    }
}

// default version: leaf node provides tag
// alternative: leaf node provides word
void TreeNode::getRules(string &lh, vector<pair<TreeNode*, int> > &rh, int &xcnt, int layer) {
//    cerr << label << " " << child.size() << " " << layer << endl;
    if (word != "" || (layer <= 0 )) {// && child.size() > 1)) {
        lh += label;
        rh.push_back(make_pair(this, xcnt++));
    } else {
        lh += label + "(";
        for (vector<TreeNode*>::iterator i = child.begin(); i != child.end(); ++i) {
            lh += " ";
            (*i) -> getRules(lh, rh, xcnt, layer - 1);// - (child.size() > 1));
        }
        lh += ")";
    }
}

void TreeNode::print() const {
    cerr << "[ " << label << " " << word << " " << pos << " " << left << " " << right << " ] ";
    if (!child.empty()) {
        cerr << "( ";
        for (vector<TreeNode*>::const_iterator i = child.begin(); i != child.end(); ++i)
            (*i) -> print();
        cerr << " ) ";
    }
}

void TreeNode::void applyRules(map<string, pair<vector<int>, double> > &rules, vector<GraphNode*> &mpath, int layers) {

    for (vector<TreeNode*>::iterator i = child.begin(); i != child.end(); ++i)
        if ((*i) -> word == "")
            (*i) -> applyRules(rules, mpath, layer);

    if (child.size() < 2)
        return;

    string lh = "";

    vector<pair<TreeNode*, int> > rh;
    rh.clear();
    int xcnt = 0;
    getRules(lh, rh, xcnt, layer);
    sort(rh.begin(), rh.end(), TreeNode::cmp);

    if (rules.find(lh) != rules.end()) {
        vector<int> ord = rules[lh].first;
        double prob = rules[lh].second;
        vector<int> branch;
        branch.clear();
        for (vector<int>::iterator i = ord.begin(); i != ord.end(); ++i) {
            for (int j = (*i)->left; j != (*i)->right; ++j)
                branch.push_back(j);
        }
        GraphNode::addBranch(mpath, left, right, branch, prob, make_pair(lh, rules[lh]));
    }
}
