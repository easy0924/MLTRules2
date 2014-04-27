#include "MLTLatticeCreator.h"

MLTLatticeCreator::MLTLatticeCreator(char* ruleFile, int layers0) {
    layers = layers0;
    dummy.clear();
    rules.clear();
    for (int i = 0; i < 10000; ++i)
        dummy.insert(make_pair(i, i));

    string line, lh;
    ifstream in(ruleFile);
    int nrules = 0;
    while (getline(in, line)) {
        int pos = line.find("----");
        lh = line.substr(0, pos - 1);
        int pos2 = line.find("----", pos + 1);
        istringstream ord(line.substr(pos + 4, pos2 - pos - 4));
        vector<int> rh;
        rh.clear();
        int x;
        while (ord >> x) rh.push_back(x);
        istringstream prob(line.substr(pos2 + 4));
        double y;
        prob >> y;

        if (y <= RULE_APPLICATION_THRESHOLD - EPS)
            continue;
        cerr << "adding rule to lattice creator: " << endl;
        if (rules.find(lh) == rules.end())
            rules[lh] = RIGHT_RULES();
        rules[lh].insert(make_pair(rh, y));
        cerr << "|" + lh + "|" << line.substr(pos + 4, pos2 - pos - 4) + "|" << line.substr(pos2 + 4) + "|" << endl;
        nrules++;
    }
    cerr << nrules << " Rules readed" << endl;
}

MLTLatticeCreator::~MLTLatticeCreator() {
}

void MLTLatticeCreator::createLatticesForText(char* textFileName, char* tagFileName, char* treeFileName, char* destinationDir) {
	ifstream txtReader(textFileName);
	ifstream treeReader(treeFileName);

	string text;
	string tree;
	string tags = "";
	char buffer[10];

	ofstream Lattices;
	string destDir(destinationDir);
	Lattices.open((destDir+"/Lattices.lst").c_str());

	long latticeCounter=0;
    while (getline(txtReader, text) && getline(treeReader, tree)) {
        sprintf(buffer, "%ld", latticeCounter);
        string latname(buffer);
        Lattices << destDir << "/" << latticeCounter << ".lat" << endl;
        createLatticeForSentence(text, tags, tree, (destDir + "/" + latname + ".lat").c_str());
        cerr << destDir << "/" << latticeCounter << ".lat finished" << endl;
        latticeCounter++;
	}

	Lattices.close();
	//cerr << applyedRules << " rules applyed with avg. lenght of: " << 1.0*allLength/applyedRules << endl;
	//cerr << appliedRulesSyntax << " Syntax rules were applied with average Prob " << allProbsSyntax/appliedRulesSyntax << " and average length " << 1.0*allLengthSyntax/appliedRulesSyntax << endl;
};

void MLTLatticeCreator::createLatticeForSentence(string txt, string tags, string tree, const char* destFile) {
    ofstream latFile;
    latFile.open(destFile, ofstream::out);
    TreeNode* tre = TreeNode::parseSentence(tree, dummy, dummy);
    vector<GraphNode*> mpath = GraphNode::parseSentence(txt);
    tre -> applyRules(rules, mpath, layers);
    GraphNode::outputLattice(latFile, mpath[0]);
    latFile.close();
}

int main (int args, char* argv[]) {
    time_t start = time(NULL);
    cerr << argv[0] << " is used to create the lattices" << endl;

    if (args != 7) {
        cerr << "usage: <RuleFile> <SourceText> <SourceTag> <SourceTrees> <TargetDir> <NumberOfLayers>" << endl;
        exit(1);
    }
    cerr << "rules used: " << argv[1] << endl;

    char* rulesFile = argv[1];
    char* source = argv[2];
    char* sourceTags = argv[3];
    char* sourceTrees = argv[4];
    char* targetDir = argv[5];
    int layers = atoi(argv[6]);

    MLTLatticeCreator lc(rulesFile, layers);

    cerr << "starting to create lattice" << endl;
    lc.createLatticesForText(source, sourceTags, sourceTrees, targetDir);

    cerr << "Runing time: " << difftime(time(NULL), start) << "s" << endl;
    return 0;
}
