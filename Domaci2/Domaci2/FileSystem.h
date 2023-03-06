#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include"Commands.h"
#include"Exceptions.h"
#include"TreeNode.h"

#include<algorithm>
#include<fstream>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<stack>
#include<string>
#include<queue>
#include<vector>
#include<utility>


/*
FileSystem je stablo sacinjeno od TreeNodeova
svaki TreeNode je ili Folder ili ExeFile ili TxtFile
Folderi sadrze decu u sebi
ExeFileovi sadrze komande koje treba da se odrade kada se pozove komanda EXE na njih
TxtFileovi sadrze samo tekst
*/

enum comm_ { LS, CD, NEW, DEL, EXE, DEF };//enum za komande

using namespace std;

class FileSystem {
public:

	FileSystem() : input_file_(""), output_file_(""), root_(nullptr), current_(nullptr), exception_("") {}; //default konstruktor
	~FileSystem();//destruktor
	void loadHierarchy(const string& fs_filepath, const string& log_filepath);
	void execute(const string& filepath);

private:

	//polja klase TreeNode:
	TreeNode* root_; // koren
	string input_file_;
	string output_file_;
	TreeNode* current_; //trenutni cvor
	string exception_;//cuva exception u obliku u {Error *komanda* *sadrzaj*}

	//unutrasnje funkcije:
	void createNewNode(string& path); // trazenje mesta cvora u stablu na osnovu njegove putanje koju smo izvukli iz inputa
	comm_ hash(string in); //potrebno za switch posto switch ne prima vrednosti stringa pa moramo enum da uvedemo i hesiramo (svakom stringu dodelimo odredjenu enumeraciju)
	TreeNode* parse(string& s, TreeNode* parent); // kada smo vec dosli do toga sto pravimo onda parsiramo string do kraja 
	void parseExeFinal(string s, vector<Commands*> &c); //parsiranje exe fajla
	string readContent(string str, int& pos); //procitaj sadrzaj jedne komande u Exefileu
	string toLower(string s); //prebacuje ime u lowercase
	void sortTree(); //sortiranje stabla

	//funkcije koje pomazu izrsavanje neke od komandi:
	void changeDirectory(string directory); //CD
	void createNew(string s, TreeNode* parent); //NEW
	void deleteNode(string s, TreeNode* parent); //DEL
	void doExe(string s, TreeNode* parent, fstream& output, fstream& input); //EXE
	void print(TreeNode* t, fstream& output); //LS

};

#endif 
