export module engine;

import engine.thread_pool;

import <memory>;
import <vector>;
import <string>;
import <string_view>;
// import <shared_mutex>;
import <unordered_map>;
import <cstdint>;
import <shared_mutex>;

export class Node;

namespace node {
  export class Compiler;
};

namespace node::scope::context {
  export class Global;
};

export class Engine {
private:
  static Engine* instance;
  engine::ThreadPool thread_pool;
  // std::shared_mutex mutex;
  // std::unordered_map<std::string, node::Compiler> compilers;
  std::vector<std::string> commands;
  // node::Compiler* library;
public:
  Engine(std::vector<std::string>&& commands);

  // node::Compiler* Register(node::Compiler* parent, std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard);

  // node::Compiler* Find(const std::string& name);
  // node::Compiler* Library();
  bool Queue(Node* node);
};