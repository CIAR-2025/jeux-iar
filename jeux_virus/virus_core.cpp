#include "virus_core.h"

#include <algorithm>
#include <sstream>

namespace virus {

GameState::GameState(int n_) : n(n_), board(static_cast<size_t>(n_ * n_), Cell::Empty) {
  // Initial positions: two whites and two blacks in opposite corners.
  // Match the figure: Black at (0,0) and (n-1,n-1); White at (0,n-1) and (n-1,0).
  set(0, 0, Cell::Black);
  set(n - 1, n - 1, Cell::Black);
  set(0, n - 1, Cell::White);
  set(n - 1, 0, Cell::White);
  toPlay = Cell::White;
}

int GameState::count(Cell who) const {
  return static_cast<int>(std::count(board.begin(), board.end(), who));
}

int GameState::emptyCount() const {
  return static_cast<int>(std::count(board.begin(), board.end(), Cell::Empty));
}

static bool adjacent8(int r1, int c1, int r2, int c2) {
  int dr = std::abs(r1 - r2);
  int dc = std::abs(c1 - c2);
  return (dr <= 1 && dc <= 1) && !(dr == 0 && dc == 0);
}

bool GameState::isLegal(Cell who, Move m) const {
  if (!inBounds(m.r, m.c)) return false;
  if (at(m.r, m.c) != Cell::Empty) return false;

  // Must be adjacent (8-neighborhood) to one of current player's existing pieces.
  for (int r = 0; r < n; ++r) {
    for (int c = 0; c < n; ++c) {
      if (at(r, c) == who && adjacent8(r, c, m.r, m.c)) return true;
    }
  }
  return false;
}

std::vector<Move> GameState::legalMoves(Cell who) const {
  std::vector<Move> moves;
  // Speed-up: gather candidate empties by scanning neighbors around own pieces.
  std::vector<uint8_t> seen(static_cast<size_t>(n * n), 0);
  for (int r = 0; r < n; ++r) {
    for (int c = 0; c < n; ++c) {
      if (at(r, c) != who) continue;
      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          if (dr == 0 && dc == 0) continue;
          int nr = r + dr, nc = c + dc;
          if (!inBounds(nr, nc)) continue;
          if (at(nr, nc) != Cell::Empty) continue;
          int idx = nr * n + nc;
          if (seen[static_cast<size_t>(idx)]) continue;
          seen[static_cast<size_t>(idx)] = 1;
          moves.push_back({nr, nc});
        }
      }
    }
  }
  return moves;
}

GameState GameState::apply(Cell who, Move m) const {
  GameState out = *this;
  out.toPlay = opponent(who);
  out.set(m.r, m.c, who);

  // Flip all adjacent enemy pieces around the placed piece.
  Cell opp = opponent(who);
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      int nr = m.r + dr, nc = m.c + dc;
      if (!inBounds(nr, nc)) continue;
      if (out.at(nr, nc) == opp) out.set(nr, nc, who);
    }
  }
  return out;
}

bool GameState::terminal() const {
  if (emptyCount() == 0) return true;
  // If nobody can play, also terminal (avoids infinite passes).
  return legalMoves(Cell::White).empty() && legalMoves(Cell::Black).empty();
}

std::string GameState::winnerString() const {
  int w = count(Cell::White);
  int b = count(Cell::Black);
  if (w > b) return "Blanc";
  if (b > w) return "Noir";
  return "Égalité";
}

std::string cellChar(Cell c) {
  switch (c) {
    case Cell::Empty:
      return ".";
    case Cell::White:
      return "O";
    case Cell::Black:
      return "X";
  }
  return "?";
}

} // namespace virus

