export module node.declaration._import;

import node.declaration;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration {
  export class Import : public Declaration {
  private:
    std::string name;
    Declaration* link;
  public:
    using Declaration::Declaration;

    Import(const lexical::source::Span& span, node::Scope& scope, std::string&& name) noexcept;
    Import(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept;
    // Import(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};