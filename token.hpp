#pragma once

/*!
 *  \file token.hpp
 *  \enum An enum type
 *  The documentation block cannot be put after the enum!
 */
enum TokenType
{
	T_INTEGER, /*!< The Token type_ is an integer */
	T_PLUS,    /*!< The Token type_ is a plus operator + */
	T_MINUS,   /*!< The Token type_ is a minus operator - */
	T_EOF,     /*!< The Token type_ is EOF */

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

	private:
		std::string getTokenTypeLabel();
		void raiseLabelError();

		TokenType type_;
		std::string value_;
};

