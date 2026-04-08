#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QVariantList>

#include "taquin_core.h"

class TaquinGame : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList tiles READ tiles NOTIFY tilesChanged)
  Q_PROPERTY(QVariantList startTiles READ startTiles NOTIFY startTilesChanged)
  Q_PROPERTY(QVariantList goalTiles READ goalTiles NOTIFY goalTilesChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(QString status READ status NOTIFY statusChanged)
  Q_PROPERTY(int movesCount READ movesCount NOTIFY movesCountChanged)
  Q_PROPERTY(QString outcome READ outcome NOTIFY outcomeChanged)
  Q_PROPERTY(bool solvable READ solvable NOTIFY solvableChanged)
  Q_PROPERTY(int manhattanDistance READ manhattanDistance NOTIFY manhattanDistanceChanged)
  Q_PROPERTY(QStringList history READ history NOTIFY historyChanged)
  Q_PROPERTY(int lastSolutionLength READ lastSolutionLength NOTIFY lastSolutionLengthChanged)
  Q_PROPERTY(int lastExpandedNodes READ lastExpandedNodes NOTIFY lastExpandedNodesChanged)
  Q_PROPERTY(bool autoPlaying READ autoPlaying NOTIFY autoPlayingChanged)

public:
  explicit TaquinGame(QObject* parent = nullptr);

  QVariantList tiles() const;
  QVariantList startTiles() const;
  QVariantList goalTiles() const;
  bool busy() const { return m_busy; }
  QString status() const { return m_status; }
  int movesCount() const { return m_movesCount; }
  QString outcome() const { return m_outcome; } // "GAGNÉ", "EN COURS", "IMPOSSIBLE"
  bool solvable() const { return m_solvable; }
  int manhattanDistance() const { return m_manhattan; }
  QStringList history() const { return m_history; }
  int lastSolutionLength() const { return m_lastSolutionLength; }
  int lastExpandedNodes() const { return m_lastExpandedNodes; }
  bool autoPlaying() const { return m_autoPlaying; }

  Q_INVOKABLE void resetToGoal();
  Q_INVOKABLE void shuffle(int steps = 80);
  Q_INVOKABLE bool moveIndex(int index); // click a tile to move into empty
  Q_INVOKABLE void solve();
  Q_INVOKABLE void applySolutionStep(); // step-by-step playback
  Q_INVOKABLE void clearHistory();
  Q_INVOKABLE bool setStartFromString(const QString& values);
  Q_INVOKABLE bool setGoalFromString(const QString& values);
  Q_INVOKABLE void startFromInitial();
  Q_INVOKABLE void loadSujetExample();
  Q_INVOKABLE void startAutoPlay();
  Q_INVOKABLE void stopAutoPlay();

signals:
  void tilesChanged();
  void startTilesChanged();
  void goalTilesChanged();
  void busyChanged();
  void statusChanged();
  void movesCountChanged();
  void outcomeChanged();
  void solvableChanged();
  void manhattanDistanceChanged();
  void historyChanged();
  void lastSolutionLengthChanged();
  void lastExpandedNodesChanged();
  void autoPlayingChanged();

private:
  void setBusy(bool v);
  void setStatus(const QString& s);
  void setMovesCount(int v);
  void setOutcome(const QString& s);
  void setSolvable(bool v);
  void setManhattan(int v);
  void setLastSolutionLength(int v);
  void setLastExpandedNodes(int v);
  void commitBoard(const taquin::Board& b);
  void appendHistory(const QString& entry);
  void recomputeDerived();
  bool setBoardFromString(const QString& values, taquin::Board& out, QString& error) const;
  QString boardToString(const taquin::Board& b) const;
  void setAutoPlaying(bool v);
  void stopAutoPlayInternal(bool keepStatus = false);

  taquin::Board m_board{};
  taquin::Board m_start{};
  taquin::Board m_goal{};

  bool m_busy = false;
  QString m_status;
  int m_movesCount = 0;
  QString m_outcome = "EN COURS";
  bool m_solvable = true;
  int m_manhattan = 0;
  QStringList m_history;
  int m_lastSolutionLength = 0;
  int m_lastExpandedNodes = 0;
  bool m_autoPlaying = false;
  bool m_pendingAutoPlay = false;

  std::vector<taquin::Move> m_solution;
  int m_solutionPos = 0;
  QTimer m_autoPlayTimer;
};
