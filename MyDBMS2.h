#include "TreeNode.h"
using namespace std;
void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple);
bool comPare(int m, int n, string comp);
int calCulate(int m, int n, string oper);
bool is_number(string str);
void writeFromDiskToMemory(Relation* table, vector<Tuple>& tempTuples, MainMemory& mem);
void writeBackToDisk(Schema final_schema, Relation* &final_table_ptr, vector<Tuple> final_tuples, MainMemory& mem);
template<class T>
TreeNode<T>* intermediateTables(TreeNode<T>* root, MainMemory& mem, SchemaManager& schema_manager);
Relation* naturalJoinAndSelection(TreeNode<string>* root, TreeNode<string>* interTables, MainMemory& mem, SchemaManager& schema_manager);
