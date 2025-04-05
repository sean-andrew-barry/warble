export module node.declaration.rvalue.literal;

import node.declaration;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

export namespace code::x64 {
  class Encoder;
};

namespace node::declaration::rvalue {
  export class Literal : public Declaration {
  private:
  public:
    using Declaration::Declaration;

    void Allocate(code::x64::Encoder& encoder) override;
    void Deallocate(code::x64::Encoder& encoder) override;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};