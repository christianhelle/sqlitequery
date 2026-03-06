import type { SquadConfig } from '@bradygaster/squad';

/**
 * Squad Configuration for SQLite Query Analyzer
 * 
 * A cross-platform Qt6 C++ desktop application for managing SQLite databases.
 * Supports GUI and CLI interfaces on Windows, macOS, and Linux.
 * 
 * Project: https://github.com/christianhelle/sqlitequery
 */
const config: SquadConfig = {
  version: '1.0.0',
  
  // Project metadata for context-aware agent behavior
  project: {
    name: 'SQLite Query Analyzer',
    language: 'C++17',
    framework: 'Qt6',
    platforms: ['Windows', 'macOS', 'Linux'],
    buildSystem: 'CMake 3.16+',
    description: 'Cross-platform desktop utility for managing SQLite databases with GUI and CLI interfaces'
  },
  
  models: {
    defaultModel: 'claude-sonnet-4.5',
    defaultTier: 'standard',
    // Fallback chains prioritize providers and models by task complexity
    fallbackChains: {
      // Premium tier for complex architectural decisions and optimizations
      premium: ['claude-opus-4.6', 'claude-opus-4.6-fast', 'claude-opus-4.5', 'claude-sonnet-4.5'],
      // Standard tier for typical development tasks (feature work, bug fixes)
      standard: ['claude-sonnet-4.5', 'gpt-5.2-codex', 'claude-sonnet-4', 'gpt-5.2'],
      // Fast tier for quick fixes, documentation, and simple tasks
      fast: ['claude-haiku-4.5', 'gpt-5.1-codex-mini', 'gpt-4.1', 'gpt-5-mini']
    },
    preferSameProvider: true,
    respectTierCeiling: true,
    nuclearFallback: {
      enabled: false,
      model: 'claude-haiku-4.5',
      maxRetriesBeforeNuclear: 3
    }
  },
  
  routing: {
    // Routing rules map work types to appropriate agents and model tiers
    rules: [
      {
        workType: 'feature-dev',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'standard',
        description: 'C++ feature development: GUI enhancements, database operations, CLI additions'
      },
      {
        workType: 'bug-fix',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'standard',
        description: 'Cross-platform bug fixes and issue resolution'
      },
      {
        workType: 'testing',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'fast',
        description: 'Manual validation, CLI testing, cross-platform verification'
      },
      {
        workType: 'documentation',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'fast',
        description: 'README updates, build guides, code documentation'
      },
      {
        workType: 'build-system',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'standard',
        description: 'CMake configuration, CPack packaging, cross-platform build scripts'
      },
      {
        workType: 'code-review',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'premium',
        description: 'Code quality review, architecture assessment, performance optimization'
      },
      {
        workType: 'architecture',
        agents: ['@scribe'],
        confidence: 'high',
        modelTier: 'premium',
        description: 'Design decisions, module restructuring, cross-platform considerations'
      }
    ],
    governance: {
      eagerByDefault: true,
      scribeAutoRuns: false,
      allowRecursiveSpawn: false
    }
  },
  
  casting: {
    // Communication style and character preferences for code and documentation
    allowlistUniverses: [
      'The Usual Suspects',
      'Breaking Bad',
      'The Wire',
      'Firefly'
    ],
    overflowStrategy: 'generic',
    universeCapacity: {}
  },
  
  platforms: {
    vscode: {
      disableModelSelection: false,
      scribeMode: 'sync'
    }
  },
  
  // Agent context and capabilities for this project
  agentContext: {
    preferredLanguage: 'en-US',
    codeStyle: {
      language: 'C++17',
      formatTool: 'clang-format',
      formatConfig: 'src/.clang-format',
      standard: 'LLVM',
      indentation: 2
    },
    buildCommand: 'cd src/project && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release --parallel 4',
    testCommand: 'QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --help',
    keyDirectories: {
      gui: 'src/gui/',
      database: 'src/database/',
      cli: 'src/cli/',
      build: 'src/project/',
      settings: 'src/settings/',
      threading: 'src/threading/'
    },
    codeQuality: {
      tools: ['clang-format', 'SonarCloud'],
      formatBeforeCommit: true,
      platforms: ['Linux', 'macOS', 'Windows']
    },
    git: {
      addCoAuthorTrailer: false
    }
  }
};

export default config;
