#include "virus_ai.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <random>

namespace virus {

static int depthFor(Difficulty d) {
  switch (d) {
    case Difficulty::Beginner:
      return 2;
    case Difficulty::Medium:
      return 4;
    case Difficulty::Expert:
      return 6;
  }
  return 4;
}

static int terminalScore(const GameState& s, Cell who) {
  int diff = s.count(who) - s.count(opponent(who));
  if (diff > 0) return 100000 + diff;
  if (diff < 0) return -100000 + diff;
  return 0;
}

int evaluate(const GameState& s, Cell who) {
  if (s.terminal()) return terminalScore(s, who);

  const Cell opp = opponent(who);
  const int n = s.n;

  int material = s.count(who) - s.count(opp);
  int mobility = static_cast<int>(s.legalMoves(who).size()) - static_cast<int>(s.legalMoves(opp).size());

  // Corner weight: corners are sticky in many flip-adjacent games.
  auto corner = [&](int r, int c) {
    Cell v = s.at(r, c);
    if (v == who) return 1;
    if (v == opp) return -1;
    return 0;
  };
  int corners = corner(0, 0) + corner(0, n - 1) + corner(n - 1, 0) + corner(n - 1, n - 1);

  // Frontier penalty: pieces adjacent to empties are "unstable".
  int frontierWho = 0, frontierOpp = 0;
  for (int r = 0; r < n; ++r) {
    for (int c = 0; c < n; ++c) {
      Cell v = s.at(r, c);
      if (v == Cell::Empty) continue;
      bool isFrontier = false;
      for (int dr = -1; dr <= 1 && !isFrontier; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          if (dr == 0 && dc == 0) continue;
          int nr = r + dr, nc = c + dc;
          if (!s.inBounds(nr, nc)) continue;
          if (s.at(nr, nc) == Cell::Empty) {
            isFrontier = true;
            break;
          }
        }
      }
      if (isFrontier) {
        if (v == who)
          ++frontierWho;
        else
          ++frontierOpp;
      }
    }
  }
  int frontier = frontierOpp - frontierWho; // prefer fewer frontier pieces

  // Weighting: keep simple & stable.
  return material * 10 + mobility * 3 + corners * 25 + frontier * 1;
}

static int alphabeta(const GameState& s,
                     Cell maxPlayer,
                     Cell toMove,
                     int depth,
                     int alpha,
                     int beta,
                     AiStats& st) {
  if (depth == 0 || s.terminal()) {
    ++st.evals;
    return evaluate(s, maxPlayer);
  }

  auto moves = s.legalMoves(toMove);
  if (moves.empty()) {
    // pass
    return alphabeta(s, maxPlayer, opponent(toMove), depth - 1, alpha, beta, st);
  }

  // Move ordering: prefer moves that immediately flip many pieces.
  const Cell opp = opponent(toMove);
  std::vector<std::pair<int, Move>> scored;
  scored.reserve(moves.size());
  for (auto m : moves) {
    int flips = 0;
    for (int dr = -1; dr <= 1; ++dr) {
      for (int dc = -1; dc <= 1; ++dc) {
        if (dr == 0 && dc == 0) continue;
        int nr = m.r + dr, nc = m.c + dc;
        if (!s.inBounds(nr, nc)) continue;
        if (s.at(nr, nc) == opp) ++flips;
      }
    }
    scored.push_back({-flips, m}); // smaller first = more flips
  }
  std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

  const bool maximizing = (toMove == maxPlayer);
  if (maximizing) {
    int best = std::numeric_limits<int>::min();
    for (auto [_, m] : scored) {
      ++st.nodes;
      GameState ns = s.apply(toMove, m);
      int v = alphabeta(ns, maxPlayer, opponent(toMove), depth - 1, alpha, beta, st);
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
    for (auto [_, m] : scored) {
      ++st.nodes;
      GameState ns = s.apply(toMove, m);
      int v = alphabeta(ns, maxPlayer, opponent(toMove), depth - 1, alpha, beta, st);
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

AiResult chooseMoveAlphaBeta(const GameState& s, Cell who, Difficulty d) {
  AiResult out;
  out.stats.depth = depthFor(d);
  auto moves = s.legalMoves(who);
  if (moves.empty()) {
    out.move = std::nullopt;
    out.score = evaluate(s, who);
    return out;
  }

  int bestScore = std::numeric_limits<int>::min();
  std::vector<std::pair<int, Move>> evaluated;

  // Deterministic tie-break by preferring center-ish moves.
  auto centerDist = [&](Move m) {
    double cr = (s.n - 1) / 2.0;
    double cc = (s.n - 1) / 2.0;
    return std::abs(m.r - cr) + std::abs(m.c - cc);
  };

  // Root move ordering as well (same flip-count heuristic).
  std::vector<std::pair<int, Move>> scored;
  scored.reserve(moves.size());
  Cell opp = opponent(who);
  for (auto m : moves) {
    int flips = 0;
    for (int dr = -1; dr <= 1; ++dr) {
      for (int dc = -1; dc <= 1; ++dc) {
        if (dr == 0 && dc == 0) continue;
        int nr = m.r + dr, nc = m.c + dc;
        if (!s.inBounds(nr, nc)) continue;
        if (s.at(nr, nc) == opp) ++flips;
      }
    }
    scored.push_back({-flips, m});
  }
  std::sort(scored.begin(), scored.end(), [&](const auto& a, const auto& b) {
    if (a.first != b.first) return a.first < b.first;
    return centerDist(a.second) < centerDist(b.second);
  });

  int alpha = std::numeric_limits<int>::min();
  int beta = std::numeric_limits<int>::max();

  for (auto [_, m] : scored) {
    GameState ns = s.apply(who, m);
    int v = alphabeta(ns, who, opponent(who), out.stats.depth - 1, alpha, beta, out.stats);
    evaluated.push_back({v, m});
    if (v > bestScore) bestScore = v;
    alpha = std::max(alpha, bestScore);
  }

  // Make AI less predictable while keeping quality:
  // - Expert: random among exact best moves
  // - Medium: random among near-best moves (<= 3 points from best)
  // - Beginner: random among near-best moves (<= 8 points from best)
  int slack = 0;
  switch (d) {
    case Difficulty::Beginner:
      slack = 8;
      break;
    case Difficulty::Medium:
      slack = 3;
      break;
    case Difficulty::Expert:
      slack = 0;
      break;
  }

  std::vector<Move> candidates;
  for (const auto& [score, mv] : evaluated) {
    if (score >= bestScore - slack) candidates.push_back(mv);
  }

  static thread_local std::mt19937 rng(
      static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count()));

  Move selected = candidates.front();
  if (candidates.size() > 1) {
    std::uniform_int_distribution<int> pick(0, static_cast<int>(candidates.size()) - 1);
    selected = candidates[static_cast<size_t>(pick(rng))];
  }

  out.move = selected;
  out.score = bestScore;
  return out;
}

} // namespace virus
