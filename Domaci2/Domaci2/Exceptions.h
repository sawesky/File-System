#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include<exception>
using namespace std;

class ErrorExe : public exception {
public:
	ErrorExe(const char* msg) : exception(msg) {};
};

class ErrorInput : public exception {
public:
	ErrorInput(const char* msg) : exception(msg) {};
};
#endif 
