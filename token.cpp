#include <iostream>
#include <sstream>
#include <cassert>
#include <string.h>

#include <fstream>
#include <sstream>
#include <iomanip>

#include "clog.hpp"
#include "interpreter.hpp"
#include "token.hpp"

/*
 *  \file token.cpp
 */

/*********************************************/
/*********************************************/
/*********************************************/
/************ Static variables ***************/
/*********************************************/
/*********************************************/
/*********************************************/
/**
 * \var static CLog::m_bInitialised
 * \brief A variable about visibility of prints
 */
bool CLog::m_bInitialised = true;

/**********************************************************************/
/**
 * \var static CLog::m_bInitialised
 * \brief A variable about visibility of prints
 * If RELEASE, prints marked with DEBUG will not be displayed.
 * If DEBUG, all prints will be displayed.
 */
int CLog::m_nLevel = CLog::RELEASE;

/*********************************************/
/*********************************************/
/*********************************************/
/*********************************************/
/*********************************************/
/*********************************************/



/*********************************************/
/*********************************************/
/*********************************************/
/*           Token Class Methods             */
/*********************************************/
/*********************************************/
/*********************************************/
/*!
 *  \fn void Token::raiseLabelError()
 *  \brief Raises assertion when getTokenTypeLabel()
 *  is not updated!
 */
void raiseLabelError()
{
	std::cout << "getTokenTypeLabel(): Needs update!" << std::endl;
	assert(false);
} /* Token::raiseLabelError */

/**********************************************************************/
/*!
 *  \fn std::string Token::getTokenTypeLabel()
 *  \brief Gets the string label of the enum type
 *  the token is.
 *  \return A label as string
 */
std::string getTokenTypeLabel(const TokenType &type)
{
	switch(type)
	{
		case T_INTEGER: return "T_INTEGER";
		case T_REAL   : return "T_REAL";
		case T_PLUS   : return "T_PLUS";
		case T_MINUS  : return "T_MINUS";
		case T_DIV    : return "T_DIV";
		case T_MUL    : return "T_MUL";
		case T_EOF    : return "T_EOF";
		case T_LPAREN : return "T_LPAREN";
		case T_RPAREN : return "T_RPAREN";
		case T_COLON  : return "T_COLON";
		case T_COMMA  : return "T_COMMA";
		case T_SEMI   : return "T_SEMI";

		case T_PASC_BEGIN_RESERV  : return "T_PASC_BEGIN_RESERV";
		case T_PASC_END_RESERV    : return "T_PASC_END_RESERV";
		case T_PASC_ASSIGN 	  : return "T_PASC_ASSIGN";
		case T_PASC_DOT_RESERV    : return "T_PASC_DOT_RESERV";
		case T_PASC_REAL_RESERV   : return "T_PASC_REAL_RESERV";
		case T_PASC_INTEGER_RESERV: return "T_PASC_INTEGER_RESERV";
		case T_PASC_INT_DIV_RESERV: return "T_PASC_INT_DIV_RESERV";
		case T_PASC_VAR_RESERV    : return "T_PASC_VAR_RESERV";
		case T_PASC_PROGRAM_RESERV: return "T_PASC_PROGRAM_RESERV";
		case T_PASC_ID     	  : return "T_PASC_ID";
		case T_PASC_PROCEDURE	  : return "T_PASC_PROCEDURE";

		case T_MAX    : return "T_MAX";
		default:
			raiseLabelError();
			break;
	}
	return "";
} /* getTokenTypeLabel */

/**********************************************************************/
/*!
 *  \brief Gives details concerning the value and the type
 *  of a Token.
 *  \return a string with the details
 */
std::string Token::representation()
{
	std::ostringstream stringStream;
	stringStream << "Token(" << getTokenTypeLabel(type_) << ", " << value_ << ")";
	std::string msg = stringStream.str();
	return msg;
} /* Token::representation */

/**********************************************************************/
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
} /* Token::isOperator */

/**********************************************************************/
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
		case T_PASC_INT_DIV_RESERV:
			return true;
		default:
			return false;
	}
	return false;
} /* Token::isOperatorSecondPrecedence */

/**********************************************************************/
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
} /* Token::isOperatorFirstPrecedence */













/*****************************************************/
/*****************************************************/
/*****************************************************/
/*********************LEXER***************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*!
 *  \fn void Lexer::raiseError()
 *  \brief Raises assertion when getTokenTypeLabel()
 *  is not updated!
 */
void Lexer::raiseError()
{
	std::cout << "Invalid Syntax!!" << std::endl;
	assert(false);
} /* Lexer::raiseError */

/**********************************************************************/
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
} /* Lexer::advance */

/**********************************************************************/
/*!
 * fn void Lexer::peek()
 * \brief No brief here. TODO !
 */
char Lexer::peek()
{
	size_t peekPos = pos_ + 1;
	if (peekPos > text_.size() - 1) {
		CLog::write(CLog::DEBUG, "peek()\n");
		return '\0';
	} else {
		CLog::write(CLog::DEBUG, "peek text_[pos] %c\n", text_[peekPos]);
		return text_[peekPos];
	}
} /* Lexer::peek */

/**********************************************************************/
/*!
 * fn void Lexer::skipComment()
 * \brief Discards all the characters until the closing curly brace is found. 
 */
void Lexer::skipComment()
{
	while (currentChar_ != '}') {
		advance();
	}
	advance(); // the closing curly brace
} /* Lexer::skipComment */

/**********************************************************************/
/*!
 * fn void Lexer::_getReservedKeywords()
 * \brief
 */
Token Lexer::_getReservedKeyword(const std::string &result)
{
	CLog::write(CLog::DEBUG, "_getReservedKeyword() %s\n", result.c_str());
	if (!strcmp(result.c_str(), "BEGIN")) {
		return Token(T_PASC_BEGIN_RESERV, result);
	} else if (!strcmp(result.c_str(), "END")) {
		return Token(T_PASC_END_RESERV, result);
	} else if (!strcmp(result.c_str(), "PROGRAM")) {
		return Token(T_PASC_PROGRAM_RESERV, result);
	} else if (!strcmp(result.c_str(), "VAR")) {
		return Token(T_PASC_VAR_RESERV, result);
	} else if (!strcmp(result.c_str(), "DIV")) {
		return Token(T_PASC_INT_DIV_RESERV, result);
	} else if (!strcmp(result.c_str(), "INTEGER")) {
		return Token(T_PASC_INTEGER_RESERV, result);
	} else if (!strcmp(result.c_str(), "REAL")) {
		return Token(T_PASC_REAL_RESERV, result);
	} else if (!strcmp(result.c_str(), "PROCEDURE")) {
		return Token(T_PASC_PROCEDURE, result);
	}

	//If result is not a reserved keyword, then it's a variable or a mistake :)
	return Token(T_PASC_ID, result); 
} /* Lexer::_getReservedKeyword */

/**********************************************************************/
/*!
 * fn void Lexer::_id()
 * \brief Handles identifiers and reserved keywords
 * \return Token
 */
Token Lexer::_id()
{
	std::string result = "";

	while (!isblank(currentChar_) && isalnum(currentChar_)) {
		result.append(1, currentChar_);
		advance();
	}
	CLog::write(CLog::DEBUG, "_id() --> %s\n", result.c_str());
	Token tok = _getReservedKeyword(result); 
	return tok;
} /* Lexer::_id */

/**********************************************************************/
/*!
 * fn void Lexer::skipWhiteSpace()
 */
void Lexer::skipWhiteSpace()
{
	while (currentChar_ != '\0' && isspace(currentChar_)) {
		advance();
	}
} /* Lexer::skipWhiteSpace */

/**********************************************************************/
/*!
 * fn void Lexer::number()
 * \brief Return a multidigit integer or float consumed from the input.
 * \return The number in string format
 */
Token Lexer::number()
{
	std::string result = "";
	while (currentChar_ != '\0' && isdigit(currentChar_)) {
		result += currentChar_;
		advance();
	}
	if (currentChar_ == '.') {
		result += currentChar_;
		advance();

		while ( currentChar_ != '\0' &&
			isdigit(currentChar_)) {
			result += currentChar_;
			advance();
		}
		
		return Token(T_REAL, result);
	} else {
		return Token(T_INTEGER, result);
	}
	return Token();
} /* Lexer::number */

/**********************************************************************/
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
	CLog::write(CLog::DEBUG, "Lexer::getNextToken\n\n");
	while (currentChar_ != '\0') {
		CLog::write(CLog::DEBUG, "%c\n", currentChar_);
		if (currentChar_ == '{') {
			advance();
			skipComment();
		}
		if (isalpha(currentChar_)) {
			return _id();
		}
		if (currentChar_ == ':' && peek() == '=') {
			advance();
			advance();
			return Token(T_PASC_ASSIGN, ":=");
		}
		if (currentChar_ == ';') {
			advance();
			return Token(T_SEMI, ";");
		}
		if (currentChar_ == '.') {
			advance();
			return Token(T_PASC_DOT_RESERV, ".");
		}
		if (isspace(currentChar_)) {
			skipWhiteSpace();
			continue;
		}
		if (isdigit(currentChar_)) {
			return number();
		}
		if (currentChar_ == ':') {
			advance();
			return Token(T_COLON, ":");
		}
		if (currentChar_ == ',') {
			advance();
			return Token(T_COMMA, ",");
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
		if (currentChar_ == '(') {
			advance();
			return Token(T_LPAREN, "(");
		}
		if (currentChar_ == ')') {
			advance();
			return Token(T_RPAREN, ")");
		}
		CLog::write(CLog::DEBUG, "getNextTOken! should not Readch!");
		raiseError();
	}
	CLog::write(CLog::DEBUG, " getNextToken: Will return EOF!");
	return Token(T_EOF, "");
} /* Lexer::getNextToken */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/









static void start(std::ifstream &file)
{
	std::string expr;

	if (file.is_open()) {
		std::string prog { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		std::cout << prog << std::endl;
		Lexer lex(prog) ;
		CLog::write(CLog::DEBUG, "start() before parser, parse(lex)\n"); 
		Parser parser(lex);
		CLog::write(CLog::DEBUG, "interpret() before interpret, interpret(parser)\n"); 
		Interpreter interpreter(parser);
		Node *result = interpreter.interpret();

		ASTPresenter pres;
		pres.visit(result);
	}
} /* start */

/**********************************************************************/

/**********************************************************************/
static int getFileWithRandomExpression(const char *argv, std::ifstream &file)
{
	file.open(argv, std::ifstream::in);
	if (!file.is_open()) {
		assert(false);
		return 0;
	} 
	return 1;
} /* getFileWithRandomExpression */

/**********************************************************************/

/**********************************************************************/
static int parseArgs(const int argc, char **argv, std::ifstream &file)
{
	for (int i = 0; i < argc; i++) {
		//if (!strcmp(argv[i], "-f")) {
			if (argv[i+1]) {
				getFileWithRandomExpression(argv[i+1], file);
				return 1;
			}
		//}
	}
	return 0;
} /* parseArgs */

/**********************************************************************/

/**********************************************************************/
int main(int argc, char **argv)
{
	std::ifstream file;
	parseArgs(argc, argv, file);
	start(file);
} /* main */

/**********************************************************************/

/**********************************************************************/
