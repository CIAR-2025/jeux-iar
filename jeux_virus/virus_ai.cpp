#include "virus_ai.h"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <limits>
#include <random>
#include <unordered_map>

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

static int countAdjacentFlips(const GameState& s, Cell who, Move m) {
  int flips = 0;
  Cell opp = opponent(who);
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      int nr = m.r + dr, nc = m.c + dc;
      if (!s.inBounds(nr, nc)) continue;
      if (s.at(nr, nc) == opp) ++flips;
    }
  }
  return flips;
}

static std::vector<Move> orderedMoves(const GameState& s, Cell who, bool preferCenterOnTie) {
  auto moves = s.legalMoves(who);
  std::vector<std::pair<int, Move>> scored;
  scored.reserve(moves.size());

  auto centerDist = [&](Move m) {
    double cr = (s.n - 1) / 2.0;
    double cc = (s.n - 1) / 2.0;
    return std::abs(m.r - cr) + std::abs(m.c - cc);
  };

  for (auto m : moves) scored.push_back({-countAdjacentFlips(s, who, m), m});

  std::sort(scored.begin(), scored.end(), [&](const auto& a, const auto& b) {
    if (a.first != b.first) return a.first < b.first;
    if (!preferCenterOnTie) return false;
    return centerDist(a.second) < centerDist(b.second);
  });

  std::vector<Move> out;
  out.reserve(scored.size());
  for (auto& item : scored) out.push_back(item.second);
  return out;
}

struct TTEntry {
  int depth = -1;
  int score = 0;
};

static uint64_t hashState(const GameState& s, Cell toMove) {
  uint64_t h = 1469598103934665603ULL;  // FNV offset basis
  for (Cell c : s.board) {
    uint64_t v = static_cast<uint64_t>(static_cast<int>(c) + 2);  // map {-1,0,1} -> {1,2,3}
    h ^= v;
    h *= 1099511628211ULL;  // FNV prime
  }
  h ^= static_cast<uint64_t>(static_cast<int>(toMove) + 3);
  h *= 1099511628211ULL;
  return h;
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
                     AiStats& st,
                     std::unordered_map<uint64_t, TTEntry>& tt) {
  if (depth == 0 || s.terminal()) {
    ++st.evals;
    return evaluate(s, maxPlayer);
  }

  const uint64_t key = hashState(s, toMove);
  auto ttIt = tt.find(key);
  if (ttIt != tt.end() && ttIt->second.depth >= depth) {
    ++st.ttHits;
    return ttIt->second.score;
  }

  auto moves = orderedMoves(s, toMove, false);
  if (moves.empty()) {
    // pass
    int v = alphabeta(s, maxPlayer, opponent(toMove), depth - 1, alpha, beta, st, tt);
    tt[key] = TTEntry{depth, v};
    ++st.ttStores;
    return v;
  }

  const bool maximizing = (toMove == maxPlayer);
  bool exact = true;
  if (maximizing) {
    int best = std::numeric_limits<int>::min();
    for (auto m : moves) {
      ++st.nodes;
      GameState ns = s.apply(toMove, m);
      int v = alphabeta(ns, maxPlayer, opponent(toMove), depth - 1, alpha, beta, st, tt);
      best = std::max(best, v);
      alpha = std::max(alpha, best);
      if (alpha >= beta) {
        ++st.cutoffs;
        exact = false;  // cutoff => bound only
        break;
      }
    }
    if (exact) {
      tt[key] = TTEntry{depth, best};
      ++st.ttStores;
    }
    return best;
  } else {
    int best = std::numeric_limits<int>::max();
    for (auto m : moves) {
      ++st.nodes;
      GameState ns = s.apply(toMove, m);
      int v = alphabeta(ns, maxPlayer, opponent(toMove), depth - 1, alpha, beta, st, tt);
      best = std::min(best, v);
      beta = std::min(beta, best);
      if (alpha >= beta) {
        ++st.cutoffs;
        exact = false;  // cutoff => bound only
        break;
      }
    }
    if (exact) {
      tt[key] = TTEntry{depth, best};
      ++st.ttStores;
    }
    return best;
  }
}

AiResult chooseMoveAlphaBeta(const GameState& s, Cell who, Difficulty d) {
  AiResult out;
  out.stats.depth = depthFor(d);
  auto moves = orderedMoves(s, who, true);
  if (moves.empty()) {
    out.move = std::nullopt;
    out.score = evaluate(s, who);
    return out;
  }

  int bestScore = std::numeric_limits<int>::min();
  std::vector<std::pair<int, Move>> evaluated;

  std::unordered_map<uint64_t, TTEntry> tt;
  tt.reserve(static_cast<size_t>(15000 + out.stats.depth * 8000));

  int alpha = std::numeric_limits<int>::min();
  int beta = std::numeric_limits<int>::max();

  for (auto m : moves) {
    GameState ns = s.apply(who, m);
    int v = alphabeta(ns, who, opponent(who), out.stats.depth - 1, alpha, beta, out.stats, tt);
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
