#pragma once

#include "taquin_core.h"

#include <optional>
#include <vector>

namespace taquin {

struct SolveResult {
  bool success = false;
  int expanded = 0;
  int cost = 0;
  std::vector<Move> path; // from start to goal
};

// A* with Manhattan heuristic. Returns empty on timeout/limit.
SolveResult solveAStar(const Board& start, const Board& goal, int nodeLimit = 250000);

} // namespace taquin

