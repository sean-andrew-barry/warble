export module node;

import lexical.source.span;
import <vector>;
import <string_view>;
import <string>;
import <memory>;

export namespace code {
  class Encoder;

  namespace x64 { class Encoder; };
  namespace arm64 { class Encoder; };
  namespace riscv { class Encoder; };
};

namespace symbol::rvalue::literal::structured {
  export class Enum;
};

namespace symbol::rvalue::literal::primitive {
  export class String;
};

export class Symbol {
private:
  lexical::source::Span span;
  std::bitset<64> flags;
  Symbol* parent;
  Symbol* prev;
  Symbol* next;
  symbol::rvalue::literal::primitive::String* name;
  symbol::rvalue::literal::structured::Enum* link;
public:
  Symbol() noexcept;
  Symbol(const lexical::source::Span& s) noexcept;
  virtual ~Symbol();

  std::string_view View() const;
  const lexical::source::Span& Span() const;
  lexical::source::Span::TokenIterator Begin() const;
  lexical::source::Span::TokenIterator End() const;

  void Span(const lexical::source::Span& s);
  void Begin(lexical::source::Span::TokenIterator iter);
  void End(lexical::source::Span::TokenIterator iter);
  
  virtual int32_t Size() const = 0;
  virtual std::string Name() const = 0;
  virtual std::string ToString() const = 0;

  virtual void Declare(code::Encoder& encoder) {}
  virtual void Compile(code::Encoder& encoder) {}
  virtual void Declare(code::x64::Encoder& encoder) {}
  virtual void Compile(code::x64::Encoder& encoder) {}
  virtual void Declare(code::arm64::Encoder& encoder) {}
  virtual void Compile(code::arm64::Encoder& encoder) {}
  virtual void Declare(code::riscv::Encoder& encoder) {}
  virtual void Compile(code::riscv::Encoder& encoder) {}

  virtual void Allocate(code::x64::Encoder& encoder) {}
  virtual void Deallocate(code::x64::Encoder& encoder) {}
  virtual void Construct(code::x64::Encoder& encoder) {}
  virtual void Destruct(code::x64::Encoder& encoder) {}
};