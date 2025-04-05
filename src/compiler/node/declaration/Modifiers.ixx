export module node.declaration.modifiers;

import <cstdint>;

namespace node::declaration {
  export enum class Modifiers : uint8_t {
    NONE,
    LET,
    CONST,
    MUTABLE,
    STATIC,
    PRIVATE,
    PUBLIC,
    PROTECTED,
    EXPORT,
    COMTIME,
    RUNTIME,
  };
};