# Squad Decisions

## Active Decisions

### 1. Workflow Security and Version Constant Modernization

**Date:** 2026-01
**Decider:** Ripley (Lead)
**Status:** Implemented

**Problem:** SonarCloud flagged script injection vulnerability (githubactions:S7630) via direct interpolation of `${{ inputs.version }}` in PowerShell run blocks, plus macro-based version constant (cpp:S5028).

**Decision:** Move GitHub Actions inputs to step-level environment variables; replace preprocessor macro with `constexpr` typed constant.

**Pattern:**
```yaml
env:
  VERSION: ${{ inputs.version }}
run: |
  $version = ... -replace ... "constexpr auto Version = `"$env:VERSION`";"
```

**Files:** `.github/workflows/windows-template.yml`, `macos-template.yml`, `linux-template.yml`, `src/main.cpp`

**Impact:** Resolves 5 BLOCKER/CRITICAL findings; hardens CI/CD pipeline against script injection.

---

### 2. Rely on Qt Parent-Child Ownership for Widget Memory Management

**Date:** 2026-01
**Decider:** Fenster (Backend Dev)
**Status:** Implemented

**Problem:** SonarCloud flagged 16 ownership issues: raw `new` allocations without RAII (cpp:S5025), redundant initializer lists (cpp:S3230), non-const read-only pointers (cpp:S5350).

**Decision:** Adopt Qt's parent-child ownership model with RAII transfer via `std::make_unique` + `.release()`.

**Pattern:**
```cpp
auto item = std::make_unique<QTreeWidgetItem>();
const auto itemPtr = item.get();
tree->addTopLevelItem(item.release());
```

**Files:** `src/database/dbtree.cpp`, `dbtree.h`, `src/database/dbquery.cpp`, `dbquery.h`, `src/gui/mainwindow.cpp`, `mainwindow.h`

**Impact:** 16 findings resolved; RAII protection against exception-induced leaks; eliminated double-deletion risk.

---

### 3. SonarCloud GUI and Docs Modernization

**Date:** 2026-01-28
**Decider:** Dallas (Frontend Dev)
**Status:** Implemented

**Problem:** SonarCloud flagged 8 issues: 1 C++ (cpp:S5018) in move semantics, 7 JavaScript (S7761, S6582, S7764) in theme script using legacy patterns.

**Decision:** Explicitly default noexcept move members in `HighlightingRule`; modernize JavaScript with dataset API, optional chaining, and `globalThis`.

**Patterns:**
```cpp
// C++: Explicit noexcept move
HighlightingRule(HighlightingRule&&) noexcept = default;
HighlightingRule& operator=(HighlightingRule&&) noexcept = default;
```

```javascript
// JavaScript: Modern APIs
body.dataset.theme  // instead of getAttribute
globalThis.matchMedia?.()  // instead of window.matchMedia && window.matchMedia()
```

**Files:** `src/gui/highlighter.h`, `docs/script.js`

**Impact:** 8 findings resolved; improved type safety and cross-environment compatibility; zero behavior change.

---

### 4. Sonar Shell Script Modernization Strategy

**Date:** 2026-01
**Decider:** Hockney (DevOps)
**Status:** Implemented

**Problem:** SonarCloud flagged 21+ shell quality issues: missing default cases, POSIX `[ ... ]` syntax, implicit returns, direct positional parameters, repeated literals, unguarded variable expansion.

**Decision:** Systematic modernization: explicit default cases, Bash `[[ ... ]]` conditionals, explicit `return 0`, named parameter variables, extracted URL pattern, safe expansion (`${DISABLE_SNAP:-}`).

**Patterns:**
```bash
# Default cases with error handling
case "$ARCH" in
  *) echo "ERROR: Unsupported"; return 1 ;;
esac

# Modern conditionals
if [[ "$var" = "value" ]]; then

# Safe expansion
${DISABLE_SNAP:-}
```

**Files:** `install.sh`, `src/project/build.sh`

**Impact:** 21+ findings resolved; improved maintainability and robustness; no behavior change.

---

## Governance

- All meaningful changes require team consensus
- Document architectural decisions here
- Keep history focused on work, decisions focused on direction
