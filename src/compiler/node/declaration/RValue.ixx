export module node.declaration.rvalue;

import node.declaration;

import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration {
  export class RValue : public Declaration {
  private:
  public:
    using Declaration::Declaration;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};