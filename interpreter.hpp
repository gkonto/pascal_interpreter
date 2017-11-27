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
		virtual void visitSymbolTable() {}
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

		void   nodeDetails(int indent);
		double visitData();
		void   visitSymbolTable();

		Node *block() { return block_; };
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
		std::string getValue() { return value_; }
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
		void visitSymbolTable();
		std::vector<Node *> getChildren() { return children_; }
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
		void visitSymbolTable();
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
		void visitSymbolTable(); 
		Type *getTypeNode() { return typeNode_; }
		Var  *getVarNode()  { return varNode_; }

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
		void visitSymbolTable();

		std::vector<VarDecl *>getDeclarations() { return declarations_; }
		Compound *getCompound() { return compoundStatement_; }
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
		void visitSymbolTable();
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
		void visitSymbolTable();

		TokenNode *getLhs() { return lhs_; }
		Node *getRhs() { return rhs_; }
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
		void visitSymbolTable();
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
		void visitSymbolTable();

		Node *getRhs() { return NULL;}
		Node *getLhs() { return NULL;}
		Node *getExpr() { return expr_; }
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
		int value() { return atoi(getToken().value().c_str()); }

		double visitData() { return value(); }
		void nodeDetails(int indent);
		void visitSymbolTable();

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

/*------------------------------------------*/

/*------------------------------------------*/
enum SymbolType {
	INTEGER,
	REAL,
	NONE
};

class Symbol
{
	public:
		Symbol(std::string name, Symbol *type = NULL) : name_(name), type_(type) {}

		virtual std::string representation() { return name_; }
		std::string name() { return name_; }
		Symbol *type() { return type_; }
	protected:
		std::string name_;
		Symbol *type_;
};

/*------------------------------------------*/
class BuiltinTypeSymbol : public Symbol
{
	public:
		BuiltinTypeSymbol(std::string name) : Symbol(name, NULL) {}
	private:
};

/*------------------------------------------*/
class VarSymbol : public Symbol
{
	public:
		VarSymbol(std::string name, Symbol *type) : Symbol(name, type) {}
		std::string representation() const { return name_; }
	private:
};

/*------------------------------------------*/
class SymbolTable
{
	public:
		SymbolTable();

		void representation();
		static Symbol *lookup(std::string &name);
		void define(Symbol *symbol);

		void visit(Node *node);

		static std::map<std::string, Symbol *> symbols_;
	private:
		void initializeBuiltins();
};

/*------------------------------------------*/
class NodeVisitor
{
	public:
		NodeVisitor(int ind = 0) : indent_(ind) {}
		void visitData(Node *node);//FIXME
		void visitForDetails(Node *node);
	private:
		int indent_;
};

/*------------------------------------------*/
class SymbolTableBuilder : public NodeVisitor
{
	public:
		void visit(Node *node);
		static SymbolTable symtab() { return symtab_; }
	private:
		static SymbolTable symtab_;
};

/*------------------------------------------*/
class Interpreter : public NodeVisitor
{
	public:
		Interpreter(Parser parser) : parser_(parser) {}
		Node *interpret();
		static std::map<std::string, double> GLOBAL_SCOPE;
	private:
		Parser parser_;
};

/*------------------------------------------*/
