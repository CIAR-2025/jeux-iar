#include "taquin_game.h"

#include <QRandomGenerator>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>

#include "taquin_solver.h"

static int indexOfZero(const taquin::Board& b) {
  for (int i = 0; i < 9; ++i)
    if (b[i] == 0) return i;
  return -1;
}

TaquinGame::TaquinGame(QObject* parent) : QObject(parent) {
  m_goal = taquin::Board{1, 2, 3, 4, 5, 6, 7, 8, 0};
  m_start = m_goal;
  m_autoPlayTimer.setInterval(220);
  connect(&m_autoPlayTimer, &QTimer::timeout, this, [this]() {
    if (m_busy || m_solution.empty() || m_solutionPos >= static_cast<int>(m_solution.size())) {
      stopAutoPlayInternal();
      return;
    }
    applySolutionStep();
    if (m_solutionPos >= static_cast<int>(m_solution.size()) || taquin::isGoal(m_board, m_goal)) {
      stopAutoPlayInternal(true);
      setStatus("Lecture auto terminee.");
    }
  });
  startFromInitial();
}

QVariantList TaquinGame::tiles() const {
  QVariantList out;
  out.reserve(9);
  for (int v : m_board) out.push_back(v);
  return out;
}

QVariantList TaquinGame::startTiles() const {
  QVariantList out;
  out.reserve(9);
  for (int v : m_start) out.push_back(v);
  return out;
}

QVariantList TaquinGame::goalTiles() const {
  QVariantList out;
  out.reserve(9);
  for (int v : m_goal) out.push_back(v);
  return out;
}

void TaquinGame::setBusy(bool v) {
  if (m_busy == v) return;
  m_busy = v;
  emit busyChanged();
}

void TaquinGame::setStatus(const QString& s) {
  if (m_status == s) return;
  m_status = s;
  emit statusChanged();
}

void TaquinGame::setMovesCount(int v) {
  if (m_movesCount == v) return;
  m_movesCount = v;
  emit movesCountChanged();
}

void TaquinGame::setOutcome(const QString& s) {
  if (m_outcome == s) return;
  m_outcome = s;
  emit outcomeChanged();
}

void TaquinGame::setSolvable(bool v) {
  if (m_solvable == v) return;
  m_solvable = v;
  emit solvableChanged();
}

void TaquinGame::setManhattan(int v) {
  if (m_manhattan == v) return;
  m_manhattan = v;
  emit manhattanDistanceChanged();
}

void TaquinGame::setLastSolutionLength(int v) {
  if (m_lastSolutionLength == v) return;
  m_lastSolutionLength = v;
  emit lastSolutionLengthChanged();
}

void TaquinGame::setLastExpandedNodes(int v) {
  if (m_lastExpandedNodes == v) return;
  m_lastExpandedNodes = v;
  emit lastExpandedNodesChanged();
}

void TaquinGame::commitBoard(const taquin::Board& b) {
  m_board = b;
  emit tilesChanged();
  recomputeDerived();
}

void TaquinGame::appendHistory(const QString& entry) {
  m_history.push_back(entry);
  emit historyChanged();
}

void TaquinGame::recomputeDerived() {
  setSolvable(taquin::isSolvable(m_board, m_goal));
  setManhattan(taquin::manhattan(m_board, m_goal));
  if (taquin::isGoal(m_board, m_goal)) {
    setOutcome("GAGNÉ");
  } else if (!m_solvable) {
    setOutcome("IMPOSSIBLE");
  } else {
    setOutcome("EN COURS");
  }
}

void TaquinGame::resetToGoal() {
  stopAutoPlayInternal(true);
  m_start = m_goal;
  emit startTilesChanged();
  startFromInitial();
}

void TaquinGame::startFromInitial() {
  stopAutoPlayInternal(true);
  m_solution.clear();
  m_solutionPos = 0;
  commitBoard(m_start);
  setMovesCount(0);
  setLastSolutionLength(0);
  setLastExpandedNodes(0);
  m_history.clear();
  emit historyChanged();
  setStatus("Pret depuis l'etat initial. Cliquez une tuile adjacente au vide.");
}

void TaquinGame::shuffle(int steps) {
  if (m_busy) return;
  stopAutoPlayInternal(true);
  taquin::Board b = m_goal;
  taquin::Dir last = taquin::Dir::Left;
  for (int i = 0; i < steps; ++i) {
    auto moves = taquin::legalMoves(b);
    if (moves.empty()) break;
    // Avoid immediate backtracking most of the time.
    std::vector<taquin::Move> filtered;
    filtered.reserve(moves.size());
    for (auto m : moves) {
      bool isBack =
          (last == taquin::Dir::Left && m.dir == taquin::Dir::Right) ||
          (last == taquin::Dir::Right && m.dir == taquin::Dir::Left) ||
          (last == taquin::Dir::Up && m.dir == taquin::Dir::Down) ||
          (last == taquin::Dir::Down && m.dir == taquin::Dir::Up);
      if (!isBack) filtered.push_back(m);
    }
    const auto& pickFrom = filtered.empty() ? moves : filtered;
    int idx = QRandomGenerator::global()->bounded(static_cast<int>(pickFrom.size()));
    auto m = pickFrom[idx];
    b = taquin::applyMove(b, m);
    last = m.dir;
  }
  m_solution.clear();
  m_solutionPos = 0;
  commitBoard(b);
  setMovesCount(0);
  setLastSolutionLength(0);
  setLastExpandedNodes(0);
  m_history.clear();
  emit historyChanged();
  setStatus("Mélangé. À vous de jouer (ou Résoudre).");
}

bool TaquinGame::moveIndex(int index) {
  if (m_busy) return false;
  if (m_autoPlaying) stopAutoPlayInternal(true);
  if (index < 0 || index >= 9) return false;
  if (m_board[index] == 0) return false;

  int z = indexOfZero(m_board);
  int r = index / 3, c = index % 3;
  int zr = z / 3, zc = z % 3;
  int dr = r - zr, dc = c - zc;
  if (std::abs(dr) + std::abs(dc) != 1) return false;

  // Determine move direction for empty (swap tile with empty).
  taquin::Dir d;
  if (dc == 1)
    d = taquin::Dir::Right;
  else if (dc == -1)
    d = taquin::Dir::Left;
  else if (dr == 1)
    d = taquin::Dir::Down;
  else
    d = taquin::Dir::Up;

  const int tile = m_board[index];
  commitBoard(taquin::applyMove(m_board, {d}));
  setMovesCount(m_movesCount + 1);
  m_solution.clear();
  m_solutionPos = 0;
  setLastSolutionLength(0);
  setLastExpandedNodes(0);

  appendHistory(QString("#%1 : tuile %2 — %3")
                    .arg(m_movesCount)
                    .arg(tile)
                    .arg(QString::fromStdString(taquin::dirToString(d))));

  if (taquin::isGoal(m_board, m_goal)) {
    setStatus("Bravo: état final atteint. Vous avez GAGNÉ.");
  } else {
    setStatus(m_solvable ? "OK. Continuez ou Résoudre." : "Attention: cette configuration est IMPOSSIBLE (parité).");
  }
  return true;
}

void TaquinGame::solve() {
  if (m_busy) return;
  stopAutoPlayInternal(true);
  if (taquin::isGoal(m_board, m_goal)) {
    setStatus("Déjà résolu.");
    return;
  }
  if (!taquin::isSolvable(m_board, m_goal)) {
    setStatus("État non résoluble (parité).");
    return;
  }

  setBusy(true);
  setStatus("Résolution A* (Manhattan) en cours…");
  setLastSolutionLength(0);
  setLastExpandedNodes(0);

  taquin::Board start = m_board;
  taquin::Board goal = m_goal;

  auto* watcher = new QFutureWatcher<taquin::SolveResult>(this);
  connect(watcher,
          &QFutureWatcher<taquin::SolveResult>::finished,
          this,
          [this, watcher]() {
            auto result = watcher->result();
            watcher->deleteLater();

            setBusy(false);
            if (!result.success) {
              setStatus("A* n'a pas trouvé (limite de nœuds atteinte).");
              m_solution.clear();
              m_solutionPos = 0;
              m_pendingAutoPlay = false;
              return;
            }

            m_solution = std::move(result.path);
            m_solutionPos = 0;
            setLastSolutionLength(static_cast<int>(m_solution.size()));
            setLastExpandedNodes(result.expanded);
            setStatus(QString("Solution trouvée: %1 coups, %2 nœuds explorés. Appuyez sur 'Pas'.")
                          .arg(static_cast<int>(m_solution.size()))
                          .arg(result.expanded));
            if (m_pendingAutoPlay) {
              m_pendingAutoPlay = false;
              startAutoPlay();
            }
          });

  watcher->setFuture(QtConcurrent::run([start, goal]() { return taquin::solveAStar(start, goal); }));
}

void TaquinGame::applySolutionStep() {
  if (m_busy) return;
  if (m_solution.empty()) {
    setStatus("Pas de solution chargée. Cliquez 'Résoudre' d'abord.");
    return;
  }
  if (m_solutionPos >= static_cast<int>(m_solution.size())) {
    setStatus("Solution terminée.");
    return;
  }

  auto mv = m_solution[m_solutionPos];
  commitBoard(taquin::applyMove(m_board, m_solution[m_solutionPos]));
  ++m_solutionPos;
  appendHistory(QString("[A*] pas %1/%2 — %3")
                    .arg(m_solutionPos)
                    .arg(static_cast<int>(m_solution.size()))
                    .arg(QString::fromStdString(taquin::dirToString(mv.dir))));

  if (taquin::isGoal(m_board, m_goal)) {
    setStatus("Résolu.");
  } else {
    setStatus(QString("Pas %1/%2").arg(m_solutionPos).arg(static_cast<int>(m_solution.size())));
  }
}

void TaquinGame::clearHistory() {
  m_history.clear();
  emit historyChanged();
}

bool TaquinGame::setBoardFromString(const QString& values, taquin::Board& out, QString& error) const {
  const QStringList tokens = values.split(QRegularExpression("[,;\\s]+"), Qt::SkipEmptyParts);
  if (tokens.size() != 9) {
    error = "Format invalide: 9 valeurs attendues.";
    return false;
  }

  std::array<int, 9> seen{};
  taquin::Board parsed{};
  for (int i = 0; i < 9; ++i) {
    bool ok = false;
    int v = tokens[i].toInt(&ok);
    if (!ok || v < 0 || v > 8) {
      error = "Chaque valeur doit etre un entier entre 0 et 8.";
      return false;
    }
    if (++seen[static_cast<size_t>(v)] > 1) {
      error = "Les valeurs doivent etre uniques (permutation de 0..8).";
      return false;
    }
    parsed[i] = v;
  }
  out = parsed;
  return true;
}

QString TaquinGame::boardToString(const taquin::Board& b) const {
  QStringList parts;
  parts.reserve(9);
  for (int v : b) parts << QString::number(v);
  return parts.join(' ');
}

bool TaquinGame::setStartFromString(const QString& values) {
  if (m_busy) return false;
  stopAutoPlayInternal(true);
  taquin::Board parsed{};
  QString error;
  if (!setBoardFromString(values, parsed, error)) {
    setStatus("Etat initial invalide: " + error);
    return false;
  }
  m_start = parsed;
  emit startTilesChanged();
  recomputeDerived();
  setStatus("Etat initial mis a jour: " + boardToString(m_start));
  return true;
}

bool TaquinGame::setGoalFromString(const QString& values) {
  if (m_busy) return false;
  stopAutoPlayInternal(true);
  taquin::Board parsed{};
  QString error;
  if (!setBoardFromString(values, parsed, error)) {
    setStatus("Etat final invalide: " + error);
    return false;
  }
  m_goal = parsed;
  emit goalTilesChanged();
  m_solution.clear();
  m_solutionPos = 0;
  setLastSolutionLength(0);
  setLastExpandedNodes(0);
  recomputeDerived();
  setStatus("Etat final mis a jour: " + boardToString(m_goal));
  return true;
}

void TaquinGame::loadSujetExample() {
  if (m_busy) return;
  stopAutoPlayInternal(true);
  m_start = taquin::Board{7, 0, 5, 1, 6, 4, 2, 8, 3};
  m_goal = taquin::Board{7, 6, 5, 8, 0, 4, 1, 2, 3};
  emit startTilesChanged();
  emit goalTilesChanged();
  startFromInitial();
  setStatus("Exemple du sujet charge. Etat initial/final personnalises.");
}

void TaquinGame::setAutoPlaying(bool v) {
  if (m_autoPlaying == v) return;
  m_autoPlaying = v;
  emit autoPlayingChanged();
}

void TaquinGame::stopAutoPlayInternal(bool keepStatus) {
  if (m_autoPlayTimer.isActive()) m_autoPlayTimer.stop();
  m_pendingAutoPlay = false;
  if (m_autoPlaying) {
    setAutoPlaying(false);
    if (!keepStatus) setStatus("Lecture auto arretee.");
  }
}

void TaquinGame::startAutoPlay() {
  if (m_busy) {
    m_pendingAutoPlay = true;
    return;
  }
  if (taquin::isGoal(m_board, m_goal)) {
    setStatus("Deja resolu.");
    return;
  }
  if (m_solution.empty() || m_solutionPos >= static_cast<int>(m_solution.size())) {
    m_pendingAutoPlay = true;
    setStatus("Calcul de solution avant lecture auto...");
    solve();
    return;
  }

  m_pendingAutoPlay = false;
  setAutoPlaying(true);
  if (!m_autoPlayTimer.isActive()) m_autoPlayTimer.start();
  setStatus(QString("Lecture auto: pas %1/%2")
                .arg(m_solutionPos)
                .arg(static_cast<int>(m_solution.size())));
}

void TaquinGame::stopAutoPlay() {
  stopAutoPlayInternal();
}
