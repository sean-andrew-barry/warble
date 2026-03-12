export module compiler.ir.symbol.Kind;

import <cstdint>;
import <string_view>;

namespace compiler::ir::symbol {
  export enum class Kind : uint8_t {
    Undefined,
    Unresolved,
    Auto,
    Null,
    Readonly,
    Boolean, // Payload is a `bool`
    Character, // Payload is a `char32_t`
    Integer, // Payload is a `uint64_t` or `std::pair<uint32_t, uint32_t>` depending on flags
    Float, // Payload is a `std::pair<int16_t, uint32_t>` or `std::pair<uint32_t, uint32_t>` depending on flags
    Symbol, // Payload is a `std::pair<uint32_t, uint32_t>` of the dependency module and symbol indexes
    Reference, // Payload is a `std::pair<uint32_t, uint32_t>` of the dependency module and symbol indexes
    Identifier, // Payload is a `std::pair<uint32_t, uint32_t>` of the data start index and length. TODO: Remove this? It's basically just a String.
    String, // Payload is a `std::pair<uint32_t, uint32_t>` of the data start index and length
    Enum, // Payload is a `std::pair<uint32_t, uint32_t>` of the data start index and length
    Union, // Payload is a `std::tuple<uint32_t, uint32_t, uint16_t, uint16_t>` of the data start index, length, and fail count
    Intersection, // Payload is a `std::pair<uint32_t, uint32_t>` of the true type and source conjunction
    Conjunction, // Payload is a `std::pair<uint32_t, uint32_t>` of the LHS and RHS of the conjunction
    Disjunction, // Payload is a `std::pair<uint32_t, uint32_t>` of the LHS and RHS of the disjunction
    Negation, // Payload is a `uint32_t` of the type being negated
    Array, // Payload is a `std::pair<uint32_t, uint32_t>` of the symbol start index and length
    Tuple,
    TemplateString,
    Object,
    Range,
    Phi,
    Function, // Is the payload a slice of the instructions? But what about the end marker?
    Module, // Payload is a `compiler::program::Module*` during compilation
    Label, // Payload is a `std::pair<uint32_t, uint32_t>` of the instruction index and length

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

  export constexpr std::string_view ToString(Kind kind) {
    switch (kind) {
      case Kind::Undefined: return "Undefined";
      case Kind::Unresolved: return "Unresolved";
      case Kind::Auto: return "Auto";
      case Kind::Null: return "Null";
      case Kind::Readonly: return "Readonly";
      case Kind::Boolean: return "Boolean";
      case Kind::Character: return "Character";
      case Kind::Integer: return "Integer";
      case Kind::Float: return "Float";
      case Kind::Identifier: return "Identifier";
      case Kind::Symbol: return "Symbol";
      case Kind::Reference: return "Reference";
      case Kind::Intersection: return "Intersection";
      case Kind::Conjunction: return "Conjunction";
      case Kind::Disjunction: return "Disjunction";
      case Kind::Negation: return "Negation";
      case Kind::Array: return "Array";
      case Kind::String: return "String";
      case Kind::Enum: return "Enum";
      case Kind::Tuple: return "Tuple";
      case Kind::TemplateString: return "TemplateString";
      case Kind::Object: return "Object";
      case Kind::Range: return "Range";
      case Kind::Phi: return "Phi";
      case Kind::Function: return "Function";
      case Kind::Module: return "Module";
      case Kind::Label: return "Label";

      case Kind::Do: return "Do";
      case Kind::Error: return "Error";
      case Kind::If: return "If";
      case Kind::ElseIf: return "ElseIf";
      case Kind::Else: return "Else";
      case Kind::When: return "When";
      case Kind::For: return "For";
      case Kind::Repeat: return "Repeat";
      case Kind::While: return "While";
      case Kind::RepeatWhile: return "RepeatWhile";
      case Kind::Is: return "Is";
      case Kind::Has: return "Has";
      case Kind::From: return "From";
      case Kind::Default: return "Default";
      case Kind::Register: return "Register";
      case Kind::Import: return "Import";
    }

    return "<unknown Kind>";
  }
}
