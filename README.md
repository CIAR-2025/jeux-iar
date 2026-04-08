# Jeux IAR (C++ / Qt)

Projet d'introduction a l'IA base sur des jeux de recherche.

## Contenu du depot

- `taquin_ui/` : Taquin 3x3 (8 pieces) avec interface Qt/QML
- `jeux_virus/` : Jeu du virus 7x7 avec interface Qt/QML + mode console
- `Projet-Jeux-IAR.md` : enonce du projet

## 1) Taquin (`taquin_ui`)

### Fonctionnalites

- Plateau 3x3 (8 tuiles + 1 case vide)
- Deplacements valides via clic sur tuile adjacente (Gauche/Droite/Haut/Bas)
- Etats initial/final configurables
- Chargement de l'exemple du sujet
- Verification de solvabilite (parite)
- Heuristique de Manhattan
- Resolution A* (`g + h`)
- Lecture pas-a-pas de la solution
- Auto-play de la solution
- Historique des coups

### Build & run

```bash
cd taquin_ui
cmake -S . -B build
cmake --build build -j
./build/apptaquin_ui.app/Contents/MacOS/apptaquin_ui
```

## 2) Jeu du virus (`jeux_virus`)

### Fonctionnalites

- Grille 7x7
- Position initiale sur coins opposes
- Coup legal: case vide adjacente (8 voisins) a un pion du joueur
- Conversion des pions adverses voisins apres placement
- Gestion des passes si aucun coup legal
- Fin de partie + gagnant au nombre de pions
- IA Minimax + Alpha-Beta
- 3 niveaux: Debutant, Moyen, Expert
- Modes: Humain vs IA, IA vs IA, Humain vs Humain
- Historique des coups
- UI Qt/QML + executable console (`virus_console`)

### Build & run (UI)

```bash
cd jeux_virus
cmake -S . -B build
cmake --build build -j
./build/appjeux_virus.app/Contents/MacOS/appjeux_virus
```

### Build & run (console)

```bash
cd jeux_virus
cmake -S . -B build
cmake --build build -j
./build/virus_console
```

## Notes

- Un `.gitignore` racine est fourni pour ignorer les dossiers de build Qt/CMake et fichiers IDE.
- Le projet 3 (Morpion/Tic-Tac-Toe) de l'enonce n'est pas encore implemente dans ce depot.
