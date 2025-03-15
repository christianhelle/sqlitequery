#!/bin/bash
qmake SQLiteAnalyzer.pro
make
macdeployqt SQLiteQueryAnalyzer.app -dmg
