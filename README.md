# Jeux IAR (C++ / Qt)

Projet d’introduction à l’intelligence artificielle : modélisation de jeux comme **problèmes de recherche**, puis résolution par algorithmes classiques (A\*, Minimax, Alpha-Bêta).

## Prérequis

- **CMake** ≥ 3.16  
- **Compilateur C++17** (Clang, GCC, MSVC)  
- **Qt 6** avec **Quick** (Qt 6.10+ recommandé, voir `qt_standard_project_setup` dans chaque `CMakeLists.txt`)  
- Pour `jeux_virus` uniquement : composant **Multimedia** (son)

## Contenu du dépôt

| Dossier | Projet | Rôle principal |
|--------|--------|----------------|
| `taquin_ui/` | **Projet 1 — Taquin (8-puzzle)** | Grille 3×3, A\*, heuristique Manhattan, UI Qt/QML |
| `jeux_virus/` | **Projet 2 — Jeu du virus** | Grille 7×7, Minimax + Alpha-Bêta, UI Qt/QML + console |
| `jeux_morpion/` | **Projet 3 — Morpion** | Grille 3×3, Minimax + Alpha-Bêta, analyse des configurations, UI Qt/QML + console |
| `Projet-Jeux-IAR.md` | Énoncé | Texte de référence du sujet |

---

## 1) Taquin (`taquin_ui/`)

### Fonctionnalités

- Plateau 3×3 (8 tuiles + 1 case vide)
- Déplacements valides (clic sur tuile adjacente à la case vide)
- États initial / final configurables, chargement d’un exemple du sujet
- Vérification de **solvabilité** (parité des inversions)
- **A\*** avec \(f = g + h\), **h = distance de Manhattan**
- Lecture pas à pas de la solution, auto-play, historique des coups

### Build et exécution

```bash
cd taquin_ui
cmake -S . -B build
cmake --build build -j
```

**macOS** (bundle Qt) :

```bash
open build/apptaquin_ui.app
# ou en ligne de commande :
./build/apptaquin_ui.app/Contents/MacOS/apptaquin_ui
```

**Linux / Windows** : l’exécutable se trouve généralement sous `build/` (nom selon la cible CMake, souvent `apptaquin_ui`).

---

## 2) Jeu du virus (`jeux_virus/`)

### Fonctionnalités

- Grille **7×7**, position initiale sur coins opposés
- Coup légal : case vide **adjacente (8 voisins)** à un pion du joueur
- **Conversion** des pions adverses voisins après placement
- Passes si aucun coup légal ; fin au **remplissage** ou **blocage**
- Gagnant : **plus grand nombre de pions**
- **IA Minimax + Alpha-Bêta**, 3 niveaux (Débutant, Moyen, Expert)
- Modes : Humain vs IA, IA vs IA, Humain vs Humain
- Historique des coups, retour visuel des conversions, son optionnel (GUI)

### Build et exécution (interface graphique)

```bash
cd jeux_virus
cmake -S . -B build
cmake --build build -j
```

**macOS** :

```bash
open build/appjeux_virus.app
./build/appjeux_virus.app/Contents/MacOS/appjeux_virus
```

### Build et exécution (console)

```bash
cd jeux_virus
cmake -S . -B build
cmake --build build -j
./build/virus_console
```

---

## 3) Morpion (`jeux_morpion/`)

### Fonctionnalités

- Morpion **3×3** (X = joueur MAX, O = joueur MIN)
- **Minimax + Alpha-Bêta**, 3 niveaux de difficulté pour l’IA
- Modes : Humain vs IA, IA vs IA, Humain vs Humain
- **Analyse des configurations** (énumération sur \(3^9\) grilles) :
  - configurations **valides** (contraintes du sujet : écart X/O, pas de double victoire, cohérence après victoire)
  - décompte par **tour** (nombre de coups joués)
  - décompte des configurations **gagnantes** par tour
- Historique des coups, indicateur « IA en réflexion » (GUI)

### Build et exécution (interface graphique)

```bash
cd jeux_morpion
cmake -S . -B build
cmake --build build -j
```

**macOS** :

```bash
open build/appjeux_morpion.app
./build/appjeux_morpion.app/Contents/MacOS/appjeux_morpion
```

### Build et exécution (console)

```bash
cd jeux_morpion
cmake -S . -B build
cmake --build build -j
./build/morpion_console
```

Le menu console permet notamment de lancer l’**analyse exhaustive** ou des parties contre l’IA.

---

## Notes

- Un `.gitignore` à la racine ignore les répertoires de build CMake/Qt et les fichiers IDE.
- L’énoncé détaillé se trouve dans `Projet-Jeux-IAR.md` (et `Projet-Jeux-IAR.pdf` si présent dans votre copie locale).
