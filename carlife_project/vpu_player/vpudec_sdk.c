#include <pthread.h>
#include "vpudecodec.h"
#include "vpudec_sdk.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static pthread_t decoder_handle;
static vpu_decode_t *vpu_dec;

int vpu_player_dbg_level = 6;

static void *thread_decoder_start(void *arg)
{
    (void)arg;

    FMT_SENSE(decoder_open(vpu_dec) < 0, goto err2;, "decoder_open failed.\n");
    FMT_SENSE(decoder_parse(vpu_dec) < 0, goto err2;, "decoder_parse failed.\n");
    FMT_SENSE(decoder_allocate_framebuffer(vpu_dec) < 0, goto err2;, "decoder_allocate_framebuffer failed.\n");
    FMT_SENSE(decoder_start(vpu_dec) < 0, goto err2;, "decoder_start failed.\n");

err2:
    decoder_close(vpu_dec);
    free_handle(vpu_dec);
    decoder_common_uninit();

    return NULL;
}

int start_vpu_decoder(void)
{
    if(pthread_create(&decoder_handle, NULL, thread_decoder_start, NULL) != 0)
    {
        err_msg("Create the thread_decoder_start thread failed.\n");
        
        decoder_close(vpu_dec);
        free_handle(vpu_dec);
        decoder_common_uninit();

        return -1;
    }

    return 0;
}

int init_vpu_decoder(void)
{ 
    FMT_SENSE(decoder_common_init() < 0, goto err;, "decoder_common_init failed.\n");
    FMT_SENSE(decoder_config(&vpu_dec) < 0, goto err1;, "decoder_config failed.\n");
    
    return 0;
    
err1:
    DEBUG
    decoder_close(vpu_dec);
    free_handle(vpu_dec);
err:
    DEBUG
    decoder_common_uninit();

    return -1;
}

int destory_vpu_decoder(void)
{
    if(pthread_cancel(decoder_handle) != 0)
    {
        err_msg("Cancel thread failed.\n");
        return -1;
    }
    
    decoder_close(vpu_dec);
    free_handle(vpu_dec);
    decoder_common_uninit();

    return 0;
}
