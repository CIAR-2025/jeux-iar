import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

ApplicationWindow {
    id: root
    width: 1080
    height: 760
    visible: true
    title: "Projet 2 - Jeu du virus"

    property int n: gameController.boardSize
    property bool soundEnabled: true

    function cellValue(row, col) {
        var index = row * n + col
        return gameController.board[index]
    }

    function isLegal(row, col) {
        return gameController.isLegalMove(row, col)
    }

    function isConverted(row, col) {
        var index = row * n + col
        return gameController.convertedIndices.indexOf(index) !== -1
    }

    SoundEffect {
        id: flipSound
        source: "qrc:/qt/qml/jeux_virus/assets/flip.wav"
        volume: soundEnabled ? 0.72 : 0.0
    }

    Connections {
        target: gameController
        function onFlipSoundRequested() {
            flipSound.stop()
            flipSound.play()
        }
    }

    Dialog {
        id: conclusionDialog
        title: "A propos"
        modal: true
        standardButtons: Dialog.Ok
        width: 720

        contentItem: ColumnLayout {
            spacing: 10

            Label {
                text: "Projet 2 - Jeu du virus"
                font.bold: true
                font.pixelSize: 20
                color: "#2d3f2d"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            Label {
                text: "Le jeu du virus se joue sur une grille 7x7. Chaque joueur place un pion sur une case vide adjacente a l'un de ses pions."
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            Label {
                text: "Apres chaque placement, tous les pions adverses voisins sont convertis. La partie se termine quand plus aucun coup n'est possible."
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            Label {
                text: "Cette version integre une IA Minimax avec optimisation Alpha-Beta, 3 niveaux de difficulte (Debutant, Moyen, Expert), et les modes Humain vs IA, IA vs IA, Humain vs Humain."
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            Label {
                text: "Fonctionnalites: surbrillance des conversions, son optionnel, historique des coups et statistiques IA."
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                color: "#435343"
            }
            Label {
                text: "Version interface: Qt Quick / QML"
                color: "#435343"
            }
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
            Layout.preferredWidth: 320

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Jeu du virus"
                    font.pixelSize: 28
                    font.bold: true
                }

                Label {
                    text: gameController.statusText
                    font.pixelSize: 15
                    color: gameController.gameOver ? "#8f1d1d" : "#2d3f2d"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: gameController.aiThinking
                    spacing: 8

                    Item {
                        visible: gameController.aiThinking
                        implicitWidth: 22
                        implicitHeight: 22

                        Rectangle {
                            id: spinRing
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
                                running: gameController.aiThinking
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

                GridLayout {
                    columns: 2
                    columnSpacing: 8
                    rowSpacing: 8

                    Label { text: "Mode" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Humain vs IA", "IA vs IA", "Humain vs Humain"]
                        currentIndex: gameController.gameMode
                        onActivated: gameController.gameMode = currentIndex
                    }

                    Label { text: "IA Blanc" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Debutant", "Moyen", "Expert"]
                        currentIndex: gameController.difficultyWhite
                        onActivated: gameController.difficultyWhite = currentIndex
                    }

                    Label { text: "IA Noir" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Debutant", "Moyen", "Expert"]
                        currentIndex: gameController.difficultyBlack
                        onActivated: gameController.difficultyBlack = currentIndex
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                Label { text: "Score"; font.bold: true }
                Label {
                    text: "Blanc: " + gameController.whiteScore + "    Noir: " + gameController.blackScore
                    font.pixelSize: 16
                }
                Label {
                    text: "Cases vides: " + gameController.emptyCount
                    color: "#435343"
                }

                Button {
                    text: "Nouvelle partie"
                    Layout.fillWidth: true
                    onClicked: gameController.resetGame()
                }

                Button {
                    Layout.fillWidth: true
                    checkable: true
                    checked: soundEnabled
                    text: checked ? "Son ON" : "Muet"
                    onToggled: soundEnabled = checked
                }

                Button {
                    Layout.fillWidth: true
                    text: "A propos"
                    onClicked: conclusionDialog.open()
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#c8d3c8" }

                Label { text: "Historique"; font.bold: true }
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: gameController.history
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
                        rows: n
                        columns: n
                        rowSpacing: 4
                        columnSpacing: 4

                        Repeater {
                            model: n * n

                            Rectangle {
                                readonly property int row: Math.floor(index / n)
                                readonly property int col: index % n
                                readonly property int value: cellValue(row, col)
                                property int previousValue: value

                                width: (boardGrid.width - (n - 1) * boardGrid.columnSpacing) / n
                                height: (boardGrid.height - (n - 1) * boardGrid.rowSpacing) / n
                                radius: 8
                                color: isLegal(row, col) && !gameController.gameOver ? "#b9d4a7" : "#d9e6d9"
                                border.color: "#7b917b"
                                border.width: 1

                                onValueChanged: {
                                    if (previousValue !== value && value !== 0) {
                                        pieceDisc.scale = 0.35
                                        pieceDisc.opacity = 0.4
                                        popAnim.restart()
                                        fadeAnim.restart()
                                    }
                                    previousValue = value
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    enabled: !gameController.gameOver
                                    onClicked: gameController.playCell(parent.row, parent.col)
                                }

                                Rectangle {
                                    id: pieceDisc
                                    anchors.centerIn: parent
                                    width: parent.width * 0.62
                                    height: width
                                    radius: width / 2
                                    visible: value !== 0
                                    color: value === 1 ? "#f0f0f0" : "#101010"
                                    border.color: value === 1 ? "#bcc0c0" : "#2e2e2e"
                                    border.width: 2
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    radius: parent.radius
                                    color: "#ffd66b"
                                    opacity: isConverted(row, col) ? 0.5 : 0.0

                                    Behavior on opacity {
                                        NumberAnimation {
                                            duration: 180
                                            easing.type: Easing.OutQuad
                                        }
                                    }
                                }

                                SequentialAnimation {
                                    id: popAnim
                                    NumberAnimation {
                                        target: pieceDisc
                                        property: "scale"
                                        to: 1.08
                                        duration: 110
                                        easing.type: Easing.OutQuad
                                    }
                                    NumberAnimation {
                                        target: pieceDisc
                                        property: "scale"
                                        to: 1.0
                                        duration: 90
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                                NumberAnimation {
                                    id: fadeAnim
                                    target: pieceDisc
                                    property: "opacity"
                                    to: 1.0
                                    duration: 170
                                    easing.type: Easing.OutQuad
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
