export module code.modifiers;

import <cstdint>;
import <bitset>;

namespace code {
  export class Modifiers {
  private:
    std::bitset<16> modifiers;
  public:
    void Let(bool v);
    void Const(bool v);
    void Mutable(bool v);
    void Static(bool v);
    void Private(bool v);
    void Public(bool v);
    void Protected(bool v);
    void Export(bool v);
    void Comtime(bool v);
    void Runtime(bool v);
    void ConstFunction(bool v);
    void AsyncFunction(bool v);
    void CaseFunction(bool v);

    bool Let() const;
    bool Const() const;
    bool Mutable() const;
    bool Static() const;
    bool Private() const;
    bool Public() const;
    bool Protected() const;
    bool Export() const;
    bool Comtime() const;
    bool Runtime() const;
    bool ConstFunction() const;
    bool AsyncFunction() const;
    bool CaseFunction() const;

    bool Any() const;
  };
};