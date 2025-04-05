export module node.declaration.rvalue.literal.structured._module;

import node.declaration.rvalue.literal.structured;
import <string>;
import <vector>;

namespace node::declaration::rvalue::literal::structured {
  export class Object;
  
  export class Module : public Structured {
  private:
    std::string name;
  public:
    using Structured::Structured;

    Module(const lexical::source::Span& span, std::string name) noexcept;

    int32_t GetSize() const final;
    std::string GetName() const final;
    std::vector<Symbol> GetLinks() const final;
    std::vector<Symbol> GetChildren() const final;

    std::string ToString() const final;
  };
};