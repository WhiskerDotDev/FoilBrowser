// THIS PROJECT IS THE CREATION OF WHISKERDOTDEV

/*
AI DISCLOSURE: The vast majority of code was written by humans (me) ;

AI was used to research classes, types, and help me understand said objects across QML, C++ and CMake (CMakeLists) .
AI was also used to help translate the QMake file this project used to use.

Otherwise, the prevalence of AI is kept to a minimal.
*/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtWebEngineQuick>
#include <QFontDatabase>
#include <QFont>
#include <QSurfaceFormat>
#include <QQuickWindow>
#include <QWebEngineProfile>
#include <QMetaObject>
#include "utils.hpp"
// #include <QAudioDecoder>

//TODO:
// create Border-Image derived QML type that uses QRect (Qt.Rect) for borders
//Use framecount to derive the "features" present in the GIF
// 1: normal
// 2: pressed
// 3: disabled
// 4: selected -- Probably won't use
// 5: hovered -- Probably won't use

int main(int argc, char *argv[])
{
    // qputenv("QT_LOGGING_RULES", "qt.rhi.general=true;qt.scenegraph.general=true");
    // Folders "translations" and "qmltooling" are not strictly necessary. in deployment
    // Neither, are the DLLs for other QtQuickStyles other than Basic and the default one needed.
    // The "\qml\QtQuick\Controls" folder contains files that may be safely removed. Modify the QMLdir to ensure stability.
    // qputenv("QV4_FORCE_INTERPRETER", "1");
    // qputenv("QSG_INFO", "1");
    // qputenv("QSG_RHI_PREFER_SOFTWARE_RENDERER", "1");

    // qputenv("QSG_RENDER_LOOP", "basic");
    qputenv("QV4_JIT_CALL_THRESHOLD","1"); //Slower first-run, otherwise, helps ensure all functions are JIT-compiled.
    qputenv("QV4_CRASH_ON_STACKOVERFLOW", "1");
    qputenv("QV4_JS_MAX_STACK_SIZE","4000"); //Higher Memory usage, but, better overall performance... NOT!
    //Default stack size is 4 MB (4,000,000 Bytes)
    qputenv("QV4_MAX_CALL_DEPTH","1"); // Why would we need to call a JS function recursively over 600 times?

    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);
    qputenv("QSG_RHI_DISABLE_SHADER_DISK_CACHE", "1"); //Really? We had to do the Attribute AND environment variable?

    // QSurfaceFormat defFormat;
    // defFormat.setSamples(-1);
    // defFormat.setSwapInterval(0);
    // defFormat.setSwapBehavior(QSurfaceFormat::SwapBehavior::SingleBuffer);
    // QSurfaceFormat::setDefaultFormat(defFormat);
    QSurfaceFormat::defaultFormat().setSamples(-1);
    QSurfaceFormat::defaultFormat().setSwapInterval(0);
    // QSurfaceFormat::defaultFormat().setSwapBehavior(QSurfaceFormat::SwapBehavior::SingleBuffer);
    // QSurfaceFormat::defaultFormat().setColorSpace(QColorSpace::NamedColorSpace::Bt2100Pq);
    // QSurfaceFormat::defaultFormat().setAlphaBufferSize(2);
    // QSurfaceFormat::defaultFormat().setBlueBufferSize(10);
    // QSurfaceFormat::defaultFormat().setGreenBufferSize(10);
    // QSurfaceFormat::defaultFormat().setRedBufferSize(10);

    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    // QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    // QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents);
    QCoreApplication::setAttribute(Qt::AA_CompressTabletEvents);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);

    // consider enabling devtools (via remote debugging) on port 9222
    if( Q_FileHelper::fileExists("flags.txt") ){
        // qDebug() << qgetenv("QTWEBENGINE_CHROMIUM_FLAGS");
        QByteArray* currentFlags = new QByteArray();
        currentFlags ->prepend(Q_FileHelper::readLines_String("flags.txt", -1, false, " --", true).trimmed().toLocal8Bit());
        // prepends file-based flags instead of overriding!
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", *currentFlags); // FINALLY WORKS!
        // You can now tweak chromium flags via a text file!!!!
        delete currentFlags;
    }


    QtWebEngineQuick::initialize();
    QGuiApplication app(argc, argv);
    Q_FileHelper::mkFolder("webcache");
    Q_FileHelper::mkFolder("webstorage");
    Q_FileHelper::mkFolder("qmlcache");
    //Creating cache dirs...
    QByteArray* path = new QByteArray(Q_FileHelper::getLocalResource("qmlcache").toString().toLocal8Bit());
    qputenv( "QML_DISK_CACHE_PATH", *path ); // Will need this sooner or later...
    qDebug()<< *path;
    delete path;

    bool wipeHistoryOnClose = true;
    //WEBENGINEPROFILE SETTINGS:
    QWebEngineProfile::defaultProfile()->setCachePath( Q_FileHelper::getLocalResource("webcache", true).toString() );
    QWebEngineProfile::defaultProfile()->setPersistentStoragePath( Q_FileHelper::getLocalResource("webstorage", true).toString() );
    QWebEngineProfile::defaultProfile()->setHttpCacheType(QWebEngineProfile::HttpCacheType::DiskHttpCache);
    QWebEngineProfile::defaultProfile() ->setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::AllowPersistentCookies);
    QWebEngineProfile::defaultProfile() ->setPersistentPermissionsPolicy(QWebEngineProfile::PersistentPermissionsPolicy::StoreOnDisk);
    // We can always expose these settings to an INI file :)

    // QObject::connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested,
    //                  [&](){qDebug() << "download requested!";}
    //                  );

    int fontid = QFontDatabase::addApplicationFont(Q_FileHelper::getLocalResource("font/FONT_MAIN.ttf").toString() ); // consider custom fonts?
    if(fontid != -1){
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontid).constFirst();
        if( fontFamily.isEmpty() == false ){
            QFont globalFont;
            globalFont.setFamily( fontFamily );
            globalFont.setKerning( 0 );
            globalFont.setPixelSize( 15 );
            globalFont.setHintingPreference( QFont::HintingPreference::PreferFullHinting  );
            app.setFont( globalFont );
        }
    }

    qDebug() << qgetenv("QTWEBENGINE_CHROMIUM_FLAGS");

    Q_Searcher searchHelper;
    searchHelper.setSearchEngine( Q_Searcher::DuckDuckGo );

    QQmlApplicationEngine engine;
    engine.rootContext() ->setContextProperty( "urlHelper", &searchHelper ); // try to stop depending on this  ASAP!
    engine.loadFromModule( "App.Core", "Main" );
    if( engine.rootObjects().isEmpty() )return 1;
    // QObject* webBoxOBJ = engine.rootObjects().constFirst() ->findChild<QObject*>("_WebBox");


    // if(webBoxOBJ != nullptr){
    //     QObject::connect(QQuickWebEngineProfile::defaultProfile(), &QQuickWebEngineProfile::downloadRequested, [&](QQuickWebEngineDownloadRequest * download){
    //         qDebug() <<"caught!";
    //         QMetaObject::invokeMethod(webBoxOBJ, "handleDownload",
    //                                   Q_ARG(QQuickWebEngineDownloadRequest, *download));
    //     });
    // }

    // QWindow* wnd = new QWindow;
    // wnd->setWidth(100);
    // wnd->setHeight(200);

    QQuickWindow* wnd = qobject_cast<QQuickWindow*>(engine.rootObjects().constFirst());
    wnd->setFlags(Qt::Window | Qt::FramelessWindowHint);
    // wnd->setObjectName("ROOTWIN");
    wnd->contentItem()->setSmooth(false);
    wnd->contentItem()->setAntialiasing(false);
    // qDebug()<<"WIDTH: "<<wnd->contentItem()->width();
    wnd->setColor(QColor(80, 80, 80));
    wnd->setVisible(true);

    // while(true){
    //     _mm_pause();
    // }
    QGuiApplication::exec( );
    // after exec:
    if(wipeHistoryOnClose == true)QWebEngineProfile::defaultProfile() -> clearAllVisitedLinks();

    return 0;
}
