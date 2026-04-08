#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace morpion {

enum class Cell : int8_t { Empty = 0, X = 1, O = -1 }; // X=MAX, O=MIN

inline Cell opponent(Cell c) { return c == Cell::X ? Cell::O : Cell::X; }

struct Move {
  int idx = 0; // 0..8
};

struct Board {
  std::array<Cell, 9> c{};
  Cell toPlay = Cell::X;

  Cell at(int r, int col) const { return c[r * 3 + col]; }
  void setIdx(int idx, Cell v) { c[idx] = v; }
  int count(Cell who) const;
  int empties() const;
};

struct TerminalInfo {
  bool terminal = false;
  std::optional<Cell> winner; // X/O, nullopt for draw or non-terminal
};

TerminalInfo terminal(const Board& b);
bool hasLineWin(const Board& b, Cell who);

// Validity per subject:
// - |#X - #O| <= 1
// - cannot have both winners simultaneously
// - if someone has won, no extra moves afterwards (counts must match who just played)
bool isValidConfiguration(const Board& b);

std::vector<Move> legalMoves(const Board& b);
Board applyMove(const Board& b, Move m);

std::string cellChar(Cell c);

} // namespace morpion

