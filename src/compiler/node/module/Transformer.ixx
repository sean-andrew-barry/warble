export module node._module.transformer;

import node._module;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::_module {
  export class Transformer : public Module {
  private:
  public:
    using Module::Module;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};