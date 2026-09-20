#include "GitTimeEstimator.h"
#include <QDateTime>
#include <QDebug>
#include <algorithm>

GitTimeEstimator::GitTimeEstimator(QObject *parent)
    : QObject(parent) {}

qint64 GitTimeEstimator::calculateElapsedTime(const QString &repoPath, qint64 thresholdSec, qint64 defaultCommitSec) {
    QProcess process;
    process.setWorkingDirectory(repoPath);

    // Retrieve UNIX timestamps (%at) for all commits in repository
    process.start("git", QStringList() << "log" << "--pretty=format:%at");

    if (!process.waitForStarted(3000)) {
        qWarning() << "Failed to start git process.";
        return -1;
    }

    // Wait for process to complete (timeout: 10 seconds)
    if (!process.waitForFinished(10000)) {
        qWarning() << "Git process timed out or crashed:" << process.errorString();
        return -1;
    }

    if (process.exitCode() != 0) {
        qWarning() << "Git command returned non-zero code:" << process.readAllStandardError();
        return -1;
    }

    return parseAndSumTimestamps(process.readAllStandardOutput(), thresholdSec, defaultCommitSec);
}

qint64 GitTimeEstimator::parseAndSumTimestamps(const QByteArray &output, qint64 thresholdSec, qint64 defaultCommitSec) {
    QString rawOutput = QString::fromUtf8(output).trimmed();
    if (rawOutput.isEmpty()) {
        return 0;
    }

    QStringList lines = rawOutput.split('\n', Qt::SkipEmptyParts);
    QList<qint64> timestamps;
    timestamps.reserve(lines.size());

    for (const QString &line : lines) {
        bool ok = false;
        qint64 ts = line.toLongLong(&ok);
        if (ok) {
            timestamps.append(ts);
        }
    }

    if (timestamps.isEmpty()) {
        return 0;
    }

    // Sort chronologically (git log outputs newest first)
    std::sort(timestamps.begin(), timestamps.end());

    qint64 totalSeconds = 0;
    qint64 lastTs = 0;

    for (qint64 ts : timestamps) {
        if (lastTs == 0) {
            // First commit of repository
            totalSeconds += defaultCommitSec;
        } else {
            qint64 diff = ts - lastTs;
            if (diff <= thresholdSec) {
                // Within threshold -> count actual gap as active work
                totalSeconds += diff;
            } else {
                // Gap too large -> start new session with default overhead
                totalSeconds += defaultCommitSec;
            }
        }
        lastTs = ts;
    }

    return totalSeconds;
}