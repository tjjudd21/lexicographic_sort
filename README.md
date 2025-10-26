Modern C++ tool that prints all permutations of a string using Johnson–Trotter (adjacent swaps, distinct symbols) and/or Lexicographic order (std::next_permutation, supports duplicates). Clean CLI, stdin for input, optional self-tests.

# Johnson–Trotter + Lexicographic Permutations

Generate all permutations of a string using:

- **Johnson–Trotter (JT)** — adjacent transpositions; **requires all distinct characters**. Prints **direction arrows ABOVE the characters** for each step, mirroring the legacy output. Arrows are ASCII `"<-"` and `"->"`. :contentReference[oaicite:2]{index=2} :contentReference[oaicite:3]{index=3}
- **Lexicographic** order — via `std::next_permutation`; handles duplicates.

Input is read from **stdin** (a single line). Flags are passed via **argv**. 

---

## Features

- **Arrows on top (default):** two-line JT print: first line shows arrows, second line shows characters, aligned under the step header. :contentReference[oaicite:6]{index=6}
- **Algorithm selection:** `--algo jt|lex|both` (default: `both`). :contentReference[oaicite:7]{index=7}
- **Quiet mode:** `--quiet` suppresses per-step prints, still shows summaries. :contentReference[oaicite:8]{index=8}
- **Arrows toggle:** `--arrowsoff` hides JT arrows and prints only characters. :contentReference[oaicite:9]{index=9}
- **Self-tests:** `--test` runs a small suite (JT properties, duplicates handling, lex cases, edge cases). :contentReference[oaicite:10]{index=10}

---

## Build

### g++ (Windows / Linux / macOS)
```bash
g++ -std=gnu++17 -O2 -Wall -Wextra -pedantic johnson-trotter-lex.cpp -o johnson-trotter-lex
```
Windows will produce johnson-trotter-lex.exe.
Put the binary on your PATH or run it from the build directory.

---

## Usage

Input: one line from stdin – your raw string only (no flags).
Flags:

--algo jt|lex|both (default: both)

--quiet

--test

--help

--arrowsoff

Windows PowerShell
```
# Johnson–Trotter only (distinct characters)
'ABC' | .\johnson-trotter-lex.exe --algo jt

# Lexicographic only (supports duplicates)
'BANANA' | .\johnson-trotter-lex.exe --algo lex

# Both algorithms, quiet mode
'HELLO' | .\johnson-trotter-lex.exe --algo both --quiet

# Help / Tests
.\johnson-trotter-lex.exe --help
.\johnson-trotter-lex.exe --test

```

---

## Examples

JT on ABC

```
== Johnson–Trotter (3! up to 6) ==
    <- <- <-
[1] A  B  C
    <- <- <-
[2] A  C  B
    <- <- <-
[3] C  A  B
    -> <- <-
[4] C  B  A
    <- -> <-
[5] B  C  A
    <- <- ->
[6] B  A  C
-- Count: 6
```
Lex on AAB
```
== Lexicographic (std::next_permutation) ==
[0] AAB
[1] ABA
[2] BAA
-- Count: 3
```

---

## Options

--algo jt
Johnson–Trotter. All characters must be distinct; duplicates cause JT to be skipped with a warning.

--algo lex
Lexicographic using std::next_permutation. Works with duplicates.

--algo both
Runs JT (if valid) then Lexicographic.

--quiet
Suppresses per-step prints; still prints summary counts.

--test
Runs self-tests and exits.

--help
Prints usage and exits.
