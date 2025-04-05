export module node.declaration.phi;

import node.declaration;
import <string>;
import <vector>;

export class Visitor;
export class Updater;

namespace node::declaration {
  export class Phi : public node::Declaration {
  private:
    std::vector<node::Declaration*> declarations;
  public:
    using node::Declaration::Declaration;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};