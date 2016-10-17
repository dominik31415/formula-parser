#include <cmath>


// all expressions are derived from generic_expression
// the formula is evaluated recursively by following all tree branches until they hit a number/parameter expression
// operators/functions carry the pointers to their argument expressions
// numbers/parameters store their value directly
// evaluate() works recursively on the daughter nodes

class generic_expression{
	public:
	virtual double evaluate(const map<unsigned int,double>& parameters) = 0;
	virtual ~generic_expression() {};
	
};

class neg2_expression : public generic_expression{
	//Negation as binary operator. The first argument is always initialized with a dummy 0
	generic_expression *ptr_term1, *ptr_term2;
	
	public:	
	neg2_expression(generic_expression *ptr_in1, generic_expression *ptr_in2){
		ptr_term1 = ptr_in1;
		ptr_term2 = ptr_in2;
	};		
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return ptr_term1->evaluate(parameters) - ptr_term2->evaluate(parameters);
	};
};

class power_expression : public generic_expression{
	generic_expression *ptr_term1, *ptr_term2;
	
	public:	
	power_expression(generic_expression *ptr_in1, generic_expression *ptr_in2){
		ptr_term1 = ptr_in1;
		ptr_term2 = ptr_in2;
	};		
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return pow(ptr_term1->evaluate(parameters), ptr_term2->evaluate(parameters));
	};
};

class plus_expression : public generic_expression{
	generic_expression *ptr_term1, *ptr_term2;
	
	public:	
	plus_expression(generic_expression *ptr_in1, generic_expression *ptr_in2){
		ptr_term1 = ptr_in1;
		ptr_term2 = ptr_in2;
	};		
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return ptr_term1->evaluate(parameters) + ptr_term2->evaluate(parameters);
	};
};

class minus_expression : public generic_expression{
	generic_expression *ptr_term1, *ptr_term2;
	
	public:	
	minus_expression(generic_expression *ptr_in1, generic_expression *ptr_in2){
		ptr_term1 = ptr_in1;
		ptr_term2 = ptr_in2;
	};		
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return ptr_term1->evaluate(parameters) - ptr_term2->evaluate(parameters);
	};
};

class times_expression : public generic_expression{
	generic_expression *ptr_term1, *ptr_term2;
	
	public:	
	times_expression(generic_expression *ptr_in1, generic_expression *ptr_in2){
		ptr_term1 = ptr_in1;
		ptr_term2 = ptr_in2;
	};		
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return ptr_term1->evaluate(parameters) * ptr_term2->evaluate(parameters);
	};
};

class ratio_expression : public generic_expression{
	generic_expression *ptr_term1, *ptr_term2;
	
	public:	
	ratio_expression(generic_expression *ptr_in1, generic_expression *ptr_in2){
		ptr_term1 = ptr_in1;
		ptr_term2 = ptr_in2;
	};		
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return ptr_term1->evaluate(parameters) / ptr_term2->evaluate(parameters);
	};
};

class sqrt_expression : public generic_expression{
	generic_expression *ptr_term;
	
	public:
	sqrt_expression(generic_expression *ptr_in){
		ptr_term = ptr_in;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return sqrt(ptr_term->evaluate(parameters));
	};
};

class exp_expression : public generic_expression{
	generic_expression *ptr_term;
	
	public:
	exp_expression(generic_expression *ptr_in){
		ptr_term = ptr_in;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return exp(ptr_term->evaluate(parameters));
	};
};

class log_expression : public generic_expression{
	generic_expression *ptr_term;
	
	public:
	log_expression(generic_expression *ptr_in){
		ptr_term = ptr_in;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return log(ptr_term->evaluate(parameters));
	};
};

class sin_expression : public generic_expression{
	generic_expression *ptr_term;
	
	public:
	sin_expression(generic_expression *ptr_in){
		ptr_term = ptr_in;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return sin(ptr_term->evaluate(parameters));
	};
};

class cos_expression : public generic_expression{
	generic_expression *ptr_term;
	
	public:
	cos_expression(generic_expression *ptr_in){
		ptr_term = ptr_in;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return cos(ptr_term->evaluate(parameters));
	};
};

class neg_expression : public generic_expression{
	generic_expression *ptr_term;
	
	public:
	neg_expression(generic_expression *ptr_in){
		ptr_term = ptr_in;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return -(ptr_term->evaluate(parameters));
	};
};

class number_expression : public generic_expression{
	double myvalue;
			
	public:
	number_expression(const double& value): myvalue(value) {};
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return myvalue;
	};
};

class parameter_expression : public generic_expression{
	unsigned int parameter_number;
	
	public:
	
	parameter_expression(const double& value){
		parameter_number = (unsigned int)value;
	};
	
	double evaluate(const map<unsigned int,double>& parameters) override final{
		return parameters.at(parameter_number);
	};
};
