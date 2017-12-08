#pragma once

/*!
 *  \file token.hpp
 *  \enum An enum type
 *  The documentation block cannot be put after the enum!
 */
enum TokenType
{
	T_INTEGER,	/*!< The Token type_ is an integer            */
	T_REAL, 	/*!< The Token type_ is a  real            */
	T_PLUS,		/*!< The Token type_ is a plus operator +     */
	T_MINUS,	/*!< The Token type_ is a minus operator -    */
	T_DIV,		/*!< The Token type_ is a division operator - */
	T_MUL,		/*!< The Token type_ is a multi operator -    */
	T_EOF,		/*!< The Token type_ is EOF                   */
	T_LPAREN,	/*!< The Token type_ is Left Parenthesis (    */
	T_RPAREN,	/*!< The Token type_ is Right Parenthesis )   */
	T_COLON,
	T_COMMA,
	T_SEMI,         /*!< The Token type_ is SEMI ;     */

	T_PASC_BEGIN_RESERV,   /*!< The Token type_ is BEGIN    */
	T_PASC_END_RESERV,     /*!< The Token type_ is END     */
	T_PASC_ASSIGN,         /*!< The Token type_ is ASSIGMENT :=     */
	T_PASC_DOT_RESERV,     /*!< The Token type_ is DOT  .     */
	T_PASC_REAL_RESERV,
	T_PASC_INTEGER_RESERV,
	T_PASC_INT_DIV_RESERV,
	T_PASC_VAR_RESERV,
	T_PASC_PROGRAM_RESERV,
	T_PASC_ID,
	T_PASC_PROCEDURE,

	T_MAX
};

class Token
{
	public:
		///Create a Token with default values.
		Token() : type_(T_MAX), value_("") {}
		///Create a Token with given values.
		Token(TokenType type, std::string value) : type_(type), value_(value){}

		std::string representation();
		std::string value() { return value_; }

		TokenType type() { return type_; }

		bool isOperator();
		bool isOperatorFirstPrecedence();
		bool isOperatorSecondPrecedence();
	private:
		std::string getTokenTypeLabel();
		void raiseLabelError();

		TokenType type_;
		std::string value_;
};


class Lexer
{
	public:
		Lexer(std::string text) : text_(text), pos_(0), currentChar_(text_[pos_]) {}

		void advance();
		void skipWhiteSpace();
		char peek();
		Token _id();
		Token getNextToken();
		Token number();
		void skipComment();
	private:
		void raiseError();
		Token _getReservedKeyword(const std::string &result);

		std::string text_;
		size_t pos_;
		char currentChar_;

};



