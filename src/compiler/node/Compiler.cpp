import node.compiler;
// import node.compiler.registration;
import node.scope.context._module;
import utility.print;
import engine;
import visitor;
import updater;

import <stdexcept>;
import <vector>;
import <string>;
import <mutex>;
import <filesystem>;
import <fstream>;

std::string ReadFile(const std::string& path) {
  std::ifstream file{path, std::ios::ate | std::ios::binary};

  if (file.is_open()) {
    std::string content{};
    content.resize(static_cast<size_t>(file.tellg()));

    file.seekg(0);
    file.read(content.data(), content.size());
    file.close();

    return content;
  }
  else {
    throw utility::Error("Failed to open file at", path);
  }
}

namespace node {
  Compiler::Compiler(Engine& engine, node::Compiler* parent, std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard)
    : engine{engine}
    , parent{parent}
    , path{std::move(path)}
    , name{std::move(name)}
    , allowed{std::move(allowed)}
    , wildcard{wildcard}
  {
    // // null_value = Symbol::Make<NullLiteralDeclaration>("null", nullptr);
    // // true_value = Symbol::Make<BooleanLiteralDeclaration>("true", nullptr, true);
    // // false_value = Symbol::Make<BooleanLiteralDeclaration>("false", nullptr, false);
    // // auto_type = Symbol::Make<AutoLiteralDeclaration>("auto", nullptr);
    // // void_type = Symbol::Make<VoidLiteralDeclaration>("void", nullptr);

    // // Assuming the 'path' is the directory where the package resides,
    // // and 'specifier' is the file or subdirectory for the entry point.

    // // Full path to the entry point.
    // std::string resolved_path = path + specifier;

    // // Allowed modules for the entry point package.
    // // Using "*" to denote all modules are allowed.
    // std::vector<std::string> allowed{};

    // // Create the registration object for the entry package.
    // // 'path' is used here since it represents the package directory.
    // auto registration = std::make_unique<Registration>(std::move(path), std::move(name), std::move(allowed), nullptr, true);
    // auto* registrationPtr = registration.get();

    // std::vector<std::unique_ptr<Registration>> vector;
    // vector.emplace_back(std::move(registration));

    // registrations.emplace(registrationPtr->GetName(), std::move(vector));

    // // Create the module object for the entry point.
    // // 'resolved_path' is the full path to the entry point file.
    // auto mod = std::make_unique<Module>(*this, std::move(resolved_path), std::move(specifier), registrationPtr);
    // auto* modulePtr = mod.get();

    // // Emplace the module into the 'modules' map.
    // modules.emplace(modulePtr->GetPath(), std::move(mod));

    // // Register("library/index.clear", "index", {}, registrationPtr, true);
    // Register("library/class.clear", "class", {}, registrationPtr, true);
    // Register("library/int.clear", "int", {}, registrationPtr, true);
    // Register("library/string.clear", "string", {}, registrationPtr, true);
    // Register("library/array.clear", "array", {}, registrationPtr, true);
    // Register("library/defaults.clear", "defaults", {}, registrationPtr, true);

    // modulePtr->Run();

    // // // Queue the module for processing.
    // // Queue(modulePtr);

    // // thread_pool.Activate();
  }

  Compiler::~Compiler() = default;

  node::Compiler* Compiler::Register(std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard) {
    return nullptr;
    // return engine.Register(this, std::move(path), std::move(name), std::move(allowed), wildcard);
  }

  node::scope::context::Module* Compiler::Import(std::string&& specifier) {
    return nullptr;
    // utility::Debug("Compiler", name, "importing module", specifier);

    // if (this == engine.Library()) {
    //   specifier = '/' + specifier;
    // }
    // else if (specifier.starts_with(name)) {
    //   specifier.replace(0, name.length(), "/");
    // }
    // else if (specifier.front() != '/') {
    //   // Named specifier, resolve the name and path
    //   auto pos = specifier.find('/');
    //   if (pos == std::string::npos) {
    //     return engine.Library()->Import(std::move(specifier));
    //   }

    //   std::string compiler_name = specifier.substr(0, pos);

    //   auto* compiler = engine.Find(compiler_name);
    //   if (!compiler) {
    //     throw utility::Error("Import error: no compiler has been registered for", compiler_name);
    //   }

    //   return compiler->Import(specifier.substr(pos));
    // }

    // if (!AllowsImport(specifier)) {
    //   throw utility::Error("Import error: compiler", name, "doesn't allow the specifier", specifier);
    // }

    // std::string resolved_path = path + specifier;
    // std::string content = ReadFile(resolved_path);

    // std::unique_lock<std::shared_mutex> lock{mutex};

    // auto iter = modules.find(resolved_path);
    // if (iter == modules.end()) {
    //   std::string key = resolved_path; // Make a copy since we're going to move the resolved_path
    //   auto [iter, success] = modules.try_emplace(key, *this, std::move(resolved_path), std::move(content));
    //   return &iter->second;
    // }
    // else {
    //   return &iter->second;
    // }
  }

  bool Compiler::AllowsImport(const std::string_view specifier) const {
    if (wildcard) {
      return true;
    }

    for (const auto& allow : allowed) {
      if (allow == specifier) {
        return true;
      }
    }

    return false;
  }

  bool Compiler::Queue(Node* node) {
    // std::cout << "Queuing: " << node->Emit() << std::endl;
    return engine.Queue(node);
  }

  node::scope::context::Module* Compiler::Find(const std::string& name) {
    return nullptr;

    // std::shared_lock<std::shared_mutex> lock{mutex};
    // auto it = modules.find(name);
    // if (it == modules.end()) {
    //   return nullptr;
    // }

    // return &it->second;
  }

  node::Compiler* Compiler::Parent() { return parent; }
  const std::string& Compiler::Path() const { return path; }
  // const std::string& Compiler::Name() const { return name; }

  void Compiler::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Compiler::Update(Updater& updater) {}
  int32_t Compiler::Size() const { return 0; }
  std::string Compiler::Name() const { return name; }
  std::string Compiler::ToString() const { return std::string{Name()}; }
};