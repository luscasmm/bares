#include "../include/parser.h"

//support methods

//converts the input character c into its corresponding terminal symbol code
terminal_symbol_t const Parser::lexer(char c){
	switch(c){
        case '+':	return terminal_symbol_t::TS_ADD;
        case '-':	return terminal_symbol_t::TS_SUB;
        case '*':	return terminal_symbol_t::TS_MULT;
        case '/':	return terminal_symbol_t::TS_DIV;
        case '^':	return terminal_symbol_t::TS_POW;
        case '%':	return terminal_symbol_t::TS_MOD;
        case ' ':	return terminal_symbol_t::TS_WS;
        case   9:	return terminal_symbol_t::TS_TAB;
        case '0':	return terminal_symbol_t::TS_ZERO;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':	return terminal_symbol_t::TS_NON_ZERO_DIGIT;
        case '\0':	return terminal_symbol_t::TS_EOS; // end of string: the $ terminal symbol
    }

    return terminal_symbol_t::TS_INVALID;
}

//advances current_symbol iterator to the next valid symbol for processing
void Parser::next_symbol(){
	this->current_symbol++;
}

bool Parser::accept(terminal_symbol_t c){
	if(!end_input() && lexer(*current_symbol) == c){
		next_symbol();
		return true;
	}

	return false;
}

//advances the current_symbol iterator through blank spaces - white spaces (ascii: 32) or tabs (ascii: 9) - untill reach a valid character or end of the input string
void Parser::skip_bs(void){
	while(!end_input() && std::isspace(*current_symbol)){
        next_symbol(); // consumir um caractere.
    }
}

//returns true if the processing reached the end of the input string
bool const Parser::end_input(void){
	return this->current_symbol == this->expression.end();
}

//methodos to evaluate the grammar

//<expression> := <term>, { ( "+" | "-" | "*" | "/" | "%" | "^" ), <term> };
bool Parser::expression(){
	term();
    
    while(this->result.type == ResultType::OK){
        skip_ws();

        //parses the operator
        if(accept(Parser::terminal_symbol_t::TS_ADD)){
           this->tokens.emplace_back(Token( "+", Token::token_t::OPERATOR));

        }else if(accept(Parser::terminal_symbol_t::TS_SUB)){
           this->tokens.emplace_back(Token( "-", Token::token_t::OPERATOR));

        }else if(accept(Parser::terminal_symbol_t::TS_MULT)){
           this->tokens.emplace_back(Token( "*", Token::token_t::OPERATOR));

        }else if(accept(Parser::terminal_symbol_t::TS_DIV)){
           this->tokens.emplace_back(Token( "/", Token::token_t::OPERATOR));

        }else if(accept(Parser::terminal_symbol_t::TS_POW)){
           this->tokens.emplace_back(Token( "^", Token::token_t::OPERATOR));

        }else if(accept(Parser::terminal_symbol_t::TS_MOD)){
           this->tokens.emplace_back(Token( "%%", Token::token_t::OPERATOR));

        }else break;

        //verifies if there's a valid term after the operator
        if(!term() && this->result.type == ResultType::ILL_FORMED_INTEGER){
            this->result.type = ResultType::MISSING_TERM;
        }
    }

    return this->result.type == ResultType::OK;
}

bool Parser::term(){

}

bool Parser::integer(){

}

bool Parser::natural_number(){

}

bool Parser::digit_excl_zero(){

}

bool Parser::digit(){

}

//Parser main function
ResultType Parser::parse(std::string expression){
	this->expression = expression;
    this->current_symbol = this->expression.begin();
    this->result = ResultType(ResultType::OK);

    this->tokens.clear();

    skip_bs();

    if(end_input()){
        this->result =  ResultType( ResultType::UNEXPECTED_END_OF_EXPRESSION, std::distance(this->expression.begin(), this->current_symbol));
    
    }else{
        if(expression()){
            skip_bs();
            if(!end_input()){
                this->result.type = ResultType::EXTRANEOUS_SYMBOL;
                this->result.at_col = std::distance(this->expression.begin(), this->current_symbol);
            }
        }
    }
    return this->result;
}

//tokens getter
std::vector<Token> const Parser::get_tokens(void){
	return this->tokens;
}