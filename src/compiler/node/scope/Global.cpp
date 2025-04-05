import node.scope.global;
import node.package;
import node.declaration;
import node.declaration.rvalue.literal.structured.object;
import utility.print;
import visitor;
import updater;

import <stdexcept>;
import <typeinfo>;
import <memory>;

namespace node::scope {
  Global Global::instance{};

  const Global& Global::Get() { return instance; }

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Object() { return instance.GetObject(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Boolean() { return instance.GetBoolean(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Character() { return instance.GetCharacter(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Number() { return instance.GetNumber(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Integer() { return instance.GetInteger(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Decimal() { return instance.GetDecimal(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Null() { return instance.GetNull(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Undefined() { return instance.GetUndefined(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Symbol() { return instance.GetSymbol(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Function() { return instance.GetFunction(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Array() { return instance.GetArray(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Enum() { return instance.GetEnum(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::String() { return instance.GetString(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::Tuple() { return instance.GetTuple(); }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::TemplateString() { return instance.GetTemplateString(); }

  Global::Global()
    : Scope{lexical::source::Span{}}
    , object{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , boolean{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , character{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , number{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , integer{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , decimal{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , null{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , undefined{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , symbol{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , function{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , array{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , _enum{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , string{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , tuple{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
    , template_string{std::make_shared<node::declaration::rvalue::literal::structured::Object>(Span(), *this)}
  {
  }

  Global::~Global() = default;

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetObject() const { return object; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetBoolean() const { return boolean; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetCharacter() const { return character; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetNumber() const { return number; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetInteger() const { return integer; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetDecimal() const { return decimal; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetNull() const { return null; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetUndefined() const { return undefined; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetSymbol() const { return symbol; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetFunction() const { return function; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetArray() const { return array; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetEnum() const { return _enum; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetString() const { return string; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetTuple() const { return tuple; }
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Global::GetTemplateString() const { return template_string; }

  // node::scope::Package& Global::Register(std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard) {
  //   utility::Debug("Global scope registering package:", name, "at", path);

  //   std::unique_lock<std::shared_mutex> lock{mutex};

  //   for (auto& uptr : packages) {
  //     if (uptr->Path() == path && uptr->Name() == name) {
  //       return *uptr;
  //     }
  //   }

  //   // Create it since it didn't already exist
  //   packages.push_back(std::make_unique<node::scope::Package>(std::move(path), std::move(name), std::move(allowed), wildcard));

  //   return *packages.back();
  // }

  void Global::Accept(Visitor& visitor) {}
  void Global::Update(Updater& updater) {}
  int32_t Global::Size() const { return 0; }
  std::string Global::Name() const { return "Global Scope"; }
  std::string Global::ToString() const { return "Global Scope"; }

  void Global::Compile(code::x64::Encoder& encoder) {}
};