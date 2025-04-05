export module node.declaration.lvalue;

import node.declaration;

import <string>;
import <bitset>;
import <vector>;
import <memory>;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue::literal::structured {
  export class Enum;
};

namespace node::declaration {
  export class LValue : public Declaration {
  private:
    std::shared_ptr<node::Declaration> contract;
    std::shared_ptr<node::Declaration> value;
  public:
    using Declaration::Declaration;

    void Contract(std::shared_ptr<node::Declaration> v);
    void Value(std::shared_ptr<node::Declaration> v);

    std::shared_ptr<node::Declaration> Contract() const;
    std::shared_ptr<node::Declaration> Value() const;

    virtual bool Match(const std::string_view v) const { return false; }
    virtual bool Match(const std::string& v) const { return false; }
    virtual bool Match(const node::declaration::rvalue::literal::structured::Enum& v) const { return false; }

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};