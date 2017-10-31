#pragma once
#include "token.hpp"

class Interpreter
{
	public:
		///Creates an Interpreter object
		Interpreter(Lexer lexer) : lexer_(lexer), currentToken_(lexer_.getNextToken()) {}
		void eat(const TokenType &toktype);
		int  expr();
		int  term();
		std::string factor();
	private:
		void raiseError();
		Lexer lexer_;
		Token currentToken_;
};
