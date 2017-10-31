#pragma once

/*!
 *  \file token.hpp
 *  \enum An enum type
 *  The documentation block cannot be put after the enum!
 */
enum TokenType
{
	T_INTEGER, /*!< The Token type_ is an integer            */
	T_PLUS,    /*!< The Token type_ is a plus operator +     */
	T_MINUS,   /*!< The Token type_ is a minus operator -    */
	T_DIV,     /*!< The Token type_ is a division operator - */
	T_MUL,     /*!< The Token type_ is a multi operator -    */
	T_EOF,     /*!< The Token type_ is EOF                   */
	T_LPAREN,  /*!< The Token type_ is Left Parenthesis (    */
	T_RPAREN,  /*!< The Token type_ is Right Parenthesis )   */

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
		TokenType type() { return type_; }
		std::string value() { return value_; }
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
		Lexer(std::string text) : text_(text), pos_(0), currentChar_(text_[pos_])
		{}
		void advance();
		void skipWhiteSpace();
		std::string integer();
		Token getNextToken();

	private:
		void raiseError();
		std::string text_;
		size_t pos_;
		char currentChar_;

};












