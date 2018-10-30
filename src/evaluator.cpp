#include "../include/evaluator.h"

//result getter
Result Evaluator::get_result(){
	return this->result;
}

// support functions
// value_t Evaluator::ctoi(symbol s){
// 	return std::stoi(s);
// }

bool Evaluator::is_operator(Token t){
	return t.type == Token::token_t::OPERATOR;
}

bool Evaluator::is_operand(Token t){
	return t.type == Token::token_t::OPERAND;
}

bool Evaluator::is_opening_scope(symbol s){
	return s == "(";
}

bool Evaluator::is_closing_scope(symbol s){
	return s == ")";
}

bool Evaluator::is_right_association(symbol s){
	return s == "^";
}

short Evaluator::get_precedence(symbol s){
	switch(s[0]){
        case '^' : return 3;
        case '*' :
        case '/' :
        case '%' : return 2;
        case '+' :
        case '-' : return 1;
        case '(' : return 0;
        default  : assert( false );  return -1;
    }
}

bool Evaluator::higher_precedence(symbol op1, symbol op2){
	int precedence_op1 = get_precedence(op1);
	int precedence_op2 = get_precedence(op2);

    if(precedence_op1 > precedence_op2){
    	return true;

	}else if(precedence_op1 < precedence_op2){
		return false;

	}else{
    	if(is_right_association(op1)) return false;
    	return true;

    }
}

//main functions

value_t Evaluator::pow(value_t b, value_t e){
	if(e < 1) return 1;
	return b * pow(b, e-1);
}

value_t Evaluator::execute_operator(value_t x, value_t y, symbol o){
	switch (o[0]){
		case '^':	return pow(x, y);
		case '*':	return x * y;
		case '/':	if(y == 0){
						this->result.type = Result::DIVISION_BY_ZERO;
						return -1;
					}
					return x / y;
		case '%':	if(y == 0){
						this->result.type = Result::DIVISION_BY_ZERO;
						return -1;
					}
					return x % y;
		case '+': 	return x + y;
		case '-': 	return x - y;
		default	: 	assert (false); return -1;
	}
}

//converts an infix expression (as Token list) into a postfix expression (as string list)
std::vector<Token> Evaluator::infix_to_postfix(std::vector<Token> infix){
	std::vector<Token> postfix;
	std::stack<Token> stack;

	for(auto token : infix){
		if(is_operand(token)){
			postfix.emplace_back(token);

		}else if(is_opening_scope(token.value)){
			stack.push(token);

		}else if(is_closing_scope(token.value)){
			while(!is_opening_scope(stack.top().value)){
				postfix.emplace_back(stack.top());
				stack.pop();
			}
			stack.pop();

		}else if(is_operator(token)){
			while(!stack.empty() && higher_precedence(stack.top().value, token.value)){
				postfix.emplace_back(stack.top());
				stack.pop();
			}
			stack.push(token);

		}else{
		}
	}

	while(!stack.empty()){
		postfix.emplace_back(stack.top());
		stack.pop();
	}

	return postfix;
}

value_t Evaluator::evaluate_postfix(std::vector<Token> postfix){
	this->result = Result(Result::OK);
	std::stack<value_t> stack;

	for(auto t : postfix){
		if(is_operand(t)){
			stack.push(std::stoi(t.value));

		}else if(is_operator(t)){
			auto y = stack.top();
			stack.pop();

			auto x = stack.top();
			stack.pop();

			auto result = execute_operator(x, y, t.value);

			//std::cout << result << std::endl;

			if(result < -32768 || result > 32767){
				this->result.type = Result::NUMERIC_OVERFLOW;
				stack.push(-1);	
			}else{
				stack.push(result);	
			}
		}
	}

	return stack.top(); // o resultado deve estar no topo da pilha.
}