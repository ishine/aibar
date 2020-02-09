/**
 * @Description: （iFly Auto Transform） Iat demo
 * @Author     :  Kun Gan
 * @Date       :  Sep. 2019
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

#include "../../../include/iat_tts/qtts.h"
#include "../../../include/iat_tts/qisr.h"
#include "../../../include/iat_tts/msp_cmn.h"
#include "../../../include/iat_tts/msp_errors.h"
#include "speech_recognizer.h"

bool SpeakDone = false;

/**
 * PartI: iat 
 */
#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096

/* Upload User words */
static int upload_userwords()
{
	char*			userwords	=	NULL;
	size_t			len			=	0;
	size_t			read_len	=	0;
	FILE*			fp			=	NULL;
	int				ret			=	-1;

	fp = fopen("userwords.txt", "rb");
	if (NULL == fp)										
	{
		printf("\nopen [userwords.txt] failed! \n");
		goto upload_exit;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp); 
	fseek(fp, 0, SEEK_SET);  					
	
	userwords = (char*)malloc(len + 1);
	if (NULL == userwords)
	{
		printf("\nout of memory! \n");
		goto upload_exit;
	}

	read_len = fread((void*)userwords, 1, len, fp); 
	if (read_len != len)
	{
		printf("\nread [userwords.txt] failed!\n");
		goto upload_exit;
	}
	userwords[len] = '\0';
	
	MSPUploadData("userwords", userwords, len, "sub = uup, dtt = userword", &ret); //ÉÏ´«ÓÃ»§´Ê±í
	if (MSP_SUCCESS != ret)
	{
		printf("\nMSPUploadData failed ! errorCode: %d \n", ret);
		goto upload_exit;
	}
	
upload_exit:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}	
	if (NULL != userwords)
	{
		free(userwords);
		userwords = NULL;
	}
	
	return ret;
}

static void show_result(char *string, char is_over)
{
	printf("Input: [ %s ]...show_resul +1\n", string);

	if(is_over)
		putchar('\n');
}

char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

void on_result(const char *result, char is_last)
{
	if (result) {
		size_t left = g_buffersize - 1 - strlen(g_result);
		size_t size = strlen(result);
		if (left < size) {
			g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
			if (g_result)
				g_buffersize += BUFFER_SIZE;
			else {
				printf("mem alloc failed\n");
				return;
			}
		}
		strncat(g_result, result, size);
		/* show the final result, result is saved in  g_result */
		show_result(g_result, is_last);
		

	}
}
void on_speech_begin()
{
	if (g_result)
	{
		free(g_result);
	}
	g_result = (char*)malloc(BUFFER_SIZE);
	g_buffersize = BUFFER_SIZE;
	memset(g_result, 0, g_buffersize);

	printf(">>>>> Start Listening >>>>>\n");
}
void on_speech_end(int reason)
{
	if (reason == END_REASON_VAD_DETECT){
		printf("Speaking done \n");
		usleep(1000*200); //0.1s
		SpeakDone = true;
	}
	
	else
		printf("\nRecognizer error %d\n", reason);
}



/* demo send audio data from a file */
static void demo_file(const char* audio_file, const char* session_begin_params_iat)
{
	int	errcode = 0;
	FILE*	f_pcm = NULL;
	char*	p_pcm = NULL;
	unsigned long	pcm_count = 0;
	unsigned long	pcm_size = 0;
	unsigned long	read_size = 0;
	struct speech_rec iat;
	struct speech_rec_notifier recnotifier = {
		on_result,
		on_speech_begin,
		on_speech_end
	};

	if (NULL == audio_file)
		goto iat_exit;

	f_pcm = fopen(audio_file, "rb");
	if (NULL == f_pcm)
	{
		printf("\nopen [%s] failed! \n", audio_file);
		goto iat_exit;
	}

	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm);
	fseek(f_pcm, 0, SEEK_SET);

	p_pcm = (char *)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		printf("\nout of memory! \n");
		goto iat_exit;
	}

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm);
	if (read_size != pcm_size)
	{
		printf("\nread [%s] error!\n", audio_file);
		goto iat_exit;
	}

	errcode = sr_init(&iat, session_begin_params_iat, SR_USER, &recnotifier);
	if (errcode) {
		printf("speech recognizer init failed : %d\n", errcode);
		goto iat_exit;
	}

	errcode = sr_start_listening(&iat);
	if (errcode) {
		printf("\nsr_start_listening failed! error code:%d\n", errcode);
		goto iat_exit;
	}

	while (1)
	{
		unsigned int len = 10 * FRAME_LEN; /* 200ms audio */
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;

		ret = sr_write_audio_data(&iat, &p_pcm[pcm_count], len);

		if (0 != ret)
		{
			printf("\nwrite audio data failed! error code:%d\n", ret);
			goto iat_exit;
		}

		pcm_count += (long)len;
		pcm_size -= (long)len;		
	}

	errcode = sr_stop_listening(&iat);
	if (errcode) {
		printf("\nsr_stop_listening failed! error code:%d \n", errcode);
		goto iat_exit;
	}

iat_exit:
	if (NULL != f_pcm)
	{
		fclose(f_pcm);
		f_pcm = NULL;
	}
	if (NULL != p_pcm)
	{
		free(p_pcm);
		p_pcm = NULL;
	}

	sr_stop_listening(&iat);
	sr_uninit(&iat);
}

/* demo recognize the audio from microphone */
static void demo_mic(const char* session_begin_params_iat)
{
	int errcode;
	int i = 0;

	struct speech_rec iat;

	struct speech_rec_notifier recnotifier = {
		on_result,
		on_speech_begin,
		on_speech_end
	};

	errcode = sr_init(&iat, session_begin_params_iat, SR_MIC, &recnotifier);
	if (errcode) {
		printf("speech recognizer init failed\n");
		return;
	}
	errcode = sr_start_listening(&iat);
	if (errcode) {
		printf("start listen failed %d\n", errcode);
	}
	//SpeakDone
	while(SpeakDone==false)
		//sleep(1);
		usleep(1000*300); //0.2s


	// 可以尝试在这里更新语音识别结果到前端
	errcode = sr_stop_listening(&iat);
	if (errcode) {
		printf("stop listening failed %d\n", errcode);
	}

	sr_uninit(&iat);
}

/**************************************************************************************************************/


/* Reconition result is saved in  g_result */
int SpeechRecognition(void)
{
	int         ret                  = MSP_SUCCESS;
	const char* login_params         = "appid = 5d6e777f, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
	

 	int aud_src = 0; /* from mic or file */

	/*
	* See "iFlytek MSC Reference Manual"
	*/
	const char* session_begin_params_iat =
		"sub = iat, domain = iat, language = zh_cn, "
		"accent = mandarin, sample_rate = 16000, "
		"result_type = plain, result_encoding = utf8";


	/* rdn:           合成音频数字发音方式
	 * volume:        合成音频的音量
	 * pitch:         合成音频的音调
	 * speed:         合成音频对应的语速
	 * voice_name:    合成发音人
	 * sample_rate:   合成音频采样率
	 * text_encoding: 合成文本编码格式
	 * */
	
	const char* recorded_audio_file_path     = "./bin/wav/iflytek01.wav";  //待修改！！！！！！！！！！！！！！！！！！！


	/* Login first. 
	 * the 1st arg is username
	 * the 2nd arg is password
	 * the 3rd arg is login paramertes 
	 * */
	ret = MSPLogin(NULL, NULL, login_params);
	if (MSP_SUCCESS != ret)
	{
		printf("MSPLogin failed, Error code: %d.\n", ret);
		goto exit ;//login fail, exit the program
	}

	/* 
	* The following is the speech recognition code.
	*/
	printf("\n\n== 1# RECOGNITION PROCESS ================\n");
	// printf("\nWhere the audio comes from?\n"
	// 		"0: From a audio file.\n1: From microphone.\n");
	// scanf("%d", &aud_src);
	aud_src = 1;
	if(aud_src != 0) {
		// printf("Recognizing the speech from microphone\n");

		SpeakDone = false; /* Set SpeakDone = true after finishing speaking */
		demo_mic(session_begin_params_iat);
		// while(SpeakDone == false);
	} else {
		printf("\nDemo recgonizing the speech from a recorded audio file:/%s\n",recorded_audio_file_path);
		demo_file(recorded_audio_file_path, session_begin_params_iat); 
	}

	
exit:
	MSPLogout(); // Logout...


	return 0;
}






