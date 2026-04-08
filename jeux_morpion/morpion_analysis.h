#pragma once

#include "morpion_core.h"

#include <array>
#include <cstdint>

namespace morpion {

struct AnalysisResult {
  uint32_t totalConfigurations = 0;          // 3^9
  uint32_t validConfigurations = 0;          // all turns
  std::array<uint32_t, 10> validByTurn{};    // turn = #marks on board (0..9)
  std::array<uint32_t, 10> winningByTurn{};  // terminal wins (X or O) by turn
};

AnalysisResult analyzeAll();

} // namespace morpion

