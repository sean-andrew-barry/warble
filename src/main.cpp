import Compiler;
import compiler.utility.Print;
import compiler.ir.Token;
import compiler.fs.File;
import compiler.input.Lexer;
import compiler.text.Builder;

import <vector>;
import <string>;
import <thread>;
import <filesystem>;

// Temporary just for fast experiments without running the whole pipeline
void Test() {
	compiler::fs::File file{std::filesystem::path{"E:/Users/Sean/Source/Repos/warble/local/tests/main.wbl"}};

	compiler::utility::Print("Starting compiler engine loop... ", std::this_thread::get_id());

	compiler::input::Lexer lexer{file.ReadToString()};
	bool result = lexer.StatementList();

	compiler::utility::Print(result ? "Lexing succeeded: " : "Lexing failed: ", lexer.Tokens().size(), " tokens and ", lexer.Errors().size(), " errors and ", lexer.Characters().size(), " characters.");

	compiler::text::Builder builder{};
	builder.Append(lexer);

	// compiler::utility::Print(lexer);
	compiler::utility::Print(builder.ToString());

	// compiler::utility::Print(lexer.Characters());

	// auto tokens = std::move(lexer).Tokens();
	// compiler::utility::Print(tokens);
	// compiler::utility::Print("Errors:", std::move(lexer).Errors());
	// // for (auto t : tokens) {
	// // 	compiler::utility::Print(t);
	// // }

	// auto characters = std::move(lexer).Characters();
	// auto limbs = std::move(lexer).Limbs();
	// // compiler::utility::Print(characters);
	// for (auto cp : characters) {
	// 	compiler::utility::Write(cp);
	// }
	// compiler::utility::Print("\nlimbs:", limbs.size());
}

int main(int argc, char* argv[]) {
	std::vector<std::string> commands{};
	commands.assign(argv + 1, argv + argc);

	if (commands.size() < 2) {
		// Just for testing purposes, use hardcoded paths if none are provided
		commands.push_back("E:/Users/Sean/Source/Repos/warble/local/tests"); // Package path
		commands.push_back("main.wbl"); // Entry path
	}

	Test();

	// // Create the compiler; it owns the engine and manages programs.
	// Compiler compiler{std::move(commands)};

	// // Enter the engine loop via the compiler; returns on exit.
	// compiler.Run();
	return 0;
}