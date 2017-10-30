#pragma once
#include "token.hpp"

class Interpreter
{
	public:
		///Creates an Interpreter object
		Interpreter(std::string text) : text_(text), pos_(0), currentToken_(Token()), currentChar_(text_[pos_]) {}
		Token getNextToken();
		void eat(const TokenType &toktype);
		int  expr();
		void advance();
		void factor();
		std::string term();
		void skipWhiteSpace();
		std::string  integer();

	private:
		void raiseParseError();

		std::string text_;
		size_t      pos_;
		Token       currentToken_;
		char        currentChar_;

};
