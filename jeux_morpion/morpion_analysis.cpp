#include "morpion_analysis.h"

#include <array>

namespace morpion {

static Board boardFromTernary(uint32_t code) {
  Board b;
  for (int i = 0; i < 9; ++i) {
    uint32_t digit = code % 3;
    code /= 3;
    // 0=Empty, 1=X, 2=O
    b.c[i] = (digit == 0) ? Cell::Empty : (digit == 1 ? Cell::X : Cell::O);
  }

  int x = b.count(Cell::X);
  int o = b.count(Cell::O);
  b.toPlay = (x == o) ? Cell::X : Cell::O;
  return b;
}

AnalysisResult analyzeAll() {
  AnalysisResult r;
  r.totalConfigurations = 1;
  for (int i = 0; i < 9; ++i) r.totalConfigurations *= 3;

  for (uint32_t code = 0; code < r.totalConfigurations; ++code) {
    Board b = boardFromTernary(code);
    int turn = 9 - b.empties();

    if (!isValidConfiguration(b)) continue;
    ++r.validConfigurations;
    ++r.validByTurn[static_cast<size_t>(turn)];

    auto t = terminal(b);
    if (t.terminal && t.winner.has_value()) {
      ++r.winningByTurn[static_cast<size_t>(turn)];
    }
  }

  return r;
}

} // namespace morpion

