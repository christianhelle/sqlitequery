# SQLite Query Analyzer — Domain Model

## Domain Glossary

**Database** — A SQLite file that the application opens, queries, and manipulates. The Database module manages the connection lifecycle (open, close, shrink).

**Query** — A SQL statement or list of SQL statements submitted to the database for execution. Queries may return results (SELECT) or modify the database (INSERT, UPDATE, DELETE, CREATE, DROP).

**Result** — The output of a query: a set of rows with columns, plus any errors that occurred during execution.

**Export** — Writing database content to an external format. Two kinds:
- **Schema export** — Converting the database schema to CREATE TABLE SQL statements.
- **Data export** — Converting table data to SQL INSERT statements or CSV files.

**Analyzer** — The module that inspects a database file and populates a `DatabaseInfo` struct with metadata: filename, file size, creation date, tables, columns, and their properties.

**Table** — A named collection of rows in the database. Has columns with names, types, and constraints (primary key, not null, default value).

**Column** — A named attribute within a table with a data type and constraints.

**Tree** — The hierarchical UI representation of the database schema: database info nodes and table/column nodes.

**Session** — The application's saved state: open database file, unsaved query text, window geometry, and splitter positions.

**Recent File** — A recently opened database file path, persisted to disk for quick re-opening.

**Prompt** — A user-facing dialog for confirmation, file selection, or input (e.g., confirm delete, choose export path, specify CSV delimiter).

**Highlighter** — A SQL syntax highlighter that applies formatting rules to the query editor text.

**Cancellation** — A mechanism to signal long-running operations (export, script execution) to stop. Uses `CancellationToken` and `CancellationTokenSource`.

**Progress** — A progress tracker for long-running operations, reporting the number of rows processed.

**SessionManager** — The module that manages application session persistence: saving and restoring window state, open database file, unsaved query text, and splitter positions.

**DatabaseManager** — The module that manages database lifecycle operations: opening, analyzing, and closing databases.

**ExportOrchestrator** — The module that coordinates async export operations (SQL script, CSV) with cancellation support.

**QueryResult** — A plain struct representing query execution output: success flag, rows with column values, and errors.

**ISchemaScript** — Interface for schema export operations. The seam between schema generation and callers.

**IDataExport** — Interface for data export operations. The seam between data export and callers.

**IQueryExecutor** — Interface for SQL query execution. The seam between query execution and result presentation.

**IQueryResultPresenter** — Interface for displaying query results. The seam between query results and UI.

**Architecture Vocabulary**

See [LANGUAGE.md](LANGUAGE.md) for full definitions.

**Module** — Anything with an interface and implementation (function, class, package).

**Interface** — Everything a caller must know to use the module: types, invariants, error modes, ordering, config.

**Seam** — Where an interface lives; a place behaviour can be altered without editing in place.

**Adapter** — A concrete thing satisfying an interface at a seam.

**Depth** — Leverage at the interface: a lot of behaviour behind a small interface.

**Locality** — What maintainers get from depth: change, bugs, knowledge concentrated in one place.

**Leverage** — What callers get from depth.
