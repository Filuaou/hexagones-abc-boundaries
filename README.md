# Hexagones: A-B-C coordinate boundary extraction

**Philippe Mullhaupt**  
Laboratoire d'Automatique, EPFL

Research materials accompanying the manuscript **An A-B-C Coordinate Construction for Exact Boundary-Cycle Extraction on Hexagonal Lattices**. The preprint is available in the arXiv Combinatorics category: **[arXiv:2610.25451](https://arxiv.org/abs/2610.25451)**.

The work describes an integer-coordinate, role-qualified point/tail representation of hexagonal cell boundaries, exact edge cancellation, and extraction of boundary cycles, including holes. It compares the encoding against a cube-coordinate vertex-triple/half-edge hashing baseline.

## Hexagones 2.0 — original application

![Hexagones 2.0: hexagonal terrain, rivers, roads, railways, boundaries and military counters](images/HexagonesScreenshot.png)

*Hexagones 2.0 — an example game map showing terrain, transport networks, boundaries, and military counters.*

The Hexagones game provides the practical context for the boundary-extraction work. The complete game is not distributed in this repository.

## Contents

- `paper/main.tex` — LaTeX article with editable TikZ figures in `paper/figures/`.
- `paper/hexagones_boundary_comparison.pdf` — compiled manuscript for reading.
- `verification/verify_appendices.py` — checks for the A-B-C identities and finite regions.
- `verification/compare_encodings.py` — independent cube-coordinate baseline and comparison checks.
- `original-c/` — two **unaltered source excerpts** from the Hexagones game (not standalone programs). They require the original project environment and are included for inspection of the historical implementation, not as a claimed reproducible C build.

## Reproduce the checks

```sh
python3 verification/verify_appendices.py
python3 verification/compare_encodings.py
python3 verification/compare_encodings.py --benchmark
```

The benchmark is illustrative, not a controlled comparison with an optimized half-edge implementation.

## Build the paper

```sh
cd paper
pdflatex -interaction=nonstopmode -halt-on-error main.tex
pdflatex -interaction=nonstopmode -halt-on-error main.tex
```

Requires a TeX distribution with TikZ and standard LaTeX packages.

## Scientific scope

Three-axis coordinates, boundary operators, and cycle extraction are established concepts. The manuscript focuses on the explicit A-B-C point/tail incidence encoding. No historical-priority claim is made. The arXiv preprint is publicly available; arXiv posting does not constitute journal peer review.

## Game design acknowledgments

The game design of **Hexagones 2.0** draws inspiration from the original **PanzerKrieg** board game published by **Avalon Hill**. This acknowledgment recognizes the historical game-design inspiration; Hexagones 2.0 is not presented as an official Avalon Hill product or an endorsement by its rights holders.

Special thanks to **Dr. Gianluca Vecchietti** for directing the game-design work and for the many games played together, whose experience and discussions helped shape the project.

## Attribution and code status

The author is Philippe Mullhaupt. ChatGPT (OpenAI) assisted with code inspection, refactoring-related analysis, mathematical exposition, verification scripts, and LaTeX/TikZ preparation. The two C excerpts are copied without modifications in this repository; this does not establish that every other file in the game was unchanged during prior restoration work.

## Copyright and licensing

Copyright (c) 2026 Philippe Mullhaupt. The work is personally authored; **Laboratoire d'Automatique, EPFL** is the author's professional affiliation only and does not imply EPFL ownership or endorsement.

- **MIT license** (`LICENSE-CODE`): `original-c/` and `verification/` source code.
- **Creative Commons Attribution 4.0 International** (`LICENSE-PAPER`): the article, PDF, TikZ illustrations, and original explanatory documentation.

The original C excerpts remain unmodified. These licenses reflect the author's stated choice for this repository; they do not grant rights over any third-party dependencies or the unreleased game project.
