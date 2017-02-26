template<class T>
struct TreeNode
{
	T data;
	TreeNode<T> *firstChild, *nextSibling;
	TreeNode(T value = 0, TreeNode<T> *fc = nullptr, TreeNode<T> *ns = nullptr)
		:data(value), firstChild(fc), nextSibling(ns) {}
};
