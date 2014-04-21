
#include "TreeReorderingExtractor.h"

TreeReorderingExtractor::TreeReorderingExtractor()
{
	sentenceNumber = 0;
}


TreeReorderingExtractor::~TreeReorderingExtractor()
{
}

void TreeReorderingExtractor::extractFromSentences(char * alignmentFileName, char * sourceFileName, char * sourceTreeFileName) {

	int line = 0;

	ifstream alignmentFile(alignmentFileName);
	ifstream sourceFile(sourceFileName);
	ifstream sourceTreeFile(sourceTreeFileName);

	string alignmentLine;
	string sourceLine;
	string sourceTreeLine;

	string * reorderinglist;


	while(!(alignmentFile.eof() || sourceFile.eof() || sourceTreeFile.eof() )) {
		sentenceNumber++;
		getline(alignmentFile,alignmentLine);
		getline(sourceFile,sourceLine);
		getline(sourceTreeFile,sourceTreeLine);
		line ++;

		//cout << line << endl;
		cerr << sourceLine << endl;
		cerr << sourceTreeLine << endl;
		cerr << alignmentLine << endl;

		if(sourceLine.length() != 0) {
			LoadTextLine(sourceLine,words);
			int pos = 0;
			int index = 1;
			LoadAlignment(alignmentLine);
			tree = LoadTreeLine(sourceTreeLine,pos,index);
			FindDistortions(tree);
			delete[] maxAlignment;
			delete[] minAlignment;
			delete tree;
		}
	}
}

void TreeReorderingExtractor::LoadTextLine(const string line,vector<string> & w){
	w.clear();
	stringstream stream(line);
	string tempWord;
	w.push_back("BEGIN_OF_SENTENCE");
	while(stream >> tempWord) {
		//cout << tempWord<< " ";
		w.push_back(tempWord);
	}
	w.push_back("END_OF_SENTENCE");
	//cout << endl;
}

SyntaxTree * TreeReorderingExtractor::LoadTreeLine(const string line,int & pos, int & index){
	stringstream stream(line);
	string tempItem;
	string cat;
	string word;
	SyntaxTree * tree = new SyntaxTree();
	tree->start_source=index;


	if (line.find("(",pos) == pos){
		tree->head=line.substr(pos+1,line.find(" ",pos)-(pos+1));
		pos = line.find(" ",pos);
		while(line.find(")",pos) != pos) {
			pos+= 1;
			tree->children.push_back(LoadTreeLine(line,pos,index));
		}
		pos+=1;
	}else {
		tree->head = line.substr(pos,line.find(")",pos)-pos);
		pos = line.find(")",pos);

		tree->start_target=minAlignment[index];
		tree->end_target=maxAlignment[index];
		tree->setLeaf(true);
		index++;
	}
	tree->end_source=index-1;

	for (int i=0; i<tree->children.size(); i++){
		if (tree->start_target > tree->children[i]->start_target || tree->start_target == -1){
			tree->start_target = tree->children[i]->start_target;
		}
		if (tree->end_target < tree->children[i]->end_target){
			tree->end_target = tree->children[i]->end_target;
		}
	}
	return tree;
}

SyntaxTree * TreeReorderingExtractor::LoadTreeLineLight(const string line,int & pos, int & index){
	stringstream stream(line);
	string tempItem;
	string cat;
	string word;
	SyntaxTree * tree = new SyntaxTree();
	tree->start_source=index;


	if (line.find("(",pos) == pos){
		tree->head=line.substr(pos+1,line.find(" ",pos)-(pos+1));
		pos = line.find(" ",pos);
		while(line.find(")",pos) != pos) {
			pos+= 1;
			tree->children.push_back(LoadTreeLineLight(line,pos,index));
		}
		pos+=1;
	}else {
		tree->head = line.substr(pos,line.find(")",pos)-pos);
		pos = line.find(")",pos);
		tree->setLeaf(true);

		index++;
	}
	tree->end_source=index-1;

	return tree;
}

void TreeReorderingExtractor::LoadAlignment(const string alignmentLine){


	InitAlignment();
	stringstream stream(alignmentLine);
	string temp;
	while(stream >> temp) {
		int s;
		int t;
		sscanf(temp.c_str(),"%i-%i",&s,&t);
		assert(s > 0);
		//cerr << words.size() << endl;
		assert(s < words.size());
		assert(t > 0);
		assert(t < MAX_TARGET_WORDS - 1);
		maxAlignment[s] = t > maxAlignment[s] ? t : maxAlignment[s];
		minAlignment[s] = t < minAlignment[s] ? t : minAlignment[s];
	}


}

void TreeReorderingExtractor::FindDistortions(SyntaxTree * tree){
    //search for reorderings
	int rflag=0;   // reordering found
	int nflag =0;  // weird alignments
	int lflag = 0; // is leaf
	int inR = 0;   // in recursion
	int startReordering = -1, endReordering =-1 ;
	//set<pair<int, int> > rpos ;
	stringstream subtree;
	subtree << "@" << tree->head << " ";
	//cerr << "head: "<< tree->head << endl;
	for(int i = 0; i < tree->children.size(); i++) {
		subtree << tree->children[i]->head << " ";
		nflag=0;
		for (int j=1; j < tree->children.size(); j++) {
			if ( i < j ){
						if (tree->children[i]->start_target > MAX_TARGET_WORDS-1 || tree->children[i]->end_target == -1
								||  tree->children[i+1]->start_target == MAX_TARGET_WORDS || tree->children[i+1]->end_target == -1){
							// weird alignments in current subtree, no reorderings can be extracted here
							nflag=1;
						} else if ( tree->children[i]->start_target > tree->children[i+1]->start_target || tree->children[i]->end_target > tree->children[i+1]->end_target ){
							// found possible reordering
							rflag=1;
							//rpos.insert(make_pair(i,j));
							//cerr << "i  : " <<i<<" -> j: "<<j<<"---"<< tree->children[i]->start_target << " .. "<<  tree->children[i]->end_target <<endl;
							//cerr << "i+1: " <<i+1<<" -> j: "<< j<<" --- "<<tree->children[i+1]->start_target << " .. "<<  tree->children[i+1]->end_target <<endl;
						}

						if(!tree->isLeaf()) {
							lflag = 1;
						}
			}
		}
		FindDistortions(tree->children[i]);

	}
	//cerr << subtree.str() << endl;


	if ( rflag == 1 && nflag == 0){//  && lflag == 1){


		//for (set<pair<int, int> >::iterator myi=rpos.begin(); myi != rpos.end(); myi++){

			vector<pair<int, int> > startTargetSeq;
			vector<pair<int, int> > startTargetSeq_copy;
			vector<pair<int, int> > endTargetSeq;
			vector<pair<int, int> > endTargetSeq_copy;
			//vector<pair<int, int> > startTargetSeqPartial;
			//vector<pair<int, int> > startTargetSeqPartial_copy;
			//vector<pair<int, int> > endTargetSeqPartial;
			//vector<pair<int, int> > endTargetSeqPartial_copy;
			string reorderedTagSeq = "";
			string originalTagSeq = "";
			string reorderedAlignmentPoints="";
			//string reorderedTagSeqPartial = "";
			//string originalTagSeqPartial = "";
			//string reorderedAlignmentPointsPartial="";


			//int endR, startR;
			//int partial=1;

			/*if (tree->children.size()<myi->second+1){
				endR=tree->children.size()-1;
			}else{
				endR=myi->second;
			}
			if (myi->first<0){
				startR=0;
			}else{
				startR=myi->first;
			}


			if (startR==0 and endR==tree->children.size()-1){
				partial=0;
			}else{
				partial=1;
			}

			cerr << "startR "<< startR << endl;
			cerr << "endR "<< endR << endl;
			*/
			for(int j=0; j < tree->children.size();j++){
				//cerr << tree->children.size() << " ... " << tree->children[j]->start_target << " ... " << tree->children[j]->end_target << " ... " << tree->children[j]->head << endl;
				startTargetSeq.push_back(make_pair(tree->children[j]->start_target, j));
				endTargetSeq.push_back(make_pair(tree->children[j]->end_target, j));
			}

			//cerr << "startTargetSeq"  <<startTargetSeq[0].first <<endl;

			/*for(int j=startR; j < endR+1;j++){
				startTargetSeqPartial.push_back(make_pair(tree->children[j]->start_target, j));
				endTargetSeqPartial.push_back(make_pair(tree->children[j]->end_target, j));
			}*/

			startTargetSeq_copy=startTargetSeq;
			endTargetSeq_copy=endTargetSeq;
			/*startTargetSeqPartial_copy=startTargetSeqPartial;
			endTargetSeqPartial_copy=endTargetSeqPartial;
			*/
			// sort startTargetSeq according to alignment order
			sort(startTargetSeq.begin(), startTargetSeq.end());
			//sort(startTargetSeqPartial.begin(), startTargetSeqPartial.end());

			//sort endTargetSeq according to alignment order
			sort(endTargetSeq.begin(), endTargetSeq.end());
			//sort(endTargetSeqPartial.begin(), endTargetSeqPartial.end());

			// check if reordered sequence is same for start and end alignments
			int seqFlag=1;
			stringstream mystream;

			for (int m=0; m < startTargetSeq.size(); m++){

						if (startTargetSeq[m].second == endTargetSeq[m].second){
							originalTagSeq=originalTagSeq+" "+tree->children[startTargetSeq_copy[m].second]->head;
							reorderedTagSeq=reorderedTagSeq+" "+tree->children[startTargetSeq[m].second]->head;
							//if(partial==0){
							mystream<<startTargetSeq[m].second<<" ";
							/*}else if (partial ==1){
								mystream<<startTargetSeq[m].second-startR<<" ";
							}
							*/
							continue;
						}else{
							seqFlag=0;
						}
			}

			//same for partial
			/*int seqFlagPartial=1;
			stringstream mystreamPartial;
			for (int m=0; m < startTargetSeqPartial.size(); m++){

						if (startTargetSeqPartial[m].second == endTargetSeqPartial[m].second){
							originalTagSeqPartial=originalTagSeqPartial+" "+tree->children[startTargetSeqPartial_copy[m].second]->head;
							reorderedTagSeqPartial=reorderedTagSeqPartial+" "+tree->children[startTargetSeqPartial[m].second]->head;
							mystreamPartial<<startTargetSeqPartial[m].second-startR<<" ";
							//}
							continue;
						}else{
							seqFlagPartial=0;
						}
			}

			*/
			reorderedAlignmentPoints=mystream.str();
			//reorderedAlignmentPointsPartial=mystreamPartial.str();
			originalTagSeq=tree->head+originalTagSeq;
			//originalTagSeqPartial=tree->head+originalTagSeqPartial;



			// store reordering rules
			pair<string,string> rule;
			//pair<string,string> rulePartial;
			if (seqFlag == 1){
				rule=make_pair(originalTagSeq,reorderedAlignmentPoints);
				if(reorderings.find(rule) != reorderings.end()) {
					reorderings[rule]+=1;
					cerr << originalTagSeq << " --- " << reorderedAlignmentPoints << endl;
				}else{
					reorderings[rule]=1;
					cerr << originalTagSeq << " --- " << reorderedAlignmentPoints << endl;
				}
			}
/*			if (seqFlagPartial == 1){
				rule=make_pair(originalTagSeq,reorderedAlignmentPoints);
				rulePartial=make_pair(originalTagSeqPartial,reorderedAlignmentPointsPartial);
				if (partial==1){
					if(reorderings.find(rule) != reorderings.end()) {
						reorderings[rule]+=1;
						cerr << originalTagSeq << " 1--- " << reorderedAlignmentPoints << endl;
					}else{
						reorderings[rule]=1;
						cerr << originalTagSeq << " 2--- " << reorderedAlignmentPoints << endl;
					}
					if(reorderingsPartial.find(rulePartial) != reorderingsPartial.end()) {
						reorderingsPartial[rulePartial]+=1;
						cerr << originalTagSeqPartial << " 3--- " << reorderedAlignmentPointsPartial << endl;
					}else{
						reorderingsPartial[rulePartial]=1;
						cerr << originalTagSeqPartial << " 4--- " << reorderedAlignmentPointsPartial << endl;
					}
				}*/

			//}

		//}

			map<pair<int,int>,pair<int,int> > partialSequenceList;
			for(int i = 0; i < startTargetSeq.size(); i++) {
				for(int j = i+1; j < startTargetSeq.size(); j++) {
					if (i==0 && j==startTargetSeq.size()-1){
						continue;
					}
					int min = 20000;
					int max = -2;
					bool reorder = false;
					for(int k = i; k <= j; k++) {
						if(min > startTargetSeq[k].second) {
							min = startTargetSeq[k].second;
						}
						if(max < startTargetSeq[k].second) {
							max = startTargetSeq[k].second;
						}
						if(k < j && startTargetSeq[k].second+1 != startTargetSeq[k+1].second) {
							reorder = true;
						}
					}
					//all numbers occur
					if(max - min == j - i && reorder) {
						//cerr << "Insert new reordering" << endl;
						partialSequenceList[make_pair(min,max)] = make_pair(i,-1);
					}
				}
			}
			//cerr << "Number of possible partial reorderings:" << partialSequenceList.size() << endl;

			for(int i = 0; i < endTargetSeq.size(); i++) {
				for(int j = i+1; j < endTargetSeq.size(); j++) {
					int min = 20000;
					int max = -2;
					bool reorder = false;
					for(int k = i; k <= j; k++) {
						if(min > endTargetSeq[k].second) {
							min = endTargetSeq[k].second;
						}
						if(max < endTargetSeq[k].second) {
							max = endTargetSeq[k].second;
						}
						if(k < j &&  endTargetSeq[k].second+1 != endTargetSeq[k+1].second) {
							reorder = true;
						}
					}

					//all numbers occur
					if(max - min == j - i && reorder && partialSequenceList.find(make_pair(min,max)) != partialSequenceList.end()) {
						//cerr << "Test reordering the same" << endl;
						stringstream mystreamPartial;
						string reorderedTagSeqPartial = "";
						string originalTagSeqPartial = "";
						string reorderedAlignmentPointsPartial="";
						int startInStartTargetSeq = partialSequenceList[make_pair(min,max)].first;
						bool ok = true;
						for(int k = 0; k < j-i+1; k++) {
							if(startTargetSeq[startInStartTargetSeq+k].second != endTargetSeq[i+k].second) {
								ok = false;
								break;
							}
							originalTagSeqPartial=originalTagSeqPartial+" "+tree->children[startTargetSeq_copy[min+k].second]->head;
							reorderedTagSeqPartial=reorderedTagSeqPartial+" "+tree->children[startTargetSeq[startInStartTargetSeq+k].second]->head;
							mystreamPartial<<startTargetSeq[startInStartTargetSeq+k].second-min<<" ";
						}
						if(ok) {
							//cerr << "Insert reordering" << endl;
							pair<string,string> rulePartial;
							reorderedAlignmentPointsPartial=mystreamPartial.str();
							originalTagSeqPartial=tree->head+originalTagSeqPartial;
							rulePartial=make_pair(originalTagSeqPartial,reorderedAlignmentPointsPartial);
							if(reorderingsPartial.find(rulePartial) != reorderingsPartial.end()) {
								reorderingsPartial[rulePartial]+=1;
								cerr << originalTagSeqPartial << " --- " << reorderedAlignmentPointsPartial << endl;
							}else{
								reorderingsPartial[rulePartial]=1;
								cerr << originalTagSeqPartial << " --- " << reorderedAlignmentPointsPartial << endl;
							}

						}
					}
				}
			}
	}
}


void TreeReorderingExtractor::InitAlignment() {
	maxAlignment = new int[words.size()];
	minAlignment = new int[words.size()];

	maxAlignment[0] = 0;
	minAlignment[0] = 0;
	for(int i = 1;i < words.size() - 1; i++) {
		maxAlignment[i] = -1;
		minAlignment[i] = MAX_TARGET_WORDS;
	}
	maxAlignment[words.size() -1] = MAX_TARGET_WORDS - 1;
	minAlignment[words.size() -1] = MAX_TARGET_WORDS - 1;
}


void TreeReorderingExtractor::pruneRules(int minOcc) {

	pruneRules(reorderings,counts,minOcc);
	pruneRules(reorderingsPartial, countsPartial, minOcc);

}

void TreeReorderingExtractor::pruneRules(map<pair<string,string>,double > & rules, map<string,int> & c, int minOcc) {
	for(RuleIterator i = rules.begin();i != rules.end();) {

		if(i->second < minOcc) {
			rules.erase(i++);
		}else {

			if(c.find(i->first.first) == c.end()) {
				c.insert(make_pair(i->first.first,0));
			}
			i++;
		}
	}
}

void TreeReorderingExtractor::outputRules(ostream & out) {

	cout << "@TREE_REORDERING:\n";
	outputRules(reorderings,counts,out);
	cout << "@TREE_REORDERING_PARTIAL_RULES:\n";
	outputRules(reorderingsPartial,countsPartial,out);

}

void TreeReorderingExtractor::outputRules(map<pair<string,string>,double > & rules, map<string,int> & c, ostream & out){
	for(RuleIterator i = rules.begin();i != rules.end(); i++) {
		//out << i->first.first << " ---- " << i->first.second << " ---- " << i->second/c[i->first.first] << endl;
		out << i->first.first << " ---- " << i->first.second << " ---- " << i->second/c[i->first.first] << " = " << i->second << " / " << c[i->first.first]<< endl;
  }
}



void TreeReorderingExtractor::normalizeRules(char * sourceTreeFileName, char * alignmentFileName, char * sourceFileName) {

	ifstream sourceFile(sourceFileName);
	ifstream sourceTreeFile(sourceTreeFileName);
	ifstream alignmentFile(alignmentFileName);

	string sourceLine;
	string sourceTreeLine;
	string alignmentLine;
	int sentenceNumber = 0;

	while(!sourceTreeFile.eof() ) {
		sentenceNumber++;

		if (sentenceNumber % 10000 == 0){
			cerr << sentenceNumber << endl;
		}

		getline(sourceFile,sourceLine);
		getline(alignmentFile,alignmentLine);
		//cerr << alignmentLine << endl;
		getline(sourceTreeFile,sourceTreeLine);
		if(sourceTreeLine.length() != 0) {
			int pos = 0;
			int index = 1;

			LoadTextLine(sourceLine,words);
			LoadAlignment(alignmentLine);
			SyntaxTree * tree = LoadTreeLine(sourceTreeLine,pos,index);
			extractCounts(tree);

			delete tree;
		}
	}

	sourceTreeFile.close();
}

void TreeReorderingExtractor::extractCounts(SyntaxTree * tree) {

	int nflag;
	stringstream ss;
	map<string,pair<int,int> > subtreesPartial;
	vector<string> subtrees;

	for(int i = 0; i < tree->children.size(); i++) {

		stringstream ss;
		ss << tree->head;
		nflag=0;
		for (int j=i; j< tree->children.size(); j++){
			if (tree->children[j]->start_target > MAX_TARGET_WORDS-1 || tree->children[j]->end_target == -1){
				nflag=1;
				break;
			}else{
				ss << " " << tree->children[j]->head;
				//cerr << "ss: " << ss.str()<< endl;
				if (i==0 && j==tree->children.size()-1){
					subtrees.push_back(ss.str());
				}else{
					subtreesPartial[ss.str()]=make_pair(i,j);
				}
			}

		}
		extractCounts(tree->children[i]);
	}

	/* counting all occurrences of the subtree
	if(counts.find(ss.str()) != counts.end()) {
		counts[ss.str()] += 1;
	}
	*/
	// counting only occurrences of the subtree where all children are aligned
	// normal Rules
	for (int s=0; s<subtrees.size();s++){
		if(counts.find(subtrees[s]) != counts.end() && nflag != 1){// && lflag == 1) {
			vector<pair<int, int> > startTargetSeq;
			vector<pair<int, int> > endTargetSeq;

			for(int j=0; j < tree->children.size();j++){
				startTargetSeq.push_back(make_pair(tree->children[j]->start_target, j));
				endTargetSeq.push_back(make_pair(tree->children[j]->end_target, j));
			}

			// sort startTargetSeq according to alignment order
			sort(startTargetSeq.begin(), startTargetSeq.end());

			//sort endTargetSeq according to alignment order
			sort(endTargetSeq.begin(), endTargetSeq.end());

			// check if reordered sequence is same for start and end alignments
			int seqFlag=1;
			for (int m=0; m < startTargetSeq.size(); m++){
				if (startTargetSeq[m].second == endTargetSeq[m].second){
					continue;
				}else{
					seqFlag=0;
				}
			}
			if (seqFlag == 1){
				counts[subtrees[s]] += 1;
			}
		//cerr << "count: " << counts[subtrees[s]] << endl;
		}
	}

	// partial Rules
	for (map<string,pair<int,int> >::iterator s=subtreesPartial.begin();s!= subtreesPartial.end();s++){
		if(countsPartial.find(s->first) != countsPartial.end() && nflag != 1){// && lflag == 1) {
			vector<pair<int, int> > startTargetSeq;
			vector<pair<int, int> > endTargetSeq;

			for(int j=s->second.first; j <s->second.second+1 ;j++){
				startTargetSeq.push_back(make_pair(tree->children[j]->start_target, j));
				endTargetSeq.push_back(make_pair(tree->children[j]->end_target, j));
			}

			// sort startTargetSeq according to alignment order
			sort(startTargetSeq.begin(), startTargetSeq.end());

			//sort endTargetSeq according to alignment order
			sort(endTargetSeq.begin(), endTargetSeq.end());

			// check if reordered sequence is same for start and end alignments
			int seqFlag=1;
			for (int m=0; m < startTargetSeq.size(); m++){
				if (startTargetSeq[m].second == endTargetSeq[m].second){
					continue;
				}else{
					seqFlag=0;
				}
			}
			if (seqFlag == 1){
				countsPartial[s->first] += 1;
			}
		//cerr << "countPartial: " << countsPartial[s->first] << " -- " <<s->first<<  endl;
		}
	}

}


pair<string,string> TreeReorderingExtractor::getRuleParts(const string rule) {
	string partOne;
	string partTwo;
	stringstream s(rule);
	int i = 0;
	string temp;
	while(s >> temp) {
		if(temp.compare("X") == 0) {
			i++;
		}else if(i == 0) {
			if(partOne.size() != 0) {
				partOne.push_back(' ');
			}
			partOne += temp;
		}else{
			if(partTwo.size() != 0) {
				partTwo.push_back(' ');
			}
			partTwo += temp;
		}
	}

	return make_pair(partOne,partTwo);


}

int TreeReorderingExtractor::findOccurences(vector<S_SimplePhraseLocationElement> & locationsOne,vector<S_SimplePhraseLocationElement> & locationsTwo) {

	//cout << "Sentence Numbers: " << sentenceNumber << endl;
	vector<int> * occurences = new vector<int>[sentenceNumber + 1];
	//vector<int> occurences[sentenceNumber + 1];
	int number = 0;
	//cout << locationsOne.size() << " " << locationsTwo.size()<< " " << endl;
	for(int i = 0; i < locationsOne.size();i++) {
		//cout << "Sent: " << locationsOne[i].sentIdInCorpus << " ";
		//cout << "Pos:" << locationsOne[i].posInSentInCorpus << ":" << endl;
		assert(locationsOne[i].sentIdInCorpus <= sentenceNumber);
		occurences[locationsOne[i].sentIdInCorpus].push_back(locationsOne[i].posInSentInCorpus);
	}
	for(int i = 0; i < locationsTwo.size();i++) {
		//cout << "Sentence: " << locationsTwo[i].sentIdInCorpus << endl;
		assert(locationsTwo[i].sentIdInCorpus <= sentenceNumber);
		for(int j = 0; j < occurences[locationsTwo[i].sentIdInCorpus].size(); j++) {
			if (occurences[locationsTwo[i].sentIdInCorpus][j] + 1 < locationsTwo[i].posInSentInCorpus) {
			//cout << "Pos: " << (int) occurences[locationsTwo[i].sentIdInCorpus][j] + 1 << " " << (int) locationsTwo[i].posInSentInCorpus << endl;
				number++;
			}
		}
	}
	//cout << number << endl;
	delete[] occurences;
	return number;
}
