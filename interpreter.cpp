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
	std::cout << "Invalid syntax" << std::endl;
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
	currentToken_ = getNextToken();
	int result = atoi(term().c_str());

	while (currentToken_.isOperator()) {
		Token tok = currentToken_;
		if (tok.type() == T_PLUS) {
			eat(T_PLUS);
			result = result + atoi(term().c_str());
		} else if (tok.type() == T_MINUS) {
			eat(T_MINUS);
			result = result - atoi(term().c_str());
		}
	}
	return result;
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

/*!
 * fn void Interpeter::term()
 * \return An integer token value
 */
std::string Interpreter::term()
{
	Token tok = currentToken_;
	eat(T_INTEGER);
	return tok.value();
}

/*!
 * fn void Interpeter::factor()
 */
void Interpreter::factor()
{
	eat(T_INTEGER);
}



