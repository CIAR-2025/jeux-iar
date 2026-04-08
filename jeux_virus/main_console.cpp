#include "virus_ai.h"
#include "virus_core.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

static std::string playerName(virus::Cell c) { return c == virus::Cell::White ? "Blanc" : "Noir"; }

static void printBoard(const virus::GameState& s) {
  std::cout << "\n    ";
  for (int c = 0; c < s.n; ++c) std::cout << std::setw(2) << c << " ";
  std::cout << "\n";
  for (int r = 0; r < s.n; ++r) {
    std::cout << " " << std::setw(2) << r << "  ";
    for (int c = 0; c < s.n; ++c) {
      std::cout << " " << virus::cellChar(s.at(r, c)) << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n  Score — Blanc(O): " << s.count(virus::Cell::White) << " | Noir(X): " << s.count(virus::Cell::Black)
            << " | Vides: " << s.emptyCount() << "\n";
}

static std::optional<virus::Move> readMove(const virus::GameState& s, virus::Cell who) {
  while (true) {
    std::cout << "Entrez un coup sous forme 'r c' (ou 'pass', ou 'q'): ";
    std::string line;
    if (!std::getline(std::cin, line)) return std::nullopt;
    if (line == "q" || line == "quit" || line == "exit") return std::nullopt;
    if (line == "pass") return virus::Move{-1, -1};

    std::istringstream iss(line);
    int r, c;
    if (!(iss >> r >> c)) {
      std::cout << "Entrée invalide.\n";
      continue;
    }
    virus::Move m{r, c};
    if (!s.isLegal(who, m)) {
      std::cout << "Coup illégal. Rappel: la case doit être vide et adjacente (8 voisins) à un de vos pions.\n";
      continue;
    }
    return m;
  }
}

static virus::Difficulty readDifficulty() {
  while (true) {
    std::cout << "Niveau IA (0=Débutant, 1=Moyen, 2=Expert): ";
    int d = -1;
    if (!(std::cin >> d)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (d >= 0 && d <= 2) return static_cast<virus::Difficulty>(d);
  }
}

static int readMode() {
  while (true) {
    std::cout << "Mode (0=Humain vs IA, 1=IA vs IA, 2=Humain vs Humain): ";
    int m = -1;
    if (!(std::cin >> m)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (m >= 0 && m <= 2) return m;
  }
}

int main() {
  std::cout << "=== Projet 2 — Jeu du virus (console) ===\n";
  std::cout << "Symboles: Blanc=O, Noir=X, Vide=.\n";

  int mode = readMode();
  virus::Difficulty diffWhite = virus::Difficulty::Medium;
  virus::Difficulty diffBlack = virus::Difficulty::Medium;

  if (mode == 0) {
    std::cout << "Vous jouez Blanc (O). L'IA joue Noir (X).\n";
    diffBlack = readDifficulty();
  } else if (mode == 1) {
    std::cout << "IA Blanc (O):\n";
    diffWhite = readDifficulty();
    std::cout << "IA Noir (X):\n";
    diffBlack = readDifficulty();
  }

  virus::GameState s(virus::kDefaultN);
  std::vector<std::string> history;

  while (true) {
    printBoard(s);
    if (s.terminal()) {
      std::cout << "\nPartie terminée. Gagnant: " << s.winnerString() << "\n";
      break;
    }

    virus::Cell who = s.toPlay;
    auto legal = s.legalMoves(who);
    if (legal.empty()) {
      std::cout << playerName(who) << " n'a aucun coup légal → PASS.\n";
      history.push_back(playerName(who) + " PASS");
      s.toPlay = virus::opponent(who);
      continue;
    }

    std::cout << "\nTour: " << playerName(who) << " (" << (who == virus::Cell::White ? "O" : "X") << ")\n";

    std::optional<virus::Move> chosen;
    virus::AiResult aiRes;

    bool isAi = (mode == 1) || (mode == 0 && who == virus::Cell::Black);
    if (isAi) {
      virus::Difficulty d = (who == virus::Cell::White) ? diffWhite : diffBlack;
      aiRes = virus::chooseMoveAlphaBeta(s, who, d);
      if (!aiRes.move.has_value()) {
        std::cout << "IA: PASS\n";
        history.push_back(playerName(who) + " PASS");
        s.toPlay = virus::opponent(who);
        continue;
      }
      chosen = aiRes.move;
      std::cout << "IA joue: " << chosen->r << " " << chosen->c << " | score=" << aiRes.score
                << " | nodes=" << aiRes.stats.nodes << " | cutoffs=" << aiRes.stats.cutoffs
                << " | evals=" << aiRes.stats.evals << " | depth=" << aiRes.stats.depth << "\n";
    } else {
      auto m = readMove(s, who);
      if (!m.has_value()) {
        std::cout << "Arrêt.\n";
        break;
      }
      if (m->r == -1 && m->c == -1) {
        std::cout << "PASS.\n";
        history.push_back(playerName(who) + " PASS");
        s.toPlay = virus::opponent(who);
        continue;
      }
      chosen = m;
    }

    if (!chosen.has_value() || !s.isLegal(who, *chosen)) {
      std::cout << "Coup invalide (interne). Abandon.\n";
      break;
    }

    history.push_back(playerName(who) + " -> (" + std::to_string(chosen->r) + "," + std::to_string(chosen->c) + ")");
    s = s.apply(who, *chosen);
  }

  std::cout << "\n=== Historique ===\n";
  for (size_t i = 0; i < history.size(); ++i) {
    std::cout << std::setw(3) << i + 1 << ". " << history[i] << "\n";
  }
  return 0;
}

