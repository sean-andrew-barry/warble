export module node.declaration.rvalue.context;

import node.declaration;
import <string_view>;
import <string>;

namespace node::declaration::rvalue {
  export class Context : public Declaration {
  private:
  public:
    using Declaration::Declaration;

    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};