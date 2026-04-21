import Compiler;
import compiler.utility.Print;
import compiler.utility.Hash128;
import compiler.utility.BLAKE3;
import compiler.utility.Profiler;
import compiler.utility.Time;
import compiler.ir.Token;
import compiler.fs.File;
import compiler.input.Lexer;
import compiler.input.Parser;
import compiler.text.Builder;

import <vector>;
import <atomic>;
import <string>;
import <thread>;
import <filesystem>;
import <string_view>;
import <chrono>;
import <array>;
import <span>;
import <cstddef>;
import <cstdint>;
import <algorithm>;
import <random>;

// Temporary just for fast experiments without running the whole pipeline
void Test() {
	// compiler::fs::File file{"E:/Users/Sean/Source/Repos/warble/local/tests/main.wbl"};
	compiler::fs::File file{"E:/Users/Sean/Source/Repos/warble/local/tests/fail.wbl"};

	compiler::utility::Print("Starting compiler engine loop... ", std::this_thread::get_id());

	std::string source = file.ReadToString();
	// std::string copy = source; // Make a copy for appending
	// for (size_t i = 0; i < 100000; ++i) {
	// 	source += '\n';
	// 	source += copy; // Add some extra text for testing
	// }

	auto start = std::chrono::high_resolution_clock::now();
	compiler::input::Lexer lexer{std::move(source)};
	bool result = lexer.StatementList();
	auto end = std::chrono::high_resolution_clock::now();
	
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	double seconds = duration.count() / 1000000.0;
	double chars_per_second = lexer.Source().size() / seconds;

	compiler::utility::Print(result ? "Lexing succeeded: " : "Lexing failed: ", lexer.Tokens().size(), " tokens from ", lexer.Source().size(), " characters, producing ", lexer.Errors().size(), " errors and ", lexer.Data().size(), " data points. ", chars_per_second, " chars/second in ", seconds, " seconds.");

	compiler::utility::Print(lexer);

	if (lexer.Errors().size() == 0 && true) {
		auto tokens = std::move(lexer).Tokens();
		auto data = std::move(lexer).Data();
		compiler::input::Parser parser{std::move(tokens), std::move(data)};
		(void)parser.Parse();

		compiler::utility::Print("Parsing completed: ", parser.Symbols().Count(), " symbols and ", parser.Instructions().size(), " instructions.");

		compiler::utility::Print(parser);
	}
}

void TestProfiler() {
	compiler::utility::Profiler profiler{};

	profiler.Add("RDTSC", []() {
		auto time = compiler::utility::Cycles::Now();
		compiler::utility::Profiler::DoNotOptimize(time);
	});

	profiler.Add("System", []() {
		auto time = compiler::utility::System::Now();
		compiler::utility::Profiler::DoNotOptimize(time);
	});

	profiler.Add("Steady", []() {
		auto time = compiler::utility::Steady::Now();
		compiler::utility::Profiler::DoNotOptimize(time);
	});

	profiler.Add("Precise", []() {
		auto time = compiler::utility::Precise::Now();
		compiler::utility::Profiler::DoNotOptimize(time);
	});

	std::atomic<size_t> counter = 0;
	profiler.Add("Atomic Increment", [&counter]() {
		counter.fetch_add(1, std::memory_order_relaxed);
	});

	profiler.Run();
}

int main(int argc, char* argv[]) {
	std::vector<std::string> commands{};
	commands.assign(argv + 1, argv + argc);

	if (commands.size() < 2) {
		// Just for testing purposes, use hardcoded paths if none are provided
		commands.push_back("E:/Users/Sean/Source/Repos/warble/local/tests"); // Package path
		commands.push_back("main.wbl"); // Entry path
	}

	// Test();

	// // Create the compiler; it owns the engine and manages programs.
	// Compiler compiler{std::move(commands)};

	// // Enter the engine loop via the compiler; returns on exit.
	// compiler.Run();
	// if (!ValidateBLAKE3AgainstOfficialKeyedVectors()) {
	// 	return 1;
	// }
	// CompareBLAKE3AgainstHash128Performance();

	// TestProfiler();

	return 0;
}