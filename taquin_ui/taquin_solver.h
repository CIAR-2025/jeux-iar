#pragma once

#include "taquin_core.h"

#include <optional>
#include <vector>

namespace taquin {

struct SolveResult {
  bool success = false;
  int expanded = 0;
  int generated = 0;   // nodes generated/pushed in OPEN
  int reopened = 0;    // improved states (better g found)
  int peakOpen = 0;    // max OPEN size seen
  int cost = 0;
  std::vector<Move> path; // from start to goal
};

// A* with Manhattan heuristic. Returns empty on timeout/limit.
SolveResult solveAStar(const Board& start, const Board& goal, int nodeLimit = 250000);

} // namespace taquin
