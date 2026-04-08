#include "morpion_core.h"

#include <algorithm>
#include <cassert>

namespace morpion {

int Board::count(Cell who) const { return static_cast<int>(std::count(c.begin(), c.end(), who)); }
int Board::empties() const { return static_cast<int>(std::count(c.begin(), c.end(), Cell::Empty)); }

static constexpr int kLines[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // cols
    {0, 4, 8}, {2, 4, 6}             // diags
};

bool hasLineWin(const Board& b, Cell who) {
  for (auto& line : kLines) {
    if (b.c[line[0]] == who && b.c[line[1]] == who && b.c[line[2]] == who) return true;
  }
  return false;
}

TerminalInfo terminal(const Board& b) {
  bool xWin = hasLineWin(b, Cell::X);
  bool oWin = hasLineWin(b, Cell::O);
  if (xWin && !oWin) return {true, Cell::X};
  if (oWin && !xWin) return {true, Cell::O};
  if (xWin && oWin) return {true, std::nullopt}; // invalid, but treat as terminal
  if (b.empties() == 0) return {true, std::nullopt};
  return {false, std::nullopt};
}

bool isValidConfiguration(const Board& b) {
  int x = b.count(Cell::X);
  int o = b.count(Cell::O);
  if (std::abs(x - o) > 1) return false;

  bool xWin = hasLineWin(b, Cell::X);
  bool oWin = hasLineWin(b, Cell::O);
  if (xWin && oWin) return false;

  // If someone won, ensure move counts correspond to that player having just played.
  if (xWin) {
    if (x != o + 1) return false;
  }
  if (oWin) {
    if (o != x) return false;
  }

  return true;
}

std::vector<Move> legalMoves(const Board& b) {
  std::vector<Move> moves;
  moves.reserve(9);
  if (terminal(b).terminal) return moves;
  for (int i = 0; i < 9; ++i) {
    if (b.c[i] == Cell::Empty) moves.push_back({i});
  }
  return moves;
}

Board applyMove(const Board& b, Move m) {
  Board out = b;
  assert(m.idx >= 0 && m.idx < 9);
  assert(out.c[m.idx] == Cell::Empty);
  out.c[m.idx] = b.toPlay;
  out.toPlay = opponent(b.toPlay);
  return out;
}

std::string cellChar(Cell c) {
  switch (c) {
    case Cell::Empty:
      return ".";
    case Cell::X:
      return "X";
    case Cell::O:
      return "O";
  }
  return "?";
}

} // namespace morpion

