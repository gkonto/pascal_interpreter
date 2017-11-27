#include <string>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <map>

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
 * 		   | T_REAL
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
		return new UnaryOp(tok, factor());
	} else if (tok.type() == T_MINUS) {
		eat(T_MINUS);
		return new UnaryOp(tok, factor());
	} else if (tok.type() == T_INTEGER) {
		eat(T_INTEGER);
		return new Number(tok);
	} else if (tok.type() == T_REAL) {
		eat(T_REAL);
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
 * fn Program *Parser::program()
 * \brief program : T_PASC_PROGRAM variable T_SEMI block DOT
 * \return Program * 
 */
Program *Parser::program()
{
	CLog::write(CLog::DEBUG, "program()\n"); 
	eat(T_PASC_PROGRAM_RESERV);
	Var *varNode = variable();
	std::string progName = varNode->getValue();
	CLog::write(CLog::DEBUG, "program name %s\n", progName.c_str()); 
	eat(T_SEMI);

	Block *blockNode = block();
	Program *progNode = new Program(progName, blockNode);
	eat(T_PASC_DOT_RESERV);

	return progNode;
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
	CLog::write(CLog::RELEASE, "%sCompound Node with %d children\n",ind.c_str(), childrenNum);
	std::vector<Node *>::iterator it;


	indent = indent + 5;
	for (it = this->children_.begin(); it != children_.end(); it++) {
		(*it)->nodeDetails(indent);
	}
}

/*!
 * fn void VarDecl::nodeDetails(int indent)
 * \brief : A string that reports details about the node
 */
void VarDecl::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sVarDecl Node\n",ind.c_str());
	indent = indent + 5;
	varNode_->nodeDetails(indent);
	typeNode_->nodeDetails(indent);
}

/*!
 * fn void Type::nodeDetails(int indent)
 * \brief : A string that reports details about the node
 */
void Type::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sType Node with value_: %s\n", ind.c_str(), value_.c_str());
}

/*!
 * fn void Compound::nodeDetails(int indent)
 * \brief : A string that reports details about the node
 */
void Program::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sProgram Node with name %s\n",ind.c_str(), name_.c_str());
	block_->nodeDetails(indent + 5);
}

/*!
 * fn void Block::nodeDetails(int indent)
 * \brief : A string that reports details about the node and its children
 */
void Block::nodeDetails(int indent)
{
	std::string ind(indent, ' ');
	CLog::write(CLog::RELEASE, "%sBlock Node %d variables and a compound Node\n", ind.c_str(), declarations_.size());
	std::vector<VarDecl *>::iterator it;
	indent = indent + 5;
	for (it = declarations_.begin(); it != declarations_.end(); it++)
	{
		(*it)->nodeDetails(indent);
	}
	compoundStatement_->nodeDetails(indent);
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
	eat(T_PASC_BEGIN_RESERV);
	std::vector<Node *> nodes = statementList();
	eat(T_PASC_END_RESERV);

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
 * \brief : statementList : statement | statement T_SEMI statementList
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

	while (currentToken_.type() == T_SEMI) {
		eat(T_SEMI);
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

	if (currentToken_.type() == T_PASC_BEGIN_RESERV) {
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
Var *Parser::variable()
{
	CLog::write(CLog::DEBUG, "VARIABLE node: %s\n", currentToken_.representation().c_str()); 
	Var *node = new Var(currentToken_);
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
 * fn Block *Parser::block()
 * \brief : declarations compoundStatement 
 * \return Block *
 */
Block* Parser::block()
{
	CLog::write(CLog::DEBUG, "block: %s\n", currentToken_.representation().c_str()); 
	std::vector<VarDecl *>declarationNodes = declarations();
	Compound *comp = compoundStatement();
	Block *node = new Block(declarationNodes, comp);

	CLog::write(CLog::DEBUG, "END OF BLOCK NODE Creator: %s\n", currentToken_.representation().c_str()); 
	return node;
}

/*!
 * fn VarDecl *Parser::declarations()
 * \brief : declarations : VAR(variableDeclaration SEMI) + | empty
 * \return Block *
 */
std::vector<VarDecl *> Parser::declarations()
{
	std::vector<VarDecl *> declarations;
	if (currentToken_.type() == T_PASC_VAR_RESERV) {
		eat(T_PASC_VAR_RESERV);
		while (currentToken_.type() == T_PASC_ID) {
			std::vector<VarDecl *> vd = variableDeclaration();
			declarations.reserve(declarations.size() + distance(vd.begin(), vd.end()));
			declarations.insert(declarations.end(), vd.begin(), vd.end());
			eat(T_SEMI);
		}
	}
	
	return declarations;
}

/*!
 * fn VarDecl *Parser::variableDeclaration()
 * \brief : variable_declaration : T_PASC_ID (T_COMMA T_PASC_ID)* T_COLON type_spec
 * \return 
 */
 std::vector<VarDecl *> Parser::variableDeclaration()
{
	CLog::write(CLog::DEBUG, "variableDeclaration Beginning: %s\n", currentToken_.representation().c_str()); 
	Var *var = new Var(currentToken_);
	std::vector<Var *> varNodes;
	varNodes.push_back(var);
	eat(T_PASC_ID);
	
	while (currentToken_.type() == T_COMMA) {
		eat(T_COMMA);
		Var *var = new Var(currentToken_);
		varNodes.push_back(var);
		eat(T_PASC_ID);
	}

	eat(T_COLON);
	Type *node = typeSpec();
	std::vector<VarDecl *> varDeclarations;
	std::vector<Var *>::iterator it;
	for ( it = varNodes.begin(); it != varNodes.end(); it++) {
		VarDecl *vd = new VarDecl(*it, node);
		varDeclarations.push_back(vd);
	}

	CLog::write(CLog::DEBUG, "variableDeclaration End: %s\n", currentToken_.representation().c_str()); 
	return varDeclarations;
}

/*!
 * fn VarDecl *Parser::typeSpec()
 * \brief : typeSpec : T_INTEGER | T_REAL
 * \return 
 */
Type *Parser::typeSpec()
{
	CLog::write(CLog::DEBUG, "typeSpec Start! %s\n", currentToken_.representation().c_str()); 
	Token tok = currentToken_;
	if (currentToken_.type() == T_PASC_INTEGER_RESERV) {
		eat(T_PASC_INTEGER_RESERV);
	} else if (currentToken_.type() == T_PASC_REAL_RESERV) {
		eat(T_PASC_REAL_RESERV);
	} else {
		assert(false);
	}
	Type *node = new Type(tok);
	return node;
}

/*!
 * fn void Interpeter::term()
 * \brief term : factor((MUL | INTEGER_DIV | FLOAT_DIV) factor)*
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
		} else if (tok.type() == T_PASC_INT_DIV_RESERV) {
			eat(T_PASC_INT_DIV_RESERV);
		}
		CLog::write(CLog::DEBUG, "Parser::term() before BinOp\n");
		node = new BinOp(node, tok, factor());
		CLog::write(CLog::DEBUG, "Parser::term() BinOp is %p\n", node);
	}
	return node;
}

void NodeVisitor::visitData(Node *node)
{
	CLog::write(CLog::DEBUG, "NodeVisitor::visit\n");
	CLog::write(CLog::DEBUG, "NodeVisitor::visitData() %p\n", node);
	if (node) {
		node->visitData();
	}
}

void NodeVisitor::visitForDetails(Node *node)
{
	node->nodeDetails(indent_);
}

double BinOp::visitData()
{
	CLog::write(CLog::DEBUG, "BinOp::visit\n");
	CLog::write(CLog::DEBUG, "BinOp: this->op_.type(): %s\n", this->getToken().representation().c_str());
	if (this->getToken().type() == T_PLUS) {
		return this->lhs_->visitData() + this->rhs_->visitData();
	} else if (this->getToken().type() == T_MINUS) {
		return this->lhs_->visitData() - this->rhs_->visitData();
	} else if (this->getToken().type() == T_MUL) {
		return this->lhs_->visitData() * this->rhs_->visitData();
	} else if (this->getToken().type() == T_DIV) {
		return this->lhs_->visitData() / this->rhs_->visitData(); 
	} else if (this->getToken().type() == T_PASC_INT_DIV_RESERV) {
		return this->lhs_->visitData() / this->rhs_->visitData();//FIXME oxi / alla diairesi pou krataei to akeraio meros !
	}
	assert(false);
	CLog::write(CLog::DEBUG, "BinOp::visit should not reach\n");
	return 0;
}

double Program::visitData()
{
	block_->visitData();
	return 0;
}

double Block::visitData()
{
	std::vector<VarDecl *>::iterator it;
	for (it = declarations_.begin(); it != declarations_.end(); it++) {
		(*it)->visitData();
	}
	compoundStatement_->visitData();
	return 0;
}

double VarDecl::visitData()
{
//	varNode_->visitData();
	return 0;
}

double Type::visitData()
{
	//do nothing
	return 0;
}

double UnaryOp::visitData()
{
	CLog::write(CLog::DEBUG, "UnaryOp::visitData(): Start!\n");
	Token tok = this->getToken();
	if (tok.type() == T_PLUS) {
		return +(this->expr_->visitData());
	} else if (tok.type() == T_MINUS) {
		return -(this->expr_->visitData());
	}
	CLog::write(CLog::DEBUG, "UnaryOp::visitData(): Should not reach!\n");
	return 0;
}

/*!
 * fn void Compound::visitData()
 * \brief  Compound visitor iterates over its children and visits each one in turn.
 * \return An T_INTEGER token value
 */
double Compound::visitData()
{
	std::vector<Node *>::iterator it;
	for (it = children_.begin(); it != children_.end(); it++) 
	{
		(*it)->visitData();
	}
	return 0;
}

std::map<std::string, double>Interpreter::GLOBAL_SCOPE;

double Assign::visitData()
{
	typedef std::map<std::string, double>KVMap;

	std::string varName = this->lhs_->getToken().value();
	//ena map pou tha pairno to key()
	std::map<std::string, double>::iterator it;
	CLog::write(CLog::DEBUG, "visitData %s\n", varName.c_str());

	it = Interpreter::GLOBAL_SCOPE.find(varName);
	if (it == Interpreter::GLOBAL_SCOPE.end()) {
		Interpreter::GLOBAL_SCOPE.insert(KVMap::value_type(varName, this->rhs_->visitData()));
	}
	return 0;
}

double Var::visitData()
{
	std::string varName = this->getToken().value();
	std::map<std::string, double>::iterator it;
	it = Interpreter::GLOBAL_SCOPE.find(varName);
	if (it == Interpreter::GLOBAL_SCOPE.end()) {
		CLog::write(CLog::RELEASE, "Var::visitData() --> No Such Variable!");
	} else {
		return it->second;
	}
	assert(false);
	return 0;
}

/*!
 * fn void NoOp::visitData()
 * \brief  NoOp visitor does nothing! 
 * \return An T_INTEGER token value
 */
double NoOp::visitData()
{
	return 0;
}

Node* Interpreter::interpret()
{
	CLog::write(CLog::DEBUG, "\n\nInterpreter::interpret()\n");
	Node *tree = parser_.parse();
	CLog::write(CLog::DEBUG, "Interpreter::interpret() 2\n");
	return tree;
}

/***********************************************/

/***********************************************/

//FIXME που να το βαλω?
std::map<std::string, Symbol *>SymbolTable::symbols_;
/***********************************************/
SymbolTable::SymbolTable()
{
	initializeBuiltins();	
}

/***********************************************/

/***********************************************/
void SymbolTable::initializeBuiltins()
{
	define(new BuiltinTypeSymbol("INTEGER"));
	define(new BuiltinTypeSymbol("REAL"));
}
/***********************************************/

void SymbolTable::representation()
{
	CLog::write(CLog::RELEASE, "Symbols: \n");
	std::map<std::string, Symbol *>::iterator it;

	for ( it = SymbolTable::symbols_.begin(); it != SymbolTable::symbols_.end(); it++) {
		CLog::write(CLog::RELEASE, "%s\n", it->second->representation());
	}

}
/***********************************************/

void SymbolTable::define(Symbol *symbol)
{
	typedef std::map<std::string, Symbol *> KVMap;

	CLog::write(CLog::RELEASE, "Define: %s\n", symbol->representation().c_str());
	SymbolTable::symbols_.insert(KVMap::value_type(symbol->name(), symbol));
}
/***********************************************/

Symbol *SymbolTable::lookup(std::string &name)
{
	CLog::write(CLog::RELEASE, "Lookup: %s\n", name.c_str());
	std::map<std::string, Symbol *>::iterator it;

	it = SymbolTable::symbols_.find(name);
	Symbol *symbol = NULL;

	if (it != SymbolTable::symbols_.end()) {
		symbol = it->second;
	}
	// symbol is either an instance of the Symbol class or 'None'
	return symbol;
}
/***********************************************/

void SymbolTable::visit(Node *node)
{
	node->visitSymbolTable();
}

void Block::visitSymbolTable()
{
	typedef std::vector<VarDecl *> VarDeclVec;

	VarDeclVec::iterator it;
	VarDeclVec declarations = getDeclarations();

	for (it = declarations.begin(); it != declarations.end(); it++) {
		(*it)->visitSymbolTable();
	}
	Compound *comp = getCompound();
	comp->visitSymbolTable(); //FIXME
}

void Program::visitSymbolTable()
{
	block()->visitSymbolTable();
}

void BinOp::visitSymbolTable()
{
	getLhs()->visitSymbolTable();
	getRhs()->visitSymbolTable();
}

void Number::visitSymbolTable()
{
	return;
}

void UnaryOp::visitSymbolTable()
{
	getExpr()->visitSymbolTable();
}

void Compound::visitSymbolTable()
{
	std::vector<Node *>::iterator it; 
	std::vector<Node *> children = getChildren();

	for ( it = children.begin(); it != children.end(); it++) {
		(*it)->visitSymbolTable();
	}	
}

void NoOp::visitSymbolTable()
{
	return;
}

void VarDecl::visitSymbolTable()
{
	std::string typeName = getTypeNode()->getValue();
	Symbol *typeSymbol = SymbolTableBuilder::symtab().lookup(typeName);
	std::string varName = getVarNode()->getValue();
	Symbol *varSymbol  = new VarSymbol(varName, typeSymbol);
	SymbolTableBuilder::symtab().define(varSymbol);
}

void Assign::visitSymbolTable()
{
	std::string varName = getLhs()->getToken().value(); 
	Symbol *varSymbol = SymbolTable::lookup(varName);
	if (!varSymbol) {
		CLog::write(CLog::RELEASE, "Assign Lookup: %s not found\n",varName.c_str());
	}
	getRhs()->visitSymbolTable();
}

void Var::visitSymbolTable()
{
	std::string varName = getValue();
	Symbol *varSymbol = SymbolTable::lookup(varName);

	if (!varSymbol) {
		CLog::write(CLog::RELEASE, "Assign Lookup: %s not found\n",varName.c_str());
	}
}

void SymbolTableBuilder::visit(Node *node)
{
	node->visitSymbolTable();
}











