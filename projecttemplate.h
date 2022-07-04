#ifndef PROJECTTEMPLATE_H
#define PROJECTTEMPLATE_H

#include <QtXml>
#include <QTextStream>

class ProjectTemplate
{
public:
    ProjectTemplate();
    void writeXMLFile();
    void loadXMLFile();
};

#endif // PROJECTTEMPLATE_H
