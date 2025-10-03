export module compiler.Engine;

import compiler.engine.ThreadPool;

import <memory>;
import <vector>;
import <string>;
import <string_view>;
// import <shared_mutex>;
import <unordered_map>;
import <cstdint>;
import <shared_mutex>;

namespace compiler {
  export class Engine {
  private:
    static Engine* instance;
    engine::ThreadPool thread_pool;
  public:
    Engine();

    // node::Compiler* Register(node::Compiler* parent, std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard);

    // node::Compiler* Find(const std::string& name);
    // node::Compiler* Library();
    // bool Queue(Node* node);
  };
};