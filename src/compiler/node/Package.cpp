import node.package;
import node.declaration;
import node.scope.context._module;
import utility.print;
import utility.read_file;
import visitor;
import updater;
import engine;

import <stdexcept>;
import <typeinfo>;
import <memory>;
import <utility>;

import <vector>;
import <string>;
import <mutex>;
import <filesystem>;
import <fstream>;

namespace node {
  node::Package* Package::STD{nullptr};

  Package::Package(Engine& engine, node::Package* parent, std::string&& path, std::string&& specifier, std::vector<std::pair<std::string, std::string>>&& allowed)
    : Node{lexical::source::Span{}}
    , engine{engine}
    , parent{parent}
    , path{std::move(path)}
    , name{std::move(name)}
    , allowed{std::move(allowed)}
  {
  }

  Package::~Package() = default;

  node::Package& Package::Register(std::string&& path, std::string&& name, std::vector<std::pair<std::string, std::string>>&& allowed) {
    utility::Debug("Package", name, "registering package:", name, "at", path);

    std::unique_lock<std::shared_mutex> lock{packages_mutex};

    for (auto& uptr : packages) {
      if (uptr->Path() == path && uptr->Name() == name) {
        return *uptr; // Package already exists
      }
    }

    // Create it since it didn't already exist
    packages.push_back(std::make_unique<node::Package>(engine, this, std::move(path), std::move(name), std::move(allowed)));
    return *packages.back();
  }

  node::scope::context::Module& Package::Import(std::string&& specifier, std::string&& source) {
    utility::Debug("Package", name, "registering import:", specifier, "with source", source);

    // if (specifier.starts_with(name)) {
    //   // If it starts with the package's name, like "my_package/specifier" 
    //   specifier.replace(0, name.length(), "/");
    // } else if (specifier.front() != '/') {
    //   // Named specifier like "specifier" or "my/path.wbl", resolve the name and path
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

    if (!source.empty() && source != name) {
      return parent->Import(std::move(specifier), std::move(source));
    }

    // for (const auto& [allow, source] : allowed) {
    //   if (allow == specifier) {
    //     return true;
    //   }
    // }

    // if (!AllowsImport(specifier)) {
    //   throw utility::Error("Import error: compiler", name, "doesn't allow the specifier", specifier);
    // }

    std::unique_lock<std::shared_mutex> lock{modules_mutex};

    for (auto& uptr : modules) {
      if (uptr->Specifier() == specifier) {
        return *uptr; // This module has already been registered
      }
    }

    std::string resolved_path = path + '/' + specifier;
    std::string content = utility::ReadFile(resolved_path);
    // std::string content = "";

    modules.push_back(std::make_unique<node::scope::context::Module>(*this, std::move(resolved_path), std::move(content)));
    auto& mod = *modules.back();

    // engine.Queue(&mod);

    return mod;
  }

  bool Package::AllowsImport(const std::string& specifier) const {
    for (const auto& [allow, source] : allowed) {
      if (allow == specifier) {
        return true;
      }
    }

    return false;
  }

  const std::string& Package::Path() const { return path; }
  void Package::Accept(Visitor& visitor) {}
  void Package::Update(Updater& updater) {}
  int32_t Package::Size() const { return 0; }
  std::string Package::Name() const { return "Package"; }
  std::string Package::ToString() const { return "Package"; }

  void Package::Compile(code::x64::Encoder& encoder) {}
};