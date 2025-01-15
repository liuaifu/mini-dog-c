#include "lexer.hpp"
#include <iostream>


int main(int argc, char** argv) {
	std::string input = R"(
int add(int a, int b) {
	return a + b;
}

double add(double a, double b) {
	return a + b;
}

int main() {
	int a = 100;
	int b = 200;
	println(add(a, b));

	double c1 = 123.;
	double _d1 = .456;
	println(add(c1, _d1));

	println("Hello, world!\nThis is mini-dog-c.");

	return 0;
}
)";
	Lexer lexer(input);
	while (true) {
		auto token = lexer.NextToken();
		if(token.type == TokenType::kEOF)
			break;
		std::cout << TokenTypeString(token.type) << ": " << token.literal << std::endl;
	}

	return 0;
}
