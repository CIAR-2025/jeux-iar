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
    title: "Projet 3 - Morpion"

    function cellValue(index) {
        return morpionController.board[index]
    }

    function isLegal(index) {
        return morpionController.isLegalCell(index)
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
                    text: "Jeu du morpion"
                    font.pixelSize: 28
                    font.bold: true
                }

                Label {
                    text: morpionController.statusText
                    font.pixelSize: 14
                    color: morpionController.gameOver ? "#8f1d1d" : "#2d3f2d"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: morpionController.aiThinking
                    spacing: 8

                    Item {
                        implicitWidth: 22
                        implicitHeight: 22

                        Rectangle {
                            anchors.centerIn: parent
                            width: 20
                            height: 20
                            radius: 10
                            color: "transparent"
                            border.width: 2
                            border.color: "#8aa88a"

                            Rectangle {
                                width: 6
                                height: 6
                                radius: 3
                                color: "#2d3f2d"
                                anchors.top: parent.top
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            RotationAnimator on rotation {
                                from: 0
                                to: 360
                                duration: 850
                                loops: Animation.Infinite
                                running: morpionController.aiThinking
                            }
                        }
                    }

                    Label {
                        text: "IA en reflexion..."
                        color: "#2d3f2d"
                        font.pixelSize: 13
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                TabBar {
                    id: tabBar
                    Layout.fillWidth: true
                    currentIndex: 0
                    TabButton { text: "Jeu" }
                    TabButton { text: "Analyse" }
                }

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: tabBar.currentIndex

                    Item {
                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            GridLayout {
                                columns: 2
                                columnSpacing: 8
                                rowSpacing: 8

                                Label { text: "Mode" }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["Humain vs IA", "IA vs IA", "Humain vs Humain"]
                                    currentIndex: morpionController.gameMode
                                    onActivated: morpionController.gameMode = currentIndex
                                }

                                Label { text: "IA X" }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["Debutant", "Moyen", "Expert"]
                                    currentIndex: morpionController.difficultyX
                                    onActivated: morpionController.difficultyX = currentIndex
                                }

                                Label { text: "IA O" }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["Debutant", "Moyen", "Expert"]
                                    currentIndex: morpionController.difficultyO
                                    onActivated: morpionController.difficultyO = currentIndex
                                }
                            }

                            Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                            Label { text: "Scores"; font.bold: true }
                            Label {
                                text: "X: " + morpionController.xCount + "    O: " + morpionController.oCount
                                font.pixelSize: 15
                            }
                            Label {
                                text: "Cases vides: " + morpionController.emptyCount
                                color: "#435343"
                            }

                            Button {
                                text: "Nouvelle partie"
                                Layout.fillWidth: true
                                onClicked: morpionController.resetGame()
                            }

                            Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "Historique"; font.bold: true }
                                Item { Layout.fillWidth: true }
                                Button {
                                    text: "Effacer"
                                    onClicked: morpionController.clearHistory()
                                }
                            }

                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                model: morpionController.history
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

                    Item {
                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            Label {
                                text: "Analyse des configurations"
                                font.bold: true
                                font.pixelSize: 16
                            }

                            Label {
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: "Total: " + morpionController.analysisTotal
                                      + " | Valides: " + morpionController.analysisValid
                                color: "#435343"
                            }

                            Button {
                                text: "Recalculer"
                                Layout.fillWidth: true
                                onClicked: morpionController.runAnalysis()
                            }

                            Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                            Label {
                                text: "Par tour (k = nombre de pions)"
                                font.bold: true
                            }

                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                model: 10
                                delegate: Rectangle {
                                    width: ListView.view.width
                                    height: 30
                                    color: index % 2 === 0 ? "#eef4ee" : "#e6efe6"
                                    radius: 6

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.leftMargin: 8
                                        anchors.rightMargin: 8
                                        Label { text: "k=" + index; Layout.preferredWidth: 48 }
                                        Label {
                                            text: "Valides: " + morpionController.analysisValidByTurn[index]
                                            Layout.fillWidth: true
                                        }
                                        Label {
                                            text: "Gagnantes: " + morpionController.analysisWinningByTurn[index]
                                            Layout.preferredWidth: 130
                                        }
                                    }
                                }
                            }
                        }
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
                            model: 9

                            Rectangle {
                                readonly property int value: cellValue(index)
                                width: (boardGrid.width - (boardGrid.columns - 1) * boardGrid.columnSpacing) / boardGrid.columns
                                height: (boardGrid.height - (boardGrid.rows - 1) * boardGrid.rowSpacing) / boardGrid.rows
                                radius: 10
                                color: isLegal(index) && !morpionController.gameOver ? "#b9d4a7" : "#d9e6d9"
                                border.color: "#7b917b"
                                border.width: 1

                                MouseArea {
                                    anchors.fill: parent
                                    enabled: !morpionController.gameOver
                                    onClicked: morpionController.playCell(index)
                                }

                                Rectangle {
                                    anchors.centerIn: parent
                                    width: parent.width * 0.76
                                    height: width
                                    radius: 14
                                    visible: value !== 0
                                    color: "#101010"
                                    border.color: "#2e2e2e"
                                    border.width: 2

                                    Text {
                                        anchors.centerIn: parent
                                        text: value === 1 ? "X" : "O"
                                        font.pixelSize: Math.max(26, parent.width * 0.38)
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
