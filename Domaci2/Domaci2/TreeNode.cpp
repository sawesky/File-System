#include "TreeNode.h"


TreeNode::~TreeNode()
{
	parent_ = nullptr;
	for (auto child : children_dummy_) {
		delete child;
		child = nullptr;
	}
}

vector<TreeNode*> & TreeNode::getChildren() 
{
	return children_dummy_;
}

string TreeNode::getText() const
{
	return "";
}

vector<Commands*> &TreeNode::getCommands()
{
	return commands_dummy_;
}

TreeNode* TreeNode::getParent()
{
	return parent_;
}

string TreeNode::getName() const
{
	return name_;
}


string TreeNode::getExt() const
{
	return ext_;
}

int TreeNode::getNum() const
{
	return num_;
}

void TreeNode::setParent(TreeNode* parent)
{
	parent_ = parent;
}

void TreeNode::setName(string name)
{
	name_ = name;
}


void TreeNode::setExt(string ext)
{
	ext = ext_;
}

void TreeNode::setNum(int num)
{
	num_ = num;
}

Folder::~Folder()
{
	for (auto child : children_) { //decu postavi na nullptr;
		delete child;
		child = nullptr;
	}
}

vector<TreeNode*> & Folder::getChildren()
{
	return children_;
}

string TxtFile::getText() const
{
	return text_;
}

ExeFile::~ExeFile()
{
	for (auto child : commands_) { //komande postavi na nullptr
		delete child;
		child = nullptr;
	}
}

vector<Commands*> &ExeFile::getCommands()
{
	return commands_;
}
