# AGENTS

Conventions for anyone (human or AI) modifying this codebase.

## Build verification

Before any commit, the build must pass:

```pwsh
pwsh -NoProfile -File build.ps1
```

This runs CMake configure + Ninja build + `windeployqt`. Incremental builds
take ~5–10s; full clean builds take longer. Do not commit if it fails.

## Domain vocabulary

Read [`CONTEXT.md`](./CONTEXT.md) before naming anything. New modules should
be named after domain concepts from that file, not after Qt classes or
implementation details. If you introduce a new domain term, add it to
`CONTEXT.md` in the same commit.

## Architectural rules

- **Seams are abstract.** Production code holds pointers/references to
  abstract interfaces, not concrete adapters. The compiler enforces this.
- **Two adapters per seam.** A seam with only one adapter is hypothetical
  and tends to leak. If you add a new abstraction, ship at least two
  implementations (production + test, or two production formats).
- **The interface is the test surface.** Tests call the abstract API. They
  never `friend` into a concrete class or poke at its privates.
- **No `QSqlDatabase` outside the `database` module.** If a caller needs to
  run SQL, it goes through the seam. The seam owns query construction.
- **No widget pointers in non-GUI modules.** A module that takes a
  `QWidget*` is a presenter, not a domain module. Presenters are thin.
- **No SQL string concatenation in non-database modules.** `dbquery.h`
  splitting-by-`;` lives in the database module, not in `mainwindow.cpp` or
  the CLI.

## Banned vocabulary

`component`, `service`, `API`, `boundary`, `manager` (as a suffix on a
domain word — `SessionManager` survives because it's a name, not a pattern
to extend). Use `module`, `adapter`, `seam`, `presenter`, `executor`,
`orchestrator`.

## Commit discipline

- One logical change per commit.
- Build must pass before each commit.
- Commit message: short subject line (<=72 chars), imperative mood. Body
  only when the *why* isn't obvious from the diff.
- Don't commit: build outputs, `.env` files, secrets, generated files.
