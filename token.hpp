#pragma once

#include <string>
#include <stdexcept>
#include <unordered_map>


enum class TokenType {
	kIllegal,
	kEOF,		// ������
	kIdent,		// ��ʶ��
	kChar,		// �ַ�
	kNum,		// ����
	kReal,		// ������
	kString,	// �ַ���
	kBool,		// ����ֵ

	// ��������
	kCharType,
	kBoolType,
	kIntType,
	kDoubleType,
	kStringType,

	// ������
	kAssign,		// =
	kPlus,			// +
	kMinus,			// -
	kBang,			// !
	kAsterisk,		// *
	kSlash,			// /
	kLT,			// <
	kGT,			// >
	kEQ,			// ==
	kNE,			// !=

	// �ָ���
	kComma,			// ,
	kSemi,			// ;
	kColon,			// :
	kLParen,		// (
	kRParen,		// )
	kLBrace,		// {
	kRBrace,		// }
	kLBracket,		// [
	kRBracket,		// ]

	// �ؼ���
	kFor,			// for
	kWhile,			// while
	kContinue,		// continue
	kSwitch,		// switch
	kBreak,			// break
	kDefault,		// default
	kIf,			// if
	kElse,			// else
	kReturn			// return
};

std::string TokenTypeString(TokenType token_type) {
	static std::unordered_map<TokenType, std::string> token_type_list{
		{ TokenType::kIllegal, "ILLEGAL" },
		{ TokenType::kEOF, "EOF" },
		{ TokenType::kIdent, "IDENT" },
		{ TokenType::kChar, "CHAR" },
		{ TokenType::kNum, "NUM" },
		{ TokenType::kReal, "REAL" },
		{ TokenType::kString, "STRING" },
		{ TokenType::kBool, "BOOL" },

		// ��������
		{ TokenType::kCharType, "CHAR_TYPE" },
		{ TokenType::kBoolType, "BOOL_TYPE" },
		{ TokenType::kIntType, "INT_TYPE" },
		{ TokenType::kDoubleType, "DOUBLE_TYPE" },
		{ TokenType::kStringType, "STRING_TYPE" },

		// ������
		{ TokenType::kAssign, "ASSIGN" },
		{ TokenType::kPlus, "PLUS" },
		{ TokenType::kMinus, "MINUS" },
		{ TokenType::kBang, "BANG" },
		{ TokenType::kAsterisk, "ASTERISK" },
		{ TokenType::kSlash, "SLASH" },
		{ TokenType::kLT, "LT" },
		{ TokenType::kGT, "GT" },
		{ TokenType::kEQ, "EQ" },
		{ TokenType::kNE, "NE" },

		// �ָ���
		{ TokenType::kComma, "COMMA" },
		{ TokenType::kSemi, "SEMI" },
		{ TokenType::kColon, "COLON" },
		{ TokenType::kLParen, "LPAREN" },
		{ TokenType::kRParen, "RPAREN" },
		{ TokenType::kLBrace, "LBRACE" },
		{ TokenType::kRBrace, "RBRACE" },
		{ TokenType::kLBracket, "LBRACKET" },
		{ TokenType::kRBracket, "RBRACKET" },

		// �ؼ���
		{ TokenType::kFor, "FOR" },
		{ TokenType::kWhile, "WHILE" },
		{ TokenType::kContinue, "CONTINUE" },
		{ TokenType::kSwitch, "SWITCH" },
		{ TokenType::kBreak, "BREAK" },
		{ TokenType::kDefault, "DEFAULT" },
		{ TokenType::kIf, "IF" },
		{ TokenType::kElse, "ELSE" },
		{ TokenType::kReturn, "RETURN" }
	};

	auto it = token_type_list.find(token_type);
	if (it == token_type_list.end())
		throw std::runtime_error("token type error!");

	return it->second;
}

TokenType LookupIdent(std::string ident) {
	static std::unordered_map<std::string, TokenType> keywords = {
		{"char", TokenType::kCharType},
		{"bool", TokenType::kBoolType},
		{"int", TokenType::kIntType},
		{"double", TokenType::kDoubleType},
		{"string", TokenType::kStringType},

		{"for", TokenType::kIf},
		{"while", TokenType::kWhile},
		{"continue", TokenType::kContinue},
		{"switch", TokenType::kSwitch},
		{"break", TokenType::kBreak},
		{"default", TokenType::kDefault},
		{"if", TokenType::kIf},
		{"else", TokenType::kElse},
		{"return", TokenType::kReturn},
		{"true", TokenType::kBool},
		{"false", TokenType::kBool}
	};

	auto it = keywords.find(ident);
	if (it != keywords.end())
		return it->second;
	else
		return TokenType::kIdent;
}

struct Token {
	Token() {
		type = TokenType::kIllegal;
	}

	Token(TokenType type, std::string literal) {
		this->type = type;
		this->literal = literal;
	}

	TokenType type;
	std::string literal;
};
