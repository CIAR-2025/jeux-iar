#pragma once

#include "morpion_core.h"

#include <cstdint>
#include <optional>

namespace morpion {

enum class Difficulty : int { Beginner = 0, Medium = 1, Expert = 2 };

struct AiStats {
  int depth = 0;
  int nodes = 0;
  int cutoffs = 0;
};

struct AiResult {
  std::optional<Move> move;
  int score = 0; // from MAX (X) perspective by default
  AiStats stats;
};

// Returns best move for the side to play in b (b.toPlay).
AiResult chooseMoveAlphaBeta(const Board& b, Difficulty d);

} // namespace morpion

