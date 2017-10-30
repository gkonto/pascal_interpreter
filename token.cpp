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
	switch (this->type())
	{
		case T_PLUS:
		case T_MINUS:
			return true;
		default:
			return false;
	}
	return false;
}

static void start()
{
	std::cout << "> ";
	std::string expr;

	while (getline(std::cin, expr)) {
		Interpreter interp(expr);
		int result = interp.expr();
		std::cout << result << std::endl;
		std::cout << "> ";
	}
}

int main(void)
{
	start();
}
