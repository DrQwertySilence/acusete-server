#include <QCoreApplication>

#include "application.h"
#include "data.h"

int
main(int argc, char **argv)
{
    Application core(argc, argv);

    return core.exec();
}
