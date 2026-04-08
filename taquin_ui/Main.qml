import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1080
    height: 760
    minimumWidth: 980
    minimumHeight: 720
    visible: true
    title: "Taquin (8 pieces) - A* Manhattan"

    property string startInputText: (taquinGame.startTiles || []).join(" ")
    property string goalInputText: (taquinGame.goalTiles || []).join(" ")

    Connections {
        target: taquinGame
        function onStartTilesChanged() {
            startInputText = (taquinGame.startTiles || []).join(" ")
        }
        function onGoalTilesChanged() {
            goalInputText = (taquinGame.goalTiles || []).join(" ")
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#f4f7f4" }
            GradientStop { position: 1.0; color: "#dde8dc" }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 16

        Frame {
            Layout.fillHeight: true
            Layout.preferredWidth: 360

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Jeu du taquin"
                    font.pixelSize: 28
                    font.bold: true
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: taquinGame.status
                    color: taquinGame.outcome === "IMPOSSIBLE" ? "#8f1d1d" : "#2d3f2d"
                    font.pixelSize: 14
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: taquinGame.busy
                    spacing: 8

                    BusyIndicator {
                        running: taquinGame.busy
                        visible: taquinGame.busy
                        implicitWidth: 24
                        implicitHeight: 24
                    }

                    Label {
                        text: "A* en cours..."
                        color: "#2d3f2d"
                        font.pixelSize: 13
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                Label { text: "Configuration"; font.bold: true }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 8
                    rowSpacing: 8

                    Label { text: "Etat initial" }
                    TextField {
                        Layout.fillWidth: true
                        text: startInputText
                        placeholderText: "7 0 5 1 6 4 2 8 3"
                        enabled: !taquinGame.busy
                        selectByMouse: true
                        color: "#233323"
                        onTextChanged: startInputText = text
                    }

                    Label { text: "Etat final" }
                    TextField {
                        Layout.fillWidth: true
                        text: goalInputText
                        placeholderText: "7 6 5 8 0 4 1 2 3"
                        enabled: !taquinGame.busy
                        selectByMouse: true
                        color: "#233323"
                        onTextChanged: goalInputText = text
                    }
                }

                Flow {
                    Layout.fillWidth: true
                    width: parent.width
                    spacing: 8

                    Button {
                        text: "Appliquer etats"
                        enabled: !taquinGame.busy
                        onClicked: {
                            let okStart = taquinGame.setStartFromString(startInputText)
                            let okGoal = taquinGame.setGoalFromString(goalInputText)
                            if (okStart && okGoal) taquinGame.startFromInitial()
                        }
                    }

                    Button {
                        text: "Exemple sujet"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.loadSujetExample()
                    }

                    Button {
                        text: "Revenir initial"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.startFromInitial()
                    }

                    Button {
                        text: "Reset"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.resetToGoal()
                    }

                    Button {
                        text: "Melanger"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.shuffle(90)
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                Label { text: "Resolution"; font.bold: true }

                Flow {
                    Layout.fillWidth: true
                    width: parent.width
                    spacing: 8

                    Button {
                        text: "Resoudre (A*)"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.solve()
                    }

                    Button {
                        text: "Pas"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.applySolutionStep()
                    }

                    Button {
                        text: taquinGame.autoPlaying ? "Stop auto" : "Auto-play"
                        enabled: !taquinGame.busy || taquinGame.autoPlaying
                        onClicked: {
                            if (taquinGame.autoPlaying) taquinGame.stopAutoPlay()
                            else taquinGame.startAutoPlay()
                        }
                    }
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: "Issue: " + taquinGame.outcome
                          + "  |  Coups: " + taquinGame.movesCount
                          + "  |  Manhattan: " + taquinGame.manhattanDistance
                          + "\nSoluble: " + (taquinGame.solvable ? "oui" : "non")
                          + "  |  Derniere solution: " + taquinGame.lastSolutionLength
                          + "  |  Noeuds explores: " + taquinGame.lastExpandedNodes
                    color: "#435343"
                    font.pixelSize: 13
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                RowLayout {
                    Layout.fillWidth: true
                    Label { text: "Historique"; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: "Effacer"
                        enabled: !taquinGame.busy
                        onClicked: taquinGame.clearHistory()
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: taquinGame.history
                    clip: true
                    delegate: Label {
                        width: ListView.view.width
                        text: (index + 1) + ". " + modelData
                        font.pixelSize: 13
                        elide: Text.ElideRight
                    }
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                anchors.fill: parent

                Rectangle {
                    id: boardBg
                    width: Math.min(parent.width - 24, parent.height - 24)
                    height: width
                    radius: 14
                    anchors.centerIn: parent
                    color: "#6f8f6f"
                    border.color: "#4a654a"
                    border.width: 2

                    Grid {
                        id: boardGrid
                        anchors.fill: parent
                        anchors.margins: 12
                        rows: 3
                        columns: 3
                        rowSpacing: 6
                        columnSpacing: 6

                        Repeater {
                            model: taquinGame.tiles.length

                            Rectangle {
                                required property int index
                                readonly property int value: taquinGame.tiles[index]

                                width: (boardGrid.width - (boardGrid.columns - 1) * boardGrid.columnSpacing) / boardGrid.columns
                                height: (boardGrid.height - (boardGrid.rows - 1) * boardGrid.rowSpacing) / boardGrid.rows
                                radius: 10
                                color: value === 0 ? "#d9e6d9" : "#f0f5f0"
                                border.color: value === 0 ? "#91a891" : "#7b917b"
                                border.width: 1

                                MouseArea {
                                    anchors.fill: parent
                                    enabled: !taquinGame.busy && value !== 0
                                    onClicked: taquinGame.moveIndex(index)
                                }

                                Rectangle {
                                    id: tileDisc
                                    anchors.centerIn: parent
                                    width: parent.width * 0.78
                                    height: width
                                    radius: 12
                                    visible: value !== 0
                                    color: "#101010"
                                    border.color: "#2e2e2e"
                                    border.width: 2

                                    Text {
                                        anchors.centerIn: parent
                                        text: value
                                        font.pixelSize: Math.max(22, tileDisc.width * 0.33)
                                        font.bold: true
                                        color: "#f0f0f0"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
