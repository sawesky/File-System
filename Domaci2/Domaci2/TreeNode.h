#ifndef TREENODE_H
#define TREENODE_H


#include"Commands.h"

#include<iostream>
#include<vector>
#include<utility>
#include<string>

using namespace std;

class TreeNode {
public:
	TreeNode() : parent_(nullptr), name_(""), ext_(""), num_(0), children_dummy_({}), commands_dummy_({}) {};
	TreeNode(TreeNode* parent, const string& name, const string& ext, const int& num) : parent_(parent), name_(name), ext_(ext), num_(num), children_dummy_({}), commands_dummy_({}) {};
	virtual ~TreeNode(); //virtuelni destruktor
	virtual vector<TreeNode*>& getChildren(); //da moze da dohvati decu
	virtual string getText() const; // da moze da dohvati sadrzaj TxtFilea
	virtual vector<Commands*>& getCommands();//da moze da dohvati komande
	TreeNode* getParent();
	string getName() const; 
	string getExt() const;
	int getNum() const;

	void setParent(TreeNode* parent);
	void setName(string name);
	void setExt(string ext);
	void setNum(int num);

protected:
	TreeNode* parent_; // roditelj
	string name_; //ime fajla
	string ext_; //ekstenzija
	int num_; //broj dece
	vector<TreeNode*> children_dummy_; //deca (ovo je uvek prazno)
	vector<Commands*> commands_dummy_; //deca (i ovo je uvek prazno)
};

class Folder : public TreeNode {
public:
	Folder() :TreeNode() , children_({}) {};
	Folder(TreeNode* parent, const string& name,const string& ext, const int& num) : TreeNode(parent, name, ext, num), children_({}) {};
	~Folder();
	vector<TreeNode*>& getChildren() override;
private:
	vector<TreeNode*> children_; //vektor dece
};

class ExeFile : public TreeNode {
public:
	ExeFile() : commands_({}) {};
	//ExeFile(TreeNode* parent, string name, string ext, int num, string text) : TreeNode(parent, name, ext, num), commands_({}) {};
	ExeFile(TreeNode* parent, const string& name,const string& ext, const int& num, vector<Commands*>& c) : TreeNode(parent, name, ext, num), commands_(c) {};
	~ExeFile();
	vector<Commands*>& getCommands() override;
private:
	vector<Commands*> commands_; //vektor komandi
};

class TxtFile : public TreeNode {
public:
	TxtFile() : text_("") {};
	TxtFile(TreeNode* parent, const string& name,const string& ext, const int& num, string text) : TreeNode(parent, name, ext, num),  text_(text) {};
	~TxtFile() {};
	string getText() const override;
private:
	string text_;

};

#endif