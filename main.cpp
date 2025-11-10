import Compiler;

import <string>;
import <iostream>;
import <vector>;

int main(int argc, char* argv[]) {
	std::vector<std::string> commands{};
	commands.assign(argv + 1, argv + argc);

	// Create the compiler; it owns the engine and manages programs.
	Compiler compiler{std::move(commands)};

	// Enter the engine loop via the compiler; returns on exit.
	compiler.Run();
	return 0;
}