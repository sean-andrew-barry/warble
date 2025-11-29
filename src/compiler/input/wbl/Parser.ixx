export module compiler.input.wbl.Parser;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.ir.Token;

namespace compiler::input::wbl {
  export class Parser {
  public:
    virtual ~Parser() final = default;

    constexpr int Precedence(ir::Token t) {
      switch (t) {
        // Property / member access (highest)
        case ir::Token::MemberReference: // x.y
        case ir::Token::MutableMemberReference: // x:y
        case ir::Token::OptionalMemberReference: // x?.y
        case ir::Token::OptionalMutableMemberReference: // x?:y
          return 20;

        // Function calls
        case ir::Token::Call: // x -> y
        case ir::Token::ParameterOpen: // x(){ return y; }
        case ir::Token::CaptureOpen: // x[]{ return y; }
        case ir::Token::TupleOpen: // x(y)
        case ir::Token::ArrayOpen: // x[y]
        case ir::Token::ObjectOpen: // x{y}
        case ir::Token::EnumOpen: // x<y>
        case ir::Token::CharOpen: // x'y'
        case ir::Token::StringOpen: // x"y"
        case ir::Token::TemplateStringOpen: // x`y`
          return 19;

        // Unary prefix operators
        case ir::Token::BitwiseNot: // ~x
        case ir::Token::Increment: // ++x
        case ir::Token::Positive: // +x
        case ir::Token::Decrement: // --x
        case ir::Token::Negative: // -x
        case ir::Token::MutableReference: // *x
        case ir::Token::Reference: // &x
        case ir::Token::Symbol: // $x
        case ir::Token::Counted: // #x
        case ir::Token::Copy: // @x
        case ir::Token::Not: // !x
        case ir::Token::Move: // =x (unary move)
        case ir::Token::Spread: // ...x (prefix spread)
          return 18;

        // Await / yield style (kept distinct if needed)
        case ir::Token::Await: // await x
        case ir::Token::Yield: // yield x (when treated unary)
          return 17;

        case ir::Token::ExponentOperator: // x ** y
          return 16;

        // Multiplicative
        case ir::Token::Multiply: // x * y
        case ir::Token::Divide: // x / y
        case ir::Token::Modulo: // x % y
          return 15;

        // Additive
        case ir::Token::Add: // x + y
        case ir::Token::Subtract: // x - y
          return 14;

        // Shifts
        case ir::Token::BitwiseLeftShift: // x << y
        case ir::Token::BitwiseRightShift: // x >> y
        case ir::Token::BitwiseTripleLeftShift: // x <<< y
        case ir::Token::BitwiseTripleRightShift: // x >>> y
          return 13;

        // Relational
        case ir::Token::Lesser: // x < y
        case ir::Token::LesserOrEqual: // x <= y
        case ir::Token::Greater: // x > y
        case ir::Token::GreaterOrEqual: // x >= y
        case ir::Token::AssertLesserOrEqual: // x <== y
        case ir::Token::AssertGreaterOrEqual: // x >== y
          return 11;

        // Equality
        case ir::Token::Equal: // x == y
        case ir::Token::NotEqual: // x != y
        case ir::Token::AssertEqual: // x === y
        case ir::Token::AssertNotEqual: // x !== y
          return 10;

        // Bitwise logical
        case ir::Token::BitwiseAnd: // x & y
          return 9;
        case ir::Token::BitwiseXor: // x ^ y
          return 8;
        case ir::Token::BitwiseOr: // x | y
          return 7;

        // Logical
        case ir::Token::And: // x && y
          return 6;
        case ir::Token::Or: // x || y
        case ir::Token::Unwrap: // x ?? y (truthy or / unwrap)
          return 5;

        // Range construction
        case ir::Token::Range: // x..y
          return 4;

        // Assignment family
        case ir::Token::Assign: // =
        case ir::Token::AssignAnd: // &&=
        case ir::Token::AssignOr: // ||=
        case ir::Token::AssignTruthyAnd: // !!=
        case ir::Token::AssignTruthyOr: // ??=
        case ir::Token::AssignOptional: // ?=
        case ir::Token::AssignAdd: // +=
        case ir::Token::AssignSubtract: // -=
        case ir::Token::AssignMultiply: // *=
        case ir::Token::AssignDivide: // /=
        case ir::Token::AssignModulo: // %=
        case ir::Token::AssignExponent: // **=
        case ir::Token::BitwiseAssignAnd: // &=
        case ir::Token::BitwiseAssignXor: // ^=
        case ir::Token::BitwiseAssignOr: // |=
        case ir::Token::BitwiseAssignLeftShift: // <<=
        case ir::Token::BitwiseAssignRightShift: // >>=
        case ir::Token::BitwiseAssignTripleLeftShift: // <<<=
        case ir::Token::BitwiseAssignTripleRightShift: // >>>=
          return 3;

        // Spread already handled above as unary (18). Keep Yield again for safety.
        default:
          return -1; // None / unhandled token has no precedence
      }
    }
  };
};