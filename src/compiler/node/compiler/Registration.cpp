import node.compiler.registration;
import visitor;
import updater;

import <string>;
import <vector>;

namespace node::compiler {
  Registration::Registration(std::string&& path, std::string&& name, std::vector<std::string>&& allowed, node::compiler::Registration* parent, bool wildcard) noexcept
    : path{std::move(path)}, name{std::move(name)}, allowed{std::move(allowed)}, parent{parent}, wildcard{wildcard}
  {}

  const std::string& Registration::Path() const { return path; }
  const std::vector<std::string>& Registration::Allowed() const { return allowed; }
  const Registration* Registration::Parent() const { return parent; }

  // void Registration::Accept(Visitor& visitor) { visitor.Visit(*this); }
  // void Registration::Update(Updater& updater) {}
  int32_t Registration::Size() const { return 0; }
  std::string Registration::Name() const { return name; }
  std::string Registration::ToString() const { return std::string{Name()}; }
};