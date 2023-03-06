#ifndef COMMANDS_H
#define COMMANDS_H

#include<string>
#include<utility>
#include<vector>


using namespace std;

class Commands {
public:
	Commands(): type_("") {};
	Commands(const string& s) : type_(s) {};
	virtual ~Commands() {};
	virtual string getCD() const { return ""; };
	virtual string getNew() const { return ""; };
	virtual string getDel() const { return ""; };
	virtual string getExe() const { return ""; };
	string getType() { return type_; };
protected:
	string type_;
};

class LSC : public Commands {
public:
	LSC(const string& s) : Commands(s), ls_(0) {};
private:
	int ls_;
};

class CDC : public Commands {
public:
	CDC(const string& s,const string& cd) : Commands(s), cd_(cd) {};
	string getCD() const override { return cd_; };
private:
	string cd_;
};

class NewC : public Commands {
public:
	NewC(const string& s,const string& s_1) : Commands(s), new_(s_1) {};
	string getNew() const override { return new_; };
private:
	string new_;
};

class DelC : public Commands {
public:
	DelC(const string& s, const string& del) : Commands(s), del_(del) {};
	string getDel() const override{ return del_; };
private:
	string del_;
};

class ExeC : public Commands {
public:
	ExeC(const string& s, const string& exe) : Commands(s), exe_(exe) {};
	string getExe() const override{ return exe_; };
private:
	string exe_;
};

#endif 
