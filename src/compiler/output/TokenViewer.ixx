export module compiler.output.TokenViewer;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;

import compiler.ir.Token;

namespace compiler::output {
  export class TokenViewer {
  private:
    std::string output;
    std::vector<ir::Token> tokens;
    std::vector<uint32_t> data;
    std::vector<ir::Token>::iterator token_position; // Current token index
    std::vector<uint32_t>::iterator data_position; // Current data index
  public:
    TokenViewer(std::vector<ir::Token>&& tokens, std::vector<uint32_t>&& data)
      : tokens{std::move(tokens)}
      , data{std::move(data)}
      , token_position(this->tokens.begin())
      , data_position(this->data.begin())
    {
      output.reserve(this->tokens.size() * 10);
      Print();
    }
    
    virtual ~TokenViewer() = default;

    void Append(const std::string_view str) const {
      output.append(str);
      output.append("\n");
    }

    void Process() const {
      switch (*token_position) {
        case ir::Token::Spaces0: return Append("Spaces0");
        case ir::Token::Spaces1: return Append("Spaces1");
        case ir::Token::Spaces2: return Append("Spaces2");
        case ir::Token::Spaces3: return Append("Spaces3");
        case ir::Token::Spaces4: return Append("Spaces4");
        case ir::Token::Spaces5: return Append("Spaces5");
        case ir::Token::Spaces6: return Append("Spaces6");
        case ir::Token::Spaces7: return Append("Spaces7");
        case ir::Token::Spaces8: return Append("Spaces8");
        case ir::Token::Spaces9: return Append("Spaces9");
        case ir::Token::SpacesA: return Append("SpacesA");
        case ir::Token::SpacesB: return Append("SpacesB");
        case ir::Token::SpacesC: return Append("SpacesC");
        case ir::Token::SpacesD: return Append("SpacesD");
        case ir::Token::SpacesE: return Append("SpacesE");
        case ir::Token::SpacesF: return Append("SpacesF");

        case ir::Token::None: return Append("None");
        case ir::Token::Identifier: return Append("Identifier");
        case ir::Token::Error: return Append("Error");

        default: return Append("UnknownToken");
      }
    }

    void Print() {
      while (token_position != tokens.end()) {
        Process();
        ++token_position;
      }
    }
  };
};