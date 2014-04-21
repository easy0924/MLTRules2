#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H


#include <cstring>
#include<vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

class SyntaxTree{

private:

	bool leaf;


public:
	string head;
	vector<SyntaxTree *> children;
	int start_source;
	int start_target;
	int end_source;
	int end_target;
	SyntaxTree();
	~SyntaxTree();
	SyntaxTree * findFirstLeaf();
	inline bool isLeaf() {return leaf;};
	inline void setLeaf(bool b) {leaf = b;};
	void output();
	void output_pretty();
	void output_sentence();
	int freqOfConstituent(string constituent);
};

#endif
