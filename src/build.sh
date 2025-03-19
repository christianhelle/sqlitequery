#!/bin/bash
cmake . -B build
cmake --build build --config Release
macdeployqt build/SQLiteQueryAnalyzer.app -dmg
