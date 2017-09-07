#include <mcarlifedlg.h>
#include <QApplication>

#include <iostream>
#include <unistd.h>
#include <Carlife.h>
#include <CarlifeDebug.h>

#include "circle_buffer.h"
#include "vpudec_sdk.h"

using namespace std;

int main(int argc, char *argv[])
{
    CL_UNUSED(argc);
    CL_UNUSED(argv);

    //circle buffer
    init_circle_buffer(768 * 480, 30);
    //vpu decoder
    init_vpu_decoder();

    CarlifeStart(NULL);

    QApplication a(argc, argv);

    MCarLifeDlg w;
    w.show();

    return a.exec();
}
