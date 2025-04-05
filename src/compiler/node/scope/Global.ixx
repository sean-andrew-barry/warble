export module node.scope.global;

import node;
import node.scope;
import <string_view>;
import <string>;
import <vector>;
import <memory>;
import <shared_mutex>;

export class Visitor;
export class Updater;

namespace node::scope {
  export class Package;
};

namespace node::declaration::rvalue::literal::structured {
  export class Object;
};

namespace node::scope {
  // There is one global scope. It holds the types for all of the literals. Every individual literal is a different instance, represented by a distinct node. But each literal always returns a consistent type, so that the type of any two objects will compare as equal.
  export class Global : public Scope {
  private:
    static Global instance;
  public:
    static const Global& Get();

    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Object();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Boolean();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Character();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Number();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Integer();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Decimal();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Null();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Undefined();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Symbol();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Function();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Array();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Enum();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> String();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Tuple();
    static std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TemplateString();
  private:
    // std::shared_mutex mutex;
    // std::vector<std::unique_ptr<node::scope::Package>> packages; // Direct children of this scope
    
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> object;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> boolean;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> character;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> number;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> integer;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> decimal;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> null;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> undefined;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> symbol;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> function;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> array;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> _enum;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> string;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> tuple;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> template_string;

    Global(); // Make the constructor private since it's a singleton
  public:
    // using Scope::Scope;

    ~Global() final;

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetObject() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetBoolean() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetCharacter() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetNumber() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetInteger() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetDecimal() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetNull() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetUndefined() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetSymbol() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetFunction() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetArray() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetEnum() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetString() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetTuple() const;
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> GetTemplateString() const;

    bool IsContext() const final { return false; }

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    void Compile(code::x64::Encoder& encoder) final;
  };
};