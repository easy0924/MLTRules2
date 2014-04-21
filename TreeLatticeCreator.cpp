/*
 * TreeLatticeCreator.cpp
 *
 *  Created on: Sep 12, 2011
 *      Author: therrman
 */

#include "TreeLatticeCreator.h"

TreeLatticeCreator::TreeLatticeCreator(double cutoffContinousRules,double cutoffDiscontinousRules, double cutSynt, int maxUseOfRule,int applyRulesRecursive):LatticeCreator(cutoffContinousRules,cutoffDiscontinousRules,maxUseOfRule,applyRulesRecursive) {
	// TODO Auto-generated constructor stub
	cutoffSyntaxRules = cutSynt;
	allLengthSyntax = 0;
	appliedRulesSyntax = 0;
	allProbsSyntax =0;
	cerr<< "Cutoff Syntax Rules: "<< cutoffSyntaxRules<< endl;

}

TreeLatticeCreator::~TreeLatticeCreator() {
	// TODO Auto-generated destructor stub
}

void TreeLatticeCreator::AddSyntaxRules(Edge * head, SyntaxTree * tree) {

	vector<Edge*> EdgeSequence;
	vector<vector<Edge*> > EdgeSequenceVector;
	Edge* p = (head->to->next)[0];
	for (; p->to != NULL; p = (p->to->next)[0]) {
		EdgeSequence.push_back(p);
	}
	EdgeSequence.push_back(p);

	EdgeSequenceVector.push_back(EdgeSequence);

	ApplySyntaxRulesToPath(EdgeSequenceVector, tree);

}

void TreeLatticeCreator::ApplySyntaxRulesToPath(vector<vector<Edge*> > EdgeSequence, SyntaxTree * tree) {

	map<vector<int> , map <vector<int>, double> > toApply = getReorderingForSyntax(EdgeSequence, tree);
	insertReorderingForSyntax(EdgeSequence, toApply, 0);



}
void TreeLatticeCreator::insertReorderingForSyntax(vector<vector <Edge*> > &ESequence, map<vector<int> , map <vector<int>, double> > toApply, int inRecursion) {
//void TreeLatticeCreator::insertReorderingForSyntax(vector <Edge*> &EdgeSequence, map<vector<int> , map <vector<int>, double> > toApply) {
	for (vector<vector<Edge*> >::iterator e = ESequence.begin(); e != ESequence.end(); e++) {
		vector <Edge*> EdgeSequence = *e;
		Edge * previous = EdgeSequence[0]->previous;
		assert(previous != NULL);
		Edge* currentEdge;

		cout << inRecursion << endl;

		for (map<vector<int>,map<vector<int>,double> >::iterator r = toApply.begin(); r != toApply.end(); r++) {
			if (r->first.size()>EdgeSequence.size()){
				continue;
			}else{
				int firstPos = r->first[0];
				int firstIndex = firstPos - EdgeSequence[0]->wordPosition;
				for (map<vector<int>,double> ::iterator s = r->second.begin(); s != r->second.end(); s++){

					if (inRecursion && s->second < cutoffSyntaxRules){
						continue;
					}else{

						vector<vector<Edge *> > innerEdgeSequence;
						vector<Edge *> lastList;
						Edge * lastElement;
						innerEdgeSequence.push_back(vector<Edge *>());

						appliedRulesSyntax+=1;
						allLengthSyntax+=r->first.size()-1;
						int firstTargetPos = s->first[0];
						if (r->first[0]<EdgeSequence[0]->wordPosition || r->first[r->first.size()-1]>EdgeSequence[EdgeSequence.size()-1]->wordPosition){
							continue;
						}

						for(int i = 0; i < r->first.size(); i++) {
							cout << r->first[i] << " ";
						}
						cout << "-------";
						for(int i = 0; i < s->first.size(); i++) {
							cout << s->first[i] << " ";
						}
						cout << endl;

						currentEdge = EdgeSequence[firstIndex]->previous;

						for (int j=0; j<s->first.size(); j++){
							Edge* newEdge = new Edge();
							if(j != s->first.size() -1) {
								newEdge->to = new Node();
								newEdge->to->name = -1;

							}else{
								newEdge->to = EdgeSequence[r->first[r->first.size() - 1]-EdgeSequence[0]->wordPosition]->to;
							}

							newEdge->from = currentEdge->to;
							newEdge->Word = EdgeSequence[s->first[j]-EdgeSequence[0]->wordPosition]->Word;
							newEdge->WordInfo = EdgeSequence[s->first[j]-EdgeSequence[0]->wordPosition]->WordInfo;
							newEdge->POS = EdgeSequence[s->first[j]-EdgeSequence[0]->wordPosition]->POS;
							newEdge->wordPosition = EdgeSequence[s->first[j]-EdgeSequence[0]->wordPosition]->wordPosition;

							if(j == 0) {

								newEdge->prob = s->second;
								allProbsSyntax+=s->second;
								currentEdge->to->next[0]->prob -= s->second;
							}else{
								newEdge->prob = 1;
							}

							newEdge->previous = currentEdge;
							currentEdge->to->next.push_back(newEdge);
							currentEdge = newEdge;

							lastList=innerEdgeSequence[innerEdgeSequence.size()-1];
							if (lastList.size()==0){
								innerEdgeSequence[innerEdgeSequence.size()-1].push_back(newEdge);

							}else{
								lastElement=innerEdgeSequence[innerEdgeSequence.size()-1][innerEdgeSequence[innerEdgeSequence.size()-1].size()-1];
								if ((lastElement->wordPosition)+1 == newEdge->wordPosition){
									innerEdgeSequence[innerEdgeSequence.size()-1].push_back(newEdge);
								}else {
									innerEdgeSequence.push_back(vector<Edge *>());
									innerEdgeSequence[innerEdgeSequence.size()-1].push_back(newEdge);
								}
							}
						}
						if (true){	// set to false for switching off recursive rule application
							insertReorderingForSyntax(innerEdgeSequence,toApply, 1);
						}
					}
				}
			}
		}

	}

}


map< vector<int>, map <vector<int>, double> > TreeLatticeCreator::checkTreeForReorderings(SyntaxTree * tree, map< vector<int>,map <vector<int>, double> > list){

	string constSeq = tree->head+" ";
	string constSeqPartial = tree->head+" ";

	vector<string> constSeqList;
	vector<string> constSeqPartialList;

	int startingIndex;
	double prob;

	vector<int> sourceSeq;
	vector<int> sourceSeqPartial;


	vector< vector <int > > sourceIndices;
	vector<int> targetIndices;
	vector< vector <int > >  sourceIndicesPartial;
	vector <vector< vector <int > > > sourceIndicesPartialList;
	vector<int>  targetIndicesPartial;

	for (int i=0; i<tree->children.size(); i++){
		constSeq=constSeq+tree->children[i]->head+" ";
		if (tree->children[i]->start_source == tree->children[i]->end_source){
				sourceSeq.push_back(tree->children[i]->start_source-1);
				sourceIndices.push_back(sourceSeq);
				sourceSeq.clear();
		}else{
				for (int o=tree->children[i]->start_source; o<tree->children[i]->end_source+1; o++){
					sourceSeq.push_back(o-1);
				}
				sourceIndices.push_back(sourceSeq);
				sourceSeq.clear();
		}
		int j=i;
		for (int j=i; j< tree->children.size(); j++){
			if (i==0 && j==tree->children.size()-1){
				break;
			}else if (i==tree->children.size()-1){
				break;
			}
			constSeqPartial=constSeqPartial+tree->children[j]->head+" ";

			if (tree->children[j]->start_source == tree->children[j]->end_source){
					sourceSeqPartial.push_back(tree->children[j]->start_source-1);
					sourceIndicesPartial.push_back(sourceSeqPartial);
					sourceSeqPartial.clear();


			}else{

					for (int o=tree->children[j]->start_source; o<tree->children[j]->end_source+1; o++){
						sourceSeqPartial.push_back(o-1);
					}
					sourceIndicesPartial.push_back(sourceSeqPartial);
					sourceSeqPartial.clear();

			}

			if (tokenize(constSeqPartial).size()>2){
				constSeqPartialList.push_back(constSeqPartial);
				//cout << "constSeqPartial: "<< constSeqPartial << endl;
				sourceIndicesPartialList.push_back(sourceIndicesPartial);

			}



		}


		if (tree->children.size()!=0){

			list=checkTreeForReorderings(tree->children[i], list);
		}
		constSeqPartial.clear();
		constSeqPartial = tree->head+" ";
		sourceIndicesPartial.clear();
	}


	if (tokenize(constSeq).size()>2){
		constSeqList.push_back(constSeq);
		//cout << "constSeq: " << constSeq << endl;
	}




	//normal rules
	if (TreeReordering.find(constSeq)!= TreeReordering.end()){

		//cout << TreeReordering[constSeq].size() << endl;
		for (map <string, double>::iterator r = TreeReordering[constSeq].begin(); r!=TreeReordering[constSeq].end(); ++r) {
			if (r->second > cutoffSyntaxRules){
				startingIndex=sourceIndices[0][0];
				targetIndices = tokenize(r->first);

				int rulesLength = tokenize(constSeq).size();
				if(targetIndices.size() + 1 == rulesLength) {

				vector<int> absoluteReordering;

				prob = r->second;
				for (int l=0; l<targetIndices.size(); l++){
					for (int l2=0; l2<sourceIndices[targetIndices[l]].size(); l2++){
						absoluteReordering.push_back(sourceIndices[targetIndices[l]][l2]);
					}

				}

				vector <int> flatSourceIndices;
				for (int m=0; m<sourceIndices.size(); m++){
					for (int m2=0; m2<sourceIndices[m].size(); m2++){
						flatSourceIndices.push_back(sourceIndices[m][m2]);
					}
				}
				//cout << absoluteReordering.size() << " " << targetIndices.size() + 2 << endl;
				if(true) { // applies word-level rules
				//if(absoluteReordering.size() >= targetIndices.size() + 2) { // does not apply word level rules
					cout<< constSeq << " --> " << r->first << " " << prob << endl;

				if (list.find(flatSourceIndices) == list.end()){
					list[flatSourceIndices].insert(make_pair(absoluteReordering, prob));
				}else{
					if(list[flatSourceIndices].find(absoluteReordering) == list[flatSourceIndices].end()){
						list[flatSourceIndices].insert(make_pair(absoluteReordering, prob));
					}else{
						for (map <vector <int>, double>::iterator s = list[flatSourceIndices].begin(); s!=list[flatSourceIndices].end(); ++s) {

							if(s->second < r->second) {
								list[flatSourceIndices].erase(s);
								list[flatSourceIndices].insert(make_pair(absoluteReordering, r->second));
							}
						}
					}
				}}
				}
			}
		}
	}else{

	// same for Partial Rules
	bool p=true; // set to true to apply Partial Rules
	if (p==true){ //
	for (int i = 0; i< constSeqPartialList.size(); i++){
		if (tokenize(constSeqPartialList[i]).size() < 3){
			continue;
		}else if (TreeReorderingPartial.find(constSeqPartialList[i])!= TreeReorderingPartial.end()){


			for (map <string, double>::iterator r = TreeReorderingPartial[constSeqPartialList[i]].begin(); r!=TreeReorderingPartial[constSeqPartialList[i]].end(); ++r) {
				//if (r->second > cutoffSyntaxRules){
				if (r->second > cutoffSyntaxRules*1){
					startingIndex=sourceIndicesPartialList[i][0][0];
					targetIndicesPartial = tokenize(r->first);

					int rulesLength = tokenize(constSeqPartialList[i]).size();
					if(targetIndicesPartial.size() + 1 == rulesLength) {

						vector<int> absoluteReordering;

						prob = r->second;
						for (int l=0; l<targetIndicesPartial.size(); l++){
							for (int l2=0; l2<sourceIndicesPartialList[i][targetIndicesPartial[l]].size(); l2++){
								absoluteReordering.push_back(sourceIndicesPartialList[i][targetIndicesPartial[l]][l2]);
							}

						}

						vector <int> flatSourceIndicesPartial;
						for (int m=0; m<sourceIndicesPartialList[i].size(); m++){
							for (int m2=0; m2<sourceIndicesPartialList[i][m].size(); m2++){
								flatSourceIndicesPartial.push_back(sourceIndicesPartialList[i][m][m2]);
							}
						}

						if(true) { // applies word-level rules
						//if(absoluteReordering.size() >= targetIndicesPartialList[i].size() + 2) { // does not apply word level rules
							cout<< constSeqPartialList[i] << " --> " << r->first << " " << prob << endl;

							if (list.find(flatSourceIndicesPartial) == list.end()){
								list[flatSourceIndicesPartial].insert(make_pair(absoluteReordering, prob));
							}else{
								if(list[flatSourceIndicesPartial].find(absoluteReordering) == list[flatSourceIndicesPartial].end()){
									list[flatSourceIndicesPartial].insert(make_pair(absoluteReordering, prob));
								}else{
									for (map <vector <int>, double>::iterator s = list[flatSourceIndicesPartial].begin(); s!=list[flatSourceIndicesPartial].end(); ++s) {

										if(s->second < r->second) {
											list[flatSourceIndicesPartial].erase(s);
											list[flatSourceIndicesPartial].insert(make_pair(absoluteReordering, r->second));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	}
	}
	return list;
}


map<vector<int> , map <vector<int>, double> > TreeLatticeCreator::getReorderingForSyntax(vector<vector <Edge*> > &EdgeSequence, SyntaxTree * tree) {

	map<vector<int> , map <vector<int>, double> > toApply;
	toApply = checkTreeForReorderings(tree, toApply);
	return toApply;
}


Edge * TreeLatticeCreator::checkLattice(Edge * head,string txt, string tags, string tree) {

	long maxName=0;
	long remainingPointer=0;
	vector<Edge*> remaining;

	head->to->name = 0;
	maxName++;
	for (int i=0; i< head->to->next.size(); i++) {
		remaining.push_back( (head->to->next)[i]	);
	}

	do {

		//cout << remaining[remainingPointer]->to << remaining[remainingPointer]->to->name << endl;
		if ( (remaining[remainingPointer]->to) != NULL && remaining[remainingPointer]->to->name == -1) {
			remaining[remainingPointer]->to->name = maxName;
			//cout << remaining[remainingPointer]->to << " " << remaining[remainingPointer]->to->name << endl;
			maxName++;
			for (int i=0; i< remaining[remainingPointer]->to->next.size(); i++) {
				remaining.push_back( (remaining[remainingPointer]->to->next)[i] );
			}
		}
		remainingPointer++;

	}while( remainingPointer < remaining.size() );

	if(maxPossibleNodes == -1) {
		// no checking needed
		return head;
	}

	cerr << "Max:" << maxName << endl;
	//if (false){				// noMax
	if(maxName > maxPossibleNodes) {
		double oldCutoffDiscontinousRules = cutoffDiscontinousRules;
		double oldCutoffContinousRules = cutoffContinousRules;
		double oldCutoffSyntaxRules = cutoffSyntaxRules;
		deleteLattice(head);
		if(cutoffDiscontinousRules < cutoffContinousRules) {
			cutoffDiscontinousRules += 0.01; //0.1
			//cerr << "Increase Discontinous Rules cutoff" << endl;
		}else {
			cutoffContinousRules += 0.01; //0.1
			//cerr << "Increase Continous Rules cutoff" << endl;
		} // reduce only threshold for syntax rules
		cutoffSyntaxRules += 0.01;
		//cerr << "Build New lattice" << endl;
		Edge * e = createLatticeForSentence(txt,tags, tree);
		cutoffDiscontinousRules = oldCutoffDiscontinousRules;
		cutoffContinousRules = oldCutoffContinousRules;
		cutoffSyntaxRules = oldCutoffSyntaxRules;
		return e;

	}else {
		return head;
	}
}


void TreeLatticeCreator::createLatticesForText(string & textFileName, string & tagFileName, string & treeFileName, string & destinationDir){


	ifstream txtReader(textFileName.c_str());
	ifstream tagReader(tagFileName.c_str());
	ifstream treeReader(treeFileName.c_str());

	string Text;
	string Tags;
	string Tree;

	int sentenceNumber;
	ofstream Lattices;
	string destDir(destinationDir);
	Lattices.open((destDir+"/Lattices.lst").c_str());

	long latticeCounter=0;

	while (!txtReader.eof() && !tagReader.eof() && !treeReader.eof()) {
		getline(txtReader,Text);
		cout << Text << endl;
		getline(tagReader,Tags);
		getline(treeReader,Tree);
		cout << Tree << endl;
		if(!txtReader.eof()) {
	    	//cout << "LatticeCounter: " << latticeCounter << endl;
			Lattices << destDir << "/" << latticeCounter << ".lat" << endl;
			char newFile [200];
			sprintf(newFile,"%d.lat",latticeCounter);
			if (Text !="") {
				Edge * e = createLatticeForSentence(Text, Tags, Tree);
				outputLattice(e,destDir+string(newFile));
				deleteLattice(e);
			}
			latticeCounter++;
		}
	}

	Lattices.close();

	cerr << applyedRules << " rules applyed with avg. lenght of: " << 1.0*allLength/applyedRules << endl;
	cerr << appliedRulesSyntax << " Syntax rules were applied with average Prob " << allProbsSyntax/appliedRulesSyntax << " and average length " << 1.0*allLengthSyntax/appliedRulesSyntax << endl;

};


void TreeLatticeCreator::oracleReorderText(string & textFileName, string & tagFileName, string & treeFileName, string & destFile,string & refFile){
	ifstream txtReader(textFileName.c_str());
	ifstream tagReader(tagFileName.c_str());
	ifstream treeReader(treeFileName.c_str());
	ifstream refReader(refFile.c_str());

	string Text;
	string Tags;
	string Tree;
	string ref;
	ofstream outFile;
	outFile.open(destFile.c_str());


	while (!txtReader.eof() && !tagReader.eof()) {
		getline(txtReader,Text);
		getline(tagReader,Tags);
		getline(refReader,ref);
		//cout << "Sentence:" << Text << endl;
		if (Text !="") {
			Edge * e = createLatticeForSentence(Text, Tags, Tree);
			map<int,int> reference = extractReference(ref);
			string reorderSentence = findOracleReordering(e,reference);
			outFile << reorderSentence << endl;
			deleteLattice(e);
		}
	}
	outFile.close();

	cerr << applyedRules << " rules applyed with avg. lenght of: " << 1.0*allLength/applyedRules << endl;

};

void TreeLatticeCreator::createNGramList(string & textFileName, string & tagFileName, string & treeFileName){
	ifstream txtReader(textFileName.c_str());
	ifstream tagReader(tagFileName.c_str());
	ifstream treeReader(treeFileName.c_str());

	string Text;
	string Tags;
	string Tree;
	set<string> nGrams;
	while (!txtReader.eof() && !tagReader.eof() && !treeReader.eof()) {
		getline(txtReader,Text);
		getline(tagReader,Tags);
		getline(tagReader,Tree);
		if (Text !="") {
			Edge * e = createLatticeForSentence(Text, Tags, Tree);
			insertNGrams(e,nGrams);
			deleteLattice(e);
		}

	}
	outputNGrams(nGrams);
};

Edge * TreeLatticeCreator::createLatticeForSentence(string txt, string tags, string treestring) {


	Edge* head = CreateLatticeHead();
	Edge* actualEdge = head;
	istringstream TextReader (txt);
	istringstream TagReader (tags);

	int pos = 0;
	int index = 1;

	TreeReorderingExtractor treeHelper;
	cout << "Tree line loading ..." << endl;
	SyntaxTree * tree = treeHelper.LoadTreeLineLight(treestring,pos,index);

	string tempReader;
	string tempPosReader;

	int counter =0;

  	int wordPos = -1;

	while (TextReader >> tempReader) {
		wordPos ++;
		Edge* newEdge = new Edge();
		newEdge->Word = tempReader;
		newEdge->wordPosition = wordPos;
		newEdge->prob = 1.0;
		newEdge->to = new Node();
		newEdge->to->name = -1;
		newEdge->from = actualEdge->to;
		//newEdge->next = new vector<Edge*> ();
		newEdge->previous = actualEdge;
		string wordInfo;
		//cout << "TempReader: " << tempReader << endl;
		if(tempReader[0] == '@') {
			char c = TextReader.get();
			c = TextReader.get();
			TextReader.putback(c);
			//cout << "Next symbol:" << c << endl;
			if(c == '{') {
			  while(tempReader.compare("}") != 0) {
			    TextReader >> tempReader;
			    wordInfo += tempReader + " ";
			    //cout << "Wordinfo:" << tempReader << endl;
		  	  }
			  newEdge->WordInfo = wordInfo;
			}
			//cout << "Done @" << endl;
		}
		actualEdge->to->next.push_back(newEdge);
		actualEdge = (actualEdge->to->next)[0];
	}


	Edge* newEdge = new Edge();
	newEdge->prob = 1.0;
	newEdge->to = NULL;
	newEdge->from = actualEdge->to;
	newEdge->Word = "END_OF_SENTENCE";
	newEdge->POS = "END_OF_SENTENCE";
	newEdge->wordPosition = wordPos + 1;
	//newEdge->next = new vector<Edge*>();
	//newEdge->next.push_back(NULL);
	newEdge->previous = actualEdge;
	actualEdge->to->next.push_back(newEdge);

	actualEdge = (head->to->next)[0];

	while (TagReader >> tempReader) {
		if (tempReader =="REPEAT")
				counter++;
		else {
			for (int i=0; i<=counter; i++) {
				actualEdge->POS = tempReader;
				actualEdge = (actualEdge->to->next)[0];
			}
			counter=0;
		}
	}

	AddDiscontinousRules(head);
	AddContinousRules(head);
	AddSyntaxRules(head,tree);

	head = checkLattice(head,txt,tags, treestring);

	return head;
}
