#include "projecttemplate.h"

ProjectTemplate::ProjectTemplate()
{

}

void ProjectTemplate::writeXMLFile(){
    QFile xmlFile("xmlSample.xml");
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text ))
    {
        qDebug() << "Already opened or there is another issue";
        xmlFile.close();
    }
    QTextStream xmlContent(&xmlFile);

    QDomDocument document;

    //make the root element
    QDomElement root = document.createElement("StudentList");
    //add it to document
    document.appendChild(root);

    QDomElement student = document.createElement("student");
    student.setAttribute("id", "1");
    student.setAttribute("name", "Burak");
    student.setAttribute("number", "1111");
    root.appendChild(student);

    student = document.createElement("student");
    student.setAttribute("id", "2");
    student.setAttribute("name", "Hamdi");
    student.setAttribute("number", "2222");
    root.appendChild(student);

    student = document.createElement("student");
    student.setAttribute("id", "3");
    student.setAttribute("name", "TUFAN");
    student.setAttribute("number", "33333");
    root.appendChild(student);

    student = document.createElement("student");
    student.setAttribute("id", "4");
    student.setAttribute("name", "Thecodeprogram");
    student.setAttribute("number", "4444");
    root.appendChild(student);

    xmlContent << document.toString();
}

void ProjectTemplate::loadXMLFile(){
    QDomDocument studentsXML;
    QFile xmlFile("xmlSample.xml");
    if (!xmlFile.open(QIODevice::ReadOnly ))
    {
        // Error while loading file
    }
    studentsXML.setContent(&xmlFile);
    xmlFile.close();

    QDomElement root = studentsXML.documentElement();
    QDomElement node = root.firstChild().toElement();

    QString datas = "";

    while(node.isNull() == false)
    {
        qDebug() << node.tagName();
        if(node.tagName() == "student"){
            while(!node.isNull()){
                QString id = node.attribute("id","0");
                QString name = node.attribute("name","name");
                QString number = node.attribute("number","number");

                datas.append(id).append(" - ").append(name).append(" - ").append(number).append("\n");
                node = node.nextSibling().toElement();
            }
        }
        node = node.nextSibling().toElement();
    }
    // ui->txtLoadedXmlContent->setPlainText(datas);
}


/*
<templates>
    <template name="Qt C++">
      <folder name="Headers">
          <extension>h</extension>
      </folder>
      <folder name="Sources">
          <extension>c</extension>
          <extension>cpp</extension>
      </folder>
      <folder name="Forms">
          <extension>ui</extension>
      </folder>
    </template>
    <template name="C/C++">
      <folder name="Headers">
          <extension>h</extension>
      </folder>
      <folder name="Sources">
          <extension>c</extension>
          <extension>cpp</extension>
      </folder>
    </template>
    <template name="Java">
      <folder name="Sources">
          <extension>java</extension>
      </folder>
    </template>
    <template name="Python">
      <folder name="Sources">
          <extension>py</extension>
      </folder>
    </template>
</templates>


*/
