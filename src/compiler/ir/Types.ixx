export module ir.symbols;

import <cstdint>;

namespace ir {
  export enum class Types : uint8_t {
    U8,
    U16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    F8, // Manually expanded/rounded to/from f32 on load/store
    F16, // f32 in registers, but often has hardware support
    F32,
    F64,
    Byte, // Similar to u8 but has no builtin behavior
    Bool,
    Char,
    Reference, // 64 bit address, cannot be reassigned
    Void, // The zero size type of the keyword literal `null`
    Symbol, // A special singleton that is seen when getting the symbol of a symbol
    Array,
    String, // Array of UTF-32 characters
    Enum, // Array of 32 bit symbol indexes
    Tuple,
    Template, // Fancy string based version of a tuple
    Object,
    Variant,
    Range,
    Block,
    Function,
    Module,
  };
};