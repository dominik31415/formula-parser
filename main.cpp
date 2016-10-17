using namespace std;
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <ctype.h>
#include <deque>
#include <map>

#include "expressions.cpp"
#include "formula.h"
#include "formula.cpp"


/* The formula class can be initialized with a string containing a mathematical expression. The expression is
 * subsequently parsed into tokens, converted into psotfix notation (using the shunting yard algorithm) and then compiled into a tree structure with each node 
 * corresponding to one operator/function and its children being its arguments. 
 * The nodes are all derived from the abstract 'generic_expression' class.
 * 
 * The code supports: 
 * numbers (all as doubles) 
 * parameters, which must begin with x, e.g. x0, x1, x[number]. Just'x' is okay too but is defaulted to 'x0'
 * some functions: sin(), cos(), log(), exp(), sqrt()
 * mathematical operators: +,-,*,/, and ^
 * the code can distinguish between 'minus' and 'negation'
 * brackets () 
 * The code understands precedence
 * 
 * Supported formula examples:
 * "x^2 + 7 - sin(x)", "(3+8)^x0-x1", "1+1--2+8"  
 * 
 * Tested compilation on: Ubuntu 14.04, g++ (Ubuntu 4.8.5-2ubuntu1~14.04.1) 4.8.5
 */


int main(int argn, char **argv){
	formula test; //declare empty formula
	string str;

	cout << "-----------------------------" << endl;
	cout << "input formula string: " << endl;
	getline(cin,str);
	try{
		cout << "parsing function..." << endl;
		test.init(str);		//initializing formula with given string and starts parsing
		map<unsigned int,double> xx = test.get_parameter_prototype(); //request sample parameter argument
		cout << "specify parameter values: " << endl;
		for(auto it = xx.begin(); it != xx.end(); it++){ //fill parameters with values
			cout << "x" << it->first << " = ";
			cin >> it->second;
		};
	cout << endl << "result: " << test.evaluate(xx) << endl;
	} 
	catch(...){
		cout << "error: nothing can be done" << endl;
	};
};
