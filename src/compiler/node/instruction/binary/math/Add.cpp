import node.instruction.binary.math.add;
import node.declaration;
import visitor;
import updater;
import code.encoder;
import code.x64.encoder;
import utility.print;

namespace node::instruction::binary::math {
  void Add::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Add::Update(Updater& updater) {}
  int32_t Add::Size() const { return 0; }
  std::string Add::Name() const { return "Add"; }
  // std::string Add::ToString() const { return Name(); }
  std::string Add::ToString() const { return utility::Line(Name(), " (", LHS() ? LHS()->Name() : "null", " + ", RHS() ? RHS()->Name() : "null", ")"); }

  void Add::Declare(code::Encoder& encoder) {
    
  }

  void Add::Compile(code::Encoder& encoder) {
    
  }

  void Add::Compile(code::x64::Encoder& encoder) {
    // // Load lhs and rhs values into registers
    // encoder.MOV_R64_M64(code::x64::RAX, code::x64::Memory64{LHS()->Displacement()});
    // encoder.MOV_R64_M64(code::x64::RCX, code::x64::Memory64{RHS()->Displacement()});
    
    // // Perform the addition
    // encoder.ADD_R64_R64(code::x64::RAX, code::x64::RCX);
    
    // // Store the result back into memory
    // encoder.MOV_M64_R64(code::x64::Memory64{Result()->Displacement()}, code::x64::RAX);
    // // encoder.CMP_R64_IMM32(code::x64::RAX, 8);
    // // encoder.JNZ_REL8(1);
    // // encoder.INT3();
  }
};