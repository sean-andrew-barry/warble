// Functions to print the registers
void PrintRAX(uint64_t r) { utility::Print("RAX is:", r); }
void PrintRBX(uint64_t r) { utility::Print("RBX is:", r); }
void PrintRCX(uint64_t r) { utility::Print("RCX is:", r); }
void PrintRDX(uint64_t r) { utility::Print("RDX is:", r); }
void PrintRSI(uint64_t r) { utility::Print("RSI is:", r); }
void PrintRDI(uint64_t r) { utility::Print("RDI is:", r); }
void PrintRSP(uint64_t r) { utility::Print("RSP is:", r); }
void PrintRBP(uint64_t r) { utility::Print("RBP is:", r); }
void PrintR8 (uint64_t r) { utility::Print("R8 is:" , r); }
void PrintR9 (uint64_t r) { utility::Print("R9 is:" , r); }
void PrintR10(uint64_t r) { utility::Print("R10 is:", r); }
void PrintR11(uint64_t r) { utility::Print("R11 is:", r); }
void PrintR12(uint64_t r) { utility::Print("R12 is:", r); }
void PrintR13(uint64_t r) { utility::Print("R13 is:", r); }
void PrintR14(uint64_t r) { utility::Print("R14 is:", r); }
void PrintR15(uint64_t r) { utility::Print("R15 is:", r); }
    
    void PushArgs() {
      // Base case for recursion - do nothing
    }

    template<typename T, typename... Args>
    void PushArgs(T arg, Args... args) {
      if constexpr (sizeof(T) == 1) {
        PUSH_IMM8(static_cast<uint8_t>(arg));
      } else if constexpr (sizeof(T) == 2) {
        PUSH_IMM16(static_cast<uint16_t>(arg));
      } else if constexpr (sizeof(T) == 4) {
        PUSH_IMM32(static_cast<uint32_t>(arg));
      } else if constexpr (sizeof(T) == 8) {
        MOV_RAX_IMM64(static_cast<uint64_t>(arg));
        PUSH_RAX();
      }

      PushArgs(args...);
    }

    template<typename T, typename... Args>
    void PopArgs(T arg, Args... args) {
      PopArgs(args...);

      if constexpr (sizeof(T) == 1) {
        ADD_R64_IMM8(SGPR64::RSP, 1);
      } else if constexpr (sizeof(T) == 2) {
        ADD_R64_IMM8(SGPR64::RSP, 2);
      } else if constexpr (sizeof(T) == 4) {
        ADD_R64_IMM8(SGPR64::RSP, 4);
      } else if constexpr (sizeof(T) == 8) {
        ADD_R64_IMM8(SGPR64::RSP, 8);
      }
    }

    void MoveArgs() {}

    void MoveArgs(auto a) {
      if constexpr (utility::WINDOWS) {
        MOV_RCX_IMM64(static_cast<uint64_t>(a));
      } else if constexpr (utility::UNIX) {
        MOV_RDI_IMM64(static_cast<uint64_t>(a));
      }
    }

    void MoveArgs(auto a, auto b) {
      if constexpr (utility::WINDOWS) {
        MOV_RCX_IMM64(static_cast<uint64_t>(a));
        MOV_RDX_IMM64(static_cast<uint64_t>(b));
      } else if constexpr (utility::UNIX) {
        MOV_RDI_IMM64(static_cast<uint64_t>(a));
        MOV_RSI_IMM64(static_cast<uint64_t>(b));
      }
    }

    void MoveArgs(auto a, auto b, auto c) {
      if constexpr (utility::WINDOWS) {
        MOV_RCX_IMM64(static_cast<uint64_t>(a));
        MOV_RDX_IMM64(static_cast<uint64_t>(b));
        MOV_R8_IMM64(static_cast<uint64_t>(c));
      } else if constexpr (utility::UNIX) {
        MOV_RDI_IMM64(static_cast<uint64_t>(a));
        MOV_RSI_IMM64(static_cast<uint64_t>(b));
        MOV_RDX_IMM64(static_cast<uint64_t>(c));
      }
    }

    void MoveArgs(auto a, auto b, auto c, auto d) {
      if constexpr (utility::WINDOWS) {
        MOV_RCX_IMM64(static_cast<uint64_t>(a));
        MOV_RDX_IMM64(static_cast<uint64_t>(b));
        MOV_R8_IMM64(static_cast<uint64_t>(c));
        MOV_R9_IMM64(static_cast<uint64_t>(d));
      } else if constexpr (utility::UNIX) {
        MOV_RDI_IMM64(static_cast<uint64_t>(a));
        MOV_RSI_IMM64(static_cast<uint64_t>(b));
        MOV_RDX_IMM64(static_cast<uint64_t>(c));
        MOV_RCX_IMM64(static_cast<uint64_t>(d));
      }
    }

    void MoveArgs(auto a, auto b, auto c, auto d, auto e) {
      if constexpr (utility::WINDOWS) {
        MOV_RCX_IMM64(static_cast<uint64_t>(a));
        MOV_RDX_IMM64(static_cast<uint64_t>(b));
        MOV_R8_IMM64(static_cast<uint64_t>(c));
        MOV_R9_IMM64(static_cast<uint64_t>(d));

        PushArgs(e); // Push the last onto the stack
      } else if constexpr (utility::UNIX) {
        MOV_RDI_IMM64(static_cast<uint64_t>(a));
        MOV_RSI_IMM64(static_cast<uint64_t>(b));
        MOV_RDX_IMM64(static_cast<uint64_t>(c));
        MOV_RCX_IMM64(static_cast<uint64_t>(d));
        MOV_R8_IMM64(static_cast<uint64_t>(e));
      }
    }

    void MoveArgs(auto a, auto b, auto c, auto d, auto e, auto f, auto... args) {
      if constexpr (utility::WINDOWS) {
        MOV_RCX_IMM64(static_cast<uint64_t>(a));
        MOV_RDX_IMM64(static_cast<uint64_t>(b));
        MOV_R8_IMM64(static_cast<uint64_t>(c));
        MOV_R9_IMM64(static_cast<uint64_t>(d));

        PushArgs(e, f, args...); // If there were more than 4 pass them on the stack
      } else if constexpr (utility::UNIX) {
        MOV_RDI_IMM64(static_cast<uint64_t>(a));
        MOV_RSI_IMM64(static_cast<uint64_t>(b));
        MOV_RDX_IMM64(static_cast<uint64_t>(c));
        MOV_RCX_IMM64(static_cast<uint64_t>(d));
        MOV_R8_IMM64(static_cast<uint64_t>(e));
        MOV_R9_IMM64(static_cast<uint64_t>(f));

        PushArgs(args...); // If there were more than 6 pass them on the stack
      }
    }

    void ClearArgs() {}
    void ClearArgs(auto a) {}
    void ClearArgs(auto a, auto b) {}
    void ClearArgs(auto a, auto b, auto c) {}
    void ClearArgs(auto a, auto b, auto c, auto d) {}

    void ClearArgs(auto a, auto b, auto c, auto d, auto e) {
      if constexpr (utility::WINDOWS) {
        PopArgs(e); // If there were more than 4 pop them off the stack
      }
    }

    // If there were more than 6, pop them off the stack
    void ClearArgs(auto a, auto b, auto c, auto d, auto e, auto f, auto... args) {
      if constexpr (utility::WINDOWS) {
        PopArgs(e, f, args...); // If there were more than 4 pop them off the stack
      } else if constexpr (utility::UNIX) {
        PopArgs(args...); // If there were more than 6, pop them off the stack
      }
    }

    void Call(void* func, auto... args) {
      // Save registers that will be used for arguments
      PUSH_RAX();

      constexpr size_t X = sizeof...(args);
      
      if constexpr (utility::WINDOWS) {
        if constexpr (X >= 1) PUSH_RCX();
        if constexpr (X >= 2) PUSH_RDX();
        if constexpr (X >= 3) PUSH_R8();
        if constexpr (X >= 4) PUSH_R9();
      } else if constexpr (utility::UNIX) {
        if constexpr (X >= 1) PUSH_RDI();
        if constexpr (X >= 2) PUSH_RSI();
        if constexpr (X >= 3) PUSH_RDX();
        if constexpr (X >= 4) PUSH_RCX();
        if constexpr (X >= 5) PUSH_R8();
        if constexpr (X >= 6) PUSH_R9();
      }

      // Move arguments to the appropriate registers
      MoveArgs(args...);

      // Align the stack to a 16-byte boundary before the call
      SUB_R64_IMM8(RSP, 8);

      // Call the function
      MOV_RAX_IMM64(reinterpret_cast<uint64_t>(func));
      CALL_R64(RAX);

      // Restore the stack alignment
      ADD_R64_IMM8(RSP, 8);

      // Clear the arguments from the stack
      ClearArgs(args...);

      // Restore registers
      if constexpr (utility::WINDOWS) {
        if constexpr (X >= 4) POP_R9();
        if constexpr (X >= 3) POP_R8();
        if constexpr (X >= 2) POP_RDX();
        if constexpr (X >= 1) POP_RCX();
      } else if constexpr (utility::UNIX) {
        if constexpr (X >= 6) POP_R9();
        if constexpr (X >= 5) POP_R8();
        if constexpr (X >= 4) POP_RCX();
        if constexpr (X >= 3) POP_RDX();
        if constexpr (X >= 2) POP_RSI();
        if constexpr (X >= 1) POP_RDI();
      }
      
      POP_RAX();
    }

    template<size_t X>
    void CallX(void* func) {
      // Pop the parameters into the registers
      if constexpr (utility::WINDOWS) {
        if constexpr (X >= 1) POP_RCX();
        if constexpr (X >= 2) POP_RDX();
        if constexpr (X >= 3) POP_R8();
        if constexpr (X >= 4) POP_R9();
      } else if constexpr (utility::UNIX) {
        if constexpr (X >= 1) POP_RDI();
        if constexpr (X >= 2) POP_RSI();
        if constexpr (X >= 3) POP_RDX();
        if constexpr (X >= 4) POP_RCX();
        if constexpr (X >= 5) POP_R8();
        if constexpr (X >= 6) POP_R9();
      }

      PUSH_RAX(); // Save RAX

      // Align the stack to a 16-byte boundary before the call
      SUB_R64_IMM8(RSP, 8);

      // Call the function
      MOV_RAX_IMM64(reinterpret_cast<uint64_t>(func));
      CALL_R64(RAX);

      // Restore the stack alignment
      ADD_R64_IMM8(RSP, 8);

      POP_RAX(); // Restore RAX

      // Restore registers
      if constexpr (utility::WINDOWS) {
        if constexpr (X >= 4) PUSH_R9();
        if constexpr (X >= 3) PUSH_R8();
        if constexpr (X >= 2) PUSH_RDX();
        if constexpr (X >= 1) PUSH_RCX();
      } else if constexpr (utility::UNIX) {
        if constexpr (X >= 6) PUSH_R9();
        if constexpr (X >= 5) PUSH_R8();
        if constexpr (X >= 4) PUSH_RCX();
        if constexpr (X >= 3) PUSH_RDX();
        if constexpr (X >= 2) PUSH_RSI();
        if constexpr (X >= 1) PUSH_RDI();
      }
    }

    void PrintRAX() { PUSH_RAX(); CallX<1>(::PrintRAX); POP_RAX(); }
    void PrintRBX() { PUSH_RBX(); CallX<1>(::PrintRBX); POP_RBX(); }
    void PrintRCX() { PUSH_RCX(); CallX<1>(::PrintRCX); POP_RCX(); }
    void PrintRDX() { PUSH_RDX(); CallX<1>(::PrintRDX); POP_RDX(); }
    void PrintRSI() { PUSH_RSI(); CallX<1>(::PrintRSI); POP_RSI(); }
    void PrintRDI() { PUSH_RDI(); CallX<1>(::PrintRDI); POP_RDI(); }
    void PrintRSP() { PUSH_RSP(); CallX<1>(::PrintRSP); POP_RSP(); }
    void PrintRBP() { PUSH_RBP(); CallX<1>(::PrintRBP); POP_RBP(); }
    void PrintR8() { PUSH_R8(); CallX<1>(::PrintR8); POP_R8(); }
    void PrintR9() { PUSH_R9(); CallX<1>(::PrintR9); POP_R9(); }
    void PrintR10() { PUSH_R10(); CallX<1>(::PrintR10); POP_R10(); }
    void PrintR11() { PUSH_R11(); CallX<1>(::PrintR11); POP_R11(); }
    void PrintR12() { PUSH_R12(); CallX<1>(::PrintR12); POP_R12(); }
    void PrintR13() { PUSH_R13(); CallX<1>(::PrintR13); POP_R13(); }
    void PrintR14() { PUSH_R14(); CallX<1>(::PrintR14); POP_R14(); }
    void PrintR15() { PUSH_R15(); CallX<1>(::PrintR15); POP_R15(); }