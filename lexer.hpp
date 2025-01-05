#pragma once

#include <string>
#include "token.hpp"


class Lexer {
public:
	Lexer(const std::string input)
		: input_(input)
		, pos_(0)
		, next_pos_(0)
		, ch_(' ') {
	}

	void SkipWhitespace() {
		while (ch_ == ' ' || ch_ == '\t' || ch_ == '\n' || ch_ == '\r')
			ReadChar();
	}

	void ReadChar() {
		ch_ = next_pos_ >= input_.size() ? 0 : input_[next_pos_];
		pos_ = next_pos_++;
	}

	char PeekChar() {
		return next_pos_ >= input_.size() ? 0 : input_[next_pos_];
	}

	std::string ReadIdentifier() {
		size_t old_pos = pos_;
		while (std::isalpha(ch_) || std::isdigit(ch_) || ch_ == '_')
			ReadChar();

		return input_.substr(old_pos, pos_ - old_pos);
	}

	std::string ReadString() {
		size_t old_pos = pos_ + 1;
		while (true) {
			ReadChar();
			if (ch_ == '"' || ch_ == 0)
				break;
		}

		return input_.substr(old_pos, pos_ - old_pos);
	}

	std::string ReadNumber() {
		size_t old_pos_ = pos_;
		if (old_pos_ == '.') {
			while (std::isdigit(ch_))
				ReadChar();
		}
		else {
			int dot_count = 0;
			while (std::isdigit(ch_) || (ch_ == '.' && (dot_count++) == 0))
				ReadChar();
		}

		return input_.substr(old_pos_, pos_ - old_pos_);
	}

	Token NextToken()
	{
		SkipWhitespace();

		Token tok;
		switch (ch_) {
		case '=':
		{
			if (PeekChar() == '=') {
				ReadChar();
				std::string literal("==");
				tok.type = TokenType::kEQ;
				tok.literal = literal;
			} else {
				tok = Token(TokenType::kAssign, std::string(1, ch_));
			}
			break;
		}
		case '+':
		{
			tok = Token(TokenType::kPlus, std::string(1, ch_));
			break;
		}
		case '-':
		{
			tok = Token(TokenType::kMinus, std::string(1, ch_));
			break;
		}
		case '!':
		{
			if (PeekChar() == '=')
			{
				ReadChar();
				tok.type = TokenType::kNE;
				tok.literal = "!=";
			}
			else
			{
				tok = Token(TokenType::kBang, std::string(1, ch_));
			}
			break;
		}
		case '/':
			tok = Token(TokenType::kSlash, std::string(1, ch_));
			break;
		case '*':
			tok = Token(TokenType::kAsterisk, std::string(1, ch_));
			break;
		case '<':
			tok = Token(TokenType::kLT, std::string(1, ch_));
			break;
		case '>':
			tok = Token(TokenType::kGT, std::string(1, ch_));
			break;
		case ';':
			tok = Token(TokenType::kSemicolon, std::string(1, ch_));
			break;
		case ',':
			tok = Token(TokenType::kComma, std::string(1, ch_));
			break;
		case '{':
			tok = Token(TokenType::kLBrace, std::string(1, ch_));
			break;
		case '}':
			tok = Token(TokenType::kRBrace, std::string(1, ch_));
			break;
		case '(':
			tok = Token(TokenType::kLParen, std::string(1, ch_));
			break;
		case ')':
			tok = Token(TokenType::kRParen, std::string(1, ch_));
			break;
		case '[':
			tok = Token(TokenType::kLBracket, std::string(1, ch_));
			break;
		case ']':
			tok = Token(TokenType::kRBracket, std::string(1, ch_));
			break;
		case ':':
			tok = Token(TokenType::kColon, std::string(1, ch_));
			break;
		case '"':
		{
			tok.type = TokenType::kString;
			tok.literal = ReadString();
			break;
		}
		case '.':
		{
			tok.type = TokenType::kDouble;
			tok.literal = ReadNumber();
			break;
		}
		case 0:
		{
			tok.literal = "";
			tok.type = TokenType::kEOF;
			break;
		}
		default:
		{
			if (std::isalpha(ch_) || ch_ == '_')
			{
				tok.literal = ReadIdentifier();
				tok.type = LookupIdent(tok.literal);
				return tok;
			}
			else if (std::isdigit(ch_))
			{
				tok.literal = ReadNumber();
				tok.type = tok.literal.find('.') == std::string::npos ? TokenType::kInt : TokenType::kDouble;
				return tok;
			}
			else
			{
				tok = Token(TokenType::kIllegal, std::string(1, ch_));
			}
		}
		}

		ReadChar();
		return tok;
	}

private:
	std::string input_;		// 源码
	size_t pos_;			// 当前位置
	size_t next_pos_;		// 下一位置
	char ch_;				// 当前字符
};
