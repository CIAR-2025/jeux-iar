#include "virus_controller.h"

#include <QTimer>

VirusController::VirusController(QObject* parent) : QObject(parent), state_(virus::kDefaultN) {
  maybePlayAiTurn();
}

QVariantList VirusController::board() const {
  QVariantList out;
  out.reserve(state_.n * state_.n);
  for (int r = 0; r < state_.n; ++r) {
    for (int c = 0; c < state_.n; ++c) {
      out.push_back(static_cast<int>(state_.at(r, c)));
    }
  }
  return out;
}

QString VirusController::winnerText() const {
  return QString::fromStdString(state_.winnerString());
}

QString VirusController::statusText() const {
  if (state_.terminal()) {
    return QString("Partie terminee - Gagnant: %1").arg(winnerText());
  }

  QString turn = (state_.toPlay == virus::Cell::White) ? "Blanc" : "Noir";
  if (aiThinking_) {
    return QString("Tour %1 (IA en reflexion...)").arg(turn);
  }
  if (isAiTurn()) {
    return QString("Tour %1 (IA)").arg(turn);
  }
  return QString("Tour %1").arg(turn);
}

void VirusController::resetGame() {
  state_ = virus::GameState(virus::kDefaultN);
  history_.clear();
  setAiThinking(false);
  setConvertedIndices({});
  emit historyChanged();
  emitState();
  maybePlayAiTurn();
}

bool VirusController::isLegalMove(int row, int col) const {
  return state_.isLegal(state_.toPlay, virus::Move{row, col});
}

void VirusController::playCell(int row, int col) {
  if (state_.terminal() || isAiTurn()) return;

  virus::Move m{row, col};
  if (!state_.isLegal(state_.toPlay, m)) return;

  applyMove(m);
}

void VirusController::setGameMode(int mode) {
  if (mode < 0 || mode > 2 || gameMode_ == mode) return;
  gameMode_ = mode;
  setAiThinking(false);
  emit settingsChanged();
  emitState();
  maybePlayAiTurn();
}

void VirusController::setDifficultyWhite(int difficulty) {
  virus::Difficulty d = toDifficulty(difficulty);
  if (d == diffWhite_) return;
  diffWhite_ = d;
  emit settingsChanged();
  maybePlayAiTurn();
}

void VirusController::setDifficultyBlack(int difficulty) {
  virus::Difficulty d = toDifficulty(difficulty);
  if (d == diffBlack_) return;
  diffBlack_ = d;
  emit settingsChanged();
  maybePlayAiTurn();
}

void VirusController::applyMove(virus::Move m) {
  virus::Cell who = state_.toPlay;
  virus::Cell opp = virus::opponent(who);
  QVariantList converted;
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      int nr = m.r + dr;
      int nc = m.c + dc;
      if (!state_.inBounds(nr, nc)) continue;
      if (state_.at(nr, nc) == opp) {
        converted.push_back(nr * state_.n + nc);
      }
    }
  }

  history_.append(QString("%1 -> (%2,%3)").arg(playerName(who)).arg(m.r).arg(m.c));
  state_ = state_.apply(who, m);
  setConvertedIndices(converted);
  if (!converted.isEmpty()) {
    emit flipSoundRequested();
    QTimer::singleShot(380, this, [this]() { setConvertedIndices({}); });
  }

  emit historyChanged();
  emitState();

  advanceTurnWithPassIfNeeded();
  maybePlayAiTurn();
}

void VirusController::advanceTurnWithPassIfNeeded() {
  if (state_.terminal()) return;

  int guard = 0;
  while (!state_.terminal() && state_.legalMoves(state_.toPlay).empty() && guard < 2) {
    history_.append(QString("%1 PASS").arg(playerName(state_.toPlay)));
    state_.toPlay = virus::opponent(state_.toPlay);
    ++guard;
    emit historyChanged();
    emitState();
  }
}

void VirusController::maybePlayAiTurn() {
  if (state_.terminal() || !isAiTurn()) return;

  setAiThinking(true);
  QTimer::singleShot(150, this, [this]() {
    if (state_.terminal() || !isAiTurn()) {
      setAiThinking(false);
      return;
    }

    virus::Cell who = state_.toPlay;
    virus::Difficulty d = (who == virus::Cell::White) ? diffWhite_ : diffBlack_;
    virus::AiResult result = virus::chooseMoveAlphaBeta(state_, who, d);
    setAiThinking(false);

    if (!result.move.has_value()) {
      history_.append(QString("%1 PASS (IA)").arg(playerName(who)));
      state_.toPlay = virus::opponent(who);
      emit historyChanged();
      emitState();
      advanceTurnWithPassIfNeeded();
      maybePlayAiTurn();
      return;
    }

    applyMove(*result.move);
  });
}

bool VirusController::isAiTurn() const {
  if (gameMode_ == 1) return true;
  if (gameMode_ == 2) return false;
  return state_.toPlay == virus::Cell::Black;
}

virus::Difficulty VirusController::toDifficulty(int value) const {
  switch (value) {
    case 0:
      return virus::Difficulty::Beginner;
    case 1:
      return virus::Difficulty::Medium;
    case 2:
      return virus::Difficulty::Expert;
    default:
      return virus::Difficulty::Medium;
  }
}

QString VirusController::playerName(virus::Cell c) const {
  return c == virus::Cell::White ? "Blanc" : "Noir";
}

void VirusController::emitState() {
  emit boardChanged();
  emit stateChanged();
}

void VirusController::setAiThinking(bool value) {
  if (aiThinking_ == value) return;
  aiThinking_ = value;
  emit stateChanged();
}

void VirusController::setConvertedIndices(const QVariantList& indices) {
  if (convertedIndices_ == indices) return;
  convertedIndices_ = indices;
  emit convertedChanged();
}
