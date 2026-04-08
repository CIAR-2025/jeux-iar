#pragma once

#include "virus_ai.h"
#include "virus_core.h"

#include <QObject>
#include <QStringList>
#include <QVariantList>

class VirusController : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList board READ board NOTIFY boardChanged)
  Q_PROPERTY(int boardSize READ boardSize CONSTANT)
  Q_PROPERTY(int toPlay READ toPlay NOTIFY stateChanged)
  Q_PROPERTY(int whiteScore READ whiteScore NOTIFY stateChanged)
  Q_PROPERTY(int blackScore READ blackScore NOTIFY stateChanged)
  Q_PROPERTY(int emptyCount READ emptyCount NOTIFY stateChanged)
  Q_PROPERTY(bool gameOver READ gameOver NOTIFY stateChanged)
  Q_PROPERTY(QString winnerText READ winnerText NOTIFY stateChanged)
  Q_PROPERTY(QString statusText READ statusText NOTIFY stateChanged)
  Q_PROPERTY(bool aiThinking READ aiThinking NOTIFY stateChanged)
  Q_PROPERTY(QVariantList convertedIndices READ convertedIndices NOTIFY convertedChanged)
  Q_PROPERTY(int gameMode READ gameMode WRITE setGameMode NOTIFY settingsChanged)
  Q_PROPERTY(int difficultyWhite READ difficultyWhite WRITE setDifficultyWhite NOTIFY settingsChanged)
  Q_PROPERTY(int difficultyBlack READ difficultyBlack WRITE setDifficultyBlack NOTIFY settingsChanged)
  Q_PROPERTY(QStringList history READ history NOTIFY historyChanged)

 public:
  explicit VirusController(QObject* parent = nullptr);

  QVariantList board() const;
  int boardSize() const { return state_.n; }
  int toPlay() const { return static_cast<int>(state_.toPlay); }

  int whiteScore() const { return state_.count(virus::Cell::White); }
  int blackScore() const { return state_.count(virus::Cell::Black); }
  int emptyCount() const { return state_.emptyCount(); }
  bool gameOver() const { return state_.terminal(); }
  QString winnerText() const;
  QString statusText() const;
  bool aiThinking() const { return aiThinking_; }
  QVariantList convertedIndices() const { return convertedIndices_; }

  int gameMode() const { return gameMode_; }
  int difficultyWhite() const { return static_cast<int>(diffWhite_); }
  int difficultyBlack() const { return static_cast<int>(diffBlack_); }
  QStringList history() const { return history_; }

  Q_INVOKABLE void resetGame();
  Q_INVOKABLE void playCell(int row, int col);
  Q_INVOKABLE bool isLegalMove(int row, int col) const;

  void setGameMode(int mode);
  void setDifficultyWhite(int difficulty);
  void setDifficultyBlack(int difficulty);

 signals:
  void boardChanged();
  void stateChanged();
  void settingsChanged();
  void historyChanged();
  void convertedChanged();
  void flipSoundRequested();

 private:
  virus::GameState state_;
  int gameMode_ = 0;  // 0: Human vs AI, 1: AI vs AI, 2: Human vs Human
  virus::Difficulty diffWhite_ = virus::Difficulty::Medium;
  virus::Difficulty diffBlack_ = virus::Difficulty::Medium;
  QStringList history_;
  bool aiThinking_ = false;
  QVariantList convertedIndices_;

  void applyMove(virus::Move m);
  void advanceTurnWithPassIfNeeded();
  void maybePlayAiTurn();
  bool isAiTurn() const;
  virus::Difficulty toDifficulty(int value) const;
  QString playerName(virus::Cell c) const;
  void emitState();
  void setAiThinking(bool value);
  void setConvertedIndices(const QVariantList& indices);
};
