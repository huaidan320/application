#ifndef __CIRCLE_BUFFER_H__
#define __CIRCLE_BUFFER_H__

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

typedef enum {
    CIRCLE_RET_SUCCESS = 0,
    CIRCLE_RET_WAIT,
    CIRCLE_RET_FAILED
}CIRCLE_RET_NUM;

extern CIRCLE_RET_NUM init_circle_buffer(int length, int num);
extern void destory_circle_buffer(void);
extern void clear_circle_buffer(void);
extern CIRCLE_RET_NUM write_circle_buffer(char *data, int length);
extern CIRCLE_RET_NUM read_circle_buffer(char *target, int amount);

#ifdef __cplusplus
 }
#endif //__cplusplus

#endif //__CIRCLE_BUFFER_H__