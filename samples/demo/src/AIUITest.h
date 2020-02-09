/*
* AIUIAgentTest.h
*
*  Created on: 2017年3月9日
*      Author: hj
*/

#ifndef AIUIAGENTTEST_H_
#define AIUIAGENTTEST_H_

#include "aiui/AIUI.h"

#include <string>
#include <iostream>
#include <map>

#include "FileUtil.h"

#include <unistd.h>
#include <pthread.h>

#define TEST_ROOT_DIR "./AIUI/"

//配置文件打的路径，里面是客户端设置的参数
#define CFG_FILE_PATH "/home/aibar/intelligentvoiceordering/demo_aiui_iat_tts/samples/demo/build/AIUI/cfg/aiui.cfg"

//测试音频的路径
#define TEST_AUDIO_PATH "./build/AIUI/audio/tts_sample.wav"

#define GRAMMAR_FILE_PATH "./build/AIUI/asr/call.bnf"

#define LOG_DIR "./AIUI/log"


using namespace aiui;
using namespace std;

#define OPT_HOT   0B0100
#define OPT_COLD  0B0010
#define OPT_NONE  0B0000
#define OPT_VALID 0B0001
#define OPT_ERR   0B1000	



class OrderProcessManager
{
private:
	class KEY
	{
	public:
		const char* customkey 	= "\"intentType\":\"custom\""; 
		const char* turingkey 	= "\"answerType\":\"Turing\",\"emotion\":\"default\",\"text\":\"";  
		const char* noAnswerkey = "\"intent\":{\"rc\":4,\"uuid\""; 
		const char* noSoundkey 	= "\"intent\":{}"; 
		
		const char* drinkNameSlotkey 	= "\"name\":\"DrinkNameSlot\",\"normValue\":\"";
		const char* cupNumSlotkey 		= "\"name\":\"CupNumSlot\",\"normValue\":\"";
		const char* cupTypeSlotkey 		= ""; 								
		const char* tempSlotkey 		= "\"name\":\"TempSlot\",\"normValue\":\""; 
		const char* startSkillkey 		= "\"intent\":\"StartSkill\""; 
		const char* endSignalkey 		= "\"intent\":\"EndSignal\""; 

		const char* recommendationkey 	= "\"intent\":\"Recommendation\""; 	//新增
		const char* coffeekey 			= "\"intent\":\"Coffee\""; 			//新增
		const char* cockTailkey 		= "\"intent\":\"CockTail\""; 		//新增
		const char* mockTailkey 		= "\"intent\":\"MockTail\""; 		//新增
		const char* liqueurCoffeekey 	= "\"intent\":\"LiqueurCoffee\""; 	//新增
		const char* coldDrinkkey 		= "\"intent\":\"ColdDrink\""; 		//新增
		const char* noNeedkey 			= "\"intent\":\"NoNeed\""; 			//新增

	}KEY;

public:
	class slotValue
	{
	public:
		string DrinkName 		= "";
		string DrinkNameUpdate	= "";
		string CupNum    		= "";
		string CupNumUpdate		= "";
		string CupType  		= "大杯";
		string CupTypeUpdate  	= "大杯"; 									
		string Temp     		= "热的";
		string TempUpdate 		= "热的";

	}slotValue;

	class slotFlag
	{
	public:
		bool DrinkName 	= false;
		bool CupNum 	= false;
		bool CupType 	= false; 										
		bool Temp 		= false;
		bool Full 		= false;

		bool isChange = false;

	}slotFlag;

	enum ANSWER_TYPE						/** type of answer by AIUI*/
	{
		CUSTOM,
		TURING,
		NO_ANSWER,
		NO_SOUND
	}AnswerType;

	enum INTENT 							/** the state of the dialog */
	{
		ENTRY,
		PROCESS,
		END,

		RECOMMENDATION, 					/** 新增推荐提问 */
		COFFEE,								/** 咖啡系列 */
		COCKTAIL,							/** 鸡尾酒系列 */
		MOCKTAIL, 							/** 无酒精鸡尾酒 */
		LIQUEUR_COFFEE,						/** 酒咖系列 */
		COLD_DRINK,							/** 冷饮系列 */
		NO_NEED								/** 顾客拒绝点餐 */
	}Intent;

	int n_NoSound 	= 0;
	int n_NoAnswer 	= 0;
 	
	/**
	 * extracted from resultChar, actually set by hand so far
	 * generate a random integer to decide a specific response
	 */
	string ExtractResult; 

	int TempCheck(string);
	bool _slotFlag_Full();
	string ExtractJson(char*);
};

class FixedResponse{
private:
	string m_RECOMMENDATION = "大眼睛已为您推荐了饮品，请在屏幕上查看！决定后请直接告诉我您需要的饮品！";
	// string m_COFFEE 		= "我们提供了拿铁、卡布其诺、美式、摩卡、等众多选择，具体请查看屏幕菜单！";
	string m_COFFEE 		= "我们提供了摩卡、拿铁、美式、卡布奇诺、焦糖玛奇朵等众多口味，您需要什么呢？";
	string m_COCKTAIL 		= "鸡尾酒有很多口味，请您参考屏幕上的菜单选项！";
	string m_MOCKTAIL 		= "我们提供了各种无酒精鸡尾酒，具体请查看屏幕菜单！";
	string m_LIQUEUR_COFFEE = "我们提供了系列酒咖类饮品，详情请您查看屏幕菜单选项！";
	string m_COLD_DRINK 	= "天气寒冷，建议您喝热饮哦！";
	string m_NO_NEED 		= "好的，下次再见！";
	string m_END 			= "好的，马上为您下单，大眼睛祝您用餐愉快！";
public:
	string Get(string intent){
		if(intent=="RECOMMENDATION")
			return this->m_RECOMMENDATION;
		else if(intent=="COFFEE")
			return this->m_COFFEE;
		else if(intent=="COCKTAIL")
			return this->m_COCKTAIL;
		else if(intent=="MOCKTAIL")
			return this->m_MOCKTAIL;
		else if(intent=="LIQUEUR_COFFEE")
			return this->m_LIQUEUR_COFFEE;
		else if(intent=="COLD_DRINK")
			return this->m_COLD_DRINK;
		else if(intent=="NO_NEED")
			return this->m_NO_NEED;
		else if(intent=="END")
			return this->m_END;
	}
};



//写测试音频线程
class WriteAudioThread
{
private:
	IAIUIAgent* mAgent;

	string mAudioPath;

	bool mRepeat;

	bool mRun;

	FileUtil::DataFileHelper* mFileHelper;

  pthread_t thread_id;
  bool thread_created;

private:
	bool threadLoop();

  static void* thread_proc(void * paramptr);

public:
	WriteAudioThread(IAIUIAgent* agent, const string& audioPath, bool repeat);

	~WriteAudioThread();

	void stopRun();

	bool run();

};


//监听器，SDK通过onEvent（）回调抛出状态，结果等信息
class TestListener : public IAIUIListener
{
private:
	FileUtil::DataFileHelper* mTtsFileHelper;

public:
	void onEvent(const IAIUIEvent& event) const;

	TestListener();

	~TestListener();
};


//测试主类
class AIUITester
{
private:
	IAIUIAgent* agent;

	TestListener listener;

	WriteAudioThread * writeThread;

	string encode(const unsigned char* bytes_to_encode, unsigned int in_len);

public:
	AIUITester() ;

	~AIUITester();
//private
public: 

	void showIntroduction(bool detail);
	//创建AIUI 代理，通过AIUI代理与SDK发送消息通信
	void createAgent();

	//唤醒接口
	void wakeup();

	//开始AIUI，调用stop()之后需要调用此接口才可以与AIUI继续交互
	void start();

	//停止AIUI
	void stop();

	//写音频接口，参数表示是否重复写
	void write(bool repeat);

	void stopWriteThread();

	void reset();

	//写文本接口
	void writeText(string text);
	// void writeText();

	void syncSchema();

	void querySyncStatus();

	void buildGrammar();

	void updateLocalLexicon();

	//开始tts 注意此处合成的是.pcm格式音频
	void startTts();

	//暂停tts
	void pauseTts();

	//继续上次的tts
	void resumeTts();

	//取消本次tts
	void cancelTts();

	void destory();

public:
	void readCmd();

	void test();
};



#endif /* AIUIAGENTTEST_H_ */
