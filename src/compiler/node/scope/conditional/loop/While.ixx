export module node.scope.conditional.loop._while;

import node.scope.conditional.loop;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::scope::conditional::loop {
  export class While : public Loop {
  private:
  public:
    using Loop::Loop;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};