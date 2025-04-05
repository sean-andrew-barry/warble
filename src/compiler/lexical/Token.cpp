import lexical.token;
import lexical.tokens;
import lexical.cursor.token;

import <string_view>;
import <string>;

namespace lexical {
  std::string_view Token::Name() const { return lexical::cursor::Token::Name(type); }
  std::string Token::ToString() const { return std::string{Name()}; }

  bool Token::IsTrivial() const { return lexical::cursor::Token::IsTrivial(type); }
  bool Token::IsLiteral() const { return lexical::cursor::Token::IsLiteral(type); }
  bool Token::IsEscape() const { return lexical::cursor::Token::IsEscape(type); }
  bool Token::IsContextual() const { return lexical::cursor::Token::IsContextual(type); }
  bool Token::IsImplicit() const { return lexical::cursor::Token::IsImplicit(type); }
  bool Token::IsBinary() const { return lexical::cursor::Token::IsBinary(type); }
  bool Token::IsUnaryPrefix() const { return lexical::cursor::Token::IsUnaryPrefix(type); }
  bool Token::IsUnaryPostfix() const { return lexical::cursor::Token::IsUnaryPostfix(type); }
  bool Token::IsUnary() const { return lexical::cursor::Token::IsUnary(type); }
  bool Token::IsStatement() const { return lexical::cursor::Token::IsStatement(type); }
  bool Token::IsModifier() const { return lexical::cursor::Token::IsModifier(type); }
  bool Token::IsType() const { return lexical::cursor::Token::IsType(type); }
  bool Token::IsSymbol() const { return lexical::cursor::Token::IsSymbol(type); }
  bool Token::IsMisc() const { return lexical::cursor::Token::IsMisc(type); }
  bool Token::IsIdentifiable() const { return lexical::cursor::Token::IsIdentifiable(type); }
  bool Token::IsDeclarable() const { return lexical::cursor::Token::IsDeclarable(type); }
  bool Token::IsRightAssociative() const { return lexical::cursor::Token::IsRightAssociative(type); }

  int Token::Precedence() const { return lexical::cursor::Token::Precedence(type); }
};