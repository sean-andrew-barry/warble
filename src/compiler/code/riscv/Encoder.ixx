export module code.riscv.encoder;

import code.encoder;
import code.flag;
import code._register;
import code.address;
import code.riscv.registers;
import <string>;

namespace code::riscv {
  export class Encoder : public code::Template<Encoder> {
  private:
  private:
    uint32_t IMM_I(int32_t imm) { return (imm & 0xFFF) << 20; }
    uint32_t IMM_S(int32_t imm) { return ((imm & 0xFE0) << 20) | ((imm & 0x1F) << 7); }
    uint32_t IMM_B(int32_t imm) { return ((imm & 0x1000) << 19) | ((imm & 0x7E0) << 20) | ((imm & 0x1E) << 7) | ((imm & 0x800) >> 4); }
    uint32_t IMM_U(int32_t imm) { return imm & 0xFFFFF000; }
    uint32_t IMM_J(int32_t imm) { return ((imm & 0x100000) << 11) | ((imm & 0x7FE) << 20) | ((imm & 0x800) << 9) | ((imm & 0xFF000)); }

    uint32_t RD(uint8_t rd) { return (rd & 0x1F) << 7; }
    uint32_t RS1(uint8_t rs1) { return (rs1 & 0x1F) << 15; }
    uint32_t RS2(uint8_t rs2) { return (rs2 & 0x1F) << 20; }
    uint32_t FUNCT3(uint8_t funct3) { return (funct3 & 0x7) << 12; }
    uint32_t FUNCT7(uint8_t funct7) { return (funct7 & 0x7F) << 25; }
  protected:
    virtual Register Bind(Register r) final {
      // r.Value();
      return r;
    }

    virtual Address Bind(Address a) final {
      return a;
    }
  public:
    void ADD(uint8_t rd, uint8_t rs1, uint8_t rs2) {
      Emit32(0x33 | RD(rd) | RS1(rs1) | RS2(rs2) | FUNCT3(0x0) | FUNCT7(0x00));
    }

    void ADDI(uint8_t rd, uint8_t rs1, int32_t imm) {
      Emit32(0x13 | RD(rd) | RS1(rs1) | IMM_I(imm) | FUNCT3(0x0));
    }

    void STORE(uint8_t rs1, uint8_t rs2, int32_t imm) {
      Emit32(0x23 | RS1(rs1) | RS2(rs2) | IMM_S(imm) | FUNCT3(0x0));
    }

    void BRANCH(uint8_t rs1, uint8_t rs2, int32_t imm) {
      Emit32(0x63 | RS1(rs1) | RS2(rs2) | IMM_B(imm) | FUNCT3(0x0));
    }
  };
};