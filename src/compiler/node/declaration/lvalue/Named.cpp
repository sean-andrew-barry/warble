import node.declaration.lvalue.named;
import node.declaration;
import utility.print;
import visitor;
import updater;

namespace node::declaration::lvalue {
  Named::Named(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept
    : LValue{span, scope}, name{name}
  {}

  Named::Named(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept
    : LValue{span, scope}, name{name}
  {}

  void Named::Name(std::string_view v) { name = v; }
  void Named::Name(std::string&& v) { name = std::move(v); }

  void Named::Phi(std::shared_ptr<node::declaration::lvalue::Phi>&& v) { phi = std::move(v); }
  std::shared_ptr<node::declaration::lvalue::Phi> Named::Phi() const { return phi; }

  bool Named::Match(const std::string_view v) const { return name == v; }
  bool Named::Match(const std::string& v) const { return name == v; }

  void Named::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Named::Update(Updater& updater) {}

  int32_t Named::Size() const {
    // utility::Debug("Determining size of typed declaration", Name());

    // if (type) {
    //   // utility::Debug("We do have a type for", Name());
    //   return type->Size();
    // }

    // if (value) {
    //   // utility::Debug("We do have a value for", Name());
    //   return value->Size();
    // }

    return 0;
  }

  std::string Named::Name() const { return name; }
  std::string Named::ToString() const { return utility::Line("Named(", name, ")"); }
};