#include "morpion_controller.h"

#include <QTimer>

MorpionController::MorpionController(QObject* parent) : QObject(parent) {
  resetGame();
  runAnalysis();
}

QVariantList MorpionController::board() const {
  QVariantList out;
  out.reserve(9);
  for (auto v : board_.c) out.push_back(static_cast<int>(v));
  return out;
}

QString MorpionController::winnerText() const {
  auto t = morpion::terminal(board_);
  if (!t.terminal || !t.winner.has_value()) return "Egalite";
  return t.winner.value() == morpion::Cell::X ? "X (MAX)" : "O (MIN)";
}

QString MorpionController::statusText() const {
  auto t = morpion::terminal(board_);
  if (t.terminal) {
    if (!t.winner.has_value()) return "Partie terminee - Match nul";
    return QString("Partie terminee - Gagnant: %1").arg(winnerText());
  }
  QString turn = board_.toPlay == morpion::Cell::X ? "X (MAX)" : "O (MIN)";
  if (aiThinking_) return QString("Tour %1 - IA en reflexion...").arg(turn);
  if (isAiTurn()) return QString("Tour %1 (IA)").arg(turn);
  return QString("Tour %1").arg(turn);
}

QVariantList MorpionController::analysisValidByTurn() const {
  QVariantList out;
  out.reserve(10);
  for (auto v : analysis_.validByTurn) out.push_back(static_cast<int>(v));
  return out;
}

QVariantList MorpionController::analysisWinningByTurn() const {
  QVariantList out;
  out.reserve(10);
  for (auto v : analysis_.winningByTurn) out.push_back(static_cast<int>(v));
  return out;
}

void MorpionController::resetGame() {
  board_.c.fill(morpion::Cell::Empty);
  board_.toPlay = morpion::Cell::X;
  history_.clear();
  emit historyChanged();
  setAiThinking(false);
  emitState();
  maybePlayAiTurn();
}

bool MorpionController::isLegalCell(int idx) const {
  if (idx < 0 || idx >= 9) return false;
  if (morpion::terminal(board_).terminal) return false;
  return board_.c[static_cast<size_t>(idx)] == morpion::Cell::Empty;
}

void MorpionController::playCell(int idx) {
  if (idx < 0 || idx >= 9 || isAiTurn()) return;
  if (!isLegalCell(idx)) return;
  applyMove(morpion::Move{idx}, false);
}

void MorpionController::runAnalysis() {
  analysis_ = morpion::analyzeAll();
  emit analysisChanged();
}

void MorpionController::clearHistory() {
  history_.clear();
  emit historyChanged();
}

void MorpionController::setGameMode(int mode) {
  if (mode < 0 || mode > 2 || gameMode_ == mode) return;
  gameMode_ = mode;
  emit settingsChanged();
  emitState();
  maybePlayAiTurn();
}

void MorpionController::setDifficultyX(int difficulty) {
  morpion::Difficulty d = toDifficulty(difficulty);
  if (d == diffX_) return;
  diffX_ = d;
  emit settingsChanged();
}

void MorpionController::setDifficultyO(int difficulty) {
  morpion::Difficulty d = toDifficulty(difficulty);
  if (d == diffO_) return;
  diffO_ = d;
  emit settingsChanged();
}

bool MorpionController::isAiTurn() const {
  if (gameMode_ == 1) return true;
  if (gameMode_ == 2) return false;
  return board_.toPlay == morpion::Cell::X;
}

morpion::Difficulty MorpionController::toDifficulty(int value) const {
  switch (value) {
    case 0:
      return morpion::Difficulty::Beginner;
    case 1:
      return morpion::Difficulty::Medium;
    case 2:
      return morpion::Difficulty::Expert;
    default:
      return morpion::Difficulty::Medium;
  }
}

QString MorpionController::playerName(morpion::Cell c) const {
  return c == morpion::Cell::X ? "X" : "O";
}

void MorpionController::setAiThinking(bool v) {
  if (aiThinking_ == v) return;
  aiThinking_ = v;
  emit stateChanged();
}

void MorpionController::emitState() {
  emit boardChanged();
  emit stateChanged();
}

void MorpionController::applyMove(morpion::Move m, bool fromAi) {
  morpion::Cell who = board_.toPlay;
  board_ = morpion::applyMove(board_, m);
  history_.append(QString("%1 -> case %2%3")
                      .arg(playerName(who))
                      .arg(m.idx)
                      .arg(fromAi ? " (IA)" : ""));
  emit historyChanged();
  emitState();
  maybePlayAiTurn();
}

void MorpionController::maybePlayAiTurn() {
  if (morpion::terminal(board_).terminal || !isAiTurn()) return;

  setAiThinking(true);
  QTimer::singleShot(180, this, [this]() {
    if (morpion::terminal(board_).terminal || !isAiTurn()) {
      setAiThinking(false);
      return;
    }

    morpion::Difficulty d = (board_.toPlay == morpion::Cell::X) ? diffX_ : diffO_;
    morpion::AiResult r = morpion::chooseMoveAlphaBeta(board_, d);
    setAiThinking(false);
    if (!r.move.has_value()) {
      emitState();
      return;
    }
    applyMove(*r.move, true);
  });
}
