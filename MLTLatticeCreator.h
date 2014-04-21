#ifndef MLTATTICECREATOR_H
#define MLTLATTICECREATOR_H

#include "TreeNode.h"
#include "GraphNode.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <cmath>
#include <utility>

#define RULE_APPLICATION_THRESHOLD 0.1000000

class MLTLatticeCreator {
private:
    map<string, pair<vector<int>, double> > rules;
    map<int, int> dummy;
    int layers;
//	double cutoffSyntaxRules;
//	int allLengthSyntax;
//	int appliedRulesSyntax;
//	double allProbsSyntax;
//
//	void AddSyntaxRules(Edge * head, SyntaxTree * tree);
//	void ApplySyntaxRulesToPath(vector<vector<Edge*> > edgeSequence, SyntaxTree * tree);
//	void insertReorderingForSyntax(vector<vector <Edge*> > &EdgeSequence, map<vector<int> , map <vector<int>, double> > , int);
//	map< vector<int>, map < vector<int>, double> > checkTreeForReorderings(SyntaxTree * tree, map< vector<int>, map <vector<int>, double> > list);
//	map<vector<int> , map <vector<int>, double> > getReorderingForSyntax(vector<vector <Edge*> > &EdgeSequence, SyntaxTree * tree);
//
//	Edge * checkLattice(Edge * e,string txt, string tags, string tree);
    void createLatticeForSentence(string txt, string tags, string tree, const char* destFile);

public:
	MLTLatticeCreator(char* rulesFile, int layers);
	virtual ~MLTLatticeCreator();
	void createLatticesForText(char* textFileName, char* tagFileName, char* treeFileName, char* destinationDir);
//	void oracleReorderText(string & textFileName, string & tagFileName, string & treeFileName, string & destFile,string & refFile);
//	void createNGramList(string & textFileName, string & tagFileName, string & treeFileName);
};

#endif
