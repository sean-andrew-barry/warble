export module node.scopes;

import <cstdint>;

namespace node {
  export enum class Scopes : uint8_t {
    NONE,
    GLOBAL, // I'm undecided if there should be a global scope. It would be static and immutable if present.
    MODULE,
    LOOP,
    FOR_LOOP,
    WHILE_LOOP,
    DO_LOOP,
    FUNCTION,
    COVER,
    MATCH,
    CONDITIONAL,
    IF_CONDITIONAL,
    ELSE_CONDITIONAL,
    SIMPLE,
  };
};