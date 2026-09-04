# CONTEXT

Domain vocabulary for the SQLite Query Analyzer codebase. Use these terms in
discussions, code, and documentation. The aim is shared language so the
architecture, the modules, and the tests all refer to the same things.

## Core domain objects

- **Database** — an open SQLite file. The thing the user is currently looking
  at. Has a filename, may be open or closed, owns a single Qt SQL connection.
- **Schema** — the set of tables, columns, and indexes inside a Database.
  Always filtered to user-visible objects (excludes `sqlite_sequence` and
  `sqlite_stat1`).
- **Table** — a named relation in the Schema. Owns a list of Columns and
  Indexes.
- **Column** — a typed field of a Table (name, declared type, nullability,
  default, primary-key flag).
- **Index** — a secondary access path over a Table (name, column, unique).
- **DatabaseInfo** — a snapshot of a Schema plus file-level metadata
  (filename, size, creation date, password-protected flag). Produced by the
  Analyzer; consumed by the Tree and the Exporter.

## User-facing actions

- **Query** — a single SQL statement the user wrote and wants executed.
- **Script** — a sequence of Queries loaded from a `.sql` file.
- **Export** — a one-shot transformation of Database content into a different
  representation. Two formats: **CSV** (one file per table) and **SQL**
  (single script of `INSERT` statements).
- **Session** — the user's last-opened Database, the text they had in the
  query editor, and the last folder they exported to. Persisted between runs.
- **Zoom** — a scale factor the user applies to the text of the query editor
  and the Tree, in discrete steps away from the size those widgets were built
  with. Step 0 is the untouched size. Part of the window state, so it is
  persisted between runs.

## Modules (current shape)

- **Analyzer** — reads a Database, produces a DatabaseInfo.
- **Tree** — renders a DatabaseInfo into the left-hand QTreeWidget.
- **QueryExecutor** — runs a list of Queries against a Database, returns
  results. Pure logic, no widgets.
- **PagedResult** — a lazily fetched view over a Query's result set. Rows are
  read in pages as they are scrolled into view, so a result set of any size can
  be browsed without holding it in memory. Ordering is done by the Database,
  not over the rows already fetched.
- **QueryResultPresenter** — renders QueryExecutor output into the result
  area of the main window. Owns the scroll area and table views.
- **SchemaExporter** — produces a `CREATE TABLE` script from a DatabaseInfo.
- **DataExporter** — produces CSV files or an `INSERT` script from a
  Database + DatabaseInfo. Long-running; reports progress; cancellable.
- **ExportOrchestrator** — runs a DataExporter in the background, marshals
  progress to the GUI thread, surfaces cancel and completion.
- **ZoomPresenter** — applies a Zoom to the query editor and the Tree, and
  turns the zoom gestures over those widgets into steps.
- **SessionManager** — persists and restores the Session and window state.
- **MainWindow** — Qt shell. Wires the modules to menu actions and the UI
  form. Does not contain business logic.

## Seams

- **IDatabase** — abstract Database. Production adapter is `SqliteDatabase`;
  test adapter is `InMemoryDatabase`. Anything that takes a Database takes
  `IDatabase*`. `QSqlDatabase` does not leak across this seam.
- **ExportStrategy** — abstract DataExporter workflow (`SqlExportStrategy`,
  `CsvExportStrategy`). Two adapters = real seam.

## Out of scope (do not introduce)

- **Component / Service / API / Boundary** — banned architectural words.
  Use *Module*, *Adapter*, *Seam* instead.
