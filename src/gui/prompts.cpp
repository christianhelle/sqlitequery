#include "prompts.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "../settings/settings.h"

QString Prompts::getCsvDelimiter(QWidget *parent,
                                 [[maybe_unused]] const QString &defaultDelimiter = ",") {
    bool ok{};
    auto input = QInputDialog::getText(parent,
                                       "CSV Delimeter",
                                       "Please enter delimeter to use",
                                       QLineEdit::Normal,
                                       ",",
                                       &ok);
    if (!ok || input.isEmpty()) {
        input = ",";
    }
    return input;
}

QString Prompts::getFolderPath(QWidget *parent) {
    SessionState state;
    Settings::getSessionState(&state);
    auto directory = state.lastUsedExportPath;
    if (state.lastUsedExportPath == Q_NULLPTR || state.lastUsedExportPath.isEmpty()) {
        directory = QDir::home().absolutePath();
    }

    const auto outputFolder =
            QFileDialog::getExistingDirectory(parent,
                                              "Select output folder",
                                              directory);
    return outputFolder;
}

QString Prompts::getFilePath(QWidget *parent,
                             const QFileDialog::AcceptMode mode) {
    SessionState state;
    Settings::getSessionState(&state);
    auto directory = state.lastUsedExportPath;
    if (state.lastUsedExportPath == Q_NULLPTR || state.lastUsedExportPath.isEmpty()) {
        directory = QDir::home().absolutePath();
    }

    QFileDialog dialog(parent);
    dialog.setAcceptMode(mode);
    dialog.setDirectory(directory);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()) {
        if (QStringList files = dialog.selectedFiles(); !files.empty()) {
            const auto &filepath = files.first();
            const auto folder = QFileInfo(filepath).absolutePath();
            Settings::setLastUsedExportPath(folder);
            return filepath;
        }
    }

    return Q_NULLPTR;
}

bool Prompts::confirmDelete(QWidget *parent, const QString &tableName) {
    return QMessageBox::question(parent, "Delete table",
                                 "Are you sure you want to delete table " + tableName + "?",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No)
           == QMessageBox::Yes;
}

void Prompts::showError(QWidget *parent, const QString &message) {
    QMessageBox::warning(parent, "Error", message);
}
