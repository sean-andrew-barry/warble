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
    Integer, // Value is a `uint64_t` or `std::pair<uint32_t, uint32_t>` depending on flags
    Float, // Value is a `std::pair<int16_t, uint32_t>` or `std::pair<uint32_t, uint32_t>` depending on flags
    Identifier, // Value is a `std::pair<uint32_t, uint32_t>` of the length and `characters` start index
    Reference,
    Array,
    String, // Value is a `std::pair<uint32_t, uint32_t>` of the length and `characters` start index
    Enum, // Value is a `std::pair<uint32_t, uint32_t>` of the length and `enums` start index
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
    Module, // Value is a `compiler::program::Module*` during compilation
    Label,
    Declaration,
    // DestructuredDeclaration,
    // PropertyDeclaration,
    // PropertyEnumDeclaration,
    // PropertySpreadDeclaration,
    // CaptureDeclaration,
    // ParameterDeclaration,
    // ParameterPackDeclaration,

    // Statements
    Do,
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
