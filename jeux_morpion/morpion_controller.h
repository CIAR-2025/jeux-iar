#pragma once

#include "morpion_ai.h"
#include "morpion_analysis.h"
#include "morpion_core.h"

#include <QObject>
#include <QStringList>
#include <QVariantList>

class MorpionController : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList board READ board NOTIFY boardChanged)
  Q_PROPERTY(int toPlay READ toPlay NOTIFY stateChanged)
  Q_PROPERTY(bool gameOver READ gameOver NOTIFY stateChanged)
  Q_PROPERTY(QString winnerText READ winnerText NOTIFY stateChanged)
  Q_PROPERTY(QString statusText READ statusText NOTIFY stateChanged)
  Q_PROPERTY(int xCount READ xCount NOTIFY stateChanged)
  Q_PROPERTY(int oCount READ oCount NOTIFY stateChanged)
  Q_PROPERTY(int emptyCount READ emptyCount NOTIFY stateChanged)
  Q_PROPERTY(int gameMode READ gameMode WRITE setGameMode NOTIFY settingsChanged)
  Q_PROPERTY(int difficultyX READ difficultyX WRITE setDifficultyX NOTIFY settingsChanged)
  Q_PROPERTY(int difficultyO READ difficultyO WRITE setDifficultyO NOTIFY settingsChanged)
  Q_PROPERTY(QStringList history READ history NOTIFY historyChanged)
  Q_PROPERTY(bool aiThinking READ aiThinking NOTIFY stateChanged)

  Q_PROPERTY(int analysisTotal READ analysisTotal NOTIFY analysisChanged)
  Q_PROPERTY(int analysisValid READ analysisValid NOTIFY analysisChanged)
  Q_PROPERTY(QVariantList analysisValidByTurn READ analysisValidByTurn NOTIFY analysisChanged)
  Q_PROPERTY(QVariantList analysisWinningByTurn READ analysisWinningByTurn NOTIFY analysisChanged)

 public:
  explicit MorpionController(QObject* parent = nullptr);

  QVariantList board() const;
  int toPlay() const { return static_cast<int>(board_.toPlay); }
  bool gameOver() const { return morpion::terminal(board_).terminal; }
  QString winnerText() const;
  QString statusText() const;
  int xCount() const { return board_.count(morpion::Cell::X); }
  int oCount() const { return board_.count(morpion::Cell::O); }
  int emptyCount() const { return board_.empties(); }

  int gameMode() const { return gameMode_; }
  int difficultyX() const { return static_cast<int>(diffX_); }
  int difficultyO() const { return static_cast<int>(diffO_); }
  QStringList history() const { return history_; }
  bool aiThinking() const { return aiThinking_; }

  int analysisTotal() const { return static_cast<int>(analysis_.totalConfigurations); }
  int analysisValid() const { return static_cast<int>(analysis_.validConfigurations); }
  QVariantList analysisValidByTurn() const;
  QVariantList analysisWinningByTurn() const;

  Q_INVOKABLE void resetGame();
  Q_INVOKABLE void playCell(int idx);
  Q_INVOKABLE bool isLegalCell(int idx) const;
  Q_INVOKABLE void runAnalysis();
  Q_INVOKABLE void clearHistory();

  void setGameMode(int mode);
  void setDifficultyX(int difficulty);
  void setDifficultyO(int difficulty);

 signals:
  void boardChanged();
  void stateChanged();
  void settingsChanged();
  void historyChanged();
  void analysisChanged();

 private:
  morpion::Board board_;
  int gameMode_ = 0;  // 0: Humain vs IA, 1: IA vs IA, 2: Humain vs Humain
  morpion::Difficulty diffX_ = morpion::Difficulty::Medium;
  morpion::Difficulty diffO_ = morpion::Difficulty::Medium;
  QStringList history_;
  bool aiThinking_ = false;

  morpion::AnalysisResult analysis_;

  bool isAiTurn() const;
  morpion::Difficulty toDifficulty(int value) const;
  QString playerName(morpion::Cell c) const;
  void setAiThinking(bool v);
  void emitState();
  void applyMove(morpion::Move m, bool fromAi);
  void maybePlayAiTurn();
};
