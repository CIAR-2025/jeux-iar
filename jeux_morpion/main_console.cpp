#include "morpion_ai.h"
#include "morpion_analysis.h"
#include "morpion_core.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

static void printBoard(const morpion::Board& b) {
  std::cout << "\n";
  for (int r = 0; r < 3; ++r) {
    std::cout << " ";
    for (int c = 0; c < 3; ++c) {
      int idx = r * 3 + c;
      auto ch = morpion::cellChar(b.c[idx]);
      if (b.c[idx] == morpion::Cell::Empty)
        std::cout << idx;
      else
        std::cout << ch;
      if (c < 2) std::cout << " | ";
    }
    std::cout << "\n";
    if (r < 2) std::cout << "---+---+---\n";
  }
  std::cout << "\n";
}

static morpion::Difficulty readDifficulty() {
  while (true) {
    std::cout << "Niveau IA (0=Débutant, 1=Moyen, 2=Expert): ";
    int d = -1;
    if (!(std::cin >> d)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    if (d >= 0 && d <= 2) return static_cast<morpion::Difficulty>(d);
  }
}

static int readMenu() {
  while (true) {
    std::cout << "\nMenu:\n";
    std::cout << "  1) Analyse des configurations\n";
    std::cout << "  2) Jouer (Humain vs IA)\n";
    std::cout << "  3) Jouer (IA vs IA)\n";
    std::cout << "  4) Quitter\n";
    std::cout << "Choix: ";
    int c = 0;
    if (!(std::cin >> c)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    if (c >= 1 && c <= 4) return c;
  }
}

static std::optional<int> readHumanMove(const morpion::Board& b) {
  while (true) {
    std::cout << "Entrez une case (0..8) ou -1 pour quitter: ";
    int idx = -2;
    if (!(std::cin >> idx)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    if (idx == -1) return std::nullopt;
    if (idx < 0 || idx > 8) continue;
    if (b.c[idx] != morpion::Cell::Empty) {
      std::cout << "Case occupée.\n";
      continue;
    }
    return idx;
  }
}

static void playHumanVsAi() {
  std::cout << "\nVous êtes O (MIN). L'IA est X (MAX).\n";
  auto diff = readDifficulty();

  morpion::Board b;
  b.c.fill(morpion::Cell::Empty);
  b.toPlay = morpion::Cell::X; // X starts

  while (true) {
    printBoard(b);
    auto t = morpion::terminal(b);
    if (t.terminal) {
      if (!t.winner.has_value())
        std::cout << "Match nul.\n";
      else
        std::cout << "Gagnant: " << (t.winner.value() == morpion::Cell::X ? "X (IA)" : "O (Vous)") << "\n";
      break;
    }

    if (b.toPlay == morpion::Cell::X) {
      auto res = morpion::chooseMoveAlphaBeta(b, diff);
      if (!res.move.has_value()) {
        std::cout << "IA ne peut pas jouer.\n";
        break;
      }
      std::cout << "IA joue: " << res.move->idx << " | score=" << res.score << " | nodes=" << res.stats.nodes
                << " | cutoffs=" << res.stats.cutoffs << " | depth=" << res.stats.depth << "\n";
      b = morpion::applyMove(b, *res.move);
    } else {
      auto idx = readHumanMove(b);
      if (!idx.has_value()) break;
      b = morpion::applyMove(b, morpion::Move{*idx});
    }
  }
}

static void playAiVsAi() {
  std::cout << "\nIA X (MAX):\n";
  auto dx = readDifficulty();
  std::cout << "IA O (MIN):\n";
  auto do_ = readDifficulty();

  morpion::Board b;
  b.c.fill(morpion::Cell::Empty);
  b.toPlay = morpion::Cell::X;

  while (true) {
    printBoard(b);
    auto t = morpion::terminal(b);
    if (t.terminal) {
      if (!t.winner.has_value())
        std::cout << "Match nul.\n";
      else
        std::cout << "Gagnant: " << (t.winner.value() == morpion::Cell::X ? "X" : "O") << "\n";
      break;
    }

    auto diff = (b.toPlay == morpion::Cell::X) ? dx : do_;
    auto res = morpion::chooseMoveAlphaBeta(b, diff);
    if (!res.move.has_value()) break;
    std::cout << (b.toPlay == morpion::Cell::X ? "X" : "O") << " joue: " << res.move->idx
              << " | score=" << res.score << " | nodes=" << res.stats.nodes << " | cutoffs=" << res.stats.cutoffs
              << " | depth=" << res.stats.depth << "\n";
    b = morpion::applyMove(b, *res.move);
  }
}

static void runAnalysis() {
  auto r = morpion::analyzeAll();
  std::cout << "\n=== Analyse des configurations (3^9) ===\n";
  std::cout << "Configurations totales: " << r.totalConfigurations << "\n";
  std::cout << "Configurations valides: " << r.validConfigurations << "\n\n";

  std::cout << "Par tour (k = #pions sur la grille):\n";
  std::cout << "  k | valides | gagnantes\n";
  std::cout << " ---+---------+----------\n";
  for (int k = 0; k <= 9; ++k) {
    std::cout << std::setw(3) << k << " | " << std::setw(7) << r.validByTurn[static_cast<size_t>(k)] << " | "
              << std::setw(8) << r.winningByTurn[static_cast<size_t>(k)] << "\n";
  }
}

int main() {
  std::cout << "=== Projet 3 — Morpion (console) ===\n";
  std::cout << "Rappel: X=MAX, O=MIN.\n";

  while (true) {
    int choice = readMenu();
    if (choice == 1) runAnalysis();
    if (choice == 2) playHumanVsAi();
    if (choice == 3) playAiVsAi();
    if (choice == 4) break;
  }
  return 0;
}

