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
	CLog::write(CLog::DEBUG, "Parser::expr()\n");
	Node *node = term();

	CLog::write(CLog::DEBUG, "expr() 2\n");
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
	CLog::write(CLog::DEBUG, "Parser::factor()\n");
	Token tok = currentToken_;
	if (tok.type() == T_PLUS) {
		eat(T_PLUS);
		CLog::write(CLog::DEBUG, "factor() UnaryPlus create\n");
		return new UnaryOp(tok, factor());
	} else if (tok.type() == T_MINUS) {
		eat(T_MINUS);
		CLog::write(CLog::DEBUG, "factor() UnaryMinus create\n");
		return new UnaryOp(tok, factor());
	} else if (tok.type() == T_INTEGER) {
		eat(T_INTEGER);
		CLog::write(CLog::DEBUG, "Parser::factor():  %s\n", tok.representation().c_str());
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
	CLog::write(CLog::DEBUG, "Parser::term()\n");
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
	CLog::write(CLog::DEBUG, "NodeVisitor::visit\n");
	CLog::write(CLog::DEBUG, "NodeVisitor::visit() %p\n", node);
	if (node) {
		return node->visit();
	}
	return 0;
}

int BinOp::visit()
{
	CLog::write(CLog::DEBUG, "BinOp::visit\n");
	CLog::write(CLog::DEBUG, "BinOp: this->op_.type(): %s\n", this->op_.representation().c_str());
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

int UnaryOp::visit()
{
	CLog::write(CLog::DEBUG, "UnaryOp::visit(): Start!\n");
	Token tok = this->getToken();
	if (tok.type() == T_PLUS) {
		return +(this->expr_->visit());
	} else if (tok.type() == T_MINUS) {
		return -(this->expr_->visit());
	}
	CLog::write(CLog::DEBUG, "UnaryOp::visit(): Should not reach!\n");
	return 0;
}

int Number::visit()
{
	CLog::write(CLog::DEBUG, "Number::visit %s\n", this->getToken().representation().c_str());
	return this->value();
}

int Interpreter::interpret()
{
	CLog::write(CLog::DEBUG, "Interpreter::interpret()\n");
	Node *tree = parser_.parse();
	CLog::write(CLog::DEBUG, "Interpreter::interpret() 2\n");
	return visit(tree);
}

