#include <QtGui/qguiapplication.h>
#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include "MainWindowModel.h"

int main(int argc, char *argv[]) {
	QGuiApplication app{ argc, argv };
	QQmlApplicationEngine engine;
	fs_stat::MainWindowModel* mainModel = new fs_stat::MainWindowModel{ &engine };

	qmlRegisterType<fs_stat::MainWindowModel>("fs_stat", 1, 0, "MainWindowModel");
	qmlRegisterType<fs_stat::StatisticsModel>("fs_stat", 1, 0, "StatisticsModel");
	engine.rootContext()->setContextProperty("mainModel", mainModel);
	engine.load(QUrl{ "qrc:/MainWindow.qml" });

	return app.exec();
}
