#pragma once
#include "token.hpp"
#include "clog.hpp"
#include <vector>
#include <map>

/*!
 * \file interpreter.hpp
 */

enum SymbolType {
	INTEGER,
	REAL,
	NONE
};
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**
 * A Node class.
 * An abstract class.
 */
class Node
{
	public:
		virtual Node *getRhs() { return NULL; }
		virtual Node *getLhs() { return NULL; }
		virtual double  value()   { return 1; }

		virtual void   visitASTPresenter(int ind) = 0;
		virtual void   visitSemanticAnalyzer()    = 0;
		virtual double visitEvaluate()            = 0;
	private:
}; /* Node */

/**********************************************************************/
/**
 * A TokenNode class.
 * Inherits from Node
 * It's the node that contain a token.
 */
class TokenNode : public Node
{
	public:
		Token getToken() { return token_; }
		void  setToken(Token tok) { token_ = tok; }
	private:	
		Token token_;
}; /* TokenNode */

/**********************************************************************/
/**
 *  A Type class.
 *  Represents a variable type
 */
class Type : public Node
{
	public:
		Type(Token tok) : token_(tok), value_(tok.value()) {}
		std::string getValue() { return value_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		Token token_;
		std::string value_;
}; /* Type */

/**********************************************************************/
/**
 *  A Compound class.
 *  Represents a 'BEGIN ... END' block
 *  it contains a list of statement nodes in its children variable
 */
class Compound : public Node
{
	public:
		typedef std::vector<Node *>::iterator iterator;

		void addNode(Node *node); 
		std::vector<Node *> getChildren() { return children_; }
		size_t size() { return children_.size(); }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();

		iterator begin() { return children_.begin(); }
		iterator end() { return children_.end(); }
	private:
		std::vector<Node *> children_;
}; /* Compound */

/**********************************************************************/
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

		std::string getValue() { return value_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		std::string value_;
}; /* Var */

/**********************************************************************/
/**
 * A VarDecl class.
 * Represents a variable declaration. It holds a variable node and a type node.
 */
class VarDecl : public Node
{
	public:
		VarDecl(Var *varNode, Type *typeNode) : varNode_(varNode), typeNode_(typeNode) {}
		Type *getTypeNode() { return typeNode_; }
		Var  *getVarNode()  { return varNode_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		Var  *varNode_;
		Type *typeNode_;
}; /* VarDecl */

/**********************************************************************/
/**
 *  A Block class.
 *  Holds the declarations and a compound statement 
 */
class Block : public Node
{
	public:
		typedef std::vector<Node *>::iterator iterator;

		Block(std::vector<Node *>declarations, Compound *compoundStatement) : declarations_(declarations), compoundStatement_(compoundStatement) {}

		Block::iterator begin() { return declarations_.begin(); }
		Block::iterator end()   { return declarations_.end(); }

		std::vector<Node *>getDeclarations() { return declarations_; }
		size_t size() { return declarations_.size(); }
		
		Compound *getCompound() { return compoundStatement_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		std::vector<Node *>declarations_;
		Compound *compoundStatement_;
}; /* Block */

/**********************************************************************/
/**
 *  A Program class.
 *  Represents a program and will be out root node
 */
class Program : public Node
{
	public:

		Program(std::string name, Block *block) : name_(name), block_(block) {}
		Block *getBlock() { return block_; };
		std::string getName() { return name_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		std::string name_;
		Block *block_;
}; /* Program */

/**********************************************************************/
/**
 * A NodeOp class
 * A dummy object
 */
class NoOp : public Node
{
	public:
		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
}; /* NoOp */

/**********************************************************************/
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
		TokenNode *getLhs() { return lhs_; }
		Node *getRhs() { return rhs_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		TokenNode *lhs_;
		Node *rhs_;
}; /* Assign */

/**********************************************************************/
/**
 * A BinOp class
 * Represents a node with two children of Node type
 */
class BinOp : public TokenNode
{
	public:
		BinOp(Node *lhs, Token &op, Node *rhs) : lhs_(lhs), rhs_(rhs)
		{
			setToken(op);
		}
		Node *getRhs() { return rhs_; }
		Node *getLhs() { return lhs_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		Node *lhs_;
		Node *rhs_;

};

/**********************************************************************/
/**
 * A UnaryOp class
 * Represents a Unary Node
 */
class UnaryOp : public TokenNode
{
	public:
		UnaryOp(Token &op, Node *expr) : expr_(expr)
		{
			setToken(op);
		}

		Node *getExpr() { return expr_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		Node *expr_;
}; /* UnaryOp */

/**********************************************************************/
class Param : public Node
{
	public:
		Param(Var *var, Type *type) : var_(var), type_(type) {}

		Var *getVar() { return var_; }
		Type *getType() { return type_; }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		Var *var_;
		Type *type_;
}; /* Param */

/**********************************************************************/
class ProcedureDecl : public Node
{
	public:
		typedef std::vector<Param *>::iterator iterator;

		ProcedureDecl(const std::string &procName, std::vector<Param *>params, Block *block)
		       	: name_(procName), params_(params), block_(block) {}

		Block *getBlock() { return block_; }
		std::string getName() { return name_; }
		iterator begin() { return params_.begin(); }
		iterator end()   { return params_.end(); }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
		std::string name_;
		std::vector<Param *> params_;
		Block *block_;
}; /* ProcedureDecl */

/**********************************************************************/
class Number : public TokenNode
{
	public:
		Number(Token tok)
       		{
			setToken(tok);
		}

		double value() { return atof(getToken().value().c_str()); }

		void visitASTPresenter(int ind);
		void visitSemanticAnalyzer();
		double visitEvaluate();
	private:
}; /* Number */


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
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
		std::vector<Node *> declarations();	
		std::vector<VarDecl *>variableDeclaration();
		std::vector<Param *>formalParameters();
		std::vector<Param *>formalParameterList();
	private:
		void raiseError(const TokenType &tokType);
		Lexer lexer_;
		Token currentToken_;
}; /* Parser */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
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
}; /* Symbol */

/**********************************************************************/
class BuiltinTypeSymbol : public Symbol
{
	public:
		BuiltinTypeSymbol(std::string name) : Symbol(name, NULL) {}
	private:
}; /* BuiltinTypeSymbol */

/**********************************************************************/
class VarSymbol : public Symbol
{
	public:
		VarSymbol(std::string name, Symbol *type) : Symbol(name, type) {}
		std::string representation() const { return name_; }
	private:
}; /* VarSymbol */

/**********************************************************************/
class ProcedureSymbol : public Symbol
{
	public:
		ProcedureSymbol(std::string name) : Symbol(name, NULL) {}
		void add(VarSymbol *varSymbol) { params_.push_back(varSymbol); }
	private:
		std::vector<VarSymbol *> params_;


}; /* ProcedureSymbol */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
class ScopedSymbolTable
{
	public:
		ScopedSymbolTable(std::string name = "global", int level = 1, ScopedSymbolTable *enclosScope = NULL);

		void initBuiltins();
		void define(Symbol *symbol);
		Symbol *lookup(const std::string &name, bool currentScopeOnly = false);

		void representation();
		int getLevel() { return level_; }

		ScopedSymbolTable *getEnclosingScope() { return enclosingScope_; }
	private:
		std::string name_;
		int level_;
		ScopedSymbolTable *enclosingScope_;
		std::map<std::string, Symbol *> symbols_;

}; /* ScopedSymbolTable */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
class Interpreter
{
	public:
		Interpreter(Parser parser) : parser_(parser) {}
		Node *interpret();
	private:
		Parser parser_;
}; /* Interpreter */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

			/* Visitors */
class Evaluator
{
	public:
		double visit(Node *node) { return node->visitEvaluate(); }
	private:
};

/**********************************************************************/
/**
 * An ASTPresenter
 * visit method visits recursively all nodes (root is given as argument)
 * and prints info about the AST and how it is formatted
 */
class ASTPresenter  
{
	public:
		void visit(Node *node) { node->visitASTPresenter(0); }
	private:
};

/**********************************************************************/
class SemanticAnalyzer 
{
	public:
		void visit(Node *node) { node->visitSemanticAnalyzer(); }
		static ScopedSymbolTable *scope_;
		static ScopedSymbolTable *currentScope_;
	private:

};





