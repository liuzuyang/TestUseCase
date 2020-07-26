#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <tbb/tbb_stddef.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	qDebug() << "TBB_runtime_interface_version: " << 
		tbb::TBB_runtime_interface_version();

	f

    return a.exec();
}
