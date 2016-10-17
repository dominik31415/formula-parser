#include "formula.h"

// constructors
formula::formula(){
	raw_formula = "";
	ptr_root = nullptr;
};

formula::formula(const string& str){ 
	clear();
	raw_formula = str;
};
	
formula::~formula(){
	clear();
};

formula::formula(const formula& other){
	raw_formula = other.raw_formula;
	init(); 
};
	
formula& formula::operator=(const formula& other){
	clear(); //delete all old data and overwrite with new info
	raw_formula = other.raw_formula;
	init();
	return *this;
};

formula::formula(formula&& other){
	//just copy all internal variables
	raw_formula = other.raw_formula;
	pre_tokens = other.pre_tokens;
	standard_formula = other.standard_formula;
	postfix_formula = other.postfix_formula;
	all_associated_expressions = other.all_associated_expressions; 
	// we can keep the pointers, since we are reusing other's expressions,
	ptr_root = other.ptr_root;
	parameters = other.parameters; 
	//we are re-using other's expressions, so make sure other's destructor does not delete any
	other.all_associated_expressions.clear();
};
		
formula& formula::operator=(formula&& other){
	clear(); //first empty this object
	//then copy everything
	raw_formula = other.raw_formula;
	pre_tokens = other.pre_tokens;
	standard_formula = other.standard_formula;
	postfix_formula = other.postfix_formula;
	all_associated_expressions = other.all_associated_expressions;
	ptr_root = other.ptr_root;
	parameters = other.parameters;		
	other.all_associated_expressions.clear();
	return *this;
};
	
// member functions	
void formula::delete_expressions(){ 
	// simply go through list of all pointers stored in all_associated_expressions and delete them
	while(!all_associated_expressions.empty()){
		delete all_associated_expressions.back();
		all_associated_expressions.pop_back();
	};
};

void formula::clear(){ //resets object into uninitialized state, deletes all data
	raw_formula = "";
	ptr_root = nullptr;	
	
	pre_tokens.clear();
	standard_formula.clear();
	postfix_formula.clear();
	parameters.clear();

	delete_expressions();		
};	
	
void formula::init(){ //main function to compile expression tree
	if(raw_formula.find_first_not_of(' ') == raw_formula.npos){
		//only spaces, nothing to do here
		ptr_root = nullptr;
		return;
	};
	try{
		string_to_tokens();
		standard_to_postfix();
		construct_expression_tree();			
	} 
	catch(const std::runtime_error& re){
		cerr << re.what() << endl;
		cerr << "formula could not be initialized" << endl;
		// do some tidying up, in case of incomplete initialization
		string tmp = raw_formula; //but keep raw formula string, clean() deletes everything otherwise
		clear();
		raw_formula = tmp;
	};
	return;						
};		
	
void formula::init(const string& str){
	clear();
	raw_formula = str;
	init();
};

double formula::evaluate(const map<unsigned int, double>& params){
	if(ptr_root != nullptr){
		return ptr_root->evaluate(params);
	}
	else{
		cerr << "object not initialized, default result 0" << endl;
		return 0;
	};
};

map<unsigned int, double> formula::get_parameter_prototype(){
	//generates a prototype parameter map as is expected by evaluate()
	map<unsigned int,double> parameter_list;
	for(auto it = parameters.begin(); it != parameters.end(); it++){
		parameter_list.emplace(it->first,0);
	};
	return parameter_list;
};

const string& formula::get_formula_string(){
	return raw_formula;
};
	

void formula::string_to_tokens(){
	//this function involves three steps
	// (1) split formula string into substrings by using operators, brackets and spaces as delimiters, result stored in pre_tokens
	// (2) each substring is subsequently tokenized
	// (3a) the ambiguity between minus and negation is resolved
	// (3b) negation is converted into a binary operator with highest precedence (and second dummy parameter 0)

	// part (1)
	string buffer = "";		
	// The code moves through the input string character by character. As long as it doesn't know where a substring ends, the
	// characters are stored in buffer
	for(auto it = raw_formula.begin(); it != raw_formula.end(); ++it){
		char tmp = *it;
		if( (tmp =='(') || (tmp == ')') || (tmp == ' ') || (tmp == '+') || (tmp== '-') || (tmp== '*') || (tmp == '/') || (tmp == '^') ){
			//previous substring ended. It is stored in buffer, add it to the pre_tokens...
			if(buffer != "") pre_tokens.push_back(buffer); //...but not if it is a space
			buffer = "";
			if( tmp != ' ' ) pre_tokens.push_back(string(1,tmp));
			//if delimiter is not a space, it is an operator/bracket > add it to the list of tokens
		}
		else{ //no delimiter found, substring keeps on going
			buffer += tmp;
		};
	};
	if(buffer.length() != 0){
		pre_tokens.push_back(buffer);
		buffer = "";
	};
	
	// part (2)
	// now convert substrings into tokens, the minus sign is defaulted to tk_minus for now
	math_token token;
	for(auto it = pre_tokens.begin(); it != pre_tokens.end(); ++it){
		//first treat cases of all numbers
		string str = *it;
		if(str.find_first_not_of(".0123456789") == str.npos){ //verified to be only of digits
			const char *cstr = str.c_str(); //strtod requires C-strings as input
			token.type = tk_literal;
			token.name = tk_number;
			token.value = strtod(cstr,nullptr);
			standard_formula.push_back(token);
			continue;
		};
		
		//now check whether it is a parameter, must be of form 'x' or 'x0001'
		if(str[0]=='x'){
			string str2 = str.substr(1,str.npos);
			if(str2.find_first_not_of("0123456789") == str.npos){ //only digits after the x
				const char *cstr = str2.c_str(); 
				token.type = tk_literal;
				token.name = tk_parameter;
				token.value = strtod(cstr,nullptr);
				standard_formula.push_back(token);
				continue;
			};
		};				
		
		// warning: care must be taken here when initializing the tokens
		// if e.g. accidentally '*' were defined as unary operator, the algorithm would blindly treat it as such
		if(str=="sin") {token.type = tk_unary; token.name = tk_sin; token.value = 0; standard_formula.push_back(token);continue;}
		if(str=="exp") {token.type = tk_unary; token.name = tk_exp; token.value = 0;standard_formula.push_back(token);continue;};
		if(str=="cos") {token.type = tk_unary; token.name = tk_cos; token.value = 0; standard_formula.push_back(token);continue;}
		if(str=="sqrt") {token.type = tk_unary; token.name = tk_sqrt; token.value = 0;standard_formula.push_back(token);continue;};
		if(str=="log") {token.type = tk_unary; token.name = tk_log; token.value = 0;standard_formula.push_back(token);continue;};
		if(str=="+") {token.type = tk_binary; token.name = tk_plus; token.value = 0;standard_formula.push_back(token);continue;};
		if(str=="-") {token.type = tk_binary; token.name = tk_minus; token.value = 0;standard_formula.push_back(token);continue;};
		if(str=="*") {token.type = tk_binary; token.name = tk_times; token.value = 1;standard_formula.push_back(token);continue;};
		if(str=="/") {token.type = tk_binary; token.name = tk_ratio; token.value = 1;standard_formula.push_back(token);continue;};
		if(str=="^") {token.type = tk_binary; token.name = tk_power; token.value = 2;standard_formula.push_back(token);continue;};
		if(str=="(") {token.type = tk_bracket; token.name = tk_open; token.value = 0;standard_formula.push_back(token);continue;};
		if(str==")") {token.type = tk_bracket; token.name = tk_close; token.value = 0;standard_formula.push_back(token);continue;};
		
		//the code should never get to here unless the input string contained a non-defined sequence
		throw runtime_error("parsing error in input string"); 
	
	};

	// part (3a)
	//resolve ambiguity in minus/negation
	//change tk_minus to tk_neg if the preceding token is either nothing, an opening bracket or a binary operator
	auto it = standard_formula.begin();
	if(it->name==tk_minus){
			it->type = tk_unary;
			it->name = tk_neg;
			it->value = 0;
		};
	if(standard_formula.size() > 1){		
		for(it = it+1; it != standard_formula.end(); it++){
			if(it->name==tk_minus && ((it-1)->type == tk_binary || (it-1)->name == tk_open)){
				it->type = tk_unary;
				it->name = tk_neg;
				it->value = 0;
			};
		};
	}
	
	//part (3b)
	//convert neg to binary operator neg2 with high precedence
	deque<math_token> tmp;
	math_token neg2, zero;
	neg2.type = tk_binary; neg2.name = tk_neg2; neg2.value = 5;
	zero.type = tk_literal; zero.name = tk_number; zero.value = 0;
	for(it = standard_formula.begin(); it != standard_formula.end(); it++){
		if(it->name==tk_neg){		
			tmp.push_back(zero);
			tmp.push_back(neg2);
		}
		else{
			tmp.push_back(*it);
		};
	};
	standard_formula = tmp;

}; 
		
void formula::standard_to_postfix(){
	// this converts the tokenized raw string into postfix notation using the shunting yard algorithm
	// the tokenized original string is destroyed in the process
	deque<math_token> buffer;
	math_token current_token;
	while(!standard_formula.empty()){
		current_token = standard_formula.front();
		standard_formula.pop_front();
		if(current_token.type==tk_literal) postfix_formula.push_back(current_token); //is it a literal? (number/parameter)
		if(current_token.type==tk_unary) buffer.push_back(current_token); //unary operators
		if(current_token.type==tk_binary){		//for binary operators precedence matters	
			while(!buffer.empty() && (buffer.back().type == tk_binary) && (buffer.back().value >= current_token.value)){
				postfix_formula.push_back(buffer.back());
				buffer.pop_back();
			};
			buffer.push_back(current_token);
		};
		if(current_token.name==tk_open) buffer.push_back(current_token); //dealing with brackets
		if(current_token.name==tk_close){
			bool found_matching_bracket = false;
			while(!buffer.empty() && buffer.back().name != tk_open){ //pop and add everything in between open and closing bracket
				postfix_formula.push_back(buffer.back());
				buffer.pop_back();
			};
			if(buffer.back().name == tk_open){
				buffer.pop_back(); //found matching opening bracket, pop and discard
				found_matching_bracket = true;
			};
			if(!buffer.empty() && buffer.back().type == tk_unary){ //this is a function applied to whole bracket
				postfix_formula.push_back(buffer.back());
				buffer.pop_back();
			};
			if(!found_matching_bracket){
				throw runtime_error("error interpreting formula: unmatched closing bracket");
			};
		};
	};
	while(!buffer.empty()){
		if(buffer.back().name == tk_open){
			throw runtime_error("error interpreting formula: unmatched opening bracket");
		};
		postfix_formula.push_back(buffer.back());
		buffer.pop_back();
	};
};

void formula::construct_expression_tree(){
	// this algorithm goes through the tokenized formula in postfix notation. for each basic expression 
	// (numbers and parameters) it creates an appropriate node, which is stored in the buffer
	// each function/operator creates a corresponding node and it is linked to the top basic nodes fetched from the buffer
	// the resulting linked node is put back on the buffer
	// if the postfix formula is 'correct', there should always be (a) enough expressions in the buffer for new operators to act on and
	// (b) in the end only exactly one expression left
	// the postfix deque is destroyed in the process
	// in parallel the algorithm populates all_associated_expressions, a pointer list to all created expressions, for bookkeeping
	deque<generic_expression*> buffer;
	
	math_token current_token;
	generic_expression *new_expression;
	while(!postfix_formula.empty()){
		new_expression = nullptr;
		current_token = postfix_formula.front();
		postfix_formula.pop_front();
		
		if(current_token.name == tk_number){
			// deal with numbers
			new_expression = new number_expression(current_token.value);
			all_associated_expressions.push_back(new_expression);
			buffer.push_back(new_expression);
			continue;
		};
			
		if(current_token.name == tk_parameter){
			// adding a parameter to the tree
			// first check whether this parameter has already been created
			// if yes, reuse existing pointer, if not make new one
			auto it = parameters.find(current_token.value);
			if(it == parameters.end()){
				new_expression = new parameter_expression(current_token.value);
				all_associated_expressions.push_back(new_expression);				
				parameters.emplace(current_token.value,new_expression);
			}
			else{
				new_expression = it->second; //recycle old expression
			};				
			buffer.push_back(new_expression);	
			continue;
		};
		
		if(current_token.type == tk_unary){
			// adding unary operators
			//requires one argument, i.e. fetch top element from buffer, link it to the operator and put result back to buffer
			if(buffer.empty()){	
				throw runtime_error("syntax error: formula contains at least on unary operator without argument");
			};
			generic_expression *ptr_tmp = buffer.back();
			buffer.pop_back();
			if(current_token.name==tk_sin) {new_expression = new sin_expression(ptr_tmp);};
			if(current_token.name==tk_cos) {new_expression = new cos_expression(ptr_tmp);};
			if(current_token.name==tk_log) {new_expression = new log_expression(ptr_tmp);};
			if(current_token.name==tk_exp) {new_expression = new exp_expression(ptr_tmp);};		
			if(current_token.name==tk_neg) {new_expression = new neg_expression(ptr_tmp);};
			if(current_token.name==tk_sqrt) {new_expression = new sqrt_expression(ptr_tmp);};
			all_associated_expressions.push_back(new_expression);	
			buffer.push_back(new_expression);
			continue;
		};
		
		if(current_token.type == tk_binary){
			// same as unary operators, just fetch two objects from buffer
			if(buffer.size() < 2){
				throw runtime_error("syntax error: formula contains at least one binary operator with insufficient number of arguments");
			};
			generic_expression *ptr_tmp2 = buffer.back(); //the ordering matters here: difference between 3/5 and 5/3
			buffer.pop_back();
			generic_expression *ptr_tmp1 = buffer.back();
			buffer.pop_back();
			if(current_token.name==tk_plus) {new_expression = new plus_expression(ptr_tmp1, ptr_tmp2);};
			if(current_token.name==tk_minus) {new_expression = new minus_expression(ptr_tmp1, ptr_tmp2);};
			if(current_token.name==tk_times) {new_expression = new times_expression(ptr_tmp1, ptr_tmp2);};
			if(current_token.name==tk_ratio) {new_expression = new ratio_expression(ptr_tmp1, ptr_tmp2);};
			if(current_token.name==tk_power) {new_expression = new power_expression(ptr_tmp1, ptr_tmp2);};	
			if(current_token.name==tk_neg2) {new_expression = new neg2_expression(ptr_tmp1, ptr_tmp2);};
			all_associated_expressions.push_back(new_expression);							
			buffer.push_back(new_expression);
			continue;
		};
			
	};
	if(buffer.size() > 1){
		throw runtime_error("syntax error: formula containers two unconnected expressions");
		
	};
	ptr_root = buffer.back();
	
};

			
// not part of formula class, just a friend
void disp(const deque<math_token>& deq){
	name_token ntk; //helper function to print math expression deques, currently retired
	for(auto it = deq.begin(); it != deq.end(); it++){
		ntk = it->name;
		if(ntk==tk_plus) cout << "plus,";
		if(ntk==tk_minus) cout << "minus,"; 	
		if(ntk==tk_times) cout << "times,";	
		if(ntk==tk_ratio) cout << "ratio,";
		if(ntk==tk_power) cout << "power," ;
		if(ntk==tk_sin) cout << "sin,";
		if(ntk==tk_cos) cout << "cos,";
		if(ntk==tk_exp) cout << "exp,";
		if(ntk==tk_sqrt) cout << "sqrt," ;
		if(ntk==tk_neg) cout << "neg," ;
		if(ntk==tk_open) cout << "(,";
		if(ntk==tk_close) cout << ")," ;
		if(ntk==tk_number) cout << it->value << ",";			
		if(ntk==tk_parameter) cout << "x_" << it->value << ",";
		if(ntk==tk_neg2) cout << "s_neg" <<  ",";
	};
	cout << endl;
};	

