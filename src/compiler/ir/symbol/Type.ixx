export module compiler.ir.symbol.Type;

import <cstdint>;

namespace compiler::ir::symbol {
  export enum class Type : uint8_t {
    Undefined,
    Auto,
    Void,
    Boolean, // Value is a `bool`
    Raw,
    Character, // Value is a `char32_t`
    Symbol, // Value is a `std::pair<uint32_t, uint32_t>` of the module and symbol indexes
    Decimal, // Value is a `double`
    Integer, // Value is a `uint64_t` or `int64_t` depending on the flags
    Reference,
    Array,
    String, // Value is a `std::pair<uint32_t, uint32_t>` of the length and `characters` start index
    Enum, // Value is a `std::pair<uint32_t, uint32_t>` of the length and `enums` start index
    Tuple,
    Template,
    Object,
    Variant,
    Range,
    Phi,
    Function,
    Module, // Value is a `ir::Module*` during compilation

    // CFG blocks
    Do,
    If,
    ElseIf,
    Else,
    Match,
    For,
    Repeat,
    While,
    RepeatWhile,
  };
}
