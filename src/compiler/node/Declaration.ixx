export module node.declaration;

import node;
import node.symbol;
import code._register;
import code.address;
import code.modifiers;
import <string_view>;
import <string>;
import <vector>;
import <typeinfo>;
import <typeindex>;
import <atomic>;
import <cstdint>;
import <memory>;

export class Visitor;
export class Updater;

// export namespace node {
//   class Compiler;
// };

export namespace node::instruction::memory {
  class Construct;
  class Destruct;
};

export namespace node::declaration {
  class Literal;
};

export namespace node::scope {
  class Context;
};

export namespace node::declaration::literal::structured {
  class Enum;
};

namespace node {
  export class Scope;
  export class Package;

  export class Declaration : public Node {
  private:
    node::Scope& scope;
    // std::shared_ptr<node::Declaration> type;
    // std::shared_ptr<node::Declaration> parent{nullptr};
    node::instruction::memory::Construct* ctor;
    node::instruction::memory::Destruct* dtor;
    code::Register reg;
    code::Address address;
    code::Modifiers modifiers;
    std::atomic<int> count{0};
    uint32_t displacement{0};
  private:
    int Increment();
    int Decrement();
    friend class Symbol; // Giving Symbol access to Increment/Decrement
  public:
    using Node::Node;

    Declaration(const lexical::source::Span& span, node::Scope& scope) noexcept;
    Declaration(Declaration&& other) noexcept;

    Declaration& operator=(Declaration&& other) noexcept;

    Package& Package() const;

    void Modifiers(code::Modifiers v) { modifiers = v; }
    code::Modifiers Modifiers() const { return modifiers; }

    // void Parent(std::shared_ptr<node::Declaration> v) { parent = v; }
    // std::shared_ptr<node::Declaration> Parent() const { return parent; }

    // void Type(std::shared_ptr<node::Declaration> v) { type = v; }
    // std::shared_ptr<node::Declaration> Type() const { return type; }

    // void Register(code::Register v) { reg = v; }
    code::Register& Register() { return reg; }
    node::Scope& Scope() { return scope; }
    code::Address& Address() { return address; }

    void Ctor(node::instruction::memory::Construct* v) { ctor = v; }
    node::instruction::memory::Construct* Ctor() const { return ctor; }
    void Dtor(node::instruction::memory::Destruct* v) { dtor = v; }
    node::instruction::memory::Destruct* Dtor() const { return dtor; }

    void Constructor(node::instruction::memory::Construct* v) { ctor = v; }
    node::instruction::memory::Construct* Constructor() const { return ctor; }
    void Destructor(node::instruction::memory::Destruct* v) { dtor = v; }
    node::instruction::memory::Destruct* Destructor() const { return dtor; }

    uint32_t Displacement() const;
    void Displacement(uint32_t v);

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
    virtual std::vector<std::type_index> Signature() const;

    int32_t SizeOf(const std::vector<node::Declaration*>& values) const;
    int32_t SizeOf(const std::vector<std::shared_ptr<node::Declaration>>& values) const;
    std::vector<std::type_index> SignatureOf(const std::vector<node::Declaration*>& values) const;
    std::vector<std::type_index> SignatureOf(const std::vector<std::shared_ptr<node::Declaration>>& values) const;

    bool IsUnique() const;
    int GetCount() const;

    virtual void Assign(code::x64::Encoder& encoder, node::Declaration& lhs) {}
    virtual void Add(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Add(code::x64::Encoder& encoder, node::Declaration& lhs, node::Declaration& rhs) {}
    virtual void Subtract(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Divide(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Multiply(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Modulo(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Not(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Negate(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void And(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Or(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Xor(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Compare(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Move(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Exchange(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Convert(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void RotateLeft(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void RotateRight(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void ShiftLeft(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void ShiftRight(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Increment(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Decrement(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Call(code::x64::Encoder& encoder, node::Declaration& rhs) {}
    virtual void Lookup(code::x64::Encoder& encoder, node::declaration::literal::structured::Enum& rhs) {}
  };
};