#pragma once

#include <string>
#include <unordered_map>


enum class TokenType {
	kIllegal,
	kEOF,

	// 标识符 + 字面量
	kIdent,
	kChar,
	kBool,
	kInt,
	kDouble,
	kString,

	// 操作符
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

	// 分隔符
	kComma,			// ,
	kSemicolon,		// ;
	kColon,			// :
	kLParen,		// (
	kRParen,		// )
	kLBrace,		// {
	kRBrace,		// }
	kLBracket,		// [
	kRBracket,		// ]

	// 关键词
	kFunction,		// fn
	kLet,			// let
	kIf,			// if
	kElse,			// else
	kReturn			// return
};

TokenType LookupIdent(std::string ident) {
	static std::unordered_map<std::string, TokenType> keywords = {
		{"fn", TokenType::kFunction},
		{"let", TokenType::kLet},
		{"if", TokenType::kIf},
		{"return", TokenType::kReturn},
		{"else", TokenType::kElse},
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
