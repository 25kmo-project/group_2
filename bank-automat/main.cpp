// This is the entry point (main.cpp) for the Qt application
#include <QApplication>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>

#include "mainwindow.h"

// Anonymous namespace limits the visibility of disableProxies() to this translation unit (this .cpp file only)
namespace {
// Disables all HTTP/HTTPS proxy usage for the application
void disableProxies()
{
    // Clear proxy-related environment variables
    qputenv("HTTP_PROXY", "");
    qputenv("HTTPS_PROXY", "");
    qputenv("http_proxy", "");
    qputenv("https_proxy", "");
    // Explicitly define hosts that should never use a proxy
    qputenv("NO_PROXY", "127.0.0.1,localhost");
    qputenv("no_proxy", "127.0.0.1,localhost");

    // Disable usage of system proxy configuration
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    // Force the application to use no proxy at all
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    // Remove any custom proxy factory
    QNetworkProxyFactory::setApplicationProxyFactory(nullptr);
}
} // end anonymous namespace

int main(int argc, char *argv[])
{
    // Ensure that no network proxies interfere with API calls
    disableProxies();

    // Create the Qt application object
    QApplication app(argc, argv);

    // Create the main window
    MainWindow w;
    // Show the main window maximized
    w.showMaximized();

    // Enter the Qt event loop
    return app.exec();
}
