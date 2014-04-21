#include "TrainMLTReordering.h"

MLTReorderingExtractor::MLTReorderingExtractor() {
    rules.clear();
    lefthand.clear();
}

MLTReorderingExtractor::~MLTReorderingExtractor() {
}

void MLTReorderingExtractor::extractFromSentences(char* alignment, char* source, char* sourceTrees, int layers) {
    ifstream ali, srt;
    string sa, st;

    ali.open(alignment);
    srt.open(sourceTrees);

    time_t parse_time = 0;
    time_t extract_time = 0;
    time_t tmp;
    char ch;

    while (getline(ali, sa) && getline(srt, st)) {
        markrange(sa, left, right);
        tmp = time(NULL);
        TreeNode* tree = TreeNode::parseSentence(st, left, right);
        parse_time += time(NULL) - tmp;

     //   tree -> print();
    //    scanf("%c", &ch);
        tmp = time(NULL);

        //map<pair<string, string>, int> rules_tmp;

        tree -> extractRules(rules, lefthand, layers);
        extract_time += time(NULL) - tmp;
  //      rules.insert(rules_tmp.begin(), rules_tmp.end());

        //outputRules(rules_tmp, cerr);
    }

    cerr << "Parse time:   " << parse_time << endl;
    cerr << "Extract time: " << extract_time << endl;
}

void MLTReorderingExtractor::markrange(string ali, map<int, int> &left, map<int, int> &right) {
    istringstream in(ali);
    int x,y;
    string pairs;

    left.clear();
    right.clear();
    while (in >> pairs) {
        sscanf(pairs.c_str(), "%d-%d", &x, &y);
        x--, y--;
        if (left.find(x) == left.end()) {
            left[x] = right[x] = y;
        } else {
            left[x] = min(left[x], y);
            right[x] = max(right[x], y);
        }
    }
}

void MLTReorderingExtractor::pruneRules(int minOcc) {
    cerr << minOcc <<endl;
    for (map<pair<string, string>, int>::iterator i = rules.begin(); i != rules.end();) {
//        cerr << (*i).second << endl;
        if ((*i).second < minOcc) {
        //    lefthand[(*i).first.first] -= (*i).second;
            rules.erase(i++);
        } else {
            ++i;
        }
    }
}

void MLTReorderingExtractor::normalizeRules(char* sourceTrees, char* alignment, char* source) {
    score.clear();
    for (map<pair<string, string>, int>::iterator i = rules.begin(); i != rules.end(); ++i)
        score[(*i).first] = (*i).second * 1.0 / lefthand[(*i).first.first];
}

void MLTReorderingExtractor::outputRules(const map<pair<string, string>, int> &rules, ostream &out) {
    out << "@MLT_REORDERING" << endl;
    for (map<pair<string, string>, int>::const_iterator i = rules.begin(); i != rules.end(); ++i)
        out << fixed << setprecision(8) << (*i).first.first << " ---- " << (*i).first.second << " ---- " << score[(*i).first] << " ---- " << rules.at((*i).first) << " / " << lefthand[(*i).first.first] << endl;
}

int main (int args, char* argv[]) {
    time_t start = time(NULL);
    cerr << argv[0] <<" is used to train MLT Reorderings" << endl;
    if (args != 6) {
        cerr << "usage: <Alignment> <SourceText> <SourceTrees> <NumberOfLayers> <minOcc>\n";
        exit(1);
    }

    char* alignment = argv[1];
    char* source = argv[2];
    char* sourceTrees = argv[3];
    int layers = atoi(argv[4]);
    int minOcc = atoi(argv[5]);

    MLTReorderingExtractor ex;

    cerr << "starting to extract" << endl;
    cerr << "ExtractRules..." << endl;
    ex.extractFromSentences(alignment,source,sourceTrees, layers);
    cerr << "done" << endl;
    cerr << "Pruning Rules..." << endl;
    ex.pruneRules(minOcc);
    cerr << "done" << endl;

    cerr << "Normalize Rules..." << endl;
    ex.normalizeRules(sourceTrees, alignment, source);
    cerr << "done" << endl;

    cerr << "Output Rules..." << endl;
    ex.outputRules(ex.rules, cout);
    cerr << "done" << endl;

    cerr << "Runing time: " << difftime(time(NULL), start) << "s" << endl;
    cerr << "Total # of nodes: " << TreeNode::tcnt << endl;
    return 0;
}
