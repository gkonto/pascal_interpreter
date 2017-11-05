#pragma once
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include "token.hpp"
#include "clog.hpp"


class Node
{
	public:
		Token getToken() { return token_; }
		void  setToken(Token tok) { token_ = tok; }
		virtual Node *getRhs() { return NULL; }
		virtual Node *getLhs() { return NULL; }
		virtual int  visit()   { CLog::write(CLog::DEBUG, "virtual visit() should not reach!\n"); return 0; }
		virtual int  value()   { return 0; }
	private:
		Token token_;
};

class BinOp : public Node
{
	public:
		BinOp(Node *lhs, Token &op, Node *rhs) : lhs_(lhs), op_(op),  rhs_(rhs)
       		{
		       	setToken(op);
		}
		int visit();
		Node *getRhs() { return rhs_; }
		Node *getLhs() { return lhs_; }
	private:
		Node *lhs_;
		//FIXME to mpoulo, na ton apothikeuso sto Node
		Token op_;
		Node *rhs_;
};

class UnaryOp : public Node
{
	public:
		UnaryOp(Token &op, Node *expr) : expr_(expr)
		{
			setToken(op);
		}
	private:
		int visit();
		Node *expr_;
};

class Number : public Node
{
	public:
		Number(Token tok)
       		{
			setToken(tok);
		}
		int visit();
		int value() { return atoi(getToken().value().c_str()); }
	private:
};

class Parser
{
	public:
		///Creates an Parser object
		Parser(Lexer lexer) : lexer_(lexer), currentToken_(lexer_.getNextToken()) {}
		void eat(const TokenType &toktype);
		Node *parse() { return expr(); }
		Node *expr();
		Node *term();
		Node *factor();
	private:
		void raiseError();
		Lexer lexer_;
		Token currentToken_;
};

class NodeVisitor
{
	public:
		int visit(Node *node);//FIXME
/*		generic_visit(Node *node)://FIXME*/

	private:
};

class Interpreter : public NodeVisitor
{
	public:
		Interpreter(Parser parser) : parser_(parser) {}
		int interpret();
	private:
		Parser parser_;
};

#endif
