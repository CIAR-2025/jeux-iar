#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace virus {

constexpr int kDefaultN = 7;

enum class Cell : int8_t { Empty = 0, White = 1, Black = -1 };

inline Cell opponent(Cell c) { return c == Cell::White ? Cell::Black : Cell::White; }

struct Move {
  int r = 0;
  int c = 0;
};

struct GameState {
  int n = kDefaultN;
  std::vector<Cell> board; // n*n
  Cell toPlay = Cell::White;

  GameState(int n_ = kDefaultN);

  Cell at(int r, int c) const { return board[r * n + c]; }
  void set(int r, int c, Cell v) { board[r * n + c] = v; }

  int count(Cell who) const;
  int emptyCount() const;

  bool inBounds(int r, int c) const { return r >= 0 && c >= 0 && r < n && c < n; }

  std::vector<Move> legalMoves(Cell who) const;
  bool isLegal(Cell who, Move m) const;
  GameState apply(Cell who, Move m) const; // place + flip neighbors

  bool terminal() const; // board full OR no moves for both players
  std::string winnerString() const; // "Blanc", "Noir", or "Égalité"
};

std::string cellChar(Cell c);

} // namespace virus

