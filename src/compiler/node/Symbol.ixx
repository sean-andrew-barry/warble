export module node.symbol;

import <utility>;
import <cstddef>;
import <atomic>;
import <type_traits>;
import <string>;
import <cstdint>;

namespace node {
  export class Declaration;

  export class Symbol {
  public:
    template<typename T, typename... Args>
    static Symbol Make(Args&&... args) {
      static_assert(std::is_base_of_v<node::Declaration, T>, "Symbol::Make can only be used with Declaration derived type");

      auto* ptr = new T(std::forward<Args>(args)...);
      return Symbol{ptr};
    }
  private:
    node::Declaration* ptr;

    explicit Symbol(node::Declaration* ptr) : ptr{ptr} {}

    void Increment();
    void Decrement();
  public:
    Symbol() noexcept;
    Symbol(std::nullptr_t) noexcept;

    Symbol(const Symbol& other);
    Symbol(Symbol&& other) noexcept;

    Symbol& operator=(const Symbol& other);
    Symbol& operator=(Symbol&& other) noexcept;
    Symbol& operator=(std::nullptr_t) noexcept;

    ~Symbol();

    bool IsUnique() const;
    int GetCount() const;

    explicit operator bool() const;

    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;
    bool operator==(const std::nullptr_t) const;
    bool operator!=(const std::nullptr_t) const;

    node::Declaration* operator->();
    node::Declaration& operator*();

    const node::Declaration* operator->() const;
    const node::Declaration& operator*() const;

    code::Address* Address() const;
    std::string Name() const;
    std::vector<Symbol> Links() const;
    Symbol Parent() const;
    Symbol Type() const;
    std::vector<Symbol> Children() const;
    int32_t Line() const;
    int32_t Column() const;
    int32_t Offset() const;
    int32_t Size() const;
    std::string ToString() const;
  };
};