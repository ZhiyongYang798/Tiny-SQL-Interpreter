using namespace std;
TreeNode<string>* selectstate(std::vector<string>, int &);
TreeNode<string>* selectlist(std::vector<string> , int &);

template<class T>
TreeNode<T>* tablist(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* searchcondition(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* booleanterm(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* booleanfactor(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* expression(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* dropstate(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* createstate(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* attritypelist(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* insertstate(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* attributelist(vector<T> strings, int &numb);
template<class T>
TreeNode<T>* inserttuples(vector<T> strings, int &numb); 
template<class T>
TreeNode<T>* deletestate(vector<T>strings, int &numb);
template<class T>
void traversal(TreeNode<T>* root);
template<class T>
bool SearchNode(TreeNode<T>* , TreeNode<T>* , string );
template<class T>
TreeNode<T>* Lqp(TreeNode<T>* root);
template<class T>
TreeNode<T>* LqpOpt(TreeNode<T>* root);








