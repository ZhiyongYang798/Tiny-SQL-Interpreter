#include <sstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
//#include "TreeNode.h"
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "MyDBMS2.h"
#include "ParseTree.h"
#include "ParseTree.cpp"
using namespace std;

void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple) {
	Block* block_ptr;
	if (relation_ptr->getNumOfBlocks() == 0) {
		cout << "The relation is empty" << endl;
		cout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
		block_ptr = mem.getBlock(memory_block_index);
		block_ptr->clear(); //clear the block
		block_ptr->appendTuple(tuple); // append the tuple
		cout << "Write to the first block of the relation" << endl;
		relation_ptr->setBlock(relation_ptr->getNumOfBlocks(), memory_block_index);
	}
	else {
		cout << "Read the last block of the relation into memory block 5:" << endl;
		relation_ptr->getBlock(relation_ptr->getNumOfBlocks() - 1, memory_block_index);
		block_ptr = mem.getBlock(memory_block_index);

		if (block_ptr->isFull()) {
			cout << "(The block is full: Clear the memory block and append the tuple)" << endl;
			block_ptr->clear(); //clear the block
			block_ptr->appendTuple(tuple); // append the tuple
			cout << "Write to a new block at the end of the relation" << endl;
			relation_ptr->setBlock(relation_ptr->getNumOfBlocks(), memory_block_index); //write back to the relation
		}
		else {
			cout << "(The block is not full: Append it directly)" << endl;
			block_ptr->appendTuple(tuple); // append the tuple
			cout << "Write to the last block of the relation" << endl;
			relation_ptr->setBlock(relation_ptr->getNumOfBlocks() - 1, memory_block_index); //write back to the relation
		}
	}
}

bool comPare(int m, int n, string comp)
{
	if (comp == "=")return m == n;
	else if (comp == ">")return m > n;
	else
	{
		return m < n;
	}
}
int calCulate(int m, int n, string oper)
{
	if (oper == "+") { int k = m + n; return k; }
	else if (oper == "-") { int k = m - n; return k; }
	else
	{
		int k = m*n;
		return k;
	}
}

bool is_number(string str)
{
	if (str.c_str()[0] != 45)
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (str.c_str()[i] < '0' || str.c_str()[i] > '9')
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		for (int i = 1; i < str.length(); i++)
		{
			if (str.c_str()[i] < '0' || str.c_str()[i] > '9')
			{
				return false;
			}
		}
		return true;
	}
}

void writeFromDiskToMemory(Relation* table, vector<Tuple>& tempTuples, MainMemory& mem)
{
	int div = table->getNumOfBlocks() / 10;
	int mod = table->getNumOfBlocks() % 10;
	for (int i = 0; i < div + 1; i++)
	{
		if (i < div) {
			table->getBlocks(i * 10, 0, 10);//get blocks from disk to memory
			for (int k = 0; k < 10; k++) {
				Block* block_ptr = mem.getBlock(k);
				for (int kk = 0; kk < block_ptr->getNumTuples(); kk++) {
					//cout << block_ptr->getTuple(kk) << endl;
					tempTuples.push_back(block_ptr->getTuple(kk));
				}
				block_ptr->clear();
			}
			//tempSmallTuples = mem.getTuples(0, 10);//get tuples from memory
		}
		else
		{
			if (mod > 0) {
				table->getBlocks(i * 10, 0, mod);//when mod==0 discuss!
				for (int k = 0; k < mod; k++) {
					Block* block_ptr = mem.getBlock(k);
					for (int kk = 0; kk < block_ptr->getNumTuples(); kk++) {
						//cout << block_ptr->getTuple(kk) << endl;
						tempTuples.push_back(block_ptr->getTuple(kk));
					}
					block_ptr->clear();
				}
			}
		}
	}
}

void writeBackToDisk(Schema final_schema, Relation* &final_table_ptr, vector<Tuple> final_tuples, MainMemory& mem)
{
	int TuplesPerBlock = final_schema.getTuplesPerBlock();
	int block_num = final_tuples.size() / TuplesPerBlock;
	int tuple_left = final_tuples.size() % TuplesPerBlock;
	int mem_num = block_num / 10;
	int mem_block_left = block_num % 10;//mem_num+mem_block_left+tuple_left!!!
	for (int m = 0; m < mem_num; m++) {
		for (int i = 0; i < 10; i++) {
			Block* block_ptr = mem.getBlock(i);
			block_ptr->clear();
			for (int k = 0; k < TuplesPerBlock; k++) {
				block_ptr->appendTuple(final_tuples[m * 10 * TuplesPerBlock + i*TuplesPerBlock + k]);
			}
		}
		final_table_ptr->setBlocks(final_table_ptr->getNumOfBlocks(), 0, 10);
	}
	for (int m = 0; m < mem_block_left + 1; m++) {
		Block* block_ptr = mem.getBlock(m);
		block_ptr->clear();
		if (m < mem_block_left) {
			for (int k = 0; k < TuplesPerBlock; k++) {
				block_ptr->appendTuple(final_tuples[mem_num * 10 * TuplesPerBlock + m*TuplesPerBlock + k]);
			}
		}
		else {
			for (int i = 0; i < tuple_left; i++) {
				block_ptr->appendTuple(final_tuples[mem_num * 10 * TuplesPerBlock + m*TuplesPerBlock + i]);
			}
		}
	}
	final_table_ptr->setBlocks(final_table_ptr->getNumOfBlocks(), 0, mem_block_left + 1);
}

template<class T>
TreeNode<T>* intermediateTables(TreeNode<T>* root, MainMemory& mem, SchemaManager& schema_manager)
{
	TreeNode<string>* current;
	SearchNode(root, current, "CHI");
	TreeNode<string>* tableName = current->firstChild;
	while (tableName != nullptr)
	{
		Relation* table_ptr = schema_manager.getRelation(tableName->data);
		Schema tuple_schema = table_ptr->getSchema();//schema of tuple
		vector<Tuple> newTableTuples;
		int div = table_ptr->getNumOfBlocks() / 10;
		int mod = table_ptr->getNumOfBlocks() % 10;
		for (int i = 0; i < div + 1; i++)
		{
			vector<Tuple> tempTuples;
			if (i < div) {
				table_ptr->getBlocks(i * 10, 0, 10);//get blocks from disk to memory
				tempTuples = mem.getTuples(0, 10);//get tuples from memory
				for (int m = 0; m < 10; m++)
					mem.getBlock(m)->clear();//clear memory
			}
			else{
				if (mod > 0) {
					table_ptr->getBlocks(i * 10, 0, mod);//when mod==0 discuss!
					tempTuples = mem.getTuples(0, mod);
					for (int m = 0; m < mod; m++)
						mem.getBlock(m)->clear();
				}
			}
			for (int ii = 0; ii < tempTuples.size(); ii++) {
				vector<int> pushJudge;
				TreeNode<string>* tableNameBackup = tableName->firstChild;
				while (tableNameBackup->firstChild != nullptr)
				{
					int i = 0;
					while (tableNameBackup->nextSibling!=nullptr){
					TreeNode<string>* exp1 = tableNameBackup->nextSibling;
					TreeNode<string>* exp2 = tableNameBackup->nextSibling->nextSibling->nextSibling;
					TreeNode<string>* comp = tableNameBackup->nextSibling->nextSibling;
					TreeNode<string>* expchild1 = exp1->firstChild;
					TreeNode<string>* expchild2 = exp2->firstChild;
					string str = expchild1->data;
					string str1 = expchild2->data;
					if (str.find(".") < 100)
					{
						string attribu = str.substr(str.find(".") + 1);
						if (tuple_schema.getFieldType(attribu) == INT)
						{
							int result1 = 0, result2 = 0;
							if (expchild1->nextSibling != nullptr) {
								int n = 0;
								string nextChild = expchild1->nextSibling->nextSibling->data;
								if (is_number(nextChild))
								{
									const char* numm = nextChild.data();
									n = atoi(numm);
								}
								else { n = tempTuples[ii].getField(nextChild.substr(nextChild.find(".") + 1)).integer; }
								result1 = calCulate(tempTuples[ii].getField(attribu).integer, n, expchild1->nextSibling->data);
							}
							else result1 = tempTuples[ii].getField(attribu).integer;

							int n1 = 0;
							if (is_number(str1)) {
								const char* numm = str1.data();
								n1 = atoi(numm);
							}
							else { n1 = tempTuples[ii].getField(str1.substr(str1.find(".") + 1)).integer; }

							if (expchild2->nextSibling != nullptr) {//!
								string str2 = expchild2->nextSibling->nextSibling->data;
								int n2 = 0;
								if (is_number(str2)) {
									const char* numm = str2.data();
									n2 = atoi(numm);
								}
								else { n2 = tempTuples[ii].getField(str2.substr(str2.find(".") + 1)).integer; }
								result2 = calCulate(n1, n2, expchild2->nextSibling->data);
							}
							else result2 = n1;
							if (comPare(result1, result2, comp->data) == false) { i = 1; }
						}
						else
						{
							if (str1.find(".") < 100)
							{
								string str3 = str1.substr(str1.find(".") + 1);
								if (*(tempTuples[ii].getField(attribu).str) != *(tempTuples[ii].getField(str3).str))i = 1;
							}
							else
							{
								if (*(tempTuples[ii].getField(attribu).str) != str1)i = 1;
							}
						}

					}
					else {
						if (tuple_schema.getFieldType(str) == INT)
						{
							int result1 = 0, result2 = 0;
							if (expchild1->nextSibling != nullptr) {
								int n = 0;
								string nextChild = expchild1->nextSibling->nextSibling->data;
								if (is_number(nextChild))
								{
									const char* numm = nextChild.data();
									n = atoi(numm);
								}
								else { n = tempTuples[ii].getField(nextChild).integer; }
								result1 = calCulate(tempTuples[ii].getField(str).integer, n, expchild1->nextSibling->data);
							}
							else result1 = tempTuples[ii].getField(str).integer;

							int n1 = 0;
							if (is_number(str1)) {
								const char* numm = str1.data();
								n1 = atoi(numm);
							}
							else { n1 = tempTuples[ii].getField(str1).integer; }

							if (expchild2->nextSibling != nullptr) {//!
								string str2 = expchild2->nextSibling->nextSibling->data;
								int n2 = 0;
								if (is_number(str2)) {
									const char* numm = str2.data();
									n2 = atoi(numm);
								}
								else { n2 = tempTuples[ii].getField(str2).integer; }
								result2 = calCulate(n1, n2, expchild2->nextSibling->data);
							}
							else result2 = n1;
							if (comPare(result1, result2, comp->data) == false) { i = 1; }
						}
						else
						{
							if (str1.find("\"") < 100)
							{
								if (*(tempTuples[ii].getField(str).str) != str1)i = 1;
							}
							else
							{
								if (*(tempTuples[ii].getField(str).str) != *(tempTuples[ii].getField(str1).str))i = 1;
							}
						}

					}
					tableNameBackup = tableNameBackup->firstChild;
				  }
				  pushJudge.push_back(i);
				  if (tableNameBackup->firstChild == nullptr)break;
				  tableNameBackup = tableNameBackup->firstChild;
				}
				if (pushJudge.size()==0) newTableTuples.push_back(tempTuples[ii]);
				else {
					bool result = false;
					for (int i = 0; i < pushJudge.size(); i++) {
						if (pushJudge[i] == 0)result = true;
					}
					if (result)newTableTuples.push_back(tempTuples[ii]);
				}
			}
		}
	//****************************************** Write back to disk *********************************************//
		string newTableName = tableName->data + "Backup";
		if (schema_manager.relationExists(newTableName))schema_manager.deleteRelation(newTableName);
		schema_manager.createRelation(newTableName, tuple_schema);
		Relation* newTableName_ptr = schema_manager.getRelation(newTableName);
		writeBackToDisk(tuple_schema, newTableName_ptr, newTableTuples, mem);
		tableName = tableName->nextSibling;
	}
	return current->firstChild;
}

//************************************************* Joins Projection Distinct Order *******************************************************//
Relation* naturalJoinAndSelection(TreeNode<string>* root, TreeNode<string>* interTables, MainMemory& mem, SchemaManager& schema_manager)//
{
	string joinTableName;
	TreeNode<string>* interTable_ptr = interTables;
	while (interTable_ptr != nullptr)
	{
		joinTableName = joinTableName + interTable_ptr->data;
		interTable_ptr->data = interTable_ptr->data + "Backup";
		interTable_ptr = interTable_ptr->nextSibling;
	}
	TreeNode<string>* current;
	bool result=SearchNode(root, current, "SearchCondition");

	//******************************************** One Table ******************************************//
	if (interTables->nextSibling==nullptr) {//all circumstances of one table
		Relation* singleTable = schema_manager.getRelation(interTables->data);
		Schema singleSchema = singleTable->getSchema();
		vector<Tuple> tempTuples;
		writeFromDiskToMemory(singleTable, tempTuples, mem);
	//****************************************** Order ****************************************//
		if (SearchNode(root, current, "TAU")) {
			string orderAttri = current->nextSibling->data;
			if (singleSchema.getFieldType(orderAttri) == INT) {
				Tuple joinTu = singleTable->createTuple();
				for (int i = 1; i < tempTuples.size(); i++) {
					for (int j = 0; j < tempTuples.size() - i; j++) {
						if (tempTuples[j].getField(orderAttri).integer > tempTuples[j + 1].getField(orderAttri).integer) {
							joinTu = tempTuples[j + 1];
							tempTuples[j + 1] = tempTuples[j];
							tempTuples[j] = joinTu;
						}
					}
				}
			}
		}
	//**************************************** Projection *************************************//
		//for (int kkk = 0; kkk < tempTuples.size(); kkk++)cout << tempTuples[kkk] << endl;
		vector<string> new_field_names;
		vector<enum FIELD_TYPE> new_field_types;
		vector<Tuple> sub_final_tuples;//for several columns
		SearchNode(root, current, "PI");
		TreeNode<string>* currentTemp = current->nextSibling;

	//**************************************** Porject "*" ************************************//
		if (currentTemp->data == "*") {//distinct * from multitables
			vector<Tuple>final_tuples;
	//*************************************** Distinct ***************************************//
			if (SearchNode(root, current, "DELTA")) {
				final_tuples.push_back(tempTuples[0]);
				for (int i = 1; i < tempTuples.size(); i++) {
					int k = 0;
					for (int m = 0; m < final_tuples.size(); m++) {
						int kk = 0;
						for (int j = 0; j < singleSchema.getNumOfFields(); j++) {
							if (singleSchema.getFieldType(j) == INT) {
								if (tempTuples[i].getField(j).integer != final_tuples[m].getField(j).integer)kk = 1;
							}
							else {
								if (*(tempTuples[i].getField(j).str) != *(final_tuples[m].getField(j).str))kk = 1;
							}
						}
						if (kk == 0) k = 1;
					}
					if (k == 0)final_tuples.push_back(tempTuples[i]);
				}
			}
			else { for (int kkk = 0; kkk < tempTuples.size(); kkk++)final_tuples.push_back(tempTuples[kkk]); }

	//************************************** Write back to disk from memory ****************************************//
			string final_table = "FinalTable";
			schema_manager.createRelation(final_table, singleSchema);
			Relation* final_table_ptr = schema_manager.getRelation(final_table);
			writeBackToDisk(singleSchema,final_table_ptr, final_tuples,mem);
			return final_table_ptr;
		}

	//******************************************* Porject few columns ***************************************//
		else {
			while (currentTemp != nullptr)
			{
				string attribu = currentTemp->data;
				if (attribu.find(".") < 100) {
					attribu = attribu.substr(attribu.find(".") + 1);
				}
				new_field_names.push_back(attribu);
				new_field_types.push_back(singleSchema.getFieldType(attribu));
				currentTemp = currentTemp->nextSibling;
			}
			Schema final_schema(new_field_names, new_field_types);
			string final_table = "FinalTable";
			schema_manager.createRelation(final_table, final_schema);
			Relation* final_table_ptr = schema_manager.getRelation(final_table);

			for (int i = 0; i < tempTuples.size(); i++) {
				Tuple new_tuple = final_table_ptr->createTuple();
				for (int k = 0; k<final_schema.getNumOfFields(); k++)
				{
					if (final_schema.getFieldType(k) == INT)
						new_tuple.setField(final_schema.getFieldName(k), tempTuples[i].getField(final_schema.getFieldName(k)).integer);
					else new_tuple.setField(final_schema.getFieldName(k), *(tempTuples[i].getField(final_schema.getFieldName(k)).str));
				}
				sub_final_tuples.push_back(new_tuple);
			}
			//for (int kkk = 0; kkk < sub_final_tuples.size(); kkk++)cout << sub_final_tuples[kkk] << endl;
			vector<Tuple>final_tuples;
			if (SearchNode(root, current, "DELTA")) {
				final_tuples.push_back(sub_final_tuples[0]);
				for (int i = 1; i < sub_final_tuples.size(); i++) {
					int k = 0;
					for (int m = 0; m < final_tuples.size(); m++) {
						int kk = 0;
						for (int j = 0; j < final_schema.getNumOfFields(); j++) {
							if (final_schema.getFieldType(j) == INT) {
								if (sub_final_tuples[i].getField(j).integer != final_tuples[m].getField(j).integer)kk = 1;
							}
							else {
								if (*(sub_final_tuples[i].getField(j).str) != *(final_tuples[m].getField(j).str))kk = 1;
							}
						}
						if (kk == 0) k = 1;
					}
					if (k == 0)final_tuples.push_back(sub_final_tuples[i]);
				}
			}
			else { for (int kkk = 0; kkk < sub_final_tuples.size(); kkk++)final_tuples.push_back(sub_final_tuples[kkk]); }
			//write back to disk
			writeBackToDisk(final_schema,final_table_ptr,final_tuples,mem);
			return final_table_ptr;
		}
		//schema_manager.deleteRelation(interTables->data);
	}

	//******************************************* Multi Tables ***********************************************//
	else
	{
		int k = 0;
		//********************************* To decide if it is C Join **************************************//
		if (result&&current->firstChild->firstChild!=nullptr) {
			TreeNode<string>* boolfactor = current->firstChild->firstChild;
			while (boolfactor != nullptr) {
				if (boolfactor->firstChild->nextSibling->data == "=")k = 1;
				if (boolfactor->nextSibling == nullptr) boolfactor = boolfactor->nextSibling;
				else {
					boolfactor = boolfactor->nextSibling->nextSibling;
				}
			}
		}
		Relation* Smaller = schema_manager.getRelation(interTables->data);
		Relation* Larger = schema_manager.getRelation(interTables->nextSibling->data);
		if (Smaller->getNumOfBlocks() > Larger->getNumOfBlocks()) {
			Larger = Smaller;
			Smaller = schema_manager.getRelation(interTables->nextSibling->data);
		}
		vector<string>field_names;
		vector<enum FIELD_TYPE> field_types;
		Schema smallSchema = Smaller->getSchema();
		Schema largeSchema = Larger->getSchema();
		string smallNameOri = Smaller->getRelationName().erase(Smaller->getRelationName().size() - 6);
		string largeNameOri = Larger->getRelationName().erase(Larger->getRelationName().size() - 6);
		vector<Tuple> newTableTuples;
		int div = Smaller->getNumOfBlocks() / 10;
		int mod = Smaller->getNumOfBlocks() % 10;
		vector<string>wholeNeglect;//for natural join
		vector<string>wholeRemain;
		TreeNode<string>* current2;//for natural join
		//**************************************** Cross Join and C Join *****************************************//
		if (result==false||current->firstChild->firstChild==nullptr||k==0) {
			for (int i = 0; i < smallSchema.getNumOfFields(); i++)
			{

				field_names.push_back(smallNameOri + "." + smallSchema.getFieldName(i));
				field_types.push_back(smallSchema.getFieldType(i));
			}
			for (int m = 0; m < largeSchema.getNumOfFields(); m++)
			{
				field_names.push_back(largeNameOri + "." + largeSchema.getFieldName(m));
				field_types.push_back(largeSchema.getFieldType(m));
			}
			Schema joinTableSchema(field_names, field_types);
			if (schema_manager.relationExists(joinTableName))schema_manager.deleteRelation(joinTableName);
			schema_manager.createRelation(joinTableName, joinTableSchema);
			Relation* joinTable_ptr = schema_manager.getRelation(joinTableName);
			for (int i = 0; i < div + 1; i++)
			{
				vector<Tuple> tempSmallTuples;
				if (i < div) {
					Smaller->getBlocks(i * 10, 0, 10);//get blocks from disk to memory
					for (int k = 0; k < 10; k++) {
						Block* block_ptr = mem.getBlock(k);
						for (int kk = 0; kk < block_ptr->getNumTuples(); kk++)
							tempSmallTuples.push_back(block_ptr->getTuple(kk));
						block_ptr->clear();
					}
					//tempSmallTuples = mem.getTuples(0, 10);//get tuples from memory
				}
				else
				{
					if (mod > 0) {
						Smaller->getBlocks(i * 10, 0, mod);//when mod==0 discuss!
						for (int k = 0; k < mod; k++) {
							Block* block_ptr = mem.getBlock(k);
							for (int kk = 0; kk < block_ptr->getNumTuples(); kk++)
								tempSmallTuples.push_back(block_ptr->getTuple(kk));
							block_ptr->clear();
						}
					}
				}
				for (int k = 0; k < Larger->getNumOfBlocks(); k++) {
					Larger->getBlock(k, 0);
					vector<Tuple>tempLargeTuples;
					Block* block0_ptr = mem.getBlock(0);
					tempLargeTuples = block0_ptr->getTuples();
					block0_ptr->clear();
					for (int j = 0; j < tempLargeTuples.size(); j++) {
						for (int m = 0; m < tempSmallTuples.size(); m++) {
							Tuple joinTuple = joinTable_ptr->createTuple();
							for (int jj = 0; jj < tempSmallTuples[m].getNumOfFields(); jj++) {
								if (smallSchema.getFieldType(jj) == INT)
									joinTuple.setField(smallNameOri + "." + smallSchema.getFieldName(jj), tempSmallTuples[m].getField(jj).integer);
								else { joinTuple.setField(smallNameOri + "." + smallSchema.getFieldName(jj), *(tempSmallTuples[m].getField(jj).str)); }
							}
							for (int kk = 0; kk < tempLargeTuples[j].getNumOfFields(); kk++) {
								if (largeSchema.getFieldType(kk) == INT)
									joinTuple.setField(largeNameOri + "." + largeSchema.getFieldName(kk), tempLargeTuples[j].getField(kk).integer);
								else { joinTuple.setField(largeNameOri + "." + largeSchema.getFieldName(kk), *(tempLargeTuples[j].getField(kk).str)); }
							}
							if (result == false || current->firstChild->firstChild == nullptr)newTableTuples.push_back(joinTuple);
							else {
								int q = 0;
								TreeNode<string>* boolfactor = current->firstChild->firstChild;
								while (boolfactor != nullptr) {
									string attriLeft = boolfactor->firstChild->firstChild->data;
									string attriRight = boolfactor->firstChild->nextSibling->nextSibling->firstChild->data;
									string comp = boolfactor->firstChild->nextSibling->data;
									string leftTable = attriLeft.substr(0, attriLeft.find("."));
									string leftAttri = attriLeft.substr(attriLeft.find(".") + 1);
									string rightTable = attriRight.substr(0, attriRight.find("."));
									string rightAttri = attriRight.substr(attriRight.find(".") + 1);
									if (leftTable == smallNameOri) {
										if (smallSchema.getFieldType(leftAttri) == INT) {
											if (comPare(tempSmallTuples[m].getField(leftAttri).integer, tempLargeTuples[j].getField(rightAttri).integer, comp) == false)
												q = 1;
										}
									}
									else {
										if (smallSchema.getFieldType(rightAttri) == INT) {
											if (comPare(tempLargeTuples[j].getField(leftAttri).integer, tempSmallTuples[m].getField(rightAttri).integer, comp) == false)
												q = 1;
										}
									}
									if (boolfactor->nextSibling == nullptr) boolfactor = boolfactor->nextSibling;
									else {
										boolfactor = boolfactor->nextSibling->nextSibling;
									}
								}
								if (q == 0) {
									newTableTuples.push_back(joinTuple);
								}
							}
						}
					}
				}
			}
		}


		//********************************************* Natural Join ********************************************//
		else {
			for (int i = 0; i < smallSchema.getNumOfFields(); i++)
			{

				field_names.push_back(smallNameOri + "." + smallSchema.getFieldName(i));
				field_types.push_back(smallSchema.getFieldType(i));
			}
			SearchNode(current, current2, "BooleanFactor");
			vector<string>attribuNegelect;//in case of more than one pair of attributes are the same
			while (current2 != nullptr)
			{
				if (current2->firstChild->nextSibling->data == "=") {
					string attriLeft = current2->firstChild->firstChild->data;
					string attriRight = current2->firstChild->nextSibling->nextSibling->firstChild->data;
					string neglectAttri, wholeNeg, wholeRem;
					if (attriLeft.substr(0, attriLeft.find(".")) == smallNameOri) {
						neglectAttri = attriRight.substr(attriRight.find(".") + 1);
						wholeNeg = attriRight;
						wholeRem = attriLeft;
					}
					else {
						neglectAttri = attriLeft.substr(attriLeft.find(".") + 1);
						wholeNeg = attriLeft;
						wholeRem = attriRight;
					}
					attribuNegelect.push_back(neglectAttri);
					wholeNeglect.push_back(wholeNeg);
					wholeRemain.push_back(wholeRem);
				}
				if (current2->nextSibling == nullptr) {
					current2 = current2->nextSibling;
				}//improving!!!!
				else { current2 = current2->nextSibling->nextSibling; }
			}
			for (int m = 0; m < largeSchema.getNumOfFields(); m++)
			{
				int i = 0;
				for (int k = 0; k < attribuNegelect.size(); k++)
				{
					if (attribuNegelect[k] == largeSchema.getFieldName(m))i = 1;
				}
				if (i == 0) {
					field_names.push_back(largeNameOri + "." + largeSchema.getFieldName(m));
					field_types.push_back(largeSchema.getFieldType(m));
				}
			}
			Schema joinTableSchema(field_names, field_types);
			if (schema_manager.relationExists(joinTableName))schema_manager.deleteRelation(joinTableName);
			schema_manager.createRelation(joinTableName, joinTableSchema);
			Relation* joinTable_ptr = schema_manager.getRelation(joinTableName);
			for (int i = 0; i < div + 1; i++)
			{
				vector<Tuple> tempSmallTuples;
				if (i < div) {
					Smaller->getBlocks(i * 10, 0, 10);//get blocks from disk to memory
					for (int k = 0; k < 10; k++) {
						Block* block_ptr = mem.getBlock(k);
						for (int kk = 0; kk < block_ptr->getNumTuples(); kk++)
							tempSmallTuples.push_back(block_ptr->getTuple(kk));
						block_ptr->clear();
					}
				}
				else
				{
					if (mod > 0) {
						Smaller->getBlocks(i * 10, 0, mod);//when mod==0 discuss!
						for (int k = 0; k < mod; k++) {
							Block* block_ptr = mem.getBlock(k);
							for (int kk = 0; kk < block_ptr->getNumTuples(); kk++)
								tempSmallTuples.push_back(block_ptr->getTuple(kk));
							block_ptr->clear();
						}
					}
				}
				for (int k = 0; k < Larger->getNumOfBlocks(); k++) {
					Larger->getBlock(k, 0);
					vector<Tuple>tempLargeTuples;
					Block* block0_ptr = mem.getBlock(0);
					tempLargeTuples = block0_ptr->getTuples();
					block0_ptr->clear();
					for (int j = 0; j < tempLargeTuples.size(); j++) {
						for (int m = 0; m < tempSmallTuples.size(); m++) {
							SearchNode(current, current2, "BooleanFactor");
							int q = 0;
							while (current2 != nullptr)
							{
								string attriLeft = current2->firstChild->firstChild->data;
								string attriRight = current2->firstChild->nextSibling->nextSibling->firstChild->data;
								string comp = current2->firstChild->nextSibling->data;
								string leftTable = attriLeft.substr(0, attriLeft.find("."));
								string leftAttri = attriLeft.substr(attriLeft.find(".") + 1);
								string rightTable = attriRight.substr(0, attriRight.find("."));
								string rightAttri = attriRight.substr(attriRight.find(".") + 1);
								if (leftTable == smallNameOri) {
									if (smallSchema.getFieldType(leftAttri) == INT) {
										if (comPare(tempSmallTuples[m].getField(leftAttri).integer, tempLargeTuples[j].getField(rightAttri).integer, comp) == false)
											q = 1;
									}
									else {
										if (*(tempSmallTuples[m].getField(leftAttri).str) != *(tempLargeTuples[j].getField(rightAttri).str))
											q = 1;
									}
								}
								else {
									if (smallSchema.getFieldType(rightAttri) == INT) {
										if (comPare(tempLargeTuples[j].getField(leftAttri).integer, tempSmallTuples[m].getField(rightAttri).integer, comp) == false)
											q = 1;
									}
									else {
										if (*(tempSmallTuples[m].getField(rightAttri).str) != *(tempLargeTuples[j].getField(leftAttri).str))
											q = 1;
									}
								}
								if (current2->nextSibling == nullptr) {
									current2 = current2->nextSibling;
								}
								else { current2 = current2->nextSibling->nextSibling; }
							}
							if (q == 0) {
								Tuple joinTuple = joinTable_ptr->createTuple();
								for (int jj = 0; jj < tempSmallTuples[m].getNumOfFields(); jj++) {
									if (smallSchema.getFieldType(jj) == INT)
										joinTuple.setField(smallNameOri + "." + smallSchema.getFieldName(jj), tempSmallTuples[m].getField(jj).integer);
									else { joinTuple.setField(smallNameOri + "." + smallSchema.getFieldName(jj), *(tempSmallTuples[m].getField(jj).str)); }
								}
								for (int kk = 0; kk < tempLargeTuples[j].getNumOfFields(); kk++) {
									int m = 0;
									for (int i = 0; i < wholeNeglect.size(); i++) { if (wholeNeglect[i].substr(wholeNeglect[i].find(".") + 1) == largeSchema.getFieldName(kk)) m = 1; }
									if (m == 0) {
										if (largeSchema.getFieldType(kk) == INT)
											joinTuple.setField(largeNameOri + "." + largeSchema.getFieldName(kk), tempLargeTuples[j].getField(kk).integer);
										else { joinTuple.setField(largeNameOri + "." + largeSchema.getFieldName(kk), *(tempLargeTuples[j].getField(kk).str)); }
									}
								}
								newTableTuples.push_back(joinTuple);
							}
						}
					}
				}
			}
		}
		//********************************************** Order *************************************************//
		Schema joinTableSchema = newTableTuples[0].getSchema();
		Relation* joinTable_ptr = schema_manager.getRelation(joinTableName);
			if (SearchNode(root, current2, "TAU")) {
				string orderAttri = current2->nextSibling->data;
				if (joinTableSchema.getFieldType(orderAttri) == INT) {
					Tuple joinTu = joinTable_ptr->createTuple();
					for (int i = 1; i < newTableTuples.size(); i++) {
						for (int j = 0; j < newTableTuples.size() - i; j++) {
							if (newTableTuples[j].getField(orderAttri).integer > newTableTuples[j + 1].getField(orderAttri).integer) {
								joinTu = newTableTuples[j + 1];
								newTableTuples[j + 1] = newTableTuples[j];
								newTableTuples[j] = joinTu;
							}
						}
					}
				}
			}

		//********************************************* Projection(Project "*") ***********************************************//
			vector<string> new_field_names;
			vector<enum FIELD_TYPE> new_field_types;
			vector<Tuple> sub_final_tuples;//for several columns
			SearchNode(root, current2, "PI");
			TreeNode<string>* currentTemp = current2->nextSibling;


			if (currentTemp->data == "*") {//distinct * from multitables
				for (int kkk = 0; kkk < newTableTuples.size(); kkk++)sub_final_tuples.push_back(newTableTuples[kkk]);
				string final_table = "FinalTable";
				for (int i = 0; i < joinTableSchema.getNumOfFields(); i++) {
					new_field_names.push_back(joinTableSchema.getFieldName(i));
					new_field_types.push_back(joinTableSchema.getFieldType(i));
				}
				Schema final_schema(new_field_names, new_field_types);
				schema_manager.createRelation(final_table, final_schema);
				Relation* final_table_ptr = schema_manager.getRelation(final_table);
				vector<Tuple>final_tuples;
	   //******************************************** Distinct **************************************************//
				if (SearchNode(root, current2, "DELTA")) {
					final_tuples.push_back(sub_final_tuples[0]);
					for (int i = 1; i < sub_final_tuples.size(); i++) {
						int k = 0;
						for (int m = 0; m < final_tuples.size(); m++) {
							int kk = 0;
							for (int j = 0; j < final_schema.getNumOfFields(); j++) {
								if (final_schema.getFieldType(j) == INT) {
									if (sub_final_tuples[i].getField(j).integer != final_tuples[m].getField(j).integer)kk = 1;
								}
								else {
									if (*(sub_final_tuples[i].getField(j).str) != *(final_tuples[m].getField(j).str))kk = 1;
								}
							}
							if (kk == 0) k = 1;
						}
						if (k == 0)final_tuples.push_back(sub_final_tuples[i]);
					}
				}
				else { for (int kkk = 0; kkk < sub_final_tuples.size(); kkk++)final_tuples.push_back(sub_final_tuples[kkk]); }
	  //******************************************** Write back to disk *************************************************//
				writeBackToDisk(final_schema,final_table_ptr,final_tuples,mem);
				schema_manager.deleteRelation(interTables->data);
				schema_manager.deleteRelation(interTables->nextSibling->data);
				return final_table_ptr;
			}
    //******************************************** Projection(Project a few columns) **********************************************//
			else {
				while (currentTemp != nullptr)
				{
					string attribu = currentTemp->data;
					for (int i = 0; i < wholeNeglect.size(); i++) {
						if (attribu == wholeNeglect[i])attribu = wholeRemain[i];
					}
					new_field_names.push_back(attribu);
					new_field_types.push_back(joinTableSchema.getFieldType(attribu));
					currentTemp = currentTemp->nextSibling;
				}
				Schema final_schema(new_field_names, new_field_types);
				string final_table = "FinalTable";
				schema_manager.createRelation(final_table, final_schema);
				Relation* final_table_ptr = schema_manager.getRelation(final_table);

				for (int i = 0; i < newTableTuples.size(); i++) {
					Tuple new_tuple = final_table_ptr->createTuple();
					for (int k = 0; k < final_schema.getNumOfFields(); k++)
					{
						if (final_schema.getFieldType(k) == INT)
							new_tuple.setField(final_schema.getFieldName(k), newTableTuples[i].getField(final_schema.getFieldName(k)).integer);
						else new_tuple.setField(final_schema.getFieldName(k), *(newTableTuples[i].getField(final_schema.getFieldName(k)).str));
					}
					sub_final_tuples.push_back(new_tuple);
				}
				//for (int kkk = 0; kkk < sub_final_tuples.size(); kkk++)cout << sub_final_tuples[kkk] << endl;
				vector<Tuple>final_tuples;
				if (SearchNode(root, current2, "DELTA")) {
					final_tuples.push_back(sub_final_tuples[0]);
					for (int i = 1; i < sub_final_tuples.size(); i++) {
						int k = 0;
						for (int m = 0; m < final_tuples.size(); m++) {
							int kk = 0;
							for (int j = 0; j < final_schema.getNumOfFields(); j++) {
								if (final_schema.getFieldType(j) == INT) {
									if (sub_final_tuples[i].getField(j).integer != final_tuples[m].getField(j).integer)kk = 1;
								}
								else {
									if (*(sub_final_tuples[i].getField(j).str) != *(final_tuples[m].getField(j).str))kk = 1;
								}
							}
							if (kk == 0) k = 1;
						}
						if (k == 0)final_tuples.push_back(sub_final_tuples[i]);
					}
				}
				else { for (int kkk = 0; kkk < sub_final_tuples.size(); kkk++)final_tuples.push_back(sub_final_tuples[kkk]); }
				writeBackToDisk(final_schema, final_table_ptr,final_tuples,mem);
				schema_manager.deleteRelation(interTables->data);
				schema_manager.deleteRelation(interTables->nextSibling->data);
				return final_table_ptr;
			}

		}//return interTables;
	}

int main() {

	MainMemory mem;
	Disk disk;
	SchemaManager schema_manager(&mem, &disk);

	disk.resetDiskIOs();
	disk.resetDiskTimer();
	clock_t start_time;
	start_time = clock();
	ifstream in("TinySQL.txt");
	string line;
	while (getline(in, line)) {
		vector<string> strings;
		istringstream f(line);
		string s;
		while (getline(f, s, ' ')) {
			istringstream ff(s);
			string ss;
			while (getline(ff, ss, ',')) {
				istringstream fff(ss);
				string sss;
				while (getline(fff, sss, '('))
				{
					istringstream ffff(sss);
					string ssss;
					while (getline(ffff, ssss, ')'))
					{
						strings.push_back(ssss);
					}
				}
			}
		}

		TreeNode<string>*Root;
		TreeNode<string>* current;
		int numd = 0;
		if (strings[numd] == "SELECT") {
			string selectstatement = "SelectStatement";
			Root = new TreeNode<string>(selectstatement);//root node selectstatement
			Root->firstChild = selectstate(strings, numd);//inner node selectstatement						
		}
		else if (strings[numd] == "DROP") {
			string dropstatement = "DropStatement";
			Root = new TreeNode<string>(dropstatement);
			Root->firstChild = dropstate(strings, numd);
		}
		else if (strings[numd] == "CREATE") {
			string createstatement = "CreateStatement";
			Root = new TreeNode<string>(createstatement);
			Root->firstChild = createstate(strings, numd);
		}
		else if (strings[numd] == "INSERT") {
			string insertstatement = "InsertStatement";
			Root = new TreeNode<string>(insertstatement);
			Root->firstChild = insertstate(strings, numd);
		}
		else
		{
			string deletestatement = "DeleteStatement";
			Root = new TreeNode<string>(deletestatement);
			Root->firstChild = deletestate(strings, numd);
		}
		if (Root->data == "CreateStatement")
		{
			vector<string> field_names;
			vector<enum FIELD_TYPE> field_types;
			string tablename = Root->firstChild->nextSibling->nextSibling->data;
			TreeNode<string>* attribute = Root->firstChild->nextSibling->nextSibling->nextSibling->firstChild;
			while (attribute != nullptr)
			{
				field_names.push_back(attribute->data);
				if (attribute->nextSibling->data == "INT")
				{
					field_types.push_back(INT);
				}
				else
				{
					field_types.push_back(STR20);
				}
				attribute = attribute->nextSibling->nextSibling;
			}
			Schema schema(field_names, field_types);
			schema_manager.createRelation(tablename, schema);
		}
		else if (Root->data == "DropStatement")
		{
			schema_manager.deleteRelation(Root->firstChild->nextSibling->nextSibling->data);
		}
		else if (Root->data == "InsertStatement")
		{
			string tableName = Root->firstChild->nextSibling->nextSibling->data;
			Relation* InsertTable_ptr = schema_manager.getRelation(tableName);//get relation!
			Schema InsertSchema = InsertTable_ptr->getSchema();
			Tuple insTuple = InsertTable_ptr->createTuple();
			SearchNode(Root, current, "AttributeList");
			TreeNode<string>* current2;
			
			if (SearchNode(Root, current2, "InsertTuples"))
			{
				TreeNode<string>* currentbackup = current->firstChild;
				TreeNode<string>* current2backup = current2->firstChild;
				int ii = 0, jj = 0;
				while (currentbackup != nullptr) { ii++; currentbackup = currentbackup->nextSibling; }
				while (current2backup != nullptr) { jj++; current2backup = current2backup->nextSibling; }
				if (ii != jj) {
					cout << "The number of inserted values didn't match the number of attributes!" << endl;
					continue;
				}
				else {
					current = current->firstChild;
					current2 = current2->firstChild;
					int num = InsertTable_ptr->getSchema().getNumOfFields();
					for (int i = 0; i < num; i++)
					{
						if (InsertSchema.getFieldType(current->data) == 0)//INT
						{
								const char* str = current2->data.data();
								int n = atoi(str);
								insTuple.setField(current->data, n);
						}
						else
						{
								insTuple.setField(current->data, current2->data);
						}
						current = current->nextSibling;
						current2 = current2->nextSibling;
					}
					appendTupleToRelation(InsertTable_ptr, mem, 0, insTuple);
				}
			}
			else if(SearchNode(Root, current2, "SelectStatement"))
			{
				TreeNode<string>* ExpTreeRoot = Lqp(current2);
				TreeNode<string>* RootMoot = LqpOpt(ExpTreeRoot);
				TreeNode<string>* interTable = intermediateTables(RootMoot, mem, schema_manager);
				Relation* final_ptr = naturalJoinAndSelection(RootMoot, interTable, mem, schema_manager);
				vector<Tuple> tempTuples;
				writeFromDiskToMemory(final_ptr, tempTuples, mem);
				writeBackToDisk(InsertSchema, InsertTable_ptr, tempTuples, mem);
				schema_manager.deleteRelation(final_ptr->getRelationName());
			}
		}
		else if (Root->data == "SelectStatement") {
			TreeNode<string>* ExpTreeRoot = Lqp(Root);
			TreeNode<string>* RootMoot = LqpOpt(ExpTreeRoot);
			TreeNode<string>* interTable = intermediateTables(RootMoot, mem, schema_manager);
			Relation* final_ptr=naturalJoinAndSelection(RootMoot, interTable, mem, schema_manager);
			cout << *final_ptr << endl;
			schema_manager.deleteRelation(final_ptr->getRelationName());
		}
     }
	 cout << "Real elapse time = " << ((double)(clock() - start_time) / CLOCKS_PER_SEC * 1000) << " ms" << endl;
	 cout << "Calculated elapse time = " << disk.getDiskTimer() << " ms" << endl;
	 cout << "Calculated Disk I/Os = " << disk.getDiskIOs() << endl;

	return 0;
}


