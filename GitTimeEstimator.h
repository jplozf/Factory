#ifndef GITTIMEESTIMATOR_H
#define GITTIMEESTIMATOR_H
#pragma once

#include <QObject>
#include <QString>
#include <QProcess>

class GitTimeEstimator : public QObject {
    Q_OBJECT

public:
    explicit GitTimeEstimator(QObject *parent = nullptr);

    /**
     * Calculates development time from git commits.
     * @param repoPath Path to repository root containing .git folder.
     * @param thresholdSec Max gap between commits (in seconds) to consider part of same session (default: 2 hrs).
     * @param defaultCommitSec Estimated time attributed to first commit of a new session (default: 30 mins).
     * @return Total estimated development duration in seconds (-1 if git command fails).
     */
    qint64 calculateElapsedTime(const QString &repoPath,
                                qint64 thresholdSec = 7200,
                                qint64 defaultCommitSec = 1800);

private:
    qint64 parseAndSumTimestamps(const QByteArray &output, qint64 thresholdSec, qint64 defaultCommitSec);
};

#endif // GITTIMEESTIMATOR_H
