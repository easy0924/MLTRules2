/*
 * TreeLatticeCreator.h
 *
 *  Created on: Sep 12, 2011
 *      Author: therrman
 */

#ifndef TREELATTICECREATOR_H_
#define TREELATTICECREATOR_H_

#include "TreeReorderingExtractor.h"
#include "latticecreator.h"

class TreeLatticeCreator : public LatticeCreator {
private:

	double cutoffSyntaxRules;
	int allLengthSyntax;
	int appliedRulesSyntax;
	double allProbsSyntax;

	void AddSyntaxRules(Edge * head, SyntaxTree * tree);
	void ApplySyntaxRulesToPath(vector<vector<Edge*> > edgeSequence, SyntaxTree * tree);
	void insertReorderingForSyntax(vector<vector <Edge*> > &EdgeSequence, map<vector<int> , map <vector<int>, double> > , int);
	map< vector<int>, map < vector<int>, double> > checkTreeForReorderings(SyntaxTree * tree, map< vector<int>, map <vector<int>, double> > list);
	map<vector<int> , map <vector<int>, double> > getReorderingForSyntax(vector<vector <Edge*> > &EdgeSequence, SyntaxTree * tree);

	Edge * checkLattice(Edge * e,string txt, string tags, string tree);




public:
	TreeLatticeCreator(double cutoffContinousRules,double cutoffDiscontinousRules, double cutoffSyntaxRules, int maxUseOfRule,int applyRulesRecursive);
	virtual ~TreeLatticeCreator();

	void createLatticesForText(string & textFileName, string & tagFileName, string & treeFileName, string & destinationDir);

	void oracleReorderText(string & textFileName, string & tagFileName, string & treeFileName, string & destFile,string & refFile);
	void createNGramList(string & textFileName, string & tagFileName, string & treeFileName);

	Edge * createLatticeForSentence(string txt, string tags, string tree);

};

#endif /* TREELATTICECREATOR_H_ */
