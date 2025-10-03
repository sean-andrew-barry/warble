export module compiler.program.Stages;

import <cstdint>;
import <atomic>;

import compiler.program.Stage;

namespace compiler::program {
  export class Stages {
  private:
    std::atomic<Stage> stage;
  public:
    Stages() : stage{Stage::Initializing} {}
  };
}