import node.symbol;
import node.declaration;
import <string>;

namespace node {
  Symbol::Symbol(node::Declaration* p) : ptr{p} {}

  void Symbol::Increment() {
    if (ptr) {
      ptr->Increment();
    }
  }

  void Symbol::Decrement() {
    if (ptr && ptr->Decrement() == 1) {
      delete ptr;
      ptr = nullptr;
    }
  }

  Symbol::Symbol() noexcept : ptr{nullptr} {}
  Symbol::Symbol(std::nullptr_t) noexcept : ptr{nullptr} {}

  Symbol::Symbol(const Symbol& other) : ptr{other.ptr} {
    Increment();
  }

  Symbol::Symbol(Symbol&& other) noexcept : ptr{other.ptr} {
    other.ptr = nullptr;
  }

  Symbol& Symbol::operator=(const Symbol& other) {
    if (this != &other) {
      Decrement();
      ptr = other.ptr;
      Increment();
    }

    return *this;
  }

  Symbol& Symbol::operator=(Symbol&& other) noexcept {
    if (this != &other) {
      Decrement();
      ptr = other.ptr;
      other.ptr = nullptr;
    }

    return *this;
  }

  Symbol& Symbol::operator=(std::nullptr_t) noexcept {
    Decrement();

    return *this;
  }

  Symbol::~Symbol() {
    Decrement();
  }

  bool Symbol::IsUnique() const {
    return ptr && ptr->IsUnique();
  }

  int Symbol::GetCount() const {
    if (ptr) {
      return ptr->GetCount();
    } else {
      return 0;
    }
  }

  Symbol::operator bool() const {
    return ptr != nullptr;
  }

  bool Symbol::operator==(const Symbol& other) const { return ptr == other.ptr; }
  bool Symbol::operator!=(const Symbol& other) const { return ptr != other.ptr; }
  bool Symbol::operator==(const std::nullptr_t) const { return ptr == nullptr; }
  bool Symbol::operator!=(const std::nullptr_t) const { return ptr != nullptr; }

  node::Declaration* Symbol::operator->() { return ptr; }
  node::Declaration& Symbol::operator*() { return *ptr; }

  const node::Declaration* Symbol::operator->() const { return ptr; }
  const node::Declaration& Symbol::operator*() const { return *ptr; }

  code::Address* Symbol::Address() const { return ptr ? &ptr->GetAddress() : nullptr; }
  std::string Symbol::Name() const { return ptr ? ptr->GetName() : "null"; }
  std::vector<Symbol> Symbol::Links() const { return ptr ? ptr->GetLinks() : {}; }
  Symbol Symbol::Parent() const { return ptr ? ptr->GetParent() : Symbol{}; }
  Symbol Symbol::Type() const { return ptr ? ptr->GetType() : Symbol{}; }
  std::vector<Symbol> Symbol::Children() const { return ptr ? ptr->GetChildren() : {}; }
  int32_t Symbol::Line() const { return ptr ? ptr->GetLine() : -1; }
  int32_t Symbol::Column() const { return ptr ? ptr->GetColumn() : -1; }
  int32_t Symbol::Offset() const { return ptr ? ptr->GetOffset() : -1; }
  int32_t Symbol::Size() const { return ptr ? ptr->GetSize() : -1; }
  std::string Symbol::ToString() const { return ptr ? ptr->ToString() : "null"; }
};