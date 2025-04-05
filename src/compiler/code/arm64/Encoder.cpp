namespace code::arm64 {
  void Encoder::Move(const Register& res, const Register& lhs) override { MOV(res, lhs, res); }

  void Encoder::MoveIfEqual(const Register& res, const Register& lhs) override { CSELEQ(res, lhs, res); }

  void Encoder::MoveIfNotEqual(const Register& res, const Register& lhs) override { CSELNE(res, lhs, res); }

  void Encoder::MoveIfCarry(const Register& res, const Register& lhs) override { CSELCS(res, lhs, res); }

  void Encoder::MoveIfNotCarry(const Register& res, const Register& lhs) override { CSELCC(res, lhs, res); }

  void Encoder::MoveIfNegative(const Register& res, const Register& lhs) override { CSELMI(res, lhs, res); }

  void Encoder::MoveIfPositive(const Register& res, const Register& lhs) override { CSELPL(res, lhs, res); }

  void Encoder::MoveIfOverflow(const Register& res, const Register& lhs) override { CSELVS(res, lhs, res); }

  void Encoder::MoveIfNotOverflow(const Register& res, const Register& lhs) override { CSELVC(res, lhs, res); }

  void Encoder::MoveIfGreaterThanOrEqual(const Register& res, const Register& lhs) override { CSELGE(res, lhs, res); }

  void Encoder::MoveIfGreaterThan(const Register& res, const Register& lhs) override { CSELGT(res, lhs, res); }

  void Encoder::MoveIfLessThanOrEqual(const Register& res, const Register& lhs) override { CSELLE(res, lhs, res); }

  void Encoder::MoveIfLessThan(const Register& res, const Register& lhs) override { CSELLT(res, lhs, res); }
};