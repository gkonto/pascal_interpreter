#include <string>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <map>

#include "interpreter.hpp"
#include "clog.hpp"

ScopedSymbolTable *SemanticAnalyzer::scope_        = NULL;
ScopedSymbolTable *SemanticAnalyzer::currentScope_ = NULL;

/*!
 * \file token.cpp
 */


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* 			PARSER methods                                */
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*!
 * fn void Parser::raiseParseError()
 * \brief Raise assertion when error occurs while parsing
 */
void Parser::raiseError(const TokenType &tokType)
{
	CLog::write(CLog::RELEASE, "Invalid syntax!!\n");
	CLog::write(CLog::RELEASE, "Expected ---> %s\n", getTokenTypeLabel(tokType).c_str());
	CLog::write(CLog::RELEASE, "Got      --->  %s\n\n", currentToken_.representation().c_str());

	assert(false);
} /* Parser::raiseError */

/**********************************************************************/
Node *Parser::parse()
{
	CLog::write(CLog::DEBUG, "parse()\n");
	Node *node = program();
	if (currentToken_.type() != T_EOF) {
		raiseError(currentToken_.type());
	}
	return node;
} /* Parser::parse */

/**********************************************************************/
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
		CLog::write(CLog::DEBUG, "%s\n", currentToken_.representation().c_str());
	} else {
		raiseError(tokType);
	}
} /* Parser::eat */

/**********************************************************************/
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
} /* Parser::expr */

/**********************************************************************/
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
} /* Parser::factor */

/**********************************************************************/
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
} /* Parser::program */

/**********************************************************************/
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
		raiseError(currentToken_.type());
	}

	CLog::write(CLog::DEBUG, "statementList() results.size() --> %d\n", results.size());
	return results;
} /* Parser::statementList */

/**********************************************************************/
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
} /* Parser::statement */

/**********************************************************************/
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
} /* Parser::assignmentStatement */

/**********************************************************************/
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
} /* Parser::variable */

/**********************************************************************/
/*!
 * fn Node *Parser::empty()
 * \brief : An empty production 
 * \return Node *
 */
NoOp *Parser::empty()
{
	return new NoOp();
} /* Parser::empty */

/**********************************************************************/
/*!
 * fn Block *Parser::block()
 * \brief : declarations compoundStatement 
 * \return Block *
 */
Block* Parser::block()
{
	CLog::write(CLog::DEBUG, "block: %s\n", currentToken_.representation().c_str()); 
	std::vector<Node *>declarationNodes = declarations();
	Compound *comp = compoundStatement();
	Block *node = new Block(declarationNodes, comp);

	CLog::write(CLog::DEBUG, "END OF BLOCK NODE Creator: %s\n", currentToken_.representation().c_str()); 
	return node;
} /* Parser::block */

/**********************************************************************/
/*!
 * fn VarDecl *Parser::declarations()
 * \brief : declarations : VAR(variableDeclaration SEMI) + | empty
 * \return Block *
 */
std::vector<Node *> Parser::declarations()
{
	std::vector<Node *> declarations;

	while (true) {
		if (currentToken_.type() == T_PASC_VAR_RESERV) {
			eat(T_PASC_VAR_RESERV);
			while (currentToken_.type() == T_PASC_ID) {
				std::vector<VarDecl *> vd = variableDeclaration();
				declarations.reserve(declarations.size() + distance(vd.begin(), vd.end()));
				declarations.insert(declarations.end(), vd.begin(), vd.end());
				eat(T_SEMI);
			}
		} else if (currentToken_.type() == T_PASC_PROCEDURE) {
			eat(T_PASC_PROCEDURE);
			std::string procName = currentToken_.value();
			eat(T_PASC_ID);
			
			std::vector<Param *> params;
			if (currentToken_.type() == T_LPAREN) {
				eat(T_LPAREN);
				params = formalParameterList();
				eat(T_RPAREN);
			}

			eat(T_SEMI);
			Block *b_node = block();
			ProcedureDecl *procDecl = new ProcedureDecl(procName, params, b_node);
			declarations.push_back(procDecl);
			eat(T_SEMI);
		} else {
			break; //FIXME is it correct?
		}
	}
	return declarations;
} /* Parser::declarations */

/**********************************************************************/
/*!
 * fn Parser::formalParameterList()
 */
std::vector<Param *> Parser::formalParameterList()
{
	std::vector<Param *> params;

	if (!(currentToken_.type() == T_PASC_ID)) {
		return params;
	}

	params = formalParameters();

	while (currentToken_.type() == T_SEMI) {
		eat(T_SEMI);
		std::vector<Param *> temp = formalParameters();
		params.insert(params.end(), temp.begin(), temp.end());
	}

	return params;
} /* Parser::formalParameterList */

/**********************************************************************/
/*!
 * fn Parser::formalParameters()
 */
std::vector<Param *> Parser::formalParameters()
{
	std::vector<Param *> params;

	std::vector<Token> paramTokens;
	paramTokens.push_back(currentToken_);
	eat(T_PASC_ID);

	while (currentToken_.type() == T_COMMA) {
		eat(T_COMMA);
		paramTokens.push_back(currentToken_);
		eat(T_PASC_ID);
	}

	eat(T_COLON);
	Type *type_n = typeSpec();

	std::vector<Token>::iterator it;
	for (it = paramTokens.begin(); it != paramTokens.end(); it++)
	{
		Param *param = new Param(new Var(*it), type_n); 
		params.push_back(param);
	}
	return params;
} /* Parser::formalParameters */

/**********************************************************************/
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
} /* Parser::variableDeclaration */

/**********************************************************************/
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
} /* Parser::typeSpec */

/**********************************************************************/
/*!
 * fn void Parser::term()
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
} /* Parser::term */


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/






/*!
 * fn void Compound::addNode(Node *node)
 * \brief : Adds a node to children_
 */
void Compound::addNode(Node *node)
{
       	children_.push_back(node);
}

/**********************************************************************/
/*fn Node *Parser::compoundStatement()
 * \brief : BEGIN statementList END
 * \return Node *
 */
Compound *Parser::compoundStatement()
{
	eat(T_PASC_BEGIN_RESERV);
	std::vector<Node *> nodes = statementList();
	eat(T_PASC_END_RESERV);

	Compound *root = new Compound();
	std::vector<Node *>::const_iterator it;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		root->addNode(*it);
	}
	return root;
}

/**********************************************************************/
/**********************************************************************/

/**********************************************************************/
void Program::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sProgram Node with name %s\n", indent_s.c_str(), this->getName().c_str());
	this->getBlock()->visitASTPresenter(ind);
} /* ASTPresenter::visit */

/**********************************************************************/
void Type::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sType Node with value: %s\n", indent_s.c_str(), this->getValue().c_str());
} /* ASTPresenter::visit */

/**********************************************************************/
void Compound::visitASTPresenter(int ind)
{
	ind += 5;
	size_t childrenNum = this->size();
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sCompound Node with %d children\n", indent_s.c_str(), childrenNum);
	
	Compound::iterator it;
	ind += 5;

	for (it = this->begin(); it != this->end(); it++) {
		(*it)->visitASTPresenter(ind);
	}

} /* ASTPresenter::visit */

/**********************************************************************/
void Var::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sVar Node value_: %s\n", indent_s.c_str(), this->getValue().c_str());
} /* ASTPresenter::visit */

/**********************************************************************/
void VarDecl::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sVarDecl Node\n", indent_s.c_str());
	this->getVarNode()->visitASTPresenter(ind);
	this->getTypeNode()->visitASTPresenter(ind);
} /* ASTPresenter::visit */

/**********************************************************************/
void Block::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sBlock Node %u declarations and a compound Node\n", indent_s.c_str(), this->size());

	Block::iterator it;
	for (it = this->begin(); it != this->end(); it++) {
		(*it)->visitASTPresenter(ind);
	}
	compoundStatement_->visitASTPresenter(ind);
} /* ASTPresenter::visit */

/**********************************************************************/
void NoOp::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sNoOp Node\n", indent_s.c_str());
} /* ASTPresenter::visit */

/**********************************************************************/
void Assign::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sAssign \n", indent_s.c_str());
	this->getLhs()->visitASTPresenter(ind);
	this->getRhs()->visitASTPresenter(ind);
} /* Assign::visitASTPresenter */
/**********************************************************************/
void BinOp::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sBinOp %s\n", indent_s.c_str(), this->getToken().representation().c_str());
	this->getLhs()->visitASTPresenter(ind);
	this->getRhs()->visitASTPresenter(ind);
} /* BinOp::visitASTPresenter */

/**********************************************************************/
void UnaryOp::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sUnaryOp %s\n", indent_s.c_str(), this->getToken().representation().c_str());
	this->getExpr()->visitASTPresenter(ind);
} /* UnaryOp::visitASTPresenter */

/**********************************************************************/
void ProcedureDecl::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sProcedure Declaration Node with a block and params\n", indent_s.c_str());

	this->getBlock()->visitASTPresenter(ind);

	ProcedureDecl::iterator it;

	for (it = this->begin(); it != this->end(); it++) {
		(*it)->visitASTPresenter(ind);
	}
} /* ProcedureDecl::visitASTPresenter */

/**********************************************************************/
void Param::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sParam Node with a Var and a Type as child\n",
			indent_s.c_str());
	this->getVar()->visitASTPresenter(ind);
	this->getType()->visitASTPresenter(ind);
} /* Param::visitASTPresenter */
/**********************************************************************/
void Number::visitASTPresenter(int ind)
{
	ind += 5;
	std::string indent_s(ind, ' ');
	CLog::write(CLog::RELEASE, "%sNumber %s\n", indent_s.c_str(), this->getToken().representation().c_str());
} /* Number::visitASTPresenter */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void ProcedureDecl::visitSemanticAnalyzer()
{
	std::string procName = this->getName();
	ProcedureSymbol *procSymbol = new ProcedureSymbol(procName);
	SemanticAnalyzer::currentScope_->define(procSymbol);

	CLog::write(CLog::RELEASE, "Enter scope: %s\n", procName);

	/* Scope for parameters and local variables */
	ScopedSymbolTable *procedureScope = new ScopedSymbolTable(procName, SemanticAnalyzer::currentScope_->getLevel() + 1, SemanticAnalyzer::currentScope_);

	SemanticAnalyzer::currentScope_ = procedureScope;

	/* Insert parameters into the procedure scope */
	ProcedureDecl::iterator it;

	for (it = this->begin(); it != this->end(); it++) {
		Symbol *paramType = SemanticAnalyzer::currentScope_->lookup((*it)->getType()->getValue());
		std::string paramName = (*it)->getVar()->getValue();
		VarSymbol *varSymbol = new VarSymbol(paramName, paramType);
		SemanticAnalyzer::currentScope_->define(varSymbol);
		procSymbol->add(varSymbol);
	}

	this->getBlock()->visitSemanticAnalyzer();
	SemanticAnalyzer::currentScope_ = SemanticAnalyzer::currentScope_->getEnclosingScope();
	CLog::write(CLog::RELEASE, "LEAVE scope: %s\n", procName);
} /* ProcedureDecl::visitSemanticAnalyzer */

/**********************************************************************/
void BinOp::visitSemanticAnalyzer()
{
	this->getLhs()->visitSemanticAnalyzer();
	this->getRhs()->visitSemanticAnalyzer();
} /* BinOp::visitSemanticAnalyzer */

/**********************************************************************/
void Block::visitSemanticAnalyzer()
{
	Block::iterator it;
	for (it = this->begin(); it != this->end(); it++) {
		(*it)->visitSemanticAnalyzer();
	}
	this->getCompound()->visitSemanticAnalyzer();
} /* Block::visitSemanticAnalyzer */

/**********************************************************************/
void Program::visitSemanticAnalyzer()
{
	CLog::write(CLog::RELEASE, "Enter scope: global");
	ScopedSymbolTable *globalScope = new ScopedSymbolTable("global", 1, NULL);

	SemanticAnalyzer::currentScope_ = globalScope;	
	this->getBlock()->visitSemanticAnalyzer();
	SemanticAnalyzer::currentScope_ = SemanticAnalyzer::currentScope_->getEnclosingScope();
	CLog::write(CLog::RELEASE, "LEAVE scope: global");
} /* Program::visitSemanticAnalyzer */

/**********************************************************************/
void Compound::visitSemanticAnalyzer()
{
	Compound::iterator it;

	for (it = this->begin(); it != this->end(); it++) {
		(*it)->visitSemanticAnalyzer();
	}
} /* Compound::visitSemanticAnalyzer */

/**********************************************************************/
void NoOp::visitSemanticAnalyzer()
{
	return;
} /* NoOp::visitSemanticAnalyzer */

/**********************************************************************/
void VarDecl::visitSemanticAnalyzer()
{
	std::string typeName = this->getTypeNode()->getValue();
	Symbol *typeSymbol = SemanticAnalyzer::currentScope_->lookup(typeName);

	std::string varName = this->getVarNode()->getValue();
	Symbol *varSymbol = new VarSymbol(varName, typeSymbol);


	if (!SemanticAnalyzer::currentScope_->lookup(varName, true)) {
		CLog::write(CLog::RELEASE, "Error: Duplicate identifier %s found", varName);
	}
	SemanticAnalyzer::currentScope_->define(varSymbol);
} /* VarDecl::visitSemanticAnalyzer */

/**********************************************************************/
void Var::visitSemanticAnalyzer()
{
	std::string varName = getValue();
	Symbol *varSymbol = SemanticAnalyzer::currentScope_->lookup(varName);
	if (!varSymbol) {
		CLog::write(CLog::RELEASE, "Error: Symbol %s not found!", varName.c_str());
	}
} /* Var::visitSemanticAnalyzer */

/**********************************************************************/
void Assign::visitSemanticAnalyzer()
{
	this->getRhs()->visitSemanticAnalyzer();
	this->getLhs()->visitSemanticAnalyzer();
} /* Assign::visitSemanticAnalyzer */

/**********************************************************************/
void Type::visitSemanticAnalyzer()
{
	return;
} /* Type::visitSemanticAnalyzer */

/**********************************************************************/
void Number::visitSemanticAnalyzer()
{
	return;
} /* Number::visitSemanticAnalyzer */

/**********************************************************************/
void UnaryOp::visitSemanticAnalyzer()
{
	return;
} /* UnaryOp::visitSemanticAnalyzer */

/**********************************************************************/
void Param::visitSemanticAnalyzer()
{
	return;
} /* Param::visitSemanticAnalyzer */

/**********************************************************************/
double Param::visitEvaluate()
{
	return 0;
} /* Param::visitEvaluate */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
double BinOp::visitEvaluate()
{
	CLog::write(CLog::DEBUG, "BinOp::visit\n");
	CLog::write(CLog::DEBUG, "BinOp: this->op_.type(): %s\n", this->getToken().representation().c_str());
	if (this->getToken().type() == T_PLUS) {
		return this->lhs_->visitEvaluate() + this->rhs_->visitEvaluate();
	} else if (this->getToken().type() == T_MINUS) {
		return this->lhs_->visitEvaluate() - this->rhs_->visitEvaluate();
	} else if (this->getToken().type() == T_MUL) {
		return this->lhs_->visitEvaluate() * this->rhs_->visitEvaluate();
	} else if (this->getToken().type() == T_DIV) {
		return this->lhs_->visitEvaluate() / this->rhs_->visitEvaluate(); 
	} else if (this->getToken().type() == T_PASC_INT_DIV_RESERV) {
		return this->lhs_->visitEvaluate() / this->rhs_->visitEvaluate();//FIXME oxi / alla diairesi pou krataei to akeraio meros !
	}
	assert(false);
	CLog::write(CLog::DEBUG, "BinOp::visit should not reach\n");
	return 0;
} /* BinOp::visitEvaluate */

/**********************************************************************/
double Program::visitEvaluate()
{
	getBlock()->visitEvaluate();
	return 0;
} /* Program::visitEvaluate */

/**********************************************************************/
double Block::visitEvaluate()
{
	std::vector<Node *>::iterator it;
	for (it = declarations_.begin(); it != declarations_.end(); it++) {
		(*it)->visitEvaluate();
	}
	compoundStatement_->visitEvaluate();
	return 0;
} /* Block::visitEvaluate */

/**********************************************************************/
double VarDecl::visitEvaluate()
{
	return 0;
} /* VarDecl::visitEvaluate */

/**********************************************************************/
double Type::visitEvaluate()
{
	//do nothing
	return 0;
} /* Type::visitEvaluate */

/**********************************************************************/
double UnaryOp::visitEvaluate()
{
	CLog::write(CLog::DEBUG, "UnaryOp::visitEvaluate(): Start!\n");
	Token tok = this->getToken();
	if (tok.type() == T_PLUS) {
		return +(this->expr_->visitEvaluate());
	} else if (tok.type() == T_MINUS) {
		return -(this->expr_->visitEvaluate());
	}
	CLog::write(CLog::DEBUG, "UnaryOp::visitEvaluate(): Should not reach!\n");
	return 0;
} /* UnaryOp::visitEvaluate */

/**********************************************************************/
/*!
 * fn void Compound::visitEvaluate()
 * \brief  Compound visitor iterates over its children and visits each one in turn.
 * \return An T_INTEGER token value
 */
double Compound::visitEvaluate()
{
	std::vector<Node *>::iterator it;
	for (it = children_.begin(); it != children_.end(); it++) 
	{
		(*it)->visitEvaluate();
	}
	return 0;
} /* Compound::visitEvaluate */

/**********************************************************************/
double Number::visitEvaluate()
{
	return value();
} /* Number::visitEvaluate */

/**********************************************************************/
double Assign::visitEvaluate()
{
	//typedef std::map<std::string, double>KVMap;

	std::string varName = this->lhs_->getToken().value();
	//ena map pou tha pairno to key()
	std::map<std::string, double>::iterator it;
	CLog::write(CLog::DEBUG, "visitEvaluate %s\n", varName.c_str());
/*
	it = Interpreter::GLOBAL_SCOPE.find(varName);
	if (it == Interpreter::GLOBAL_SCOPE.end()) {
		Interpreter::GLOBAL_SCOPE.insert(KVMap::value_type(varName, this->rhs_->visitEvaluate()));
	}
*/
	return 0;
} /* Assign::visitEvaluate */

/**********************************************************************/
double Var::visitEvaluate()
{
	std::string varName = this->getToken().value();
	std::map<std::string, double>::iterator it;
/*
	it = Interpreter::GLOBAL_SCOPE.find(varName);
	if (it == Interpreter::GLOBAL_SCOPE.end()) {
		CLog::write(CLog::RELEASE, "Var::visitEvaluate() --> No Such Variable!");
	} else {
		return it->second;
	}
	assert(false);
*/
	return 0;
} /* Var::visitEvaluate */

/**********************************************************************/
/*!
 * fn void NoOp::visitEvaluate()
 * \brief  NoOp visitor does nothing! 
 * \return An T_INTEGER token value
 */
double NoOp::visitEvaluate()
{
	return 0;
} /* NoOp::visitEvaluate */

/**********************************************************************/
double ProcedureDecl::visitEvaluate()
{
	return block_->visitEvaluate();
} /* ProcedureDecl::visitEvaluate */

/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/

Node* Interpreter::interpret()
{
	CLog::write(CLog::DEBUG, "\n\nInterpreter::interpret()\n");
	Node *tree = parser_.parse();
	CLog::write(CLog::DEBUG, "Interpreter::interpret() 2\n");
	return tree;
} /* Interpreter::interpret */

/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
void ScopedSymbolTable::representation()
{
	CLog::write(CLog::RELEASE, "Symbols: \n");
	std::map<std::string, Symbol *>::iterator it;

	for ( it = ScopedSymbolTable::symbols_.begin(); it != ScopedSymbolTable::symbols_.end(); it++) {
		CLog::write(CLog::RELEASE, "%s\n", it->second->representation());
	}

} /* ScopedSymbolTable::representation */

/***********************************************/
ScopedSymbolTable::ScopedSymbolTable(std::string name, int level, ScopedSymbolTable *enclosingScope)
{
	name_  = name;
	level_ = level;
	enclosingScope_ = enclosingScope;
	initBuiltins();
} /* ScopedSymbolTable::ScopedSymbolTable */

/***********************************************/
void ScopedSymbolTable::initBuiltins()
{
	define(new BuiltinTypeSymbol("INTEGER"));
	define(new BuiltinTypeSymbol("REAL"));
} /* ScopedSymbolTable::initBuiltins */

/***********************************************/
void ScopedSymbolTable::define(Symbol *symbol)
{
	CLog::write(CLog::RELEASE, "Insert %s\n", symbol->representation().c_str());
	typedef std::map<std::string, Symbol *>KVMap;
	ScopedSymbolTable::symbols_.insert(KVMap::value_type(symbol->name(), symbol));
} /* ScopedSymbolTable::define */

/***********************************************/
Symbol *ScopedSymbolTable::lookup(const std::string &name, bool currentScopeOnly)
{
	CLog::write(CLog::RELEASE, "Lookup: %s\n", name.c_str());
	std::map<std::string, Symbol *>::iterator it;

	it = ScopedSymbolTable::symbols_.find(name);

	if (it != ScopedSymbolTable::symbols_.end()) {
		return it->second;
	}

	if (currentScopeOnly) {
		return NULL;
	}

	/* recursively go up the chain and lookup the name */
	if (getEnclosingScope()) {
		return getEnclosingScope()->lookup(name);
	}

	return NULL;
} /* ScopedSymbolTable::lookup */

/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
