/*
 * TrainTreeReordering.cpp
 *
 *  Created on: Oct 26, 2010
 *      Author: therrman
 */
#include "TreeReorderingExtractor.h"
#include "SyntaxTree.h"
#include <cstring>
#include <iostream>
#include <cstdlib>

using namespace std;

int main (int args, char* argv[]) {
	cerr << argv[0] <<" is used to train Tree Reorderings" << endl;
	if (args != 5) {
    cerr << "usage: <Alignment> <SourceText> <SourceTrees> <minOccurence>\n";
	exit(1);
  }

	char * alignment = argv[1];
	char * source = argv[2];
	char * sourceTrees = argv[3];
	int minOcc = atoi(argv[4]);
	// maxsentences = atoi (argv[5]);??

	MyTyp a = 0;

	TreeReorderingExtractor treeEx;

	cerr << "starting to extract\n";
	cerr << "ExtractRules..." << flush;
	treeEx.extractFromSentences(alignment,source,sourceTrees);
	cerr << "done" << endl << flush;

	cerr << "Pruning Rules..." << flush;
  	treeEx.pruneRules(minOcc);
	cerr << "done" << endl << flush;

	cerr << "Normalize Rules..." << flush;
  	treeEx.normalizeRules(sourceTrees, alignment, source);
	cerr << "done" << endl << flush;

	cerr << "Output Rules..." << flush;
  	treeEx.outputRules(cout);
	cerr << "done" << endl << flush;

}
