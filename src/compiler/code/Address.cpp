import code.address;
import node.declaration;
import node.scope;
import node.scope.context;

import <stdexcept>;

namespace code {
  // Address::Address(node::Declaration& declaration)
  //   : Address{Flag::RSP(), declaration.Scope().Frame(), declaration.Displacement()} {}

  // Address::Address(node::Scope& scope)
  //   : Super{Flag::RIP()}, source{scope.Frame()}, target{source}, displacement{static_cast<uint32_t>(source.Size())} {}

  // Address::Address(node::Scope& source, node::Scope& target)
  //   : Super{Flag::RIP()}, source{source.Frame()}, target{target.Frame()}, displacement{static_cast<uint32_t>(source.Size())} {}

  // Address::Address(node::Scope& scope, uint32_t displacement)
  //   : Super{Flag::RIP()}, source{scope.Frame()}, target{source}, displacement{displacement} {}

  // Address::Address(node::Scope& source, node::Scope& target, uint32_t displacement)
  //   : Super{Flag::RIP()}, source{source.Frame()}, target{target.Frame()}, displacement{displacement} {}

  // int64_t Address::Calculate() const {
  //   if (IsRSP()) {
  //     if (!IsDisplacementResolved()) return 0;
  //     else return static_cast<int64_t>(target.Width()) - displacement;
  //   } else if (IsRBP()) {
  //     if (!IsDisplacementResolved()) return 0;
  //     else return displacement;
  //   } else if (IsRIP()) {
  //     if (!IsSourceResolved()) return 0;
  //     if (!IsTargetResolved()) return 0;

  //     return static_cast<int64_t>(target.Position()) - (static_cast<int64_t>(source.Position()) + displacement);
  //   } else {
  //     // return 0;
  //     throw std::runtime_error("Unsupported addressing mode");
  //   }
  // }
};