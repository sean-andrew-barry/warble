export module node.declaration.rvalue.literal.structured.global;

import node.declaration.rvalue.literal.structured;
import symbol;
import <string>;
import <vector>;

export namespace node::declaration::rvalue::literal::primitive {
  class Boolean;
  class Character;
  class Null;
  class Undefined;

  namespace numeric {
    class Decimal;
    class Integer;
    class Number;
  };
};

export namespace node::declaration::rvalue::literal::structured {
  class Package;
  class Module;
  class Function;
  class Object;
  class Tuple;
  class Array;
  class Enum;
  class String;
  class TemplateString;
};

namespace node::declaration::rvalue::literal::structured {
  // The global declaration holds all of the compiler generated types
  // The types are object literals. First it has the primitive types, which are named
  // But then there are types generated for each unique structured literal.
  export class Global : public Structured {
  private:
    Symbol object_type;
    Symbol boolean_type;
    Symbol character_type;
    Symbol number_type;
    Symbol integer_type;
    Symbol decimal_type;
    Symbol null_type;
    Symbol undefined_type;
    // Symbol symbol_type;
    Symbol function_type;
    Symbol array_type;
    Symbol enum_type;
    Symbol string_type;
    Symbol tuple_type;
    Symbol template_string_type;

    // All of the following types are cached by their structure
    std::unordered_set<Symbol> object_types; // Hashed by sorted child names and types
    std::unordered_set<Symbol> tuple_types; // Hashed by child types
    std::unordered_set<Symbol> array_types; // Hashed by the first child type and length
    std::unordered_set<Symbol> enum_types; // Indexed (not hashed) by length, since type is constant
    std::unordered_set<Symbol> function_types; // Hashed by sorted capture child names and types, then parameter child types, then return type

    // All of the following literals use the same declarations
    Symbol null_value;
    Symbol undefined_value;
    Symbol true_value;
    Symbol false_value;

    // All of the following literals are cached by value
    std::unordered_map<double, Symbol> decimal_values;
    std::unordered_map<uint64_t, Symbol> number_values;
    std::unordered_map<int64_t, Symbol> integer_values;
    std::unordered_map<std::string, Symbol> string_values;
  public:
    // using Structured::Structured;

    Global(const lexical::source::Span& span) noexcept;

    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::Boolean& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::Character& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::Null& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::Undefined& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::numeric::Decimal& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::numeric::Integer& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::primitive::numeric::Number& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Global& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Package& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Module& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Function& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Object& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Tuple& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Array& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::Enum& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::String& literal);
    Symbol GetTypeFor(node::declaration::rvalue::literal::structured::TemplateString& literal);

    Symbol GetParent() const final { return Symbol{nullptr}; }
    int32_t GetSize() const final;

    std::string ToString() const final;
  };
};