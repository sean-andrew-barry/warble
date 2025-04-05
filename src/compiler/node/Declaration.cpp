import node.declaration;
import node.scope;
import node.scope.context._module;
import node.package;
import code.flag;
// import node.declaration.modifiers;
import visitor;
import updater;
import <atomic>;

namespace node {
  int Declaration::Increment() {
    return count.fetch_add(1, std::memory_order_relaxed);
  }

  int Declaration::Decrement() {
    return count.fetch_sub(1, std::memory_order_acq_rel);
  }

  Declaration::Declaration(const lexical::source::Span& span, node::Scope& scope) noexcept
    : Node{span}, scope{scope}, address{code::Flag::RSP(), scope.Frame()}
  {}

  Declaration::Declaration(Declaration&& other) noexcept
    : Node{other.Span()}, scope{other.scope}, address{std::move(other.address)}, count{other.count.load()}
  {
    other.count = 0;
  }

  Declaration& Declaration::operator=(Declaration&& other) noexcept {
    if (this != &other) {
      // name = std::move(other.name);
      // parent = std::move(other.parent);
      count.store(other.count.load());
      other.count = 0; // Manage moved-from state
    }

    return *this;
  }

  node::Package& Declaration::Package() const { return scope.GetModule().Parent(); }

  uint32_t Declaration::Displacement() const { return address.Displacement(); }
  void Declaration::Displacement(uint32_t v) { address.Displacement(v); }

  void Declaration::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Declaration::Update(Updater& updater) {}
  int32_t Declaration::Size() const { return 0; }
  std::string Declaration::Name() const { return "Declaration"; }
  std::string Declaration::ToString() const { return std::string{Name()}; }
  std::vector<std::type_index> Declaration::Signature() const { return {}; }

  int32_t Declaration::SizeOf(const std::vector<node::Declaration*>& values) const {
    // int32_t size = 0;
    // for (auto& v : values) {
    //   size += v ? v->Size() : 0;
    // }

    // return size;
    return 0;
  }

  int32_t Declaration::SizeOf(const std::vector<std::shared_ptr<node::Declaration>>& values) const {
    // int32_t size = 0;
    // for (auto& v : values) {
    //   size += v ? v->Size() : 0;
    // }

    // return size;
    return 0;
  }

  std::vector<std::type_index> Declaration::SignatureOf(const std::vector<node::Declaration*>& values) const {
    std::vector<std::type_index> signature{};

    for (auto v : values) {
      signature.emplace_back(typeid(v));
    }

    return signature;
  }

  std::vector<std::type_index> Declaration::SignatureOf(const std::vector<std::shared_ptr<node::Declaration>>& values) const {
    std::vector<std::type_index> signature{};

    for (auto v : values) {
      signature.emplace_back(typeid(v));
    }

    return signature;
  }

  bool Declaration::IsUnique() const {
    return count.load(std::memory_order_acquire) == 1;
  }

  int Declaration::GetCount() const {
    return count.load(std::memory_order_relaxed);
  }
};