#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>

class Languages
{
public:
    QString guessLanguage(QString dir, bool recurse = false);

private:
    QMap<QString, int> languageHits;
    QString getExtension(QString file);
    QList<QStringList> languages =
    {
        {"C", ".c", ".h", ".cats", ".idc", ".w"},
        {"C++", ".cpp", ".c++", ".cc", ".cp", ".cxx", ".hpp", ".h", ".h++", ".hh", ".hxx", ".inc", ".inl", ".ipp", ".tcc", ".tpp"},
        {"Python", ".py", ".bzl", ".cgi", ".fcgi", ".gyp", ".lmi", ".pyde", ".pyp", ".pyt", ".pyw", ".rpy", ".tac", ".wsgi", ".xpy"}
    };

};

// Java, PHP, Go, Rust, Forth, Python, C, C++, Perl

#endif // LANGUAGES_H
