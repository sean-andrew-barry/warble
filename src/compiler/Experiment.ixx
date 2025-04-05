export module experiment;

// The purpose of this file is just to have something
// with as little dependency on the rest of the project as possible.
// The hope is this will allow testing without the brutal compile time

import utility.print;

import <bit>;
import <vector>;
import <cstdint>;
import <string>;
import <optional>;
import <string_view>;
import <concepts>;

struct Tag {
  uint8_t value;
  bool valid;

  constexpr explicit Tag() : value{0}, valid{false} {}
  constexpr explicit Tag(uint8_t v) : value{v}, valid{true} {}
  constexpr explicit operator bool() const { return valid; }
};

struct Mod : public Tag { using Tag::Tag; };
struct Op : public Tag { using Tag::Tag; };
struct Esc : public Tag { using Tag::Tag; };
struct Alt : public Tag { using Tag::Tag; };

// struct Alt {
//   uint8_t value;
//   bool valid;

//   constexpr explicit Alt() : value{0}, valid{false} {}
//   constexpr explicit Alt(uint8_t v) : value{v}, valid{true} {}
//   constexpr explicit operator bool() const { return valid; }
// };

struct OpCode {
  uint8_t primary;
  uint8_t alternate;
  bool has_alternate;

  constexpr OpCode(uint8_t v) : primary{v}, alternate{0x00}, has_alternate{false} {}
  constexpr OpCode(uint8_t p, uint8_t a) : primary{p}, alternate{a}, has_alternate{true} {}
};

template<auto op, Alt alt = {}, Mod mod = {}>
void Encode() {
  utility::Print(op, op.value, op.valid);
  utility::Print(alt, alt.value, alt.valid);
  utility::Print(mod, mod.value, mod.valid);

  if constexpr (alt) {
    utility::Print("Alt is valid");
  }
}

// template<Op op>
// void Encode() { return Encode<op, Alt{}>(); }

auto test1 = [](){
  // Encode<Op{0x11}, Alt{1}>();
  // Encode<Esc{0xFF}>();
  // Encode<OpCode{0x11, 0xBF}>();
  // Encode<42>();
  // constexpr uint8_t id = 18; 
  // utility::Print(typeid(0xB8_u8 + (id & 7_u8)));
  // // std::byte mask{0b11101000};
  // std::byte mask{0b11101000};
  // // 30;
  // // 31;
  // // 8;

  // for (int i{0}; i <= 255; ++i) {
  //   utility::Print(i, (static_cast<std::byte>(i) & mask) == std::byte{0b00001000});
  //   // utility::Print(i, (static_cast<std::byte>(i) & mask) == std::byte{0});
  // }

  // std::vector<std::byte> bytes{};
  // bytes.emplace_back(std::byte{0b11111111});
  // uint8_t shift = 3;
  // size_t index = 0;
  // std::byte byte{0b00000011};
  // std::byte mask{static_cast<std::byte>((1 << shift) - 1)};
  // bytes[index] = (bytes[index] & ~mask) | (byte & mask);

  // utility::Print(bytes);
  return 0;
}();