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

  // std::string Symbol::GetName() const { return ptr ? ptr->GetName() : "null"; }
  // std::string Symbol::Emit() const { return ptr ? ptr->Emit() : "null"; }
  // std::string Symbol::ToString() const { return ptr ? ptr->ToString() : "null"; }
};
