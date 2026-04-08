#include "morpion_ai.h"

#include <algorithm>
#include <limits>

namespace morpion {

static int depthFor(Difficulty d) {
  switch (d) {
    case Difficulty::Beginner:
      return 1;
    case Difficulty::Medium:
      return 5;
    case Difficulty::Expert:
      return 9;
  }
  return 5;
}

static int utility(const Board& b) {
  auto t = terminal(b);
  if (!t.terminal) return 0;
  if (!t.winner.has_value()) return 0;
  return (t.winner.value() == Cell::X) ? 10 : -10;
}

// Heuristic for non-terminal: count "two-in-a-row with empty" opportunities.
static int heuristic(const Board& b) {
  static constexpr int lines[8][3] = {
      {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
  int score = 0;
  for (auto& line : lines) {
    int x = 0, o = 0, e = 0;
    for (int k = 0; k < 3; ++k) {
      Cell v = b.c[line[k]];
      if (v == Cell::X)
        ++x;
      else if (v == Cell::O)
        ++o;
      else
        ++e;
    }
    if (e == 1 && x == 2) score += 2;
    if (e == 1 && o == 2) score -= 2;
    if (e == 2 && x == 1) score += 1;
    if (e == 2 && o == 1) score -= 1;
  }
  return score;
}

static int eval(const Board& b) {
  auto t = terminal(b);
  if (t.terminal) return utility(b);
  return heuristic(b);
}

static int alphabeta(const Board& b, int depth, int alpha, int beta, AiStats& st) {
  auto t = terminal(b);
  if (depth == 0 || t.terminal) return eval(b);

  auto moves = legalMoves(b);
  if (moves.empty()) return eval(b);

  bool maximizing = (b.toPlay == Cell::X);
  if (maximizing) {
    int best = std::numeric_limits<int>::min();
    for (auto m : moves) {
      ++st.nodes;
      Board nb = applyMove(b, m);
      int v = alphabeta(nb, depth - 1, alpha, beta, st);
      best = std::max(best, v);
      alpha = std::max(alpha, best);
      if (alpha >= beta) {
        ++st.cutoffs;
        break;
      }
    }
    return best;
  } else {
    int best = std::numeric_limits<int>::max();
    for (auto m : moves) {
      ++st.nodes;
      Board nb = applyMove(b, m);
      int v = alphabeta(nb, depth - 1, alpha, beta, st);
      best = std::min(best, v);
      beta = std::min(beta, best);
      if (alpha >= beta) {
        ++st.cutoffs;
        break;
      }
    }
    return best;
  }
}

AiResult chooseMoveAlphaBeta(const Board& b, Difficulty d) {
  AiResult out;
  out.stats.depth = depthFor(d);
  auto moves = legalMoves(b);
  if (moves.empty()) return out;

  // Prefer center, then corners, then edges on tie.
  auto pref = [](int idx) {
    if (idx == 4) return 0;
    if (idx == 0 || idx == 2 || idx == 6 || idx == 8) return 1;
    return 2;
  };

  bool maximizing = (b.toPlay == Cell::X);
  int bestScore = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
  std::optional<Move> bestMove;
  int alpha = std::numeric_limits<int>::min();
  int beta = std::numeric_limits<int>::max();

  std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& c) { return pref(a.idx) < pref(c.idx); });

  for (auto m : moves) {
    Board nb = applyMove(b, m);
    int v = alphabeta(nb, out.stats.depth - 1, alpha, beta, out.stats);

    if (!bestMove.has_value()) {
      bestMove = m;
      bestScore = v;
    } else if (maximizing) {
      if (v > bestScore || (v == bestScore && pref(m.idx) < pref(bestMove->idx))) {
        bestScore = v;
        bestMove = m;
      }
      alpha = std::max(alpha, bestScore);
    } else {
      if (v < bestScore || (v == bestScore && pref(m.idx) < pref(bestMove->idx))) {
        bestScore = v;
        bestMove = m;
      }
      beta = std::min(beta, bestScore);
    }
  }

  out.move = bestMove;
  out.score = bestScore;
  return out;
}

} // namespace morpion

