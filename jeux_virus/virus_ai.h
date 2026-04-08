#pragma once

#include "virus_core.h"

#include <cstdint>
#include <optional>

namespace virus {

enum class Difficulty : int { Beginner = 0, Medium = 1, Expert = 2 };

struct AiStats {
  int depth = 0;
  int nodes = 0;
  int cutoffs = 0;
  int evals = 0;
};

struct AiResult {
  std::optional<Move> move;
  int score = 0;
  AiStats stats;
};

// Returns best move for "who" from state s. If no move: move=nullopt.
AiResult chooseMoveAlphaBeta(const GameState& s, Cell who, Difficulty d);

// Static evaluation from "who" perspective (higher is better for who).
int evaluate(const GameState& s, Cell who);

} // namespace virus

