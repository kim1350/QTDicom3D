#include <QtWidgets/qapplication.h>

#include <mainwindow.h>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	MainWindow mainWindow;
	mainWindow.show();
	return app.exec();
}