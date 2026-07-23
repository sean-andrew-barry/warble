#include "compiler/ir/Symbols.hpp"

namespace compiler::ir {
	Symbols::Symbols() = default;
	Symbols::Symbols(Symbols&&) noexcept = default;
	Symbols& Symbols::operator=(Symbols&&) noexcept = default;

	ir::symbol::Kind Symbols::Kind(ir::Symbol i) const { return kinds[i.Row()]; }
	uint64_t Symbols::Flags(ir::Symbol i) const { return flags[i.Row()]; }
	uint64_t Symbols::Payload(ir::Symbol i) const { return payloads[i.Row()]; }
	uint32_t Symbols::PayloadLow(ir::Symbol i) const { return static_cast<uint32_t>(Payload(i) & 0xFFFFFFFFu); }
	uint32_t Symbols::PayloadHigh(ir::Symbol i) const { return static_cast<uint32_t>(Payload(i) >> 32); }
	uint32_t Symbols::Offset(ir::Symbol i) const { return offsets[i.Row()]; }
	ir::Symbol Symbols::Parent(ir::Symbol i) const { return parents[i.Row()]; }
	ir::Symbol Symbols::Name(ir::Symbol i) const { return names[i.Row()]; }
	uint32_t Symbols::Token(ir::Symbol i) const { return tokens[i.Row()]; }
	bool Symbols::Flag(ir::Symbol i, ir::symbol::Flag f) const { return (flags[i.Row()] & (1ull << static_cast<uint64_t>(f))) != 0; }

	void Symbols::Kind(ir::Symbol i, ir::symbol::Kind v) { kinds[i.Row()] = v; }
	void Symbols::Flags(ir::Symbol i, uint64_t v) { flags[i.Row()] = v; }
	void Symbols::Payload(ir::Symbol i, uint64_t v) { payloads[i.Row()] = v; }
	void Symbols::Payload(ir::Symbol i, uint32_t low, uint32_t high) { Payload(i, (static_cast<uint64_t>(high) << 32) | low); }
	void Symbols::PayloadLow(ir::Symbol i, uint32_t v) { Payload(i, v, PayloadHigh(i)); }
	void Symbols::PayloadHigh(ir::Symbol i, uint32_t v) { Payload(i, PayloadLow(i), v); }
	void Symbols::Offset(ir::Symbol i, uint32_t v) { offsets[i.Row()] = v; }
	void Symbols::Parent(ir::Symbol i, ir::Symbol v) { parents[i.Row()] = v; }
	void Symbols::Name(ir::Symbol i, ir::Symbol v) { names[i.Row()] = v; }
	void Symbols::Token(ir::Symbol i, uint32_t v) { tokens[i.Row()] = v; }
	void Symbols::Flag(ir::Symbol i, ir::symbol::Flag f) { flags[i.Row()] |= (1ull << static_cast<uint64_t>(f)); }

	bool Symbols::IsValid(ir::Symbol i) const {
		return i.Row() < Count();
	}

	bool Symbols::IsScope(ir::Symbol i) const {
		switch (Kind(i)) {
			case ir::symbol::Kind::Function:
			case ir::symbol::Kind::Module:
			case ir::symbol::Kind::If:
			case ir::symbol::Kind::ElseIf:
			case ir::symbol::Kind::Else:
			case ir::symbol::Kind::When:
			case ir::symbol::Kind::For:
			case ir::symbol::Kind::Repeat:
			case ir::symbol::Kind::While:
			case ir::symbol::Kind::Do:
				return true;
			default:
				return false;
		}
	}

	bool Symbols::IsLoop(ir::Symbol i) const {
		switch (Kind(i)) {
			case ir::symbol::Kind::For:
			case ir::symbol::Kind::While:
			case ir::symbol::Kind::Repeat:
				return true;
			default:
				return false;
		}
	}

	bool Symbols::IsContext(ir::Symbol i) const {
		switch (Kind(i)) {
			case ir::symbol::Kind::Function:
			case ir::symbol::Kind::Module:
				return true;
			default:
				return false;
		}
	}

	bool Symbols::IsStructured(ir::Symbol i) const {
		switch (Kind(i)) {
			case ir::symbol::Kind::Function:
			case ir::symbol::Kind::Module:
			case ir::symbol::Kind::Object:
			case ir::symbol::Kind::Array:
			case ir::symbol::Kind::Enum:
			case ir::symbol::Kind::Tuple:
			case ir::symbol::Kind::TemplateString:
			case ir::symbol::Kind::Error:
			case ir::symbol::Kind::If:
			case ir::symbol::Kind::ElseIf:
			case ir::symbol::Kind::Else:
			case ir::symbol::Kind::When:
			case ir::symbol::Kind::For:
			case ir::symbol::Kind::Repeat:
			case ir::symbol::Kind::While:
			case ir::symbol::Kind::Do:
				return true;
			default:
				return false;
		}
	}

	ir::Symbol Symbols::FirstChild(ir::Symbol i) const {
		if (!IsStructured(i)) return ir::Symbol{};

		ir::Symbol next{i.Row() + 1};
		if (IsValid(next)) return next;

		return ir::Symbol{};
	}

	ir::Symbol Symbols::LastChild(ir::Symbol i) const {
		if (!IsStructured(i)) return ir::Symbol{};

		uint64_t value = Payload(i);
		uint32_t end = static_cast<uint32_t>(value >> 32);

		return ir::Symbol{end};
	}

	ir::Symbol Symbols::Scope(ir::Symbol i) const {
		if (IsScope(i)) return i;

		ir::Symbol parent = Parent(i);
		if (parent) return Scope(parent);

		return ir::Symbol{};
	}

	ir::Symbol Symbols::Loop(ir::Symbol i) const {
		if (IsLoop(i)) return i;

		auto parent = Parent(i);
		if (parent) return Loop(parent);

		return ir::Symbol{};
	}

	ir::Symbol Symbols::Context(ir::Symbol i) const {
		if (IsContext(i)) return i;

		auto parent = Parent(i);
		if (parent) return Context(parent);

		return ir::Symbol{};
	}

	ir::Symbol Symbols::Add(ir::symbol::Kind type) {
		ir::Symbol index{static_cast<uint32_t>(payloads.size())}; // Take the size of any column, they should all be the same

		kinds.emplace_back(type);
		flags.emplace_back(0);
		payloads.push_back(0);
		offsets.push_back(0);
		parents.push_back({});
		names.push_back({});
		tokens.push_back(0);

		return index;
	}

	ir::Symbol Symbols::Copy(ir::Symbol source) {
		ir::Symbol index{static_cast<uint32_t>(payloads.size())}; // Take the size of any column, they should all be the same

		kinds.emplace_back(kinds[source.Row()]);
		flags.emplace_back(flags[source.Row()]);
		payloads.push_back(payloads[source.Row()]);
		offsets.push_back(offsets[source.Row()]);

		parents.push_back({});
		names.push_back({});
		tokens.push_back(0);

		return index;
	}

	size_t Symbols::Count() const { return payloads.size(); }

	void Symbols::Resize(size_t n) {
		kinds.resize(n);
		flags.resize(n);
		payloads.resize(n);
		offsets.resize(n);
		parents.resize(n);
		names.resize(n);
		tokens.resize(n);
	}
}
