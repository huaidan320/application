#ifndef CARLIFE_H
#define CARLIFE_H

enum AUDIO_FOCUS_TYPE {

    AUDIO_FOCUS_NULL,
    AUDIO_FOCUS_EXCLUSIVE,
    AUDIO_FOCUS_UEXCLUSIVE
};

enum SCREEN_FOCUS_TYPE {
    
    SCREEN_FOCUS_NULL,
    SCREEN_FOCUS_FULL
};

enum AUDIO_TYPE {
    
    AUDIO_TYPE_MUSIC,
    AUDIO_TYPE_VR,
    AUDIO_TYPE_TTS,
    AUDIO_TYPE_MAX
};

enum TOUCH_TYPE {
    
    ACTION_DOWN = 0x00,
    ACTION_UP = 0x01,
    ACTION_MOVE = 0x02
};

typedef enum {
    
	RESULT_TYPE_SUCCESS,
	RESULT_TYPE_FAILED
}RESULT_TYPE;

typedef struct CarlifeInfo {
	
	unsigned char *manufacturer;
	unsigned char *model;
	unsigned char *product;
	unsigned char *serial;
	unsigned char *cuid;
	unsigned char *versionName;
	unsigned char *channelID;
	unsigned int height;
	unsigned int width;
	unsigned int rate;
	bool focus_ui;
	bool bt_internal_ui;
	bool bt_autopair;
	bool voice_wakeup;
	bool voice_mic;	
}sCarlifeInfo;

typedef void (*CompletionFunc)(RESULT_TYPE);
typedef void (*DuckAudioFunc)(void);
typedef void (*UnDuckAudioFunc)(void);
typedef void (*AudioAcquireFunc)(AUDIO_TYPE , AUDIO_FOCUS_TYPE);
typedef void (*AudioReleaseFunc)(AUDIO_TYPE);
typedef void (*ScreenAcquireFunc)(void);
typedef void (*ScreenReleaseFunc)(void);
//typedef void 

typedef struct {

	DuckAudioFunc		DuckAudio_f;
	UnDuckAudioFunc		UnDuckAudio_f;
	AudioAcquireFunc	AudioAcquire_f;
	AudioReleaseFunc	AudioRelease_f;
	ScreenAcquireFunc	VideoAcquire_f;
	ScreenReleaseFunc	VideoRelease_f;
}sCarlifeDelegate;

extern RESULT_TYPE CarlifeInit(sCarlifeInfo *info, sCarlifeDelegate *Delegate);
extern RESULT_TYPE CarlifeStart(CompletionFunc callback);
extern RESULT_TYPE CarlifeStop();
extern RESULT_TYPE CarlifeDstory();
extern RESULT_TYPE CarlifeLaunchNavi();
extern RESULT_TYPE CarlifeLaunchMeida();
extern RESULT_TYPE carlifeLaunchTel();
extern RESULT_TYPE CarlifeTouch(int x, int y, int action);
//extern RESULT_TYPE CarlifeInputHk(KeyCode key);
extern RESULT_TYPE UpdateVideoFocus(SCREEN_FOCUS_TYPE type);
extern RESULT_TYPE UpdateAudioFocus(AUDIO_TYPE audiotype, AUDIO_FOCUS_TYPE focustype);

#endif
