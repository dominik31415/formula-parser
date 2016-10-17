#ifndef FORMULA_H
#define FORMULA_H

//////////////
// tokens for formula parsing
// all tokens, are named tk_something, where something should give a clear indication what they refer to
// type categorizes the object roughly (i.e. number of parameters), whereas name_token specifies the full name
// math_token.double is a multi-purpose variable, with different meanings depending on math_token.type:
// for binary operators, it specifies the precedence, i.e. 0 (+/-), 1(* and /), 2 for power and 5 (for negation)
// for constant numbers, it simply contains its numerical value
// for parameters it contains the parameter index, i.e. x0 vs. x5 etc.
enum type_token {tk_bracket = -1, tk_literal = 0, tk_unary = 1, tk_binary = 2};
enum name_token {tk_plus = 2,tk_minus = 3,tk_times = 4,tk_ratio = 5, tk_power = 6, tk_number = 0, tk_parameter = 1, tk_sin = 7, tk_cos = 8, tk_exp = 9, tk_log = 10, tk_sqrt = 11, tk_neg = 12, tk_open = 13, tk_close = 14, tk_neg2 = 15}; 

struct math_token{
	type_token type;
	name_token name;
	double value; 
};

////////////
// this is a retired helper function, capable of printing token lists, as they are generated in formula; friend of formula
void disp(const deque<math_token>& deq);


///////////////
// this is the main class, compiling a given string into a tree of expression objects and evaluating them recursively
	
class formula{
	// internal variables
	string raw_formula = "";	//contains the unparsed formula string	
	generic_expression* ptr_root = nullptr;	//root to a tree of mathematical expressions objects
	
	// internal variables that store intermediate steps for parsing	
	vector<string> pre_tokens;	//raw_formula split into substrings, each corresponding to one symbol/operator/etc.
	deque<math_token> standard_formula; //now substrings are parsed into abstract tokens, minus and negation are reseolved 
	deque<math_token> postfix_formula;  //converted into postfix notation
	map<unsigned int,generic_expression*> parameters; //stores parameter expressions and how they can be accessed by their index
	
	
	// list of all associated expression objects, which are 'owned' by this class
	// whenever an expression object is added to/removed from this class it should be added/removed from this list
	// used for destructor, clean() and move copy constructor
	deque<generic_expression*> all_associated_expressions; 
	
	
	// helper methods used for parsing
	void string_to_tokens(); //splits raw_formula into substrings which are then tokenized. uses operators, brackets and spaces as delimiters
	void standard_to_postfix(); //converts tokenized formula from infix to postfix notation
	void construct_expression_tree();	//uses the infix formula to generate tree of expression objects
	
			
	// frees all objects which are listed in all_associated_expressions
	void delete_expressions(); 


	public:
	// constructors	
	formula();	//just creates empty class without any data;
	formula(const string& str); //resets object to an uninitialized object, but sets raw_formula string
	formula(const formula& other); //copy constructor, new object has identical data, but gets new expression tree	
	formula& operator=(const formula& other); //copy assignment. old data is deleted and replaced by other's data		
	formula(formula&& other);	//copies all data, takes over ownership of associated expressions	
	formula& operator=(formula&& other);
	~formula();
	
		
	//resets object into uninitialized state, deletes all data (including raw formula string)
	void clear();

	// this is the main function for parsing. it complies a tree of expressions based on raw_formula
	void init();
	
	// same as init(), but first removes old data and replaces it with a formula based on new string
	void init(const string& str);
		
	//read type functions
	const string& get_formula_string(); //returns raw_formula string
	
	// returns a map of parameters, just as it should be given to the evaluate() function
	// the values for each parameter are defaulted to zero 	
	map<unsigned int, double> get_parameter_prototype();
	
	// evaluates the formula associated with this class
	// accepts a map for the needed parameter values, e.g. the input string is "x0^2+sin(x3)"
	// then it would expect a map as [(0,value of x0),(3, value of x3)] 
	double evaluate(const map<unsigned int, double>& params);
	
	//retired helper function to print tokenized formula
	friend void disp(const deque<math_token>& deq); 
	
};

#endif
