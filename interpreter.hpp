#pragma once
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
		virtual double  visitData() = 0; //FIXME make it return void !
		virtual int  value()   { return 1; }
		virtual void nodeDetails(int indent) = 0;
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
 *  A Program class.
 *  Represents a program and will be out root node
 */
class Program : public Node
{
	public:
		Program(std::string name, Node *block) : name_(name), block_(block) {}
		void nodeDetails(int indent);
		double visitData();
	private:
		std::string name_;
		Node *block_;
};
/**
 *  A Type class.
 *  Represents a variable type
 */
class Type : public Node
{
	public:
		Type(Token tok) : token_(tok), value_(tok.value()) {}
		void nodeDetails(int indent);
		double visitData();
	private:
		Token token_;
		std::string value_;
};

/**
 *  A Compound class.
 *  Represents a 'BEGIN ... END' block
 *  it contains a list of statement nodes in its children variable
 */
class Compound : public Node
{
	public:
		void addNode(Node *node); 
		void nodeDetails(int indent);
		double visitData();
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
		double visitData();
		std::string getValue() { return value_; }
	private:
		std::string value_;
};


/**
 * Represents a variable declaration. It holds a variable node and a type node.
 */
class VarDecl : public Node
{
	public:
		VarDecl(Var *varNode, Type *typeNode) : varNode_(varNode), typeNode_(typeNode) {}
		void nodeDetails(int indent);
		double visitData();
	private:
		Var  *varNode_;
		Type *typeNode_;
};


/**
 *  A Block class.
 *  Holds the declarations and a compound statement 
 */
class Block : public Node
{
	public:
		Block(std::vector<VarDecl *>declarations, Compound *compoundStatement) : declarations_(declarations), compoundStatement_(compoundStatement) {}
		void nodeDetails(int indent);
		double visitData();
	private:
		std::vector<VarDecl *>declarations_;
		Compound *compoundStatement_;
};


/**
 * A NodeOp class
 * TODO NoOp 
 */
class NoOp : public Node
{
	public:
		double visitData();
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
		double visitData();
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
		double visitData();
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
		double visitData();
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
		double visitData() { return value(); }
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
		Program   *program();
		Node      *parse();
		TokenNode *expr();
		TokenNode *term();
		TokenNode *factor();
		Var       *variable();
		Compound  *compoundStatement();
		Assign    *assignmentStatement();
		NoOp      *empty();
		Block     *block();
		Type      *typeSpec();
		std::vector<VarDecl *> declarations();	
		std::vector<VarDecl *>variableDeclaration();
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
		static std::map<std::string, double> GLOBAL_SCOPE;
	private:
		Parser parser_;
};

