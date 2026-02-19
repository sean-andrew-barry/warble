from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


UNICODE_MAX = 0x10FFFF
SIZE = (UNICODE_MAX + 1 + 63) // 64  # == 17408
U64_MAX = (1 << 64) - 1


@dataclass(frozen=True)
class Range:
  start: int
  end: int


def Parse_ranges(file_text: str, property_name: str) -> list[Range]:
  ranges: list[Range] = []

  for line in file_text.splitlines():
    line = line.strip()
    if not line or line.startswith("#"):
      continue

    # Remove trailing comments.
    if "#" in line:
      line = line.split("#", 1)[0].strip()
      if not line:
        continue

    # Expected format: CODEPOINT[ ..CODEPOINT ] ; PROPERTY
    parts = [p.strip() for p in line.split(";")]
    if len(parts) < 2:
      continue

    code_spec = parts[0]
    prop = parts[1]
    if prop != property_name:
      continue

    if ".." in code_spec:
      start_s, end_s = [p.strip() for p in code_spec.split("..", 1)]
    else:
      start_s = code_spec.strip()
      end_s = start_s

    start = int(start_s, 16)
    end = int(end_s, 16)
    if start > end:
      start, end = end, start

    if end > UNICODE_MAX:
      raise ValueError(f"Range ends past Unicode max: {start_s}..{end_s}")

    ranges.append(Range(start=start, end=end))

  return ranges


def Apply_range(bitmap: list[int], start: int, end: int) -> None:
  """Set bits for all code points in [start, end] inclusive."""
  if start < 0 or end < 0 or start > UNICODE_MAX or end > UNICODE_MAX:
    raise ValueError("Range out of bounds")
  if start > end:
    return

  start_word = start >> 6
  end_word = end >> 6
  start_bit = start & 63
  end_bit = end & 63

  if start_word == end_word:
    mask_width = end_bit - start_bit + 1
    mask = ((1 << mask_width) - 1) << start_bit
    bitmap[start_word] |= mask
    bitmap[start_word] &= U64_MAX
    return

  # First partial word.
  bitmap[start_word] |= (U64_MAX << start_bit) & U64_MAX
  # Middle full words.
  for i in range(start_word + 1, end_word):
    bitmap[i] = U64_MAX
  # Last partial word.
  bitmap[end_word] |= (1 << (end_bit + 1)) - 1
  bitmap[end_word] &= U64_MAX


def Build_bitmap(ranges: list[Range]) -> list[int]:
  bitmap = [0] * SIZE
  for r in ranges:
    Apply_range(bitmap, r.start, r.end)
  return bitmap


def Format_u64(value: int) -> str:
  if value == 0:
    return "0"
  if value == U64_MAX:
    return "M"
  return f"0x{value:X}"


def Format_initializer(bitmap: list[int]) -> str:
  return "{" + ",".join(Format_u64(v) for v in bitmap) + "}"

def _generate_unicode_tables_ixx(*, xid_start: str, xid_continue: str) -> str:
  # Keep this layout stable: the initializer lists themselves contain no whitespace/newlines.
  return (
    "export module compiler.text.UnicodeTables;\n\n"
    "// Generated file - DO NOT EDIT\n\n"
    "import <array>;\n"
    "import <cstdint>;\n\n"
    "inline constexpr uint32_t UNICODE_MAX = 0x10FFFF;\n"
    "inline constexpr size_t SIZE = (UNICODE_MAX + 1 + 63) / 64; // == 17408\n"
    "inline constexpr uint64_t M = ~0ULL; // Max constant, makes it a bit more compact, not that it matters much.\n\n"
    "export namespace compiler::text::UnicodeTables {\n"
    f"  inline constexpr alignas(64) std::array<uint64_t, SIZE> XID_Start = {xid_start};\n\n"
    f"  inline constexpr alignas(64) std::array<uint64_t, SIZE> XID_Continue = {xid_continue};\n"
    "}\n"
  )


def Main() -> int:
  script_dir = Path(__file__).resolve().parent
  data_path = script_dir / "DerivedCoreProperties.txt"
  out_path = script_dir.parent.parent / "src" / "compiler" / "text" / "UnicodeTables.ixx"

  data_text = data_path.read_text(encoding="utf-8")

  # Use XID_* (recommended for programming-language identifiers).
  xid_start_ranges = Parse_ranges(data_text, "XID_Start")
  xid_continue_ranges = Parse_ranges(data_text, "XID_Continue")

  start_bitmap = Build_bitmap(xid_start_ranges)
  continue_bitmap = Build_bitmap(xid_continue_ranges)

  start_init = Format_initializer(start_bitmap)
  continue_init = Format_initializer(continue_bitmap)

  out_text = _generate_unicode_tables_ixx(xid_start=start_init, xid_continue=continue_init)
  out_path.write_text(out_text, encoding="utf-8")
  return 0


if __name__ == "__main__":
  raise SystemExit(Main())