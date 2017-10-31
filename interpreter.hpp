#pragma once
#include "token.hpp"


class Node
{
	public:
		Token getToken() { return token_; }
		void  setToken(Token tok) { token_ = tok; }
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
	private:
		Node *lhs_;
		Token op_;
		Node *rhs_;
};

class Number : public Node
{
	public:
		Number(Token tok) : value_(getToken().value())
       		{
			setToken(tok);
		}
	private:
		std::string value_;
};

class Interpreter
{
	public:
		///Creates an Interpreter object
		Interpreter(Lexer lexer) : lexer_(lexer), currentToken_(lexer_.getNextToken()) {}
		void eat(const TokenType &toktype);
		Node *expr();
		Node *term();
		Node *factor();
	private:
		void raiseError();
		Lexer lexer_;
		Token currentToken_;
};
