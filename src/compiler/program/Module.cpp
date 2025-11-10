module compiler.program.Module;

import <atomic>;
import <unordered_map>;
import <utility>;
import <vector>;
import <utility>;
import compiler.utility.DualBuffer;

namespace compiler::program {
	// ----- Helper mapping from next stage to required dependency stage -----
	Module::Stage Module::RequiredDependencyStageFor(Stage next) {
		switch (next) {
			case Stage::Reading:    return Stage::Created;   // no deps required
			case Stage::Lexing:     return Stage::Created;   // no deps required
			case Stage::Parsing:    return Stage::Created;   // no deps required
			case Stage::Validating: return Stage::Parsed;    // need import graph frozen
			case Stage::Allocating: return Stage::Validated; // need validation done
			case Stage::Optimizing: return Stage::Allocated; // conservative choice
			case Stage::Encoding:   return Stage::Optimized; // conservative choice
			case Stage::Merging:    return Stage::Encoded;   // conservative choice
			case Stage::Created:
			case Stage::Read:
			case Stage::Lexed:
			case Stage::Parsed:
			case Stage::Validated:
			case Stage::Allocated:
			case Stage::Optimized:
			case Stage::Encoded:
			case Stage::Merged:
			default:               return Stage::Created;   // terminal/invalid -> no deps
		}
	}

	bool Module::StageAtLeast(size_t dep_cycle, Stage required) {
		// Compare against the even stage boundary (mask off odd bit)
		const size_t even_cycle = dep_cycle & ~static_cast<size_t>(1);
		return static_cast<size_t>(static_cast<Stage>(even_cycle)) >= static_cast<size_t>(required);
	}

	// ----- Stage bodies (placeholders for now) -----
	Module::Module(Program& program, program::Package& package, fs::File&& file)
		: program{program}
		, package{package}
		, file{std::move(file)}
	{}

	bool Module::Reading() {
		// Read source text
		return true;
	}

	bool Module::Lexing() {
		// TODO
		return true;
	}

	bool Module::Parsing() {
		// TODO
		return true;
	}

	bool Module::Validating() {
    // Detect import cycles using a DFS over frozen dependency vectors.
    // Precondition: scheduler ensures all deps are at least Parsed (frozen).

    enum class Color : uint8_t { White, Gray, Black };
    std::unordered_map<Module*, Color> color;
    color.reserve(64);

    bool cycle_found = false;

    // Local recursive lambda for DFS
    auto visit = [&](auto&& self, Module* node) -> void {
      if (cycle_found) return;

      auto it = color.find(node);
      if (it != color.end()) {
        if (it->second == Color::Gray) {
          cycle_found = true; // back edge
        }
        return; // Gray/Black both mean we've seen it already
      }

      color.emplace(node, Color::Gray);

      for (auto dep : node->dependencies) {
        // Safety: deps should be even and at least Parsed here; if not, skip and let scheduler retry later.
        const auto dc = dep->cycle.load(std::memory_order_acquire);
        if (IsOdd(dc) || !StageAtLeast(dc, Stage::Parsed)) {
          // Not ready; treat as non-fatal and allow another pass to validate later.
          continue;
        }
        self(self, dep);
        if (cycle_found) break;
      }

      color[node] = Color::Black;
    };

    visit(visit, this);

    // TODO: Emit a diagnostic with the cycle path when available.
    return !cycle_found;
	}

	bool Module::Allocating() {
		// TODO
		return true;
	}

	bool Module::Optimizing() {
		// TODO
		return true;
	}

	bool Module::Encoding() {
		// TODO
		return true;
	}

	bool Module::Merging() {
		// TODO
		return true;
	}

	bool Module::Step(Stage stage) {
		switch (stage) {
			case Stage::Reading: return Reading();
			case Stage::Lexing: return Lexing();
			case Stage::Parsing: return Parsing();
			case Stage::Validating: return Validating();
			case Stage::Allocating: return Allocating();
			case Stage::Optimizing: return Optimizing();
			case Stage::Encoding: return Encoding();
			case Stage::Merging: return Merging();
			case Stage::Created:
			case Stage::Read:
			case Stage::Lexed:
			case Stage::Parsed:
			case Stage::Validated:
			case Stage::Allocated:
			case Stage::Optimized:
			case Stage::Encoded:
			case Stage::Merged: {
				// Invalid stage: these are the completed even states
				return false;
			}
			default: {
				// Out of range; treat as terminal
				return false;
			}
		}
	}

	bool Module::Run(utility::DualBuffer<program::Module*>& buffer) {
		auto mc = cycle.load(std::memory_order_acquire);
		if (IsOdd(mc)) return false; // Already claimed elsewhere; forget it

		auto current_stage = static_cast<Stage>(mc);      // even state (completed)
		auto next_stage    = static_cast<Stage>(mc + 1);  // odd state (to run)
		auto required_dep  = RequiredDependencyStageFor(next_stage);

		// Stage-aware dependency readiness check and depth-first advancement.
		for (auto dep : dependencies) {
			for (;;) {
				auto dc = dep->cycle.load(std::memory_order_acquire);

				if (StageAtLeast(dc, required_dep)) {
					break; // Dependency satisfies the requirement for this stage
				}

				if (IsOdd(dc)) {
					buffer.Reschedule(this);
					return false;
				}

				// Dependency is behind and not running — try to advance it now (depth-first).
				if (!dep->Run(buffer)) {
					// Could not advance dependency now; reschedule current and try later.
					buffer.Reschedule(this);
					return false;
				}

				// Loop back to re-check dependency readiness after attempting to run it.
			}
		}

		// All dependencies satisfy this stage's requirement — attempt to claim this module.
		size_t expected = mc;
		if (!cycle.compare_exchange_strong(expected, mc + 1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
			// Lost the race; another thread advanced it. Do not reschedule.
			return false;
		}

		// We own this module for one step. Run the stage body, then complete to even.
		(void)current_stage; // Reserved for per-stage logic if needed
		const bool ok = Step(next_stage);
		cycle.fetch_add(1, std::memory_order_release); // mc + 1 -> mc + 2
		if (ok) {
			buffer.Complete(this); // Completed this step; enqueue for the next round
			return true;
		}

		// If the step failed, we still completed the odd->even flip to avoid stalling others.
		return false;
	}

  program::Package& Module::Register(std::filesystem::path&& specifier) { return program.Register(std::move(specifier)); }
  program::Module& Module::Import(std::filesystem::path&& specifier) { return package.Import(std::move(specifier)); }
}