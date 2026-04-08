#include "taquin_solver.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>
#include <unordered_map>

namespace taquin {

struct CameFrom {
  uint64_t prev = 0;
  Dir dir = Dir::Left;
  bool hasPrev = false;
};

struct Node {
  uint64_t code = 0;
  int g = 0;
  int f = 0;
};

struct NodeGreater {
  bool operator()(const Node& a, const Node& b) const {
    if (a.f != b.f) return a.f > b.f;
    return a.g < b.g; // tie-break: prefer deeper with same f
  }
};

static std::vector<Move> reconstruct(uint64_t goalCode,
                                     const std::unordered_map<uint64_t, CameFrom>& came) {
  std::vector<Move> rev;
  uint64_t cur = goalCode;
  while (true) {
    auto it = came.find(cur);
    if (it == came.end() || !it->second.hasPrev) break;
    rev.push_back({it->second.dir});
    cur = it->second.prev;
  }
  std::reverse(rev.begin(), rev.end());
  return rev;
}

SolveResult solveAStar(const Board& start, const Board& goal, int nodeLimit) {
  SolveResult res;
  if (!isSolvable(start, goal)) return res;

  const uint64_t startCode = encode(start);
  const uint64_t goalCode = encode(goal);

  std::priority_queue<Node, std::vector<Node>, NodeGreater> open;
  std::unordered_map<uint64_t, int> gScore;
  std::unordered_map<uint64_t, CameFrom> came;

  gScore.reserve(200000);
  came.reserve(200000);

  int h0 = manhattan(start, goal);
  open.push({startCode, 0, h0});
  gScore[startCode] = 0;
  came[startCode] = CameFrom{0, Dir::Left, false};

  int expanded = 0;

  while (!open.empty()) {
    Node cur = open.top();
    open.pop();

    auto gsIt = gScore.find(cur.code);
    if (gsIt == gScore.end() || cur.g != gsIt->second) continue; // stale

    ++expanded;
    if (expanded > nodeLimit) return res;

    if (cur.code == goalCode) {
      res.success = true;
      res.expanded = expanded;
      res.cost = cur.g;
      res.path = reconstruct(goalCode, came);
      return res;
    }

    Board b = decode(cur.code);
    for (Move m : legalMoves(b)) {
      Board nb = applyMove(b, m);
      uint64_t nc = encode(nb);
      int tentative = cur.g + 1;
      auto it = gScore.find(nc);
      if (it == gScore.end() || tentative < it->second) {
        gScore[nc] = tentative;
        came[nc] = CameFrom{cur.code, m.dir, true};
        int h = manhattan(nb, goal);
        open.push({nc, tentative, tentative + h});
      }
    }
  }

  return res;
}

} // namespace taquin

