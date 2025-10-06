import compiler.input.Lexer;
import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.symbol.Type;
import compiler.text.cursor.String;
import compiler.text.Unicode;

namespace compiler::input {
  void Parser::Skip() {
    while (!cursor.Done()) {
      switch (cursor.Peek()) {
        case ir::Token::Spaces:
        case ir::Token::Tabs: {
          widths_index += 1;
          cursor.Advance();
          break;
        }
        case ir::Token::LineFeeds:
        case ir::Token::CarriageReturnLineFeeds:
        case ir::Token::CarriageReturns:
        case ir::Token::LineSeparators:
        case ir::Token::ParagraphSeparators: {
          // Advance the lines by the width
          lines_index += mod.Widths()[widths_index];
          widths_index += 1;
          cursor.Advance();
          break;
        }
        case ir::Token::CommentOpen: {
          cursor.Advance();
          
          while (!cursor.Done()) {
            if (cursor.Peek() == ir::Token::CommentClose) {
              cursor.Advance();
              break;
            } else if (cursor.Peek() == ir::Token::Comment) {
              symbols_index += 1;
            } else if (cursor.Peek() == ir::Token::Characters) {
              widths_index += 1;
            }

            cursor.Advance();
          }
          break;
        }
        case ir::Token::MultiLineCommentOpen: {
          cursor.Advance();
          
          while (!cursor.Done()) {
            if (cursor.Peek() == ir::Token::MultiLineCommentClose) {
              cursor.Advance();
              break;
            } else if (cursor.Peek() == ir::Token::MultiLineComment) {
              symbols_index += 1;
            } else if (cursor.Peek() == ir::Token::Characters) {
              widths_index += 1;
            }

            cursor.Advance();
          }
          break;
        }
        case ir::Token::VerticalTab:
        case ir::Token::FormFeed: {
          // No special behavior for these, just skip
          cursor.Advance();
          break;
        }
        default:
          return; // Not a trivial token
      }
    }
  }

  bool Parser::Match(ir::Token token) {
    Skip();
    return cursor.Match(token);
  }

  bool Parser::Expect(ir::Token token) {
    if (!Match(token)) {
      // TODO: Handle error
    }

    return true;
  }

  ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type) {
    ir::Index index = mod.AddSymbol();
    index.Parent(mod, parent);
    index.Type(mod, type);

    // Link into parent's children list if parent is structured
    if (parent.IsStructured(mod)) {
      auto last = parent.LastChild(mod);
      if (last.IsValid(mod)) {
        // There is already at least one child, link after it
        uint64_t value = last.Value(mod);
        value &= 0xFFFFFFFFu; // Keep the start of the range
        value |= (static_cast<uint64_t>(index.Value()) << 32); // Set the end of the range to this new symbol
        last.Value(mod, value);
      } else {
        // This is the first child, set both start and end to this new symbol
        uint64_t value = static_cast<uint64_t>(index.Value());
        value |= (static_cast<uint64_t>(index.Value()) << 32);
        parent.Value(mod, value);
      }
    }

    return index;
  }

  ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, uint64_t value) {
    ir::Index index = Create(parent, type);
    index.Value(mod, value);

    return index;
  }

  ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, double value) {
    ir::Index index = Create(parent, type);
    index.Value(mod, static_cast<uint64_t>(value));

    return index;
  }

  ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, char32_t value) {
    ir::Index index = Create(parent, type);
    index.Value(mod, static_cast<uint64_t>(value));

    return index;
  }

  ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, bool value) {
    ir::Index index = Create(parent, type);
    index.Value(mod, static_cast<uint64_t>(value));

    return index;
  }

  void Parser::Instruct(ir::Opcode opcode, ir::Index res, ir::Index lhs, ir::Index rhs) {
    mod.AddInstruction(opcode, res, lhs, rhs);
  }

  ir::Index Parser::Undefined(ir::Index parent) {
    Match(ir::Token::Undefined);

    return ir::Index{}; // The module always reserves index 0 for the `undefined` symbol
  }

  ir::Index Parser::Null(ir::Index parent) {
    // A required `Null` token
    // Return a `Void` symbol (yes, we use the `Void` type for `null`)
    // The lexer doesn't create these ones

    Match(ir::Token::Null);

    return Create(parent, ir::symbol::Type::Void);
  }

  ir::Index Parser::Boolean(ir::Index parent) {
    // A required `True` or `False` token
    // Return a `Boolean` symbol with the value set to true or false
    // The lexer doesn't create these ones
  }

  ir::Index Parser::Character(ir::Index parent) {
    // A required `CharacterOpen` token
    // One `Characters` OR one of the various escape tokens. The `Characters` is a variable width token, so we need to advance the widths index
    // One required `Character` token marking the location of the symbol that was already created by the lexer, advance the symbol index
    // A required `CharacterClose` token
    // Return the symbol index created by the lexer
  }

  ir::Index Parser::Integer(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Decimal(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::String(ir::Index parent) {
    // A required `StringOpen` token
    // Zero or more `Characters` or the various escape tokens. The `Characters` are variable width tokens, so we need to advance the widths index for each one we see
    // One required `String` token marking the location of the symbol that was already created by the lexer, advance the symbol index
    // A required `StringClose` token
    // Return the symbol index created by the lexer
  }

  ir::Index Parser::Array(ir::Index parent) {
    // A required `ArrayOpen` token
    // Create an `Array` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ArrayClose` token
    // Return the symbol
  }

  ir::Index Parser::Enum(ir::Index parent) {
    // A required `EnumOpen` token
    // Create an `Enum` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `EnumClose` token
    // Return the symbol
  }

  ir::Index Parser::Tuple(ir::Index parent) {
    // A required `TupleOpen` token
    // Create a `Tuple` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `TupleClose` token
    // Return the symbol
  }

  ir::Index Parser::Object(ir::Index parent) {
    // A required `ObjectOpen` token
    // Create an `Object` symbol to serve as the parent of the subsequent `ObjectDeclaration` groups
    // Zero or more `ObjectDeclaration` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ObjectClose` token
    // Return the symbol
  }

  ir::Index Parser::TemplateString(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Function(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Identifier(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Destructuring(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Error(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Expression(ir::Index parent) {
    // Quite complex, will discuss more later
  }

  ir::Index Parser::ObjectDeclaration(ir::Index parent) {
    // TODO: Declarations inside an object literal have slightly different rules
  }

  ir::Index Parser::Declaration(ir::Index parent) {
    // A `Declaration` is different from the `DeclarationStatement` statement, as it can appear in other places such as function parameters or destructured objects/arrays
    // It has slightly different rules and does not end with a semicolon

    // *Zero* or more modifier tokens, such as `Let`, `Const`, `Mutable`, `Static`, `Private`, `Public`, or `Protected`
    // A required `Identifier` group OR a `Destructuring` group
    // An optional type annotation, beginning with a `TypeStart` token, then a required `Expression` group
    // An optional assignment, beginning with an `Assign` token, then a required `Expression` group

    // NOTE: Unsure what we return here. If there was an expression, we return that symbol. If not, do we create a new `Declaration` symbol? Or just return the identifier/destructuring symbol? This will need more thought.
  }

  // Statements
  ir::Index Parser::ImportStatement(ir::Index parent) {
    Match(ir::Token::Import);

    // A required `Import` token
    // An optional `Identifier` or a destructured object, this can be skipped to import the module for side effects only
    // A required `From` token
    // A required `String` group
    // An optional `In` token, followed by a required `String` group for a target package
    // A required `Semicolon` token

    // NOTE: Unsure what we return here, if anything. Does it make sense to have an `Import` symbol?
  }

  ir::Index Parser::Permissions(ir::Index parent) {
    // A required `With` token
    // A required `Array` group
    // A required `In` token
    // A required `String` group specifying the package name this permission is granting access to
  }

  ir::Index Parser::RegisterStatement(ir::Index parent) {
    Match(ir::Token::Register);

    // A required `Register` token
    // A required `String` group naming the package
    // A required `From` token
    // A required `String` group setting the package path/url
    // Zero or more `Permissions` groups
    // A required `Semicolon` token

    // NOTE: Unsure what we return here, if anything. Does it make sense to have a `Register` symbol?
  }

  ir::Index Parser::BreakStatement(ir::Index parent) {
    // One or more stacked `Break` tokens, each one targeting the next enclosing loop
    // A required `Semicolon` token

    // NOTE: Unsure what we return here, if anything
  }

  ir::Index Parser::ContinueStatement(ir::Index parent) {
    // One or more stacked `Continue` tokens, each one targeting the next enclosing loop
    // A required `Semicolon` token

    Match(ir::Token::Continue);

    ir::Index loop = parent;
    while (Match(ir::Token::Continue)) {
      loop = loop.Loop(mod);

      // TODO: Error if no loop found
    }

    // TODO: Error if `loop` isn't `IsLoop()`

    // Create the instruction to jump to the correct loop
    Instruct(ir::Opcode::Continue, loop);

    // I don't believe this produces a new symbol, so no index to return
    return ir::Index{};
  }

  ir::Index Parser::ReturnStatement(ir::Index parent) {
    // A required `Return` token
    // An optional `Expression` group
    // A required `Semicolon` token
  }

  ir::Index Parser::DoStatement(ir::Index parent) {
    // A required `Do` token
    // Create a new `Do` symbol to serve as the parent of the subsequent groups
    // A required `Scope` group
    // An optional `Semicolon` token
    // Return the symbol
  }

  ir::Index Parser::WhileStatement(ir::Index parent) {
    // A required `While` token
    // Create a new `While` symbol to serve as the parent of the subsequent groups
    // A required `Condition` group
    // A required `Scope` group
    // An optional `Semicolon` token
    // Return the symbol
  }

  ir::Index Parser::RepeatStatement(ir::Index parent) {
    // A required `Repeat` token
    // Create a new `Repeat` symbol to serve as the parent of the subsequent groups
    // A required `Scope` group
    // An optional `While` token, followed by a required `Condition` group
    // NOTE: If a `While` was found, upgrade the symbol type to `RepeatWhile` instead of `Repeat`
    // An optional `Semicolon` token
    // Return the symbol
  }

  ir::Index Parser::ForStatement(ir::Index parent) {
    // A required `For` token
    // Create a new `For` symbol to serve as the parent of the subsequent groups
    // A required `ConditionOpen` token
    // A required `Declaration` group for the loop variable
    // A required `In` token
    // A required `Expression` group for the iterable
    // A required `ConditionClose` token
    // A required `Scope` group
    // An optional `Semicolon` token
    // Return the symbol
  }

  ir::Index Parser::ElseIfStatement(ir::Index parent) {
    // A required `Else` token
    // A required `If` token
    // Create a new `ElseIf` symbol to serve as the parent of the subsequent groups
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol
  }

  ir::Index Parser::ElseStatement(ir::Index parent) {
    // A required `Else` token
    // Create a new `Else` symbol to serve as the parent of the subsequent groups
    // A required `Scope` group
    // Return the symbol
  }

  ir::Index Parser::IfStatement(ir::Index parent) {
    // A required `If` token
    // Create a new `If` symbol to serve as the parent of the subsequent groups
    // A required `Condition` group
    // A required `Scope` group
    // Zero or more `ElseIf` groups
    // An optional trailing `Else` group
    // An optional `Semicolon` token
    // Return the symbol
  }

  ir::Index Parser::IsStatement(ir::Index parent) {
    // A required `Is` token
    // Create a new `Is` symbol
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol

    // TODO: `Is` and `Has` are suppose to be able to be chained without a scope block, allowing any number of them to share a single scope block
  }

  ir::Index Parser::HasStatement(ir::Index parent) {
    // A required `Has` keyword
    // Create a new `Has` symbol
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol

    // TODO: `Is` and `Has` are suppose to be able to be chained without a scope block, allowing any number of them to share a single scope block
  }

  ir::Index Parser::DefaultStatement(ir::Index parent) {
    // A required `Default` keyword
    // Create a new `Default` symbol
    // A required `Scope` group
    // Return the symbol
  }

  ir::Index Parser::MatchStatement(ir::Index parent) {
    // A required `Match` token
    // Create a new `Match` symbol
    // A required `Condition` group
    // A required `ScopeOpen` token
    // Zero or more `IsStatement` or `HasStatement` groups and no more than one `DefaultStatement` group, in any order
    // A required `ScopeClose` token
    // An optional `Semicolon` token
    // Return the symbol

    // NOTE: We do not use a `Scope` group here because the contents are not normal statements
  }

  ir::Index Parser::DeclarationStatement(ir::Index parent) {
    // One or more modifier tokens, such as `Let`, `Const`, `Mutable`, `Static`, `Private`, `Public`, `Protected`, or `Export`
    // A required identifier
    // An optional type annotation, beginning with a `TypeStart` token then a required expression
    // A required assignment, beginning with `Assign` token then an `Expression` group
    // A required `Semicolon` token
    // Return the expression symbol created by the assignment

    // NOTE: We do not create a symbol for the declaration itself, we use the symbol from the expression
  }

  ir::Index Parser::ExpressionStatement(ir::Index parent) {
    // A required `Expression` group
    // A required `Semicolon` token
    // Return the expression symbol
  }

  ir::Index Parser::Statement(ir::Index parent) {
    switch (cursor.Peek()) {
      case ir::Token::Import: return ImportStatement(parent);
      case ir::Token::Register: return RegisterStatement(parent);
      case ir::Token::Break: return BreakStatement(parent);
      case ir::Token::Continue: return ContinueStatement(parent);
      case ir::Token::Return: return ReturnStatement(parent);
      case ir::Token::Do: return DoStatement(parent);
      case ir::Token::While: return WhileStatement(parent);
      case ir::Token::Repeat: return RepeatStatement(parent);
      case ir::Token::For: return ForStatement(parent);
      case ir::Token::If: return IfStatement(parent);
      case ir::Token::Match: return MatchStatement(parent);

      // Modifier keywords that begin a declaration statement
      case ir::Token::Let:
      case ir::Token::Const:
      case ir::Token::Mutable:
      case ir::Token::Static:
      case ir::Token::Private:
      case ir::Token::Public:
      case ir::Token::Protected:
      case ir::Token::Export: return DeclarationStatement(parent);
      default: return ExpressionStatement(parent);
    }
  }

  ir::Index Parser::Scope(ir::Index parent) {
    // A required `ScopeOpen` token
    // Zero or more `Statement` groups
    // A required `ScopeClose` token
    // We do NOT handle a semicolon here, that is the responsibility of the caller
    // NOTE: Unsure what we return here. Perhaps a default `ir::Index{}` since the statements are linked to the parent already?
  }

  ir::Index Parser::Condition(ir::Index parent) {
    // A required `ConditionOpen` token
    // A required `Expression` group
    // A required `ConditionClose` token
    // Return the expression symbol
  }
};