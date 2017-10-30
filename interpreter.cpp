#include <string>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>

#include "interpreter.hpp"


/*!
 * \file token.cpp
 */

/*!
 * fn void Interpreter::raiseParseError()
 * \brief Raise assertion when error occurs while parsing
 */
void Interpreter::raiseParseError()
{
	std::cout << "Error parsing input" << std::endl;
	assert(true);
}

/*!
 * fn void Interpreter::getNextToken()
 * \brief Lexical analyzer (also known as scanner or tokenizer)
 * This method is responsible for breaking a sentence apart into tokens.
 * One token at a time.
 *
 * \return a Token
 */
Token Interpreter::getNextToken()
{
	while (currentChar_ != '\0') {
		if (isspace(currentChar_)) {
			skipWhiteSpace();
		}
		if (isdigit(currentChar_)) {
			return Token(T_INTEGER, integer());
		}
		if (currentChar_ == '+') {
			advance();
			return Token(T_PLUS, "+");
		}
		if (currentChar_ == '-') {
			advance();
			return Token(T_MINUS, "-");
		}
		raiseParseError();
	}
	return Token(T_EOF, "");
}

/*!
 * fn void Interpreter::eat()
 * \brief Compare the current token type with the passed token
 * type and if they match then "eat" the current token
 * and assign the next token to the currentToken,
 * otherwise raise exception. 
 */
void Interpreter::eat(const TokenType &tokType)
{
	if (currentToken_.type() == tokType) {
		currentToken_ = getNextToken();
	} else {
		raiseParseError();
	}
}

/*!
 * fn void Interpeter::expr()
 * \brief Parser / Interpreter 
 * expr -> T_INTEGER PLUS T_INTEGER
 */
int Interpreter::expr()
{
	//set current token to the first token taken from the input
	currentToken_ = getNextToken();

	//we expect the current token to be a single-digit integer
	Token left = currentToken_;
	eat(T_INTEGER);

	//we expect the current token to be either '+' or '-'
	Token op = currentToken_;
	if (op.type() == T_PLUS) {
		eat(T_PLUS);
	} else {
		eat(T_MINUS);
	}

	//we expect the current token to be a single-digit integer
	Token right = currentToken_;
	eat(T_INTEGER);

	//after the above call the currentToken is set to EOF token
	
	/*
	 * at this point either T_INTEGER T_PLUS T_INTEGER or
	 * T_INTEGER T_MINUS T_INTEGER sequence of tokens
	 * has been succesfully found and the method can just
	 * return the result of adding or subtracting two integers, thus 
	 * effectively interpreting client input
	 */
	if (op.type() == T_PLUS) {
		return atoi(left.value().c_str()) + atoi(right.value().c_str());
	} else {
		return atoi(left.value().c_str()) - atoi(right.value().c_str());
	}
	raiseParseError();
	return 0;
}

/*!
 * fn void Interpeter::advance()
 * \brief Advance the pos_ pointer and set the currentChar variable
 */
void Interpreter::advance()
{
	pos_++;
	if (pos_ > text_.length() - 1) {
		currentChar_ = '\0'; // indicates EOF
	} else {
		currentChar_ = text_[pos_];
	}

}

/*!
 * fn void Interpeter::skipWhiteSpace()
 */
void Interpreter::skipWhiteSpace()
{
	while (currentChar_ != '\0' && isspace(currentChar_)) {
		advance();
	}
}

/*!
 * fn void Interpeter::integer()
 * \brief Return a multidigit integer consumed from the input.
 * \return The integer in string format
 */
std::string Interpreter::integer()
{
	std::string result = "";
	while (currentChar_ != '\0' && isdigit(currentChar_)) {
		result += currentChar_;
		advance();
	}
	return result;
}





