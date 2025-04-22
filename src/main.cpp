#include <QApplication>
#include <QCommandLineParser>
#include <QDir>

#include "cli/export.h"
#include "gui/mainwindow.h"

#define VERSION "1.0.0"

int exportDataToCsvFiles(const QStringList args, bool showProgress, QString targetDir) {
    if (targetDir.isEmpty()) {
        qWarning("Target directory is required for export.");
        qWarning("Setting target directory to current working directory.");
        targetDir = QDir::currentPath();
    }

    Export::exportDataToCsvFile(args.at(0),
                                targetDir,
                                ",",
                                showProgress);
    return 0;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationVersion(VERSION);
    QApplication::setOrganizationDomain("christianhelle.com");
    QApplication::setOrganizationName("Christian Helle");
    QApplication::setApplicationName("SQLite Query Analyzer");

    QCommandLineParser parser;
    parser.setApplicationDescription(
            "A fast and lightweight cross-platform command line and GUI tool for querying and manipulating SQLite databases");
    parser.addPositionalArgument("database", "Database file to open.");
    const QCommandLineOption progressOption(QStringList() << "p" << "progress", "Show progress during copy");
    const QCommandLineOption exportCsvOption(QStringList() << "e" << "export-csv", "Export data to CSV.");
    const QCommandLineOption targetDirectoryOption(QStringList() << "d" << "target-directory",
                                                   "Target directory for export.");
    const auto helpOption = parser.addHelpOption();
    const auto versionOption = parser.addVersionOption();
    parser.addOption(progressOption);
    parser.addOption(exportCsvOption);
    parser.addOption(targetDirectoryOption);
    parser.process(app);

    if (parser.isSet(helpOption)) {
        parser.showHelp();
    }

    if (parser.isSet(versionOption)) {
        parser.showVersion();
    }

    const auto args = parser.positionalArguments();
    bool showProgress = parser.isSet(progressOption);
    bool exportOption = parser.isSet(exportCsvOption);
    auto targetDir = parser.value(targetDirectoryOption);

    if (exportOption) {
        if (args.length() != 1) {
            qWarning("Export option requires a database file.");
            return 1;
        }
        return exportDataToCsvFiles(args, showProgress, targetDir);
    }

    if (args.length() == 1) {
        MainWindow window;
        window.openDatabase(args.at(0));
        window.show();
        return QApplication::exec();
    }

    MainWindow window;
    window.restoreLastSession();
    window.show();
    return QApplication::exec();
}
