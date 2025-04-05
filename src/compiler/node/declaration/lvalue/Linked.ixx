export module node.declaration.lvalue.linked;

import node.declaration;
import node.declaration.lvalue;
import <string_view>;
import <string>;
import <bitset>;
import <vector>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue::literal::structured {
  export class Enum;
};

namespace node::declaration::lvalue {
  export class Linked : public LValue {
  private:
    std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> links;
  public:
    using LValue::LValue;

    void Links(std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> v);
    std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> Links() const;

    void Add(std::shared_ptr<node::Declaration>&& link);

    bool Match(const node::declaration::rvalue::literal::structured::Enum& v) const final;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};