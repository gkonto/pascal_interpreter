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
void Interpreter::raiseError()
{
	std::cout << "Invalid syntax" << std::endl;
	assert(true);
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
/*		std::cout << "Eat after: " << lexer_.getNextToken().representation() << std::endl;*/
		currentToken_ = lexer_.getNextToken();
/*		std::cout << "Eat after: " << currentToken_.representation() << std::endl;*/
	} else {
		raiseError();
	}
}

/*!
 * fn void Interpeter::expr()
 * \brief  Arithmetic expression parser / interpreter
 * expr   : term((PLUS | MINUS) term)*
 * term   : factor(( MUL | DIV) factor)*
 * factor : INTEGER
 */
int Interpreter::expr()
{
	int result = term();

	while (currentToken_.isOperatorFirstPrecedence()) {
		Token tok = currentToken_;
		if (tok.type() == T_PLUS) {
			eat(T_PLUS);
			result = result + term();
		} else if (tok.type() == T_MINUS) {
			eat(T_MINUS);
			result = result - term();
		}
	}
	return result;
}

/*!
 * fn void Interpeter::factor()
 * \brief factor : INTEGER
 * \return An T_INTEGER token value
 */
std::string Interpreter::factor()
{
	Token tok = currentToken_;
	eat(T_INTEGER);
	return tok.value();
}

/*!
 * fn void Interpeter::term()
 * \brief term : factor((MUL | DIV) factor)*
 * \return An T_INTEGER token value
 */
int Interpreter::term()
{
	int result = atoi(factor().c_str());

	while (currentToken_.isOperatorSecondPrecedence()) {
		Token tok = currentToken_;
		if (tok.type() == T_MUL) {
			eat(T_MUL);
			result = result * atoi(factor().c_str());
		} else if (tok.type() == T_DIV) {
			eat(T_DIV);
			result = result / atoi(factor().c_str());
		}
	}
	return result;
}



