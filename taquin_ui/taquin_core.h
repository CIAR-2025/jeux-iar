#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace taquin {

// 0 = empty
using Board = std::array<int, 9>;

enum class Dir : uint8_t { Left = 0, Right = 1, Up = 2, Down = 3 };

struct Move {
  Dir dir; // direction the empty cell moves
};

// Compact board encoding (9 nibbles).
uint64_t encode(const Board& b);
Board decode(uint64_t code);

bool isGoal(const Board& b, const Board& goal);
int manhattan(const Board& b, const Board& goal);

// For 3x3: solvable iff inversion parity matches goal.
bool isSolvable(const Board& start, const Board& goal);

std::vector<Move> legalMoves(const Board& b);
Board applyMove(const Board& b, Move m);

std::string dirToString(Dir d);

} // namespace taquin

