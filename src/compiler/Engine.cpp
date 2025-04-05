import engine;
import node;
// import node.compiler;
// import node.scope.context.global;
import utility.print;

// import <mutex>;

Engine* Engine::instance{nullptr};

Engine::Engine(std::vector<std::string>&& commands)
  : commands{std::move(commands)}
{
  instance = this;
  // library = Register(nullptr, "library", "library", {}, true);
}

// node::Compiler* Engine::Register(node::Compiler* parent, node::scope::context::Global& scope, std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard) {
//   utility::Debug("Engine registering package:", name, "at", path);

//   std::unique_lock<std::shared_mutex> lock{mutex};

//   std::string key = path; // Make a copy since we're going to move the path

//   auto [iter, result] = compilers.try_emplace(key, *this, parent, std::move(path), std::move(name), std::move(allowed), wildcard);
//   if (!result) {
//     throw utility::Error("A node::Compiler was already registered at", key);
//   }

//   return &iter->second;
// }

// node::Compiler* Engine::Find(const std::string& name) {
//   std::shared_lock<std::shared_mutex> lock{mutex};

//   auto it = compilers.find(name);
//   if (it == compilers.end()) {
//     return nullptr;
//   }

//   return &it->second;
// }

// node::Compiler* Engine::Library() { return library; }

bool Engine::Queue(Node* node) {
  return thread_pool.Push(node);
}