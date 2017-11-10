#include <string>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <map>

#include "interpreter.hpp"
#include "clog.hpp"

std::map<std::string, int> GLOBAL_SCOPE;

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

Node *Parser::parse()
{
	CLog::write(CLog::DEBUG, "parse()\n");
	Node *node = program();
	if (currentToken_.type() != T_EOF) {
		raiseError();
	}
	return node;
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
TokenNode *Parser::expr()
{
	CLog::write(CLog::DEBUG, "Parser::expr()\n");
	TokenNode *node = term();

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
 * fn void Parser::factor()
 * \brief factor : T_PLUS factor 
 * 		   | T_MINUS factor
 * 		   | T_INTEGER
 * 		   | T_LPAREN expr T_RPAREN
 * 		   | variable
 * \return An T_INTEGER token value
 */
TokenNode *Parser::factor()
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
		TokenNode *node = expr();
		eat(T_RPAREN);
		return node;
	} else {
		return variable();
	}
	return NULL;
}

/*!
 * fn Node *Parser::program()
 * \brief program : compound_statement DOT
 * \return Node * 
 */
Node *Parser::program()
{
	CLog::write(CLog::DEBUG, "program()\n"); 
	Compound *node = compoundStatement();

	eat(T_PASC_DOT);
	return node;
}

/*!
 * fn void Compound::addNode(Node *node)
 * \brief : Adds a node to children_
 */
void Compound::addNode(Node *node)
{
       	children_.push_back(node);
}

/*!
 * fn void Compound::nodeDetails(int indent)
 * \brief : A string that reports details about the node
 */
void Compound::nodeDetails(int indent)
{
	size_t childrenNum = children_.size();
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sThis is a Compound Node with %d children\n",ind.c_str(), childrenNum);
	std::vector<Node *>::iterator it;


	indent = indent + 5;
	for (it = this->children_.begin(); it != children_.end(); it++) {
		(*it)->nodeDetails(indent);
	}
}

/*!
 * fn void Var::nodeDetails(int indent)
 * \brief : A string that reports details about the node 
 */
void Var::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sVar Node value_: %s\n", ind.c_str(), this->value_.c_str());
}

/*!
 * fn void NoOp::nodeDetails(int indent)
 * \brief : A string that reports details about the node! 
 */
void NoOp::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sNoOp Node\n", ind.c_str());
}

/*!
 * fn void Assign::nodeDetails(int indent)
 * \brief : A string that reports details about the node! 
 */
void Assign::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sAssign \n", ind.c_str());
	this->lhs_->nodeDetails(indent + 5);
	this->rhs_->nodeDetails(indent + 5);
}

/*!
 * fn void BinOp::nodeDetails(int indent)
 * \brief : A string that reports details about the node! 
 */
void BinOp::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sBinOp %s\n", ind.c_str(), this->getToken().representation().c_str());

	this->lhs_->nodeDetails(indent + 5);
	this->rhs_->nodeDetails(indent + 5);
}

/*!
 * fn void UnaryOp::nodeDetails(int indent)
 * \brief : A string that reports details about the node! 
 */
void UnaryOp::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sUnaryOp %s\n", ind.c_str(), this->getToken().representation().c_str());

	this->expr_->nodeDetails(indent + 5);
}

/*!
 * fn void Number::nodeDetails(int indent)
 * \brief : A string that reports details about the node! 
 */
void Number::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sNumber %s\n", ind.c_str(), this->getToken().representation().c_str());
}

/*!
 * fn Node *Parser::compoundStatement()
 * \brief : BEGIN statementList END 
 * \return Node * 
 */
Compound *Parser::compoundStatement()
{
	CLog::write(CLog::DEBUG, "compoundStatement():\n"); 
	eat(T_PASC_BEGIN);
	std::vector<Node *> nodes = statementList();
	eat(T_PASC_END);

	CLog::write(CLog::DEBUG, "nodes number(): %d\n", nodes.size()); 
	Compound *root = new Compound();

	//FIXME for_each with functor!!
	std::vector<Node *>::const_iterator it;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		root->addNode(*it);
	}
	return root;
}

/*!
 * fn std::vector<Node *>Parser::statementList()
 * \brief : statementList : statement | statement T_PASC_SEMI statementList
 * \return std::vector<Node *>
 */
std::vector<Node *> Parser::statementList()
{
	CLog::write(CLog::DEBUG, "statementList():\n"); 
	Node *node = statement();

	std::vector<Node *> results;

	CLog::write(CLog::DEBUG, "\t currentToken_ %s\n", currentToken_.representation().c_str());
	results.push_back(node);
	CLog::write(CLog::DEBUG, "\nstatementList(): statementsnum %d\n", results.size()); 

	while (currentToken_.type() == T_PASC_SEMI) {
		eat(T_PASC_SEMI);
		results.push_back(statement());
	}

	if (currentToken_.type() == T_PASC_ID) {
		raiseError();
	}

	CLog::write(CLog::DEBUG, "statementList() results.size() --> %d\n", results.size());
	return results;
}

/*!
 * fn Node *Parser::statement()
 * \brief : statement : compoundStatement | assigmentStatement | empty
 * \return Node *
 */
Node *Parser::statement()
{
	CLog::write(CLog::DEBUG, "statement()\n"); 
	CLog::write(CLog::DEBUG, "\t currentToken_ %s\n", currentToken_.representation().c_str());
	Node *node = NULL;

	if (currentToken_.type() == T_PASC_BEGIN) {
		node = compoundStatement();
	} else if (currentToken_.type() == T_PASC_ID) {
		node = assignmentStatement();
	} else {
		node = empty();
	}
	return node;
}

/*!
 * fn Node *Parser::assignmentStatement()
 * \brief : assignmentStatement : variable T_PASC_ASSIGN expr
 * \return Node *
 */
Assign *Parser::assignmentStatement()
{
	CLog::write(CLog::DEBUG, "assignmentStatement\n"); 
	TokenNode *lhs = variable();
	Token tok = currentToken_;

	eat(T_PASC_ASSIGN);

	Node *rhs = expr();
	Assign *node = new Assign(lhs, tok, rhs);

	return node;
}

/*!
 * fn Node *Parser::variable()
 * \brief : variable : T_PASC_ID
 * \return Node *
 */
TokenNode *Parser::variable()
{
	CLog::write(CLog::DEBUG, "VARIABLE node %s\n", currentToken_.representation().c_str()); 
	TokenNode *node = new Var(currentToken_);
	eat(T_PASC_ID);
	CLog::write(CLog::DEBUG, "\t variable %s\n", currentToken_.representation().c_str());

	return node;
}

/*!
 * fn Node *Parser::empty()
 * \brief : An empty production 
 * \return Node *
 */
NoOp *Parser::empty()
{
	return new NoOp();
}

/*!
 * fn void Interpeter::term()
 * \brief term : factor((MUL | DIV) factor)*
 * \return An T_INTEGER token value
 */
TokenNode *Parser::term()
{
	CLog::write(CLog::DEBUG, "Parser::term()\n");
	TokenNode *node = factor();

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

void NodeVisitor::visitForDetails(Node *node)
{
	node->nodeDetails(indent_);
}

int BinOp::visit()
{
	CLog::write(CLog::DEBUG, "BinOp::visit\n");
	CLog::write(CLog::DEBUG, "BinOp: this->op_.type(): %s\n", this->getToken().representation().c_str());
	if (this->getToken().type() == T_PLUS) {
		return this->lhs_->visit() + this->rhs_->visit();
	} else if (this->getToken().type() == T_MINUS) {
		return this->lhs_->visit() - this->rhs_->visit();
	} else if (this->getToken().type() == T_MUL) {
		return this->lhs_->visit() * this->rhs_->visit();
	} else if (this->getToken().type() == T_DIV) {
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

/*!
 * fn void Compound::visit()
 * \brief  Compound visitor iterates over its children and visits each one in turn.
 * \return An T_INTEGER token value
 */
int Compound::visit()
{
	std::vector<Node *>::iterator it;
	for (it = children_.begin(); it != children_.end(); it++) 
	{
		//edo ta gamisa ti mana
/*		it->visit();*/
	}
	return 0;
}

int Assign::visit()
{
	typedef std::map<std::string, int>KVMap;

	std::string varName = this->lhs_->getToken().value();
	//ena map pou tha pairno to key()
	std::map<std::string, int>::iterator it;

	it = GLOBAL_SCOPE.find(varName);
	if (it != GLOBAL_SCOPE.end()) {
		GLOBAL_SCOPE.insert(KVMap::value_type(varName, this->rhs_->visit()));
	}
	return 0;
}

int Var::visit()
{
	std::string varName = this->getToken().value();
	std::map<std::string, int>::iterator it;
	it = GLOBAL_SCOPE.find(varName);
	if (it != GLOBAL_SCOPE.end()) {
		CLog::write(CLog::RELEASE, "Var::visit() --> No Such Variable!");
	} else {
		return it->second;
	}
	assert(false);
	return 0;
}

/*!
 * fn void NoOp::visit()
 * \brief  NoOp visitor does nothing! 
 * \return An T_INTEGER token value
 */
int NoOp::visit()
{
	return 0;
}


int Number::visit()
{
	CLog::write(CLog::DEBUG, "Number::visit %s\n", this->getToken().representation().c_str());
	return this->value();
}

Node* Interpreter::interpret()
{
	CLog::write(CLog::DEBUG, "Interpreter::interpret()\n");
	Node *tree = parser_.parse();
	CLog::write(CLog::DEBUG, "Interpreter::interpret() 2\n");
	return tree;
}

