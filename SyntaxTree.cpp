
#include "SyntaxTree.h"

SyntaxTree::SyntaxTree()
{
	string head = "";
	start_source = -1;
	end_source=-1;
	start_target = -1;
	end_target= -1;
	leaf = false;
}


SyntaxTree::~SyntaxTree()
{
	for(int i = 0; i < children.size(); i++) {
		delete children[i];
	}
}


/*SyntaxTree * SyntaxTree::findFirstLeaf(){

	for (int i=0; i<children.size(); i++){
		if (children.size() ==0){
			return *this;
		}else{
			children[i]->findFirstLeaf();
		}
}*/

void SyntaxTree::output(){


	cout << "(" << head << "[" << start_source << "," << end_source << "#"<<start_target<<","<<end_target << "]";
	for (int i=0; i<children.size(); i++){
		children[i]->output();
		cout << " ";
	}
	cout << ")";
}

void SyntaxTree::output_pretty(){


	cout << "(" << head ;
	for (int i=0; i<children.size(); i++){
		children[i]->output_pretty();
		cout << " ";
	}
	cout << ")";
}

void SyntaxTree::output_sentence(){

	if (children.size()==0) {
		cout << head << " ";
	}
	for (int i=0; i<children.size(); i++){
		children[i]->output_sentence();
	}
}

int SyntaxTree::freqOfConstituent(string constituent){
	int occ = 0;
	vector<string> constituentlist;
	stringstream stream(constituent);
	string tempConst;
	int hflag =0;
	int cflag = 0;
	while(stream >> tempConst) {
		constituentlist.push_back(tempConst);
	}

	//cout << "testing: "<< constituent << "Head: " <<constituentlist[0] << "=?" << head << endl;

	if (constituentlist[0].compare(head)==0) {
		// head matches, now check children
		hflag=1;

		// print all children
		/*
		cout << "HEAD MATCH" << endl;
		cout << "Children: ";
		for (int k=0; k<children.size(); k++){
			cout << children[k]->head << " " ;
		}
		cout << endl;
*/
		if ( constituentlist.size() == children.size()+1) {


			for (int i=1; i<constituentlist.size(); i++){
				//cout << "testing indiv const: "<< constituentlist[i] << "=?"<< children[i-1]->head << endl;
				if (constituentlist[i].compare(children[i-1]->head)==0){
					cflag=1;
			//		cout << "test successful: "<< constituentlist[i] << "==" << children[i-1]->head << endl;
				}else{
					cflag=0;
				}
			}
		}
	}

	// head doesn't match, check if one of children matches
	/*cout << "HEAD NO Try Children: ";
	for (int k=0; k<children.size(); k++){
		cout << children[k]->head << " " ;
	}
	cout << endl;
*/
	for (int j=0; j<children.size(); j++){
	//	cout << "check children: ";
		//cout << children[j]->head;
		occ+=children[j]->freqOfConstituent(constituent);
	}
	//cout << endl;

	if (hflag ==1 and cflag==1){
		occ+=1;
		//cout << "FOUND MATCH" <<endl;
	}

	return occ;
}
