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

bool CLog::m_bInitialised = true;
int CLog::m_nLevel = CLog::DEBUG ;
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
		case T_MUL    : return "T_MUL";
		case T_DIV    : return "T_DIV";
		case T_EOF    : return "T_EOF";
		case T_LPAREN : return "T_LPAREN";
		case T_RPAREN : return "T_RPAREN";

		case T_PASC_BEGIN  : return "T_PASC_BEGIN";
		case T_PASC_END    : return "T_PASC_END";
		case T_PASC_ASSIGN : return "T_PASC_ASSIGN";
		case T_PASC_SEMI   : return "T_PASC_SEMI";
		case T_PASC_DOT    : return "T_PASC_DOT";
		case T_PASC_ID     : return "T_PASC_ID";

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
}


Token Lexer::_getReservedKeyword(const std::string &result)
{
	CLog::write(CLog::DEBUG, "_getReservedKeyword() %s\n", result.c_str());
	if (!strcmp(result.c_str(), "BEGIN")) {
		return Token(T_PASC_BEGIN, result);
	} else if (!strcmp(result.c_str(), "END")) {
		return Token(T_PASC_END, result);
	}

	//If result is not a reserved keyword, then it's a variable or a mistake :)
	return Token(T_PASC_ID, result); 
}


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
	CLog::write(CLog::DEBUG, "getNextToken (beginning)\n\n");
	while (currentChar_ != '\0') {
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
			return Token(T_PASC_SEMI, ";");
		}
		if (currentChar_ == '.') {
			advance();
			return Token(T_PASC_DOT, ".");
		}
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
	return Token(T_EOF, "");
}

/*********************LEXER***************************/
/*****************************************************/

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

		// DEBUG
		NodeVisitor nodVis;
		nodVis.visitForDetails(result);

		std::cout << "GLOBAL_SCOPE" << std::endl;
		nodVis.visitData(result);
		std::cout << Interpreter::GLOBAL_SCOPE.size() << std::endl;
		std::map<std::string, int>::iterator it;
		for (it = Interpreter::GLOBAL_SCOPE.begin(); it != Interpreter::GLOBAL_SCOPE.end(); it++) {
			std::cout << it->first ;
			std::cout << "   " << it->second << std::endl;
		}

		// todo it should be 
		// compound-->compound->assign
		//ask for node details		




	} else {
		CLog::write(CLog::RELEASE, "> ");
		while (getline(std::cin, expr)) {
			if (expr.empty()) {
				continue;
			}
			Lexer lex(expr);
			Parser parser(lex);
			Interpreter interpreter(parser);
			//int result = interpreter.interpret();
			//CLog::write(CLog::RELEASE, "%d\n", result);
			CLog::write(CLog::RELEASE, "> ");
		}
	}
}

static int getFileWithRandomExpression(const char *argv, std::ifstream &file)
{
	file.open(argv, std::ifstream::in);
	if (!file.is_open()) {
		assert(false);
		return 0;
	} 
	return 1;
}

static int parseArgs(const int argc, char **argv, std::ifstream &file)
{
	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-f")) {
			if (argv[i+1]) {
				getFileWithRandomExpression(argv[i+1], file);
				return 1;
			}
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	std::ifstream file;
	parseArgs(argc, argv, file);
	start(file);
}
