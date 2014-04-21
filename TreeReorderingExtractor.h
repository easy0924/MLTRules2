#ifndef TREEREORDERINGEXTRACTOR_H
#define TREEREORDERINGEXTRACTOR_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <map>
#include <set>
#include <cstdlib>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "_SuffixArraySearchApplicationBase.h"
#include "SyntaxTree.h"

#define MAX_TARGET_WORDS 10000
using namespace std;

typedef int MyTyp;

typedef map< pair<string,string>,double >::iterator RuleIterator;

/**
@author Jan Niehues, Teresa Herrmann
*/
class TreeReorderingExtractor{
private:
	vector<string> words;
	vector<string> tags;
	SyntaxTree * tree;
	int * maxAlignment;
	int * minAlignment;

	int sentenceNumber;

	map<pair<string,string>,double > reorderings;
	map<pair<string,string>,double > reorderingsPartial;
	map<string, int> counts;
	map<string, int> countsPartial;

	map<string,pair<string,double> > reorderingsLeft;
	map<string,pair<string,double> > reorderingsLeftShort;
	map<string,pair<string,double> > reorderingsRight;
	map<string,pair<string,double> > reorderingsRightShort;


	void LoadTextLine(const string line,vector<string> & w);
	SyntaxTree * LoadTreeLine(const string line,int & pos, int & index);
	void LoadAlignment(const string alignmentLine);
	void FindDistortions(SyntaxTree * tree);
	void InitAlignment();

	void pruneRules(map<pair<string,string>,double > & rules, map<string,int>  & c, int minOcc);
	void outputRules(map<pair<string,string>,double > & rules, map<string,int> & c, ostream & out);
	
	string BuildLeftReorderingString(int l);
	string BuildLeftShortReorderingString(int l);
	string BuildRightReorderingString(int l);
	string BuildRightShortReorderingString(int l);


	pair<string,string> getRuleParts(const string rule);
	int findOccurences(vector<S_SimplePhraseLocationElement> & locationsOne, vector<S_SimplePhraseLocationElement> &locationsTwo);

	void extractCounts(SyntaxTree * tree);

public:
    TreeReorderingExtractor();

    ~TreeReorderingExtractor();
	void extractFromSentences(char * alignment, char * source, char * sourceTrees);

	void pruneRules(int minOcc);
	void outputRules(ostream & out);

	void normalizeRules(char * sourceTreeFileName, char * alignmentFileName, char * sourceFileName);

	SyntaxTree * LoadTreeLineLight(const string line,int & pos, int & index);

};

#endif
