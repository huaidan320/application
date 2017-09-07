/*************************************************************************
    > File Name: vpudisplay.h
    > Author: CM
    > Mail:
    > Created Time: 2017年07月31日 星期一 17时57分51秒
    > Note: 用于vpu解码。
    > To decode a bitstream, the application completes the following steps:
        1. Call vpu_Init() to initialize the VPU.
        2. Open a decoder instance by using vpu_DecOpen().
        3. To provide the proper amount of bitstream, get the bitstream buffer address by using vpu_DecGetBitstreamBuffer().
        4. After transferring the decoder input stream, inform the amount of bits transferred into the bitstream buffer by using
           vpu_DecUpdateBitstreamBuffer().
        5. Before starting a picture decoder operation, get the crucial parameters for decoder operations such as picture size,
           frame rate, and required frame buffer size by using vpu_DecGetInitialInfo().
        6. Using the returned frame buffer requirement, allocate the proper size of the frame buffers,
           and convey this data to i.MX6 VPU by using vpu_DecRegisterFrameBuffer().
        7. Start a picture decoder operation picture-by-picture by using vpu_DecStartOneFrame().
        8. Wait for the completion of the picture decoder operation interrupt event.
        9. Check the results of the decoder operation using vpu_DecGetOutputInfo().
        10. After displaying nth frame buffer, clear the buffer display flag by using vpu_DecClrDispFlag().
        11. If there is more bitstream to decode, go to Step 7, otherwise go to the next step.
        12. Terminate the sequence operation by closing the instance by using vpu_DecClose()
        13. Call vpu_UnInit() to release the system resources.

      The steps of random access for the video decoder are as follows:
        1. Freeze the display and reset the decoder bit-stream buffer
            vpu_DecBitBufferFlush()
        2. Read the bitstream from the new file read pointer and transfer it into the decoder
            calling vpu_DecStartOneFrame() with iframeSearchEnable of DecParam set to 1.
        3. Enable I-Search and run the picture decoding operation
        4. If the buffer empty interrupt is signaled, feed more bitstream and wait for decoding completion
        5. If decoding completion is detected, read the decoder results and resume display
            When an interrupt of decoder completion or non-busy state of the BIT processor is detected,
            the I-frame is searched and decoded
 ************************************************************************/

#ifndef VPUDECODEC_H
#define VPUDECODEC_H
#include "vpuplayer_config.h"
#include "v4ldisplay.h"
#ifdef __cplusplus
extern "C"{
#endif


extern int decoder_common_init();
extern void decoder_common_uninit();
extern int decoder_config(vpu_decode_t **phande);
extern int decoder_open(vpu_decode_t *dec);
extern void decoder_close(vpu_decode_t *dec);
extern int decoder_parse(vpu_decode_t *dec);
extern int decoder_allocate_framebuffer(vpu_decode_t *dec);
extern int decoder_start(vpu_decode_t *dec);
extern void free_handle(vpu_decode_t *dec);
#ifdef __cplusplus
}
#endif
#endif // VPUDECODEC_H
