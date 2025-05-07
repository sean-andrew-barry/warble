export module ir.symbols;

import <cstdint>;

namespace ir {
  export enum class Types : uint8_t {
    U8,
    I8,
    U16,
    I16,
    U32,
    I32,
    F32,
    U64,
    I64,
    F64,
    BOOL,
    ARRAY,
    STRING, // Array of UTF-32 characters
    ENUM, // Array of 32 bit symbol indexes
    TUPLE,
    OBJECT,
    RANGE,
  };
};