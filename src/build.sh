#!/bin/bash
cmake .
cmake --build . --config Release
macdeployqt SQLiteQueryAnalyzer.app -dmg
