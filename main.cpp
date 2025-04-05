import engine;
import node.compiler;
import node.package;
import node.scope.context._module;
// import Print;
// import OS;
// import Token;
import lexical.source.span;
// import code;
// import engine;
import utility.print;
import utility.to_string;
// import utility.to_bytes;
import utility.profiler;
// import visitor.formatter;

import code.x64.encoder;
// import code.x64.decoder;
// import code.x64.instruction;
import code.x64._register;

import <string>;
import <iostream>;
import <vector>;
// import <span>;
import <array>;
import <utility>;
import <string_view>;
import <typeinfo>;
import <unordered_map>;
import <stdexcept>;
import <ranges>;
import <initializer_list>;
// import <iostream>;
import <sstream>;
import <type_traits>;
import <tuple>;
import <concepts>;
import <ranges>;
import <cstddef>;
import <cstdint>;
import <bit>;
import <typeinfo>;
import <functional>;
import <optional>;
import <bitset>;

void Profile() {
  // utility::Profiler profiler{};
  // uint32_t u16 = 12345;
  // uint32_t u32 = 1234567890;
  // uint64_t u64 = 1234567890;
  // float f32 = 1234567.0;
  // double f64 = 1234567890.0;

  // code::x64::Encoder encoder1{};
  // profiler.Add("ManualEmit16", [&]{
  //   encoder1.ManualEmit16(u16);
  // });

  // code::x64::Encoder encoder2{};
  // profiler.Add("Emit", [&]{
  //   encoder2.Emit(u16);
  // });

  // profiler.Run();
}

// template<typename T>
// concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

// namespace utility {
//   template<TriviallyCopyable T, size_t S = sizeof(T)>
//   constexpr auto ToBytes(T& value) {
//     return std::span<std::byte, S>(reinterpret_cast<std::byte*>(&value), S);
//   }

//   template<TriviallyCopyable T, size_t S = sizeof(T)>
//   constexpr auto ToBytes(const T& value) {
//     return std::span<const std::byte, S>(reinterpret_cast<const std::byte*>(&value), S);
//   }

//   template<TriviallyCopyable T, size_t S = sizeof(T)>
//   constexpr auto ToBytesArray(const T& value) {
//     std::array<std::byte, S> array = {};
//     std::memcpy(array.data(), &value, std::min(sizeof(T), S));
//     return array;
//   }

//   template<TriviallyCopyable T>
//   constexpr auto ToBytes1(const T& value) { return ToBytesArray<T, 1>(value); }

//   template<TriviallyCopyable T>
//   constexpr auto ToBytes2(const T& value) { return ToBytesArray<T, 2>(value); }

//   template<TriviallyCopyable T>
//   constexpr auto ToBytes4(const T& value) { return ToBytesArray<T, 4>(value); }

//   template<TriviallyCopyable T>
//   constexpr auto ToBytes8(const T& value) { return ToBytesArray<T, 8>(value); }
// };

// template<size_t S>
// struct Data {
//   std::array<std::byte, S> array{};

//   Data(bool v) { reinterpret_cast<bool&>(*array.data()) = v; }
//   Data(int8_t v) { reinterpret_cast<int8_t&>(*array.data()) = v; }
//   Data(uint8_t v) { reinterpret_cast<uint8_t&>(*array.data()) = v; }
//   Data(int16_t v) { reinterpret_cast<int16_t&>(*array.data()) = v; }
//   Data(uint16_t v) { reinterpret_cast<uint16_t&>(*array.data()) = v; }
//   Data(int32_t v) { reinterpret_cast<int32_t&>(*array.data()) = v; }
//   Data(uint32_t v) { reinterpret_cast<uint32_t&>(*array.data()) = v; }
//   Data(float v) { reinterpret_cast<float&>(*array.data()) = v; }
//   Data(int64_t v) { reinterpret_cast<int64_t&>(*array.data()) = v; }
//   Data(uint64_t v) { reinterpret_cast<uint64_t&>(*array.data()) = v; }
//   Data(double v) { reinterpret_cast<double&>(*array.data()) = v; }

//   std::string ToString() const { return utility::ToString(array); }
// };

// bool Test() {
//   double d{1.5};
//   // auto result_d = utility::ToBytes(d);
//   // utility::Print(result_d, utility::ToDouble(result_d), result_d.extent == std::dynamic_extent);
//   // // utility::Print(result_d, d);

//   // std::string s{"Hello world"};
//   // auto result_s = utility::ToBytes(s);
//   // utility::Print(result_s, result_s.extent == std::dynamic_extent);

//   uint8_t value8 = 0xFF;
//   // auto span8 = utility::ToBytes8(value8);
//   // utility::Print(span8);

//   uint64_t value64 = 0x11FF;
//   // auto span1 = utility::ToBytes1(value64);
//   // utility::Print(span1);

//   utility::Print(Data<8>{value8}, utility::ToBytes(value8));
//   utility::Print(Data<1>{value64}, utility::ToBytes(value64));
//   utility::Print(Data<8>{d}, utility::ToBytes(d));

//   return false;
// }

// auto result = Test();

// struct PseudoAddress {
//   std::bitset<64> flag;
//   void* source;
//   void* target;
//   int32_t displacement{0};
//   uint16_t offset{0};
//   uint8_t shift{0};
//   uint8_t scale{1};
//   // uint8_t test{0}; // If this is included the sizeof jumps from 32 bytes to 40 bytes
// };

int main(int argc, char* argv[]) {
	std::vector<std::string> commands{};
	commands.assign(argv + 1, argv + argc);

  // std::vector<std::byte> bytes{};
  // bytes.emplace_back(0xFF);
  // uint8_t shift = 5;
  // size_t index = 0;
  // std::byte byte{0xAA};
  // // bytes[index] = bytes[index] | (static_cast<uint8_t>(byte) >> shift);
  // bytes[index] = bytes[index] | byte;

  // utility::Print(bytes);
  // return 0;

  // Test();
  // return 0;

  try {
    Engine engine{std::move(commands)};

    // Set up the standard library package, which serves as the root for all packages
    node::Package std{engine, nullptr, "library", "std", {}};
    node::Package::STD = &std; // Set the global access to STD

    // // Import the standard library's modules, which will ensure all workers are busy
    // std.Import("/bool.wbl", "");
    // std.Import("/uint64.wbl", "");
    // std.Import("/uint32.wbl", "");
    // std.Import("/uint16.wbl", "");
    // std.Import("/uint8.wbl", "");
    // std.Import("/uint.wbl", "");
    // std.Import("/int64.wbl", "");
    // std.Import("/int32.wbl", "");
    // std.Import("/int16.wbl", "");
    // std.Import("/int8.wbl", "");
    // std.Import("/int.wbl", "");

    // Register the user package, which is the main program
    node::Package& user = std.Register("tests", "tests", {{"*", "std"}});
    
    // Import the entry point module
    node::scope::context::Module& mod = user.Import("/test1.clear", "");

    bool result = mod.Parse();
    utility::Print("Parsing result was", result);
    utility::Print(mod.ToString());
    // visitor::Formatter formatter{};
    // mod->Accept(formatter);
    // utility::Print(formatter.ToString());

    if (result) {
      utility::Print("Beginning compilation");
      // code::x64::Encoder encoder{};
      // // mod->Compile(encoder);

      // encoder.Return();
      // encoder.Print();
      // // encoder.Execute();
    }
  }
  catch (const std::exception& e) {
    utility::Print("Compilation failed with error: ", e.what());
    return -1;
  }
  catch (...) {
    utility::Print("Compilation failed due to an unknown error.");
    return -2; // Unknown error
  }

  return 0;
}