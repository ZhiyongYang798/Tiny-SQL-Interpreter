#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include "TreeNode.h"
#include "ParseTree.h"


using namespace std;

TreeNode<string>* selectstate(vector<string> strings, int &numb)
{
	//TreeNode<T>* selectlist(vector<T>, int&);
	//TreeNode<T>* tablist(vector<T>, int&);
	//TreeNode<T>* searchcondition(vector<T> strings, int &numb);
	string selectlis = "SelectList";
	string tablelist = "TableList";
	TreeNode<string> *selToken = new TreeNode<string>(strings[numb++]);//node SELECT
	if (strings[numb] == "DISTINCT") {
		TreeNode<string> *disToken = new TreeNode<string>(strings[numb++]);//node distinct

		selToken->nextSibling = disToken; //node DISTINCT
										  //selToken = disToken; //!
										  //}
		TreeNode<string> *selListRoot = new TreeNode<string>(selectlis);//node selectlist
		disToken->nextSibling = selListRoot;
		selListRoot->firstChild = selectlist(strings, numb);
		TreeNode<string> *fromToken = new TreeNode<string>(strings[numb++]);
		selListRoot->nextSibling = fromToken;
		TreeNode<string> *tabListRoot = new TreeNode<string>(tablelist);
		fromToken->nextSibling = tabListRoot;
		tabListRoot->firstChild = tablist(strings, numb);//
		if (numb < strings.size()) {
			if (strings[numb] == "WHERE") {
				TreeNode<string> *whereToken = new TreeNode<string>(strings[numb++]);
				TreeNode<string> *searcon = searchcondition(strings, numb);
				tabListRoot->nextSibling = whereToken;
				whereToken->nextSibling = searcon;
				if (numb < strings.size())
				{
					searcon->nextSibling = new TreeNode<string>(strings[numb++]);
					searcon->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
					searcon->nextSibling->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
				}
			}
			else
			{
				tabListRoot->nextSibling = new TreeNode<string>(strings[numb++]);
				tabListRoot->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
				tabListRoot->nextSibling->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
			}
		}
	}
	else
	{
		TreeNode<string> *selListRoot = new TreeNode<string>(selectlis);//node selectlist
		selToken->nextSibling = selListRoot;
		selListRoot->firstChild = selectlist(strings, numb);
		TreeNode<string> *fromToken = new TreeNode<string>(strings[numb++]);
		selListRoot->nextSibling = fromToken;
		TreeNode<string> *tabListRoot = new TreeNode<string>(tablelist);
		fromToken->nextSibling = tabListRoot;
		tabListRoot->firstChild = tablist(strings, numb);
		if (numb < strings.size()) {
			if (strings[numb] == "WHERE") {
				TreeNode<string> *whereToken = new TreeNode<string>(strings[numb++]);
				TreeNode<string> *searcon = searchcondition(strings, numb);
				tabListRoot->nextSibling = whereToken;
				whereToken->nextSibling = searcon;
				if (numb < strings.size())
				{
					searcon->nextSibling = new TreeNode<string>(strings[numb++]);
					searcon->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
					searcon->nextSibling->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
				}
			}
			else
			{
				tabListRoot->nextSibling = new TreeNode<string>(strings[numb++]);
				tabListRoot->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
				tabListRoot->nextSibling->nextSibling->nextSibling = new TreeNode<string>(strings[numb++]);
			}
		}
	}

	return selToken;
}

TreeNode<string>* selectlist(vector<string> strings, int &numb) //Link all the column names as the children of selectlist node
{
	//string selectlist = "SelectList";
	TreeNode<string> *d = new TreeNode<string>(strings[numb++]);//inner nodes of selectlist
	TreeNode<string> *last = d;
	while (strings[numb] != "FROM")
	{
		TreeNode<string> *e = new TreeNode<string>(strings[numb++]);
		last->nextSibling = e;
		last = e;
	}
	return d;
}

template<class T>
TreeNode<T>* tablist(vector<T> strings, int &numb) //Link all the column names as the children of tablelist node
{
	//string selectlist = "SelectList";
	TreeNode<string> *d = new TreeNode<string>(strings[numb++]);//inner nodes of tablelist
	TreeNode<string> *last = d;                                 //pay attention to the linklist creation!!!
	while (numb<strings.size() && strings[numb] != "WHERE" && strings[numb] != "ORDER")
	{
		TreeNode<string> *e = new TreeNode<string>(strings[numb++]);
		last->nextSibling = e;
		last = e;
	}
	return d;
}

template<class T>
TreeNode<T>* searchcondition(vector<T> strings, int &numb)
{
	//TreeNode<T> *booleanterm(vector<T> strings, int &numb);
	string searcond = "SearchCondition";
	TreeNode<string> *d = new TreeNode<string>(searcond);
	TreeNode<string> *boolterm1 = booleanterm(strings, numb);//???
	d->firstChild = boolterm1;
	TreeNode<string> *last = boolterm1;
	while (numb < strings.size() && strings[numb] != "ORDER")
	{
		TreeNode<string> *OrToken = new TreeNode<string>(strings[numb++]);  //Node"OR"
		last->nextSibling = OrToken;
		TreeNode<string> *NextBoolTer = booleanterm(strings, numb);
		last->nextSibling->nextSibling = NextBoolTer;
		last = NextBoolTer;
	}
	return d;
}

template<class T>
TreeNode<T>* booleanterm(vector<T> strings, int &numb)
{
	//TreeNode<T> *booleanfactor(vector<T> strings, int &numb);
	string boolterm = "BooleanTerm";
	TreeNode<string> *d = new TreeNode<string>(boolterm);
	TreeNode<string> *boolfac1 = booleanfactor(strings, numb);
	d->firstChild = boolfac1;
	TreeNode<string> *last = boolfac1;
	int a = strings.size();
	while (numb < a&&strings[numb] != "ORDER"&&strings[numb]!="OR")
	{
		TreeNode<string> *AndToken = new TreeNode<string>(strings[numb++]);  //Node"AND"
		last->nextSibling = AndToken;
		TreeNode<string> *NextBoolFac = booleanfactor(strings, numb);
		last->nextSibling->nextSibling = NextBoolFac;
		last = NextBoolFac;
	}
	return d;
}

template<class T>
TreeNode<T>* booleanfactor(vector<T> strings, int &numb)
{
	//TreeNode<T> *expression(vector<T> strings, int &numb);
	string boolfac = "BooleanFactor";
	TreeNode<string> *d = new TreeNode<string>(boolfac);
	TreeNode<string> *exp1 = expression(strings, numb);
	d->firstChild = exp1;
	TreeNode<string> *comp = new TreeNode<string>(strings[numb++]);
	exp1->nextSibling = comp;
	TreeNode<string> *exp2 = expression(strings, numb);
	comp->nextSibling = exp2;
	return d;
}

template<class T>
TreeNode<T>* expression(vector<T> strings, int &numb)
{
	string exp = "expression";
	TreeNode<string> *ex = new TreeNode<string>(exp);
	TreeNode<string> *m = new TreeNode<string>(strings[numb++]);
	ex->firstChild = m;
	if (numb < strings.size())
	{
		if (strings[numb] == "+" || strings[numb] == "-" || strings[numb] == "*")
		{
			TreeNode<string> *operat = new TreeNode<string>(strings[numb++]);
			m->nextSibling = operat;
			operat->nextSibling = new TreeNode<string>(strings[numb++]);
		}
		return ex;
	}
	else
	{
		return ex;
	}
}

template<class T>
TreeNode<T>* dropstate(vector<T> strings, int &numb)
{
	TreeNode<string>* dropToken = new TreeNode<string>(strings[numb++]);
	dropToken->nextSibling = new TreeNode<string>(strings[numb++]);
	dropToken->nextSibling->nextSibling = new TreeNode<string>(strings[numb]);
	return dropToken;
}

template<class T>
TreeNode<T>* createstate(vector<T> strings, int &numb)
{
	TreeNode<string>* createToken = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* tableToken = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* tablename = new TreeNode<string>(strings[numb++]);
	createToken->nextSibling = tableToken;
	tableToken->nextSibling = tablename;
	tablename->nextSibling = attritypelist(strings, numb);
	return createToken;
}

template<class T>
TreeNode<T>* attritypelist(vector<T> strings, int &numb)
{
	string attri = "AttributeTypeList";
	TreeNode<string>* attributelist = new TreeNode<string>(attri);//node AttributeTypeList 
	TreeNode<string>* first = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* last = new TreeNode<string>(strings[numb++]);
	attributelist->firstChild = first;
	first->nextSibling = last;
	while (numb<strings.size())
	{
		last->nextSibling = new TreeNode<string>(strings[numb++]);
		TreeNode<string>* Temp = new TreeNode<string>(strings[numb++]);
		last->nextSibling->nextSibling = Temp;
		last = Temp;
	}
	return attributelist;
}

template<class T>
TreeNode<T>* insertstate(vector<T> strings, int &numb)
{
	TreeNode<string>* insertToken = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* intoToken = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* tableName = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* attrlis = attributelist(strings, numb);
	//TreeNode<string>* insertup = inserttuples(strings, numb);
	insertToken->nextSibling = intoToken;
	intoToken->nextSibling = tableName;
	tableName->nextSibling = attrlis;

	if (strings[numb] == "VALUES") {
		TreeNode<string>* valueToken = new TreeNode<string>(strings[numb++]);
		TreeNode<string>* insertup = inserttuples(strings, numb);
		//attrlis->nextSibling = valueToken;
		valueToken->nextSibling = insertup;
		attrlis->nextSibling = valueToken;
	}
	else
	{

		string selstate = "SelectStatement";
		TreeNode<string>* selectstatement = new TreeNode<string>(selstate);
		selectstatement->firstChild = selectstate(strings, numb);
		attrlis->nextSibling = selectstatement;
	}
	return insertToken;
}

template<class T>
TreeNode<T>* attributelist(vector<T> strings, int &numb)
{
	string attrilis = "AttributeList";
	TreeNode<string>* attrilist = new TreeNode<string>(attrilis);
	TreeNode<string>* last = new TreeNode<string>(strings[numb++]);
	attrilist->firstChild = last;
	while (strings[numb] != "VALUES"&&strings[numb] != "SELECT") {
		TreeNode<string>* temp = new TreeNode<string>(strings[numb++]);
		last->nextSibling = temp;
		last = temp;
	}
	return attrilist;
}

template<class T>
TreeNode<T>* inserttuples(vector<T> strings, int &numb) //Later I'll add the select-statement!!!
{
	string instuple = "InsertTuples";
	TreeNode<string>* insertuple = new TreeNode<string>(instuple);
	TreeNode<string>* last = new TreeNode<string>(strings[numb++]);
	insertuple->firstChild = last;
	while (numb < strings.size())
	{
		TreeNode<string>* temp = new TreeNode<string>(strings[numb++]);
		last->nextSibling = temp;
		last = temp;
	}
	return insertuple;

}

template<class T>
TreeNode<T>* deletestate(vector<T>strings, int &numb)
{
	TreeNode<string>* deleteToken = new TreeNode<string>(strings[numb++]);
	TreeNode<string>* fromToken = new TreeNode<string>(strings[numb++]);
	deleteToken->nextSibling = fromToken;
	TreeNode<string>* tableName = new TreeNode<string>(strings[numb++]);
	fromToken->nextSibling = tableName;
	if (numb < strings.size()) {
		tableName->nextSibling = new TreeNode<string>(strings[numb++]);
		tableName->nextSibling->nextSibling = searchcondition(strings, numb);
	}
	return deleteToken;
}
/*template<class T>
TreeNode<T>* attritype(vector<T> strings, int &numb)
{
TreeNode<T>*
}*/

template<class T>
void traversal(TreeNode<T>* root)
{
	if (root != nullptr)
	{
		cout << root->data << endl;
		traversal(root->firstChild);
		traversal(root->nextSibling);
	}
}

template<class T>
bool SearchNode(TreeNode<T>* root, TreeNode<T>* &current, string name)
{
	bool result = false;
	if (root->data == name) { result = true; current = root; }
	else
	{
		TreeNode<T> *q = root->firstChild;
		while (q != NULL && !(result = SearchNode(q, current, name)))
		{
			q = q->nextSibling;
		}
	}
	return result;
}

template<class T>
TreeNode<T>* Lqp(TreeNode<T>* root)
{
	TreeNode<string>* current;

		TreeNode<string>* pi = new TreeNode<string>("PI");//projection
		SearchNode(root, current, "SelectList");//current = Selectist Node
		pi->nextSibling = current->firstChild;//the nextsiblings of PI are attribute list
		TreeNode<string>*chi = new TreeNode<string>("CHI");//product join
		SearchNode(root, current, "TableList");
		chi->firstChild = current->firstChild;
		TreeNode<string>*last = chi->firstChild;
		while (last != nullptr)
		{
			TreeNode<string>* lastchild = new TreeNode<string>(last->data);
			last->firstChild = lastchild;
			last = last->nextSibling;
		}//For pushing selections

		if (SearchNode(root, current, "SearchCondition"))
		{
			TreeNode<string>*sigma = new TreeNode<string>("SIGMA");//selection
			TreeNode<string>* sigmaAttach = new TreeNode<string>("SearchCondition");
			sigmaAttach->firstChild = current->firstChild;
			sigma->nextSibling = sigmaAttach;
			pi->firstChild = sigma;
			sigma->firstChild = chi;
			string ss = "ORDER";
			if (SearchNode(root, current, ss)) {
				TreeNode<string>* sort = new TreeNode<string>("TAU");//order
				TreeNode<string>* sortAttach = new TreeNode<string>(current->nextSibling->nextSibling->data);//node sort contains ordering attribute
				sort->nextSibling = sortAttach;
				sort->firstChild = sigma;
				pi->firstChild = sort;
			}
		}
		else
		{
			pi->firstChild = chi;
			string ss = "ORDER";
			if (SearchNode(root, current, ss)) {
				TreeNode<string>* sortt = new TreeNode<string>("TAU");//order
				TreeNode<string>* sortAttachh = new TreeNode<string>(current->nextSibling->nextSibling->data);//node sort contains ordering attribute
				sortt->nextSibling = sortAttachh;
				sortt->firstChild = chi;
				pi->firstChild = sortt;
			}
		}

		string s = "DISTINCT";
		if (SearchNode(root, current, s))
		{
			TreeNode<string>*delta = new TreeNode<string>("DELTA");//distinct
			delta->firstChild = pi;
			return delta;
		}
		else {
			return pi;
		}
}

template<class T>
TreeNode<T>* LqpOpt(TreeNode<T>* root)//Improving!!!
{
	TreeNode<string>* chiTable;
	SearchNode(root, chiTable, "CHI");
	TreeNode<string>* current;
	string str = "SearchCondition";

		TreeNode<string>* SearchConPushing = new TreeNode<string>("SearchConPushing");
		if (SearchNode(root, current, str))
		{
			//TreeNode<string>* SearchConPushing = new TreeNode<string>("SearchConPushing");
			TreeNode<string>* Final = SearchConPushing;//
			current = current->firstChild;//
			while (current != nullptr)//BooleanTerm
			{
				TreeNode<string>* selecSubRoot = new TreeNode<string>("selecSubRoot");//for pushing selection!
				TreeNode<string>* last = selecSubRoot;//pointer to pointer
				TreeNode<string>* booleanfactor = current->firstChild;//
				while (booleanfactor != nullptr)//make it more beautiful. Now it smells!
				{
					vector<string> leaves;
					TreeNode<string>* expression = booleanfactor->firstChild;
					TreeNode<string>* expchild = expression->firstChild;
					bool result=true;
					string sample;
					if (chiTable->firstChild->nextSibling == nullptr) { //for one table
						result = true;
						sample = chiTable->firstChild->data;
					}
					else {
						while (expchild != nullptr)
						{
							if (expchild->data.find(".") < 100) {
								leaves.push_back(expchild->data);
							}
							expchild = expchild->nextSibling;
						}
						expression = expression->nextSibling->nextSibling;
						expchild = expression->firstChild;
						while (expchild != nullptr)
						{
							if (expchild->data.find(".") < 100) {
								leaves.push_back(expchild->data);
							}
							expchild = expchild->nextSibling;
						}
						//bool result = true;
						sample = leaves[0].substr(0, leaves[0].find("."));
						//int i = 0;
						for (int i = 0; i < leaves.size(); i++)
						{
							if (leaves[i].substr(0, leaves[i].find(".")) != sample)
								result = false;
						}
					}
					if (result)
					{
						TreeNode<string>* booleanfactorBackup = new TreeNode<string>(sample);
						booleanfactorBackup->nextSibling = booleanfactor->firstChild;
						last->firstChild = booleanfactorBackup;
						last = booleanfactorBackup;
						booleanfactor->data = "pushing";
					}
					if (booleanfactor->nextSibling == nullptr) break;
					else {
						booleanfactor = booleanfactor->nextSibling->nextSibling;
					}
				}

				TreeNode<string>* boolfactorhead = new TreeNode<string>("head");//more secure
				boolfactorhead->nextSibling = current->firstChild;
				current->firstChild = boolfactorhead;
				TreeNode<string>* andToken = new TreeNode<string>("AND");
				andToken->nextSibling = boolfactorhead->nextSibling;
				boolfactorhead->nextSibling = andToken;
				while (boolfactorhead->nextSibling != nullptr)
				{
					if (boolfactorhead->nextSibling->nextSibling->data == "pushing")
					{
						if (boolfactorhead->nextSibling->nextSibling->nextSibling == nullptr)
						{
							boolfactorhead->nextSibling = nullptr; continue;
						}
						else {
							boolfactorhead->nextSibling->nextSibling = boolfactorhead->nextSibling->nextSibling->nextSibling->nextSibling;
							continue;
						}
					}
					else
					{
						if (boolfactorhead->nextSibling->nextSibling->nextSibling == nullptr)break;
						boolfactorhead = boolfactorhead->nextSibling->nextSibling;
					}

				}
				if (current->firstChild->nextSibling == nullptr) { current->firstChild = nullptr; }
				else { current->firstChild = current->firstChild->nextSibling->nextSibling; }//delete headnode

				Final->nextSibling = selecSubRoot;
				Final = selecSubRoot;
				if (current->nextSibling == nullptr)current=current->nextSibling;
				else { current = current->nextSibling->nextSibling; }
				//}
			}
	//******************************** Push the selection down ***********************************//
			SearchNode(root, current, "CHI");//Node current is CHI Node
			TreeNode<string>* lastt = SearchConPushing;
			lastt = lastt->nextSibling;
			if (current->firstChild->nextSibling == nullptr) {
				current->firstChild->firstChild = nullptr;
				while (lastt!=nullptr)
				{
					TreeNode<string>* child = lastt->firstChild;
					TreeNode<string>* tableBackup = new TreeNode<string>(current->firstChild->data);
					tableBackup->firstChild = current->firstChild->firstChild;
					current->firstChild->firstChild = tableBackup;
					while (child!=nullptr)
					{
						TreeNode<string>* childBackup = new TreeNode<string>(child->data);
						childBackup->nextSibling = child->nextSibling;
						childBackup->firstChild = current->firstChild->firstChild;
						current->firstChild->firstChild = childBackup;
						child = child->firstChild;
					}
					lastt = lastt->nextSibling;
				}
			}
			else {
				while (lastt != nullptr)
				{
					TreeNode<string>*child = lastt->firstChild;//node of SearchConPushing subtree
					while (child != nullptr)
					{
						TreeNode<string>* chi_ptr = current->firstChild;//node table-name-backup
						while (chi_ptr != nullptr)
						{
							if (child->data == chi_ptr->data)
							{
								TreeNode<string>* childBackup = new TreeNode<string>(child->data);
								childBackup->nextSibling = child->nextSibling;
								childBackup->firstChild = chi_ptr->firstChild;
								chi_ptr->firstChild = childBackup;
							}
							chi_ptr = chi_ptr->nextSibling;
						}
						child = child->firstChild;//!!!
					}
					lastt = lastt->nextSibling;
				}
			}

		}
		return root;
}


