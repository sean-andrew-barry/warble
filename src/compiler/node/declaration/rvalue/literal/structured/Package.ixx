export module node.declaration.rvalue.literal.structured.package;

import node.declaration.rvalue.literal.structured;
import <string>;
import <vector>;

namespace node::declaration::rvalue::literal::structured {
  export class Object;
  
  export class Package : public Structured {
  private:
    std::string name;
    std::vector<Symbol> children; // All of the modules
  public:
    using Structured::Structured;

    Package(const lexical::source::Span& span, std::string name) noexcept;

    int32_t GetSize() const final;
    std::string GetName() const final;
    std::vector<Symbol> GetLinks() const final;
    std::vector<Symbol> GetChildren() const final; // All of the modules in this package

    std::string ToString() const final;
  };
};