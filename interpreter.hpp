#pragma once
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include "token.hpp"
#include "clog.hpp"
#include <vector>
#include <map>


class Node
{
	public:
		// virtual functions
		// =0 gia abstract mia apo autes i kai polles
		virtual Node *getRhs() { return NULL; }
		virtual Node *getLhs() { return NULL; }
		virtual int  visitData()   { CLog::write(CLog::DEBUG, "virtual visit Should not reach!\n"); return 0; }
		virtual int  value()   { return 1; }
		virtual void nodeDetails(int indent) {}
	private:
};

class TokenNode : public Node
{
	public:
		Token getToken() { return token_; }
		void setToken(Token tok) { token_ = tok; }
	private:	
		Token token_;
};

/**
 *  A Compound class.
 *  Represents a 'BEGIN ... END' block
 *  it contains a list of statement nodes in its children variable
 */
class Compound : public Node
{
	public:
		int visitData();
		void addNode(Node *node); 
		void nodeDetails(int indent);

	private:
		std::vector<Node *> children_;
};

/**
 * A Var class
 * Represents a Node that is constructed out of a T_PASC_ID token.
 * It represents a varialbe. The value_ holds the variable's name
 */
class Var : public TokenNode
{
	public:
		Var(Token &op)
		{
			setToken(op);
			value_ = op.value();
		}
		void nodeDetails(int indent);
		int visitData();
	private:
		std::string value_;
};

/**
 * A NodeOp class
 * TODO NoOp 
 */
class NoOp : public Node
{
	public:
		int visitData();
		void nodeDetails(int indent);
	private:
};

/**
 * An Assign class
 * Represents an assignment statement.
 * Its left variable is for storing a Var Node and its right variable is for storing a Node 
 * returned by the expr() parser method
 */
class Assign : public TokenNode
{
	public:
		Assign(TokenNode *lhs, Token &op, Node *rhs) : lhs_(lhs), rhs_(rhs)
		{
			setToken(op);
		}
		int visitData();
		void nodeDetails(int indent);
	private:
		TokenNode *lhs_;
		Node *rhs_;
};

class BinOp : public TokenNode
{
	public:
		BinOp(Node *lhs, Token &op, Node *rhs) : lhs_(lhs), rhs_(rhs)
       		{
		       	setToken(op);
		}
		int visitData();
		void nodeDetails(int indent);
		Node *getRhs() { return rhs_; }
		Node *getLhs() { return lhs_; }
	private:
		Node *lhs_;
		Node *rhs_;
};

class UnaryOp : public TokenNode
{
	public:
		UnaryOp(Token &op, Node *expr) : expr_(expr)
		{
			setToken(op);
		}
		int visitData();
		void nodeDetails(int indent);
		Node *getRhs() { return NULL;}
		Node *getLhs() { return NULL;}
	private:
		Node *expr_;
};

class Number : public TokenNode
{
	public:
		Number(Token tok)
       		{
			setToken(tok);
		}
		int visitData();
		int value() { return atoi(getToken().value().c_str()); }
		void nodeDetails(int indent);
	private:
};

class Parser
{
	public:
		Parser(Lexer lexer) : lexer_(lexer), currentToken_(lexer_.getNextToken()) {}

		void eat(const TokenType &toktype);

		std::vector<Node *>statementList();
		Node      *statement();
		Node      *program();
		Node      *parse();
		TokenNode *expr();
		TokenNode *term();
		TokenNode *factor();
		TokenNode *variable();
		Compound  *compoundStatement();
		Assign    *assignmentStatement();
		NoOp      *empty();
	private:
		void raiseError();
		Lexer lexer_;
		Token currentToken_;
};

class NodeVisitor
{
	public:
		NodeVisitor(int ind = 0) : indent_(ind) {}
		void visitData(Node *node);//FIXME
		void visitForDetails(Node *node);

/*		generic_visit(Node *node)://FIXME*/

	private:
		int indent_;
};


class Interpreter : public NodeVisitor
{
	public:
		Interpreter(Parser parser) : parser_(parser) {}
		Node *interpret();
		static std::map<std::string, int> GLOBAL_SCOPE;
	private:
		Parser parser_;
};

#endif
