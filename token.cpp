#include <iostream>
#include <sstream>
#include <cassert>

#include "interpreter.hpp"
#include "token.hpp"

/*!
 *  \file token.cpp
 */

/*!
 *  \fn void Token::raiseLabelError()
 *  \brief Raises assertion when getTokenTypeLabel()
 *  is not updated!
 */
void Token::raiseLabelError()
{
	std::cout << "getTokenTypeLabel(): Needs update!" << std::endl;
	assert(false);
}

/*!
 *  \fn std::string Token::getTokenTypeLabel()
 *  \brief Gets the string label of the enum type
 *  the token is.
 *  \return A label as string
 */
std::string Token::getTokenTypeLabel()
{
	switch(type_)
	{
		case T_INTEGER: return "T_INTEGER";
		case T_PLUS   : return "T_PLUS";
		case T_MINUS  : return "T_MINUS";
		case T_EOF    : return "T_EOF";
		case T_MAX    : return "T_MAX";
		default:
			raiseLabelError();
			break;
	}
	return "";
}

/*!
 *  \brief Gives details concerning the value and the type
 *  of a Token.
 *  \return a string with the details
 */
std::string Token::representation()
{
	std::ostringstream stringStream;
	stringStream << "Token(" << getTokenTypeLabel() << ", " << value_ << ")";
	std::string msg = stringStream.str();
	return msg;
}

/*!
 *  \brief Checks if Token is operator
 *  \return True if token is operator
 */
bool Token::isOperator()
{
	if (isOperatorFirstPrecedence() || isOperatorSecondPrecedence()) {
		return true;
	}
	return false;
}

/*!
 *  \brief Checks if Token is operator
 *  \return True if token is operator either '*' or '/'
 */
bool Token::isOperatorSecondPrecedence()
{
	switch(this->type())
	{
		case T_MUL:
		case T_DIV:
			return true;
		default:
			return false;
	}
	return false;
}

/*!
 *  \brief Checks if Token is operator
 *  \return True if token is operator either '+' or '-'
 */
bool Token::isOperatorFirstPrecedence()
{
	switch(this->type())
	{
		case T_PLUS:
		case T_MINUS:
			return true;
		default:
			return false;
	}
	return false;
}

/*****************************************************/
/*********************LEXER***************************/
/*!
 *  \fn void Lexer::raiseError()
 *  \brief Raises assertion when getTokenTypeLabel()
 *  is not updated!
 */
void Lexer::raiseError()
{
	std::cout << "Invalid Syntax!!" << std::endl;
	assert(false);
}

/*!
 * fn void Lexer::advance()
 * \brief Advance the pos_ pointer and set the currentChar_ variable
 */
void Lexer::advance()
{
	pos_++;
	if (pos_ > text_.length() - 1) {
		currentChar_ = '\0'; // indicates EOF
	} else {
		currentChar_ = text_[pos_];
	}
}

/*!
 * fn void Lexer::skipWhiteSpace()
 */
void Lexer::skipWhiteSpace()
{
	while (currentChar_ != '\0' && isspace(currentChar_)) {
		advance();
	}
}

/*!
 * fn void Lexer::integer()
 * \brief Return a multidigit integer consumed from the input.
 * \return The integer in string format
 */
std::string Lexer::integer()
{
	std::string result = "";
	while (currentChar_ != '\0' && isdigit(currentChar_)) {
		result += currentChar_;
		advance();
	}
	return result;
}

/*!
 * fn void Lexer::getNextToken()
 * \brief Lexical analyzer (also known as scanner or tokenizer)
 * This method is responsible for breaking a sentence apart into tokens.
 * One token at a time.
 *
 * \return a Token
 */
Token Lexer::getNextToken()
{
	while (currentChar_ != '\0') {
		if (isspace(currentChar_)) {
			skipWhiteSpace();
			continue;
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
		if (currentChar_ == '*') {
			advance();
			return Token(T_MUL, "*");
		}
		if (currentChar_ == '/') {
			advance();
			return Token(T_DIV, "/");
		}
		raiseError();
	}
	return Token(T_EOF, "");
}

/*********************LEXER***************************/
/*****************************************************/

static void start()
{
	std::cout << "> ";
	std::string expr;

	while (getline(std::cin, expr)) {
		if (expr.empty()) {
			continue;
		}
		Lexer lex(expr);
		Interpreter interp(lex);
		int result = interp.expr();
		std::cout << result << std::endl;
		std::cout << "> ";
	}
}

int main(void)
{
	start();
}
