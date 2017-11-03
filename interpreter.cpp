#include <string>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>

#include "interpreter.hpp"
#include "clog.hpp"


/*!
 * \file token.cpp
 */

/*!
 * fn void Parser::raiseParseError()
 * \brief Raise assertion when error occurs while parsing
 */
void Parser::raiseError()
{
	CLog::write(CLog::RELEASE, "Invalid syntax\n");
/*	std::cout << "Invalid syntax" << std::endl;*/
	assert(true);
}

/*!
 * fn void Parser::eat()
 * \brief Compare the current token type with the passed token
 * type and if they match then "eat" the current token
 * and assign the next token to the currentToken,
 * otherwise raise exception. 
 */
void Parser::eat(const TokenType &tokType)
{
	if (currentToken_.type() == tokType) {
		currentToken_ = lexer_.getNextToken();
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
Node *Parser::expr()
{
	CLog::write(CLog::DEBUG, "Entered Parser::expr()\n");
	Node *node = term();

	while (currentToken_.isOperatorFirstPrecedence()) {
		Token tok = currentToken_;
		if (tok.type() == T_PLUS) {
			eat(T_PLUS);
		} else if (tok.type() == T_MINUS) {
			eat(T_MINUS);
		}
		CLog::write(CLog::DEBUG, "Parser::expr() before BinOp First precedence\n");
		node = new BinOp(node, tok, term());
	}
	return node;
}

/*!
 * fn void Interpeter::factor()
 * \brief factor : INTEGER | LPAREN expr RPAREN
 * \return An T_INTEGER token value
 */
Node *Parser::factor()
{
	CLog::write(CLog::DEBUG, "Entered Parser::factor()\n");
	Token tok = currentToken_;
	if (tok.type() == T_INTEGER) {
		eat(T_INTEGER);
		CLog::write(CLog::DEBUG, "Entered Parser::factor():  %s\n", tok.representation().c_str());
		return new Number(tok);
	} else if (tok.type() == T_LPAREN) {
		eat(T_LPAREN);
		Node *node = expr();
		eat(T_RPAREN);
		return node;
	}
	return NULL;
}

/*!
 * fn void Interpeter::term()
 * \brief term : factor((MUL | DIV) factor)*
 * \return An T_INTEGER token value
 */
Node *Parser::term()
{
	CLog::write(CLog::DEBUG, "Entered Parser::term()\n");
	Node *node = factor();

	while (currentToken_.isOperatorSecondPrecedence()) {
		Token tok = currentToken_;
		if (tok.type() == T_MUL) {
			eat(T_MUL);
		} else if (tok.type() == T_DIV) {
			eat(T_DIV);
		}
		CLog::write(CLog::DEBUG, "Parser::term() before BinOp\n");
		node = new BinOp(node, tok, factor());
		CLog::write(CLog::DEBUG, "Parser::term() BinOp is %p\n", node);
	}
	return node;
}

int NodeVisitor::visit(Node *node)
{
	CLog::write(CLog::DEBUG, "Entered NodeVisitor::visit\n");
	return node->visit();
}


/*//FIXME*/
int BinOp::visit()
{
	CLog::write(CLog::DEBUG, "Entered BinOp::visit\n");
	CLog::write(CLog::DEBUG, "this->op_.type(): %s\n", this->op_.representation().c_str());
	if (this->op_.type() == T_PLUS) {
		return this->lhs_->visit() + this->rhs_->visit();
	} else if (this->op_.type() == T_MINUS) {
		return this->lhs_->visit() - this->rhs_->visit();
	} else if (this->op_.type() == T_MUL) {
		return this->lhs_->visit() * this->rhs_->visit();
	} else if (this->op_.type() == T_DIV) {
		return this->lhs_->visit() / this->rhs_->visit();
	}
	CLog::write(CLog::DEBUG, "BinOp::visit should not reach\n");
	return 0;
}

int Number::visit()
{
	CLog::write(CLog::DEBUG, "Entered Number::visit %d\n", this->value());
	CLog::write(CLog::DEBUG, "Entered Number::visit %s\n", this->getToken().representation().c_str());
	return this->value();
}

int Interpreter::interpret()
{
	CLog::write(CLog::DEBUG, "Entered Interpreter::interpret()\n");
	Node *tree = parser_.parse();
	return visit(tree);
}

