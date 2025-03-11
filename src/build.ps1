qmake SQLiteAnalyzer.pro -spec win32-g++ "CONFIG+=debug" "CONFIG+=qml_debug" && mingw32-make qmake_all
mingw32-make -j32
