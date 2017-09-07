#define NDEBUG

#include <lcthw/ringbuffer.h>
#include <lcthw/dbg.h>
#include "circle_buffer.h"

static RingBuffer *buffer = NULL;

CIRCLE_RET_NUM init_circle_buffer(int length, int num)
{
    buffer = RingBuffer_create(length * num);
    if(buffer == NULL)
    {
        log_err("Failed to make ringbuffer.");
        return CIRCLE_RET_FAILED;
    }

    if(!RingBuffer_empty(buffer))
    {
        log_err("Should be emty.");
        return CIRCLE_RET_FAILED;
    }

    if(RingBuffer_full(buffer))
    {
        log_err("Should NOT be full.");
        return CIRCLE_RET_FAILED;
    }
    
    debug("data is: %d, space is: %d", RingBuffer_available_data(buffer),
        RingBuffer_available_space(buffer));

    return CIRCLE_RET_SUCCESS;
}

void destory_circle_buffer(void)
{
    RingBuffer_destroy(buffer);
}

void clear_circle_buffer(void)
{
    RingBuffer_clear(buffer);
}

CIRCLE_RET_NUM write_circle_buffer(char *data, int length)
{
    int rc;

    //if(RingBuffer_available_space(buffer) < length)
    //{
    //    debug("Wait for read.");
    //    return CIRCLE_RET_WAIT;
    //}
    
    rc = RingBuffer_write(buffer, data, length);
    if(rc != length)
    {
        log_err("Failed to write the buffer.");
        
        return CIRCLE_RET_FAILED;
    }

    return CIRCLE_RET_SUCCESS;
}

CIRCLE_RET_NUM read_circle_buffer(char *target, int amount)
{
    int rc;

    debug("RingBuffer_available_data = %d", RingBuffer_available_data(buffer));
    debug("amount = %d", amount);
    
    if(RingBuffer_available_data(buffer) < amount)
    {
        debug("Wait for write.");
        return CIRCLE_RET_WAIT;
    }
    
    rc = RingBuffer_read(buffer, target, amount);
    if(rc != amount)
    {
       log_err("Failed to write the buffer."); 
       return CIRCLE_RET_FAILED;
    }

    return CIRCLE_RET_SUCCESS;
}
