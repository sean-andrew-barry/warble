export module node;

import lexical.source.span;
import <vector>;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

export namespace code {
  class Encoder;

  namespace x64 { class Encoder; };
  namespace arm64 { class Encoder; };
  namespace riscv { class Encoder; };
};

export namespace node {
  class Declaration;
};

namespace node::declaration::rvalue::literal::structured {
  export class Object;
};

export class Node {
private:
  lexical::source::Span span;
public:
  Node() noexcept;
  Node(const lexical::source::Span& s) noexcept;
  virtual ~Node();

  std::string_view View() const;
  const lexical::source::Span& Span() const;
  lexical::source::Span::TokenIterator Begin() const;
  lexical::source::Span::TokenIterator End() const;

  void Span(const lexical::source::Span& s);
  void Begin(lexical::source::Span::TokenIterator iter);
  void End(lexical::source::Span::TokenIterator iter);
  
  virtual void Accept(Visitor& visitor) = 0;
  virtual void Update(Updater& updater) = 0;
  virtual std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const { return nullptr; }
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