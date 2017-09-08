#include <QApplication>
#include "mcarlifedlg.h"

#include "CarlifeDebug.h"
#include "Carlife.h"
#include "circle_buffer.h"
#include "vpudec_sdk.h"

sCarlifeDelegate delegate;

void Completion(RESULT_TYPE)
{
}
void DuckAudio(void)
{

}
void UnDuckAudio(void)
{

}
void AudioAcquire(AUDIO_TYPE , AUDIO_FOCUS_TYPE)
{

}
void AudioRelease(AUDIO_TYPE)
{

}
void ScreenAcquire(void)
{

}
void ScreenRelease(void)
{

}

int main(int argc, char *argv[])
{
    CL_UNUSED(argc);
    CL_UNUSED(argv);

    //circle buffer
    init_circle_buffer(768 * 480, 100);
    //vpu decoder
    init_vpu_decoder();

    delegate.AudioAcquire_f = AudioAcquire;
    delegate.AudioRelease_f = AudioRelease;
    delegate.DuckAudio_f = DuckAudio;
    delegate.UnDuckAudio_f = UnDuckAudio;
    delegate.VideoAcquire_f = ScreenAcquire;
    delegate.VideoRelease_f = ScreenRelease;

    CarlifeInit(NULL, &delegate);

    CarlifeStart(Completion);

    QApplication a(argc, argv);

    MCarLifeDlg w;
    w.show();

    return a.exec();
}
