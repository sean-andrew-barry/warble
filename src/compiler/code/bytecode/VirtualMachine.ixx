export module code.bytecode.virtual_machine;

import utility.macros;
import utility.print;
import code.registers;
import code.bytecode;

import <cstddef>;
import <array>;
import <vector>;
import <string>;
import <stdexcept>;
import <type_traits>;
import <algorithm>;
import <iterator>;
import <bit>;
import <bitset>;
import <iostream>;

// namespace code {
//   export class Bytecode;
// };

namespace code::bytecode {
  export class VirtualMachine {
  private:
    constexpr static size_t STACK_SIZE = 1024 * 1024;
    std::array<std::byte, STACK_SIZE> stack; // No clue how big the stack should actually be
    std::array<uint64_t, 16> registers;

    std::vector<std::byte> code;
    std::vector<std::byte>::const_iterator ip; // Instruction pointer
  public:
    VirtualMachine()
      : stack{}, registers{}, code{}, ip{}
    {
      SetReg(code::Registers::RSP, stack.size());
      SetReg(code::Registers::RBP, stack.size());
    }

    VirtualMachine(std::vector<std::byte>&& code)
      : stack{}, registers{}, code{std::move(code)}, ip{}
    {
      SetReg(code::Registers::RSP, stack.size());
      SetReg(code::Registers::RBP, stack.size());
    }

    void Reserve(uint64_t bytes) {
      SetReg(code::Registers::RSP, GetReg(code::Registers::RSP) - bytes);
    }

    void Release(uint64_t bytes) {
      SetReg(code::Registers::RSP, GetReg(code::Registers::RSP) + bytes);
    }

    template<typename T = uint64_t>
    T Read(int64_t offset, code::Registers source = code::Registers::RBP) {
      auto base = GetReg<uint64_t>(source);
      auto address = base - offset;

      if (address < 0 || address >= stack.size()) {
        throw std::runtime_error("Stack access violation");
      }

      T value;

      // Copy the bytes from the stack into `value`
      std::memcpy(&value, stack.data() + address, sizeof(T));

      return value; // Return the read value
    }

    template<typename T = uint64_t>
    void Write(int64_t offset, const T& value, code::Registers source = code::Registers::RBP) {
      auto base = GetReg<uint64_t>(source);
      auto address = base - offset;

      if (address < 0 || address >= stack.size()) {
        throw std::runtime_error("Stack access violation");
      }

      // Copy the bytes of value into the stack at the offset
      std::memcpy(stack.data() + address, &value, sizeof(T));
    }

    template<typename T>
    void Push(const T& value) {
      constexpr size_t bytes = sizeof(T);

      auto rsp = GetReg(code::Registers::RSP);

      // Check if there's enough space on the stack
      if (bytes > rsp) {
        throw std::runtime_error("Pushing a " + std::to_string(bytes) + " byte value would overflow the stack");
      }

      // Move the RSP back to allocate space
      SetReg(code::Registers::RSP, rsp - bytes);

      // Copy the bytes of value into the stack at the new stack pointer position
      std::memcpy(stack.data() + rsp - bytes, &value, bytes);
    }

    template<typename T = uint64_t>
    T Pop() {
      constexpr size_t bytes = sizeof(T);

      auto rsp = GetReg(code::Registers::RSP);

      // Check if there's enough data to pop
      if (bytes > rsp) {
        throw std::runtime_error("Popping a " + std::to_string(bytes) + " byte value would underflow the stack");
      }

      T value; // Create a temporary value of type T

      // Copy the bytes from the stack into `value`
      std::memcpy(&value, stack.data() + rsp, bytes);

      // Adjust the stack pointer after retrieving the value
      SetReg(code::Registers::RSP, rsp + bytes);

      utility::Print("Popped", value);

      return value; // Return the popped value
    }

    template<typename T = uint64_t>
    void SetReg(code::Registers index, T value) {
      if constexpr (utility::DEBUGGING) {
        if (static_cast<uint8_t>(index) >= registers.size()) {
          throw std::runtime_error("SetReg index out of bounds");
        }
      }

      uint64_t converted = 0;
      if constexpr (sizeof(T) == sizeof(uint64_t)) {
        // Direct reinterpretation for same-size types
        converted = std::bit_cast<uint64_t>(value);
      } else {
        // Place value in the lower bits of the register
        std::memcpy(&converted, &value, sizeof(T));
      }

      registers[static_cast<uint8_t>(index)] = converted;
    }

    template<typename T = uint64_t>
    T GetReg(code::Registers index) const {
      if constexpr (utility::DEBUGGING) {
        if (static_cast<uint8_t>(index) >= registers.size()) {
          throw std::runtime_error("GetReg index out of bounds");
        }
      }

      if constexpr (sizeof(T) == sizeof(uint64_t)) {
        // Direct reinterpretation for same-size types
        return std::bit_cast<T>(registers[static_cast<uint8_t>(index)]);
      } else {
        // Extract relevant bits for smaller types
        T result;
        std::memcpy(&result, &registers[static_cast<uint8_t>(index)], sizeof(T));
        return result;
      }
    }

    void SetIP(auto address) {
      if constexpr (utility::DEBUGGING) {
        if (address >= code.size()) {
          throw std::runtime_error("SetIP address out of bounds");
        }
      }

      ip = code.cbegin() + address;
    }

    std::vector<std::byte>::const_iterator GetIP() const { return ip; }
    // void GetIP() const { return ip; }

    uint64_t ReadIP() const {
      return std::distance(code.cbegin(), ip);
    }

    void OffsetIP(size_t offset) {
      if constexpr (utility::DEBUGGING) {
        size_t newPos = std::distance(code.cbegin(), ip) + offset; // Calculate new position relative to the beginning
        if (newPos < 0 || newPos > code.size()) {
          throw std::runtime_error("OffsetIP calculation out of bounds. Offset is " + std::to_string(offset) + " and new position would be " + std::to_string(newPos) + " while the size is " + std::to_string(code.size()));
        }
      }

      ip += offset;
    }

    std::vector<std::byte>& GetCode() { return code; }

    template<typename T>
    void Decode() {
      static_assert(std::is_base_of<code::Bytecode, T>::value, "Decoded instruction must derive from the Bytecode struct");
      static_assert(std::is_trivially_copyable<T>::value, "Decoded instruction must be trivially copyable");

      constexpr size_t bytes = sizeof(T);
      const T* value = reinterpret_cast<const T*>(&*ip);

      // Print("Decoding a", T::Name());

      if constexpr (utility::DEBUGGING) {
        size_t distance = std::distance(ip, code.cend());
        if (bytes > distance) {
          throw std::runtime_error("Decoding " + std::to_string(bytes) + " bytes would go past the end");
        }
      }

      ip += bytes; // Advance the instruction pointer
      value->Run(*this);
    }

    template<typename T, typename... Args>
    void Encode(Args&&... args) {
      static_assert(std::is_base_of<code::Bytecode, T>::value, "Encoded instruction must derive from the Bytecode::Base struct");
      static_assert(std::is_trivially_copyable<T>::value, "Encoded instruction must be trivially copyable");

      T instruction{std::forward<Args>(args)...};

      const auto* ptr = reinterpret_cast<const std::byte*>(&instruction);
      code.insert(code.end(), ptr, ptr + sizeof(T));
    }
    
    void NextExtended();
    void Next();
    void Run();

    // void Compile(NumberLiteralDeclaration&);
    // void Compile(Instruction::Construct&);
  };
};