#include "taquin_core.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <unordered_map>

namespace taquin {

static int indexOf(const Board& b, int value) {
  for (int i = 0; i < 9; ++i) {
    if (b[i] == value) return i;
  }
  return -1;
}

uint64_t encode(const Board& b) {
  uint64_t code = 0;
  for (int i = 0; i < 9; ++i) {
    code |= (static_cast<uint64_t>(b[i] & 0xF) << (i * 4));
  }
  return code;
}

Board decode(uint64_t code) {
  Board b{};
  for (int i = 0; i < 9; ++i) {
    b[i] = static_cast<int>((code >> (i * 4)) & 0xF);
  }
  return b;
}

bool isGoal(const Board& b, const Board& goal) { return b == goal; }

int manhattan(const Board& b, const Board& goal) {
  std::array<int, 9> goalPos{};
  for (int i = 0; i < 9; ++i) goalPos[goal[i]] = i;

  int dist = 0;
  for (int i = 0; i < 9; ++i) {
    int v = b[i];
    if (v == 0) continue;
    int gi = goalPos[v];
    int r1 = i / 3, c1 = i % 3;
    int r2 = gi / 3, c2 = gi % 3;
    dist += std::abs(r1 - r2) + std::abs(c1 - c2);
  }
  return dist;
}

static int inversionParityIgnoringZero(const Board& b) {
  int inv = 0;
  for (int i = 0; i < 9; ++i) {
    if (b[i] == 0) continue;
    for (int j = i + 1; j < 9; ++j) {
      if (b[j] == 0) continue;
      if (b[i] > b[j]) ++inv;
    }
  }
  return inv & 1;
}

bool isSolvable(const Board& start, const Board& goal) {
  // For odd width (3), solvable iff inversion parity matches between start & goal.
  return inversionParityIgnoringZero(start) == inversionParityIgnoringZero(goal);
}

std::vector<Move> legalMoves(const Board& b) {
  std::vector<Move> moves;
  int z = indexOf(b, 0);
  assert(z >= 0);
  int r = z / 3, c = z % 3;
  if (c > 0) moves.push_back({Dir::Left});
  if (c < 2) moves.push_back({Dir::Right});
  if (r > 0) moves.push_back({Dir::Up});
  if (r < 2) moves.push_back({Dir::Down});
  return moves;
}

Board applyMove(const Board& b, Move m) {
  Board out = b;
  int z = indexOf(out, 0);
  int r = z / 3, c = z % 3;
  int nr = r, nc = c;
  switch (m.dir) {
    case Dir::Left:
      nc = c - 1;
      break;
    case Dir::Right:
      nc = c + 1;
      break;
    case Dir::Up:
      nr = r - 1;
      break;
    case Dir::Down:
      nr = r + 1;
      break;
  }
  int nz = nr * 3 + nc;
  std::swap(out[z], out[nz]);
  return out;
}

std::string dirToString(Dir d) {
  switch (d) {
    case Dir::Left:
      return "Gauche";
    case Dir::Right:
      return "Droite";
    case Dir::Up:
      return "Haut";
    case Dir::Down:
      return "Bas";
  }
  return "?";
}

} // namespace taquin

