#ifndef UTILS_H
#define UTILS_H
#pragma once

#include <QtQml/qqml.h>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QDir>
#include <qcoreapplication.h>
#include <qsettings.h>
#include <QRegularExpression>
#include <Windows.h>

class Q_TYPEHELPERS : public QObject{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:

    template<size_t N>
    QString fromTChar(const TCHAR (&data)[N]){
        return QString::fromWCharArray(data);
    }

    TCHAR toTChar(const QString &data){
        TCHAR *charData = new TCHAR[data.length()];
        data.toWCharArray(charData);
        return *charData;
    }

};

class Q_INIConfig : public QObject{
    Q_OBJECT
    QML_ELEMENT
    QML_NAMED_ELEMENT(INIHelper)
public:
    void findCurrentPath(const bool haveBackslash){
        if(path[0] == NULL){
            // now we trim the path
            for(int i = MAX_PATH; i == 0; i--){
                if(path[i] != L'\\' ){ path[i] = NULL; }
                if(path[i] == L'\\' && haveBackslash == true){break;}else
                    if(path[i] == L'\\' && haveBackslash == false){path[i] = NULL; break;} // stop on the first instance of a backslash
            }

        }
    }

private:
    TCHAR key[100];
    TCHAR section[100];
    TCHAR path[MAX_PATH];
    DWORD   dirOutput = GetModuleFileNameW(NULL, path, MAX_PATH); // if Equal to 0, an error has occured.

};

// class QConfigs
// define a QSettings inside of it...

// QVariant readKey(Const QString &keyName, enum Types); check what the data type can be converted to, or make override
// void updateKey(Const QString& keyName, QVariant &keyValue [to String by safe conversion])
// void appendKey(Const QString &keyName, Const QString &keyValue);
// void testKey(Const QString &keyName);
// void setCategory(Const QString &categoryName);
// void relativeSource(ConstQString &sourceName);
// QList<QVariant.toString [safe]> readListKey(Const QString &keyName, Const QChar &delimiter, enum Types);
// QVariant convert(Const QString &value, Enum Types);
// Void buildINI(Const QString &iniName); <- All functions beforehand add to a registry within the class
// QList<QString> keys <- Private instance of aforementioned registry


class Q_Searcher : public QObject{
    Q_OBJECT
    QML_NAMED_ELEMENT(SearchEngineHelper)
    QML_SINGLETON
    Q_PROPERTY(SearchEngine searchEngine READ searchEngine WRITE setSearchEngine NOTIFY searchEngineChanged FINAL)
public:
    enum DefaultSearchEngines{ // kept merely for convenience purposes.
        Google=0,
        DuckDuckGo=1,
        StartPage=2,
        Brave = 3
    };
    Q_ENUM(DefaultSearchEngines)

    // consider the following regex expression:
    // (http(s)?:\/\/.)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)
    // found on: https://regexr.com/3e6m0
    // had to modify it to: (http(s)?:\/\/.)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&\/\/=]*) for it to work.

    Q_INVOKABLE qint64 searchEngine() const {return m_searchProvider;}

    Q_INVOKABLE void setSearchEngine(const qint64 newEngine) {
        if (m_searchProvider == newEngine || newEngine >= m_registeredEngineURLs.length()) return;
        if(newEngine <0)return;
        m_searchProvider = newEngine;
        emit searchEngineChanged();
    }

    Q_INVOKABLE qint64 registerEngine(const QString& url){
        // returns index of already-registered engine, if you accidentally register a duplicate one.
        qint64 engineIDX = 0;
        engineIDX = m_registeredEngineURLs.indexOf(url);
        if( engineIDX ==-1){
            m_registeredEngineURLs.append(url);
            return m_registeredEngineURLs.length() -1;}
        else{return engineIDX;}
    }
    Q_INVOKABLE QUrl getURL(const QString &userInput){
        QUrl returnURL;
        QFileInfo file(userInput);
        QRegularExpression _regex = QRegularExpression( R"((http(s)?:\/\/.)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&\/\/=]*))" );
        if( _regex.match(userInput).hasMatch() == false ){ returnURL = m_https + m_registeredEngineURLs[m_searchProvider] + userInput;}
        else if( file.exists() ){ returnURL = "file:///" + file.absoluteFilePath(); }
        else { returnURL = QUrl::fromUserInput(userInput);}
        return returnURL;
    }
signals:
    Q_INVOKABLE void searchEngineChanged( );
private:
    qint64 m_searchProvider = 3;
    const QString m_https = "https://";
    // I could use a hashtable instead? How about I do?
    QList<const QString> m_registeredEngineURLs{
        "www.google.com/search?q=",
        "duckduckgo.com/?q=",
        "www.startpage.com/do/search?q=",
        "search.brave.com/search?q="
    };
};


class Q_FileHelper : public QObject{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    static Q_INVOKABLE bool isAbsolute(const QString &path){
        QDir testPath = path;
        return testPath.isAbsolute();
    }
    static Q_INVOKABLE QUrl getLocalResource(const QString &path, bool useFileURIScheme = false){
        const QString fileSchemePrefix = "file:///";
        QString absolutePath = QCoreApplication::applicationDirPath( );

        // qDebug() << "BREAKPOINT " + path;

        if(useFileURIScheme == true){absolutePath.prepend (fileSchemePrefix);}
        if( path.startsWith("/") == false ) {
            absolutePath.append( "/" ); // Ensuring proper formatting
        }absolutePath+= path.simplified(); // No spaces, sorry!
        return (QUrl)absolutePath;
    }
    static Q_INVOKABLE void mkFolder(const QString &pathName, const QString &dir = QCoreApplication::applicationDirPath()){
        QDir startDir = dir;
        startDir.mkpath(pathName);
    }
    //consider making an RM-DIR func.
    // RM-File too,
    // Rename too.

    static Q_INVOKABLE void mkFile(const QString &fileName,const QString &path = QCoreApplication::applicationDirPath() ,const QString &initData_text = ""){
        QString evalPath = path + fileName;
        // Use a bool-func to check if it is an absolute-path
        // If not, and if it is a local resource, make it an absolute path
        if( isAbsolute(evalPath) == false){
            evalPath = getLocalResource(evalPath).toString();
        }
        QFile newFile(evalPath);
        // You can do this part a lot better...

        if ( !initData_text.isEmpty() ){
            if( newFile.open(QIODevice::WriteOnly | QIODevice::Text) ){
                QTextStream writeHandler(&newFile);
                writeHandler<< initData_text;
                newFile.close();
            }
        }
    }


    static Q_INVOKABLE QString readLines_String(const QString &path_name, const qint64 &lineCount = -1,const bool &simplifyOutput = true, const QString &separatorString = "", const bool &prependSeparator = true, const QString &verbatimFlagString = "", const bool &forceSeparatorOnVerbatimStrings = false){
        // If the lineCount is less than 0, it will simply read forever.
        QString out_str = "";
        QFile fileToRead(path_name);
        bool useSeparator = !separatorString.isEmpty();
        bool verbatim_FLAG = false;
        bool verbatimFlagIsEmpty = verbatimFlagString.isEmpty();
        qint64 priv_line_count = 0;
        if(lineCount < 0){priv_line_count = LLONG_MAX;}
        else{priv_line_count = lineCount;}
        qDebug()<< priv_line_count;

        if(lineCount == 0 ||  path_name.isEmpty() ) return ""; // No empty strings!
        if( fileToRead.open(QIODevice::ReadOnly | QIODevice::Text) ){
            QTextStream readHandler(&fileToRead);
            for(qint64 i = 0; i < priv_line_count; i++){
                QString currentLine = readHandler.readLine();
                if( currentLine.isNull() ){ break; } // stop on End-Of-File
                else {
                    if( (currentLine == verbatimFlagString) && (verbatimFlagIsEmpty == false) ){verbatim_FLAG = true;qDebug() << "Verbatim flag set!"; continue;}

                    currentLine = { (simplifyOutput == true && verbatim_FLAG == false)? currentLine.simplified() : currentLine};

                    if( currentLine.isEmpty() ) continue; // skip empty lines

                    if(  useSeparator == true && verbatim_FLAG == true && forceSeparatorOnVerbatimStrings == false ){
                        out_str.append(currentLine);}
                    else if( (verbatim_FLAG == false && useSeparator == true) || (verbatim_FLAG == true && forceSeparatorOnVerbatimStrings == true && useSeparator == true)){
                        if(prependSeparator == true){currentLine.prepend(separatorString);}
                        else{currentLine.append(separatorString);}
                        out_str.append(currentLine);
                    }
                }

            } // INTERNAL LOOP
        } // IF file opens
        if( fileToRead.isOpen() ) fileToRead.close();
        return out_str;
    }

    static bool fileExists(const QString &path){
        return QFileInfo::exists(path);
    }

    //Consider adding tools to make files and paths...
    // void mkPath(Const QString &pathName)
    // void mkFile(Const QString &fileName)
    // void writeFile(Const QString &fileName,Const QString &data, fileMode [unknown] )
    // QString readFile(Const QInt16&lines)
    // void createResourceLocal(Const QString &relativePath, Const QString &name, Const QString &initData [can be null] );
    // void createResourceAbsolute(Const QString &absolutePath, Const QString &name, Const QString &initData [can be null] );
};

#include <QRect>

class Q_PixelsHelper : public QObject{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:

    Q_INVOKABLE static QRect strToRect(const QString &dataStr){
        QRect evaluatedRect;
        QString internalData = dataStr.toLower().simplified().remove('.').remove('-').remove('|').remove('[').remove(']'); //stripping spacing characters.
        // 55.30 becomes 5530, deal with it.
        internalData.replace('x', ','); //Allows usage of both 'x' and ',' for delimiters...

        QStringList list = internalData.split(',');
        while( (list.length() > 4)  ){ list.removeLast();}
        evaluatedRect.setX( list[0].toInt() );
        evaluatedRect.setY( list[1].toInt() );
        evaluatedRect.setWidth( list[2].toInt() );
        evaluatedRect.setHeight( list[3].toInt() ) ;
        return evaluatedRect;
    }
};

#endif // UTILS_H
