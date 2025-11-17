export module compiler.ir.symbol.Kind;

import <cstdint>;

namespace compiler::ir::symbol {
  export enum class Kind : uint8_t {
    Undefined,
    Auto,
    Void,
    Boolean, // Payload is a `bool`
    Raw,
    Character, // Payload is a `char32_t`
    Symbol, // Payload is a `std::pair<uint32_t, uint32_t>` of the module and symbol indexes
    Integer, // Payload is a `uint64_t` or `std::pair<uint32_t, uint32_t>` depending on flags
    Float, // Payload is a `std::pair<int16_t, uint32_t>` or `std::pair<uint32_t, uint32_t>` depending on flags
    Identifier, // Payload is a `std::pair<uint32_t, uint32_t>` of the length and `characters` start index
    Reference,
    Array,
    String, // Payload is a `std::pair<uint32_t, uint32_t>` of the length and `characters` start index
    Enum, // Payload is a `std::pair<uint32_t, uint32_t>` of the length and `enums` start index
    Tuple,
    TemplateString,
    Object,
    Variant,
    Promise,
    Expectation,
    Optional,
    Range,
    Phi,
    Function,
    // GeneratorFunction,
    // AsyncFunction,
    // AsyncGeneratorFunction,
    Module, // Payload is a `compiler::program::Module*` during compilation
    Label,
    Declaration, // Payload is a `std::pair<uint32_t, uint32_t>` of the name and type symbols
    // DestructuredDeclaration,
    // PropertyDeclaration,
    // PropertyEnumDeclaration,
    // PropertySpreadDeclaration,
    // CaptureDeclaration,
    // ParameterDeclaration,
    // ParameterPackDeclaration,

    // Statements
    Do,
    Error,
    If,
    ElseIf,
    Else,
    When,
    For,
    Repeat,
    While,
    RepeatWhile,
    Is,
    Has,
    From,
    Default,
    Register,
    Import,
  };
}
