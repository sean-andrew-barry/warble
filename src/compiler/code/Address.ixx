export module code.address;

import code.flag;
// import code._register;
import code.operand;
import code.frame;

import <cstdint>;
import <limits>;

namespace node {
  export class Declaration;
  export class Scope;
};

namespace code {
  export class Address : public OperandTemplate<Address> {
  private:
    code::Address* base{nullptr};
    code::Address* index{nullptr};
    int32_t displacement{std::numeric_limits<int32_t>::max()};
    uint8_t id{std::numeric_limits<uint8_t>::max()}; // Hardware specific register index, only meaningful to the encoder
    uint8_t uses{0};

    // uint8_t index{std::numeric_limits<uint8_t>::max()}; // Hardware specific index register, is optional
    // uint8_t shift{0}; // The hardware specific number of bits to shift when patching
    // int32_t immediate; // TODO: Would it make sense to allow an address to just be an immediate?
    // Oh, what if displacement can be the immediate based on flags?
  public:
    constexpr Address(code::Address* base)
      : Super{}, base{base} {}

    constexpr Address(code::Flag f, code::Address* base)
      : Super{f}, base{base} {}

    constexpr Address(code::Flag f, int32_t displacement)
      : Super{f}, displacement{displacement} {}

    constexpr Address(code::Flag f, int32_t displacement, uint8_t id)
      : Super{f}, displacement{displacement}, id{id} {}

    constexpr Address(code::Flag f, code::Address* base, int32_t displacement)
      : Super{f}, base{base}, index{nullptr}, displacement{displacement} {}

    constexpr Address(code::Flag f, code::Address* base, code::Address* index)
      : Super{f}, base{base}, index{index} {}

    constexpr Address(code::Flag f, code::Address* base, code::Address* index, int32_t displacement)
      : Super{f}, base{base}, index{index}, displacement{displacement} {}

    constexpr Address(const Address& other, code::Flag f)
      : Super{f}, base{other.base}, index{other.index}, displacement{other.displacement}, id{other.id} {}

    // Address(node::Declaration& declaration);
    // Address(node::Scope& scope);
    // Address(node::Scope& source, node::Scope& target);
    // Address(node::Scope& scope, uint32_t displacement);
    // Address(node::Scope& source, node::Scope& target, uint32_t displacement);

    constexpr Address(const Address& other) = default;
    constexpr Address& operator=(const Address& other) = delete;

    // bool IsSourceResolved() const { return source.Resolved(); }
    // bool IsTargetResolved() const { return target.Resolved(); }
    bool IsDisplacementResolved() const { return displacement != std::numeric_limits<int32_t>::max(); }
    bool IsBaseResolved() const { return base ? base->IsResolved() : true; }
    bool IsIndexResolved() const { return index ? index->IsResolved() : true; }
    // bool IsBaseResolved() const { return base != std::numeric_limits<uint8_t>::max(); }
    // bool IsIndexResolved() const { return index != std::numeric_limits<uint8_t>::max(); }

    bool IsResolved() const { return IsDisplacementResolved() && IsBaseResolved() && IsIndexResolved(); }

    bool IsUnresolved() const { return !IsResolved(); }
    explicit operator bool() const { return IsResolved(); }

    code::Address* Base() { return base; }
    code::Address* Index() { return index; }
    code::Address* Base() const { return base; }
    code::Address* Index() const { return index; }
    int32_t Displacement() const { return displacement; }
    // constexpr uint8_t Base() const { return base; }
    // constexpr uint8_t Index() const { return index; }
    // constexpr uint8_t Shift() const { return shift; }
    // constexpr bool HasTarget() const { return &source != &target; }

    constexpr void Displacement(int32_t v) { displacement = v; }

    uint8_t ID() const { return id; }
    void ID(uint8_t v) { id = v; }
    void Deallocate() { id = std::numeric_limits<uint8_t>::max(); }
    bool IsAllocated() const { return id != std::numeric_limits<uint8_t>::max(); }

    // constexpr void Base(uint8_t v) { base = v; }
    // constexpr void Index(uint8_t v) { index = v; }
    // constexpr void Shift(uint8_t v) { shift = v; }

    constexpr Address To8Bit() const {
      Address address{*this};
      address.Resize8();
      return address;
    }

    constexpr Address To16Bit() const {
      Address address{*this};
      address.Resize16();
      return address;
    }

    constexpr Address To32Bit() const {
      Address address{*this};
      address.Resize32();
      return address;
    }

    constexpr Address To64Bit() const {
      Address address{*this};
      address.Resize64();
      return address;
    }

    constexpr Address To128Bit() const {
      Address address{*this};
      address.Resize128();
      return address;
    }

    constexpr Address To256Bit() const {
      Address address{*this};
      address.Resize256();
      return address;
    }

    constexpr Address To512Bit() const {
      Address address{*this};
      address.Resize512();
      return address;
    }

    // int64_t Calculate() const;
  };
};