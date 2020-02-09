#include "AIUITest.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "jsoncpp/json/json.h"
#include "../../../include/iat_tts/msp_cmn.h"
#include "demo.h"

using namespace VA;
using namespace std;
static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

extern class MsgFromROS msgFromROS;				/** Msg from ROS */
extern class MsgToROS msgToROS;					/** Msg to ROS */
extern class MsgTofronEnd msgTofrontEnd;		/** Msg to Frond end */

class OrderProcessManager OrderProcessManager;
class FixedResponse FixedResponse;

#define NO_ANSWER_2_CUSTOM

#ifdef NO_ANSWER_2_CUSTOM
bool FLAG = false;
#endif
bool AIUI_Done = false;

extern int pipe_fd_wr;



#define NONE_TEMP_OPT  	 /** 暂时取消饮品冷/热问题选项 */
int OrderProcessManager::TempCheck(string drinkName){

#ifdef NONE_TEMP_OPT
		return OPT_NONE;
#else
		//咖啡系列
		if(drinkName=="拿铁") 					return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="美式") 			    return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="卡布奇诺") 		 	return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="馥芮白") 			return OPT_HOT|OPT_VALID;  
		else if(drinkName=="海盐焦糖拿铁")		 return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="斑斓拿铁")		 	return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="摩卡")			   return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="黑糖拿铁")			return OPT_HOT|OPT_COLD|OPT_VALID;  
		else if(drinkName=="冷翠咖啡") 			return OPT_COLD|OPT_VALID; 
		//酒咖系列
		else if(drinkName=="爱尔兰咖啡")		return OPT_HOT|OPT_COLD|OPT_VALID;  
		else if(drinkName=="薄荷酒摩卡")    	return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="榛果酒巧克力拿铁")	 return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="椰香酒拿铁")		return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="意式经典") 			return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="橙酒巧克力拿铁")	 return OPT_HOT|OPT_COLD|OPT_VALID; 
		else if(drinkName=="巧克力威士忌拿铁")	 return OPT_HOT|OPT_COLD|OPT_VALID; 
		//鸡尾酒系列
		else if(drinkName=="莫斯科骡子") 		return OPT_NONE; 
		else if(drinkName=="荔枝马天尼") 		return OPT_NONE;
		else if(drinkName=="意大利夏日") 		return OPT_NONE;
		else if(drinkName=="金汤尼")			return OPT_NONE;
		else if(drinkName=="咖啡尼格罗尼")		 return OPT_NONE;
		else if(drinkName=="白色俄罗斯")		return OPT_NONE; 
		else if(drinkName=="纽约酸") 			return OPT_NONE; 
		else if(drinkName=="古典比率")			return OPT_NONE;
		else if(drinkName=="煎蕊") 				return OPT_NONE; 
		else if(drinkName=="百香果")			return OPT_NONE;  
		else if(drinkName=="ss刺客")    		return OPT_NONE; 
		else if(drinkName=="藏红花")			return OPT_NONE; 
		else if(drinkName=="桂花酿")			return OPT_NONE;
		else if(drinkName=="坚果橙子")	 		return OPT_NONE; 
		else if(drinkName=="桃子金菲士")		 return OPT_NONE;
		else if(drinkName=="美国柠檬")			 return OPT_NONE;
		else if(drinkName=="苹果莫奇多")    	 return OPT_NONE; 
		else if(drinkName=="蓝色玛格丽特")		 return OPT_NONE; 
		else if(drinkName=="春晓")				return OPT_NONE; 
		else if(drinkName=="薄荷朱莉普") 		 return OPT_NONE; 
		else if(drinkName=="长岛冰茶")			 return OPT_NONE;
		//无酒精鸡尾酒系列
		else if(drinkName=="零度煎蕊")			return OPT_NONE;
		else if(drinkName=="海风") 			    return OPT_NONE; 
		else if(drinkName=="肉桂骡子")			return OPT_NONE;  
		else if(drinkName=="抹茶")    			return OPT_NONE; 
		else if(drinkName=="杂果宾治")			return OPT_NONE; 
		else if(drinkName=="香柠西柚")			return OPT_NONE;
		//清凉冷饮
		else if(drinkName=="芒果优格泡泡") 		 return OPT_NONE; 
		else if(drinkName=="西柚优格泡泡")		 return OPT_NONE;
		else if(drinkName=="蓝甘优格泡泡")		 return OPT_NONE;
		else if(drinkName=="西柚冷翠茶")    	 return OPT_NONE; 
		else if(drinkName=="荔枝冷翠茶")		 return OPT_NONE; 
		else if(drinkName=="草莓冷翠茶")		 return OPT_NONE; 

		else								   return OPT_ERR;
#endif
}



bool OrderProcessManager::_slotFlag_Full()
{
    //根据饮品名检查是否冷/热可选，在此基础上决定槽值是否填满
    if(this->slotFlag.DrinkName == true){
    	//温度存在选择，要求this->slotFlag.Temp == true
    	if(TempCheck(this->slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID))
    		this->slotFlag.Full = (this->slotFlag.Temp == true && this->slotFlag.CupNum == true) ? true : false;
    	//温度不存在选择，则忽略this->slotFlag.Temp
    	else
    		this->slotFlag.Full = (this->slotFlag.CupNum == true) ? true : false;
    }

	return this->slotFlag.Full;
}

string OrderProcessManager::ExtractJson(char* Json)
{
    char extractResult[500]; 
    char Value[500];
    char* start = NULL;
    char* end = NULL;

	if(strstr(Json, KEY.customkey)!=NULL){
		// cout << "Match the customkey!" << endl;
		this->AnswerType = CUSTOM; 				
	}
	
	if(strstr(Json, KEY.turingkey)!=NULL){
		// cout << "Match the turingkey!" << endl;
		start = strstr(Json, KEY.turingkey);
		start += strlen(KEY.turingkey); 
		this->AnswerType = TURING;
		for(end = start; *end != '\"' && *end!='\0'; ++end);
    	memcpy(extractResult, start, end-start);
    	extractResult[end-start] = 0;
	}

	if(strstr(Json, KEY.noAnswerkey)!=NULL){
		// cout << "Match the noAnswerkey!" << endl;
		this->AnswerType = NO_ANSWER;
	}

	if(strstr(Json, KEY.noSoundkey)!=NULL){
		// cout << "Match the noSoundkey!" << endl;
		this->AnswerType = NO_SOUND;
	}

	if(strstr(Json, KEY.drinkNameSlotkey)!=NULL){
		start = strstr(Json, KEY.drinkNameSlotkey);
		start += strlen(KEY.drinkNameSlotkey);
		for(end = start; *end != '\"' && *end!='\0'; ++end);
    	memcpy(Value, start, end-start);
    	Value[end-start] = 0;
    	//考虑到槽值可能二次修改
    	if(this->slotFlag.DrinkName==true){
    		this->slotValue.DrinkNameUpdate = Value;
    		if(this->slotValue.DrinkNameUpdate!=this->slotValue.DrinkName){
    			this->slotValue.DrinkName = this->slotValue.DrinkNameUpdate;
    			this->slotFlag.isChange = true;
    		}
    	}
    	else
    		this->slotValue.DrinkName = Value;

		//获取到饮品名立即更新至前端
		msgTofrontEnd.DrinkName = this->slotValue.DrinkName;
    	this->slotFlag.DrinkName = true;
    	this->Intent = PROCESS;
    	cout << "{DrinkNameSlot}:" << this->slotValue.DrinkName << endl;
	}

	if(strstr(Json, KEY.cupNumSlotkey)!=NULL){
		start = strstr(Json, KEY.cupNumSlotkey);
		start += strlen(KEY.cupNumSlotkey);
		for(end = start; *end != '\"' && *end!='\0'; ++end);
    	memcpy(Value, start, end-start);
    	Value[end-start] = 0;
    	//考虑到槽值可能二次修改
    	if(this->slotFlag.CupNum==true){
    		this->slotValue.CupNumUpdate = Value;
    		if(this->slotValue.CupNumUpdate!=this->slotValue.CupNum){	
    			this->slotValue.CupNum = this->slotValue.CupNumUpdate;
    			this->slotFlag.isChange = true;
    		}
    	}
    	else
    		this->slotValue.CupNum = Value;	

		//获取到立即更新至前端
		msgTofrontEnd.CupNum = this->slotValue.CupNum;
    	this->slotFlag.CupNum = true;
    	this->Intent = PROCESS;
    	cout << "{CupNumSlot}:" << this->slotValue.CupNum << endl;
	}

	if(strstr(Json, KEY.tempSlotkey)!=NULL){
		start = strstr(Json, KEY.tempSlotkey);
		start += strlen(KEY.tempSlotkey);
		for(end = start; *end != '\"' && *end!='\0'; ++end);
    	memcpy(Value, start, end-start);
    	Value[end-start] = 0;
    	//考虑到槽值可能二次修改
    	if(this->slotFlag.Temp==true){
    		this->slotValue.TempUpdate = Value;
    		if(this->slotValue.TempUpdate!=this->slotValue.Temp){
    			this->slotValue.Temp = this->slotValue.TempUpdate;
    			this->slotFlag.isChange = true;
    		}
    	}
    	else
    		this->slotValue.Temp = Value;

		//获取到立即更新至前端
		msgTofrontEnd.Temp = this->slotValue.Temp;
    	this->slotFlag.Temp = true;
    	this->Intent = PROCESS;
    	cout << "{TempSlot}:" << this->slotValue.Temp << endl;
	}

	if(strstr(Json, KEY.startSkillkey)!=NULL){

		//开始点餐,所有属性进行初始化
		this->slotValue.DrinkName 	  	= "";
		this->slotValue.DrinkNameUpdate	= "";
		this->slotValue.CupNum    		= "";
		this->slotValue.CupNumUpdate    = "";
		this->slotValue.CupType      	= "大杯";
		this->slotValue.CupTypeUpdate   = "大杯";
		this->slotValue.Temp      		= "热的";
		this->slotValue.TempUpdate      = "热的";

		this->slotFlag.DrinkName	= false;
		this->slotFlag.CupNum 		= false;
		this->slotFlag.Temp 		= false;
		this->slotFlag.Full 		= false;

		this->slotFlag.isChange = false;
		this->Intent = ENTRY;

		//发送至前端的订单信息初始化
		msgTofrontEnd.speechRecoResult  = "";
		msgTofrontEnd.DrinkName 		= "";
		msgTofrontEnd.CupNum    		= "";
		msgTofrontEnd.CupType   		= "大杯";
		msgTofrontEnd.Temp      		= "热的";

		//现在订单信息不用转发至ROS，直接写给dss前端
		msgToROS.DrinkName = "";
		msgToROS.CupNum    = "";
		msgToROS.CupType   = "";
		msgToROS.Temp      = "";
	}

	if(strstr(Json, KEY.endSignalkey)!=NULL){
		//检查所需槽值是否填满(此处所需槽值不一定是所有槽值，因饮品而异)，防止错误触发
		if(this->slotFlag.Full==true){
			//现在订单信息不用转发至ROS，直接写给dss前端
			msgToROS.DrinkName = this->slotValue.DrinkName;
			msgToROS.CupNum    = this->slotValue.CupNum;
			msgToROS.CupType   = this->slotValue.CupType;
			msgToROS.Temp      = this->slotValue.Temp;

			//下单结束，最终订单信息发送至前端
			msgTofrontEnd.DrinkName = this->slotValue.DrinkName;
			msgTofrontEnd.CupNum    = this->slotValue.CupNum;
			msgTofrontEnd.CupType   = this->slotValue.CupType;
			msgTofrontEnd.Temp      = this->slotValue.Temp;

			this->slotFlag.isChange = false;
			this->Intent = END;
		}
		//槽值未填满时，跳转至 NO_ANSWER 继续追问顾客
		else
			this->AnswerType = NO_ANSWER;
	}
	/*------------------------------------新增环节----------------------------------------*/
	if(strstr(Json, KEY.recommendationkey) != NULL){
		this->Intent = RECOMMENDATION; 
		cout << "{recommendation}: 推荐提问！" << endl;
	}
	if(strstr(Json, KEY.coffeekey) != NULL){
		this->Intent = COFFEE; 
		cout << "{coffee}: 咖啡系列！" << endl;
	}
	if(strstr(Json, KEY.cockTailkey) != NULL){
		this->Intent = COCKTAIL; 
		cout << "{cockTail}: 鸡尾酒系列！" << endl;
	}
	if(strstr(Json, KEY.mockTailkey) != NULL){
		this->Intent = MOCKTAIL; 
		cout << "{mockTail}: 无酒精鸡尾酒系列！" << endl;
	}
	if(strstr(Json, KEY.liqueurCoffeekey) != NULL){
		this->Intent = LIQUEUR_COFFEE; 
		cout << "{liqueurCoffee}: 酒咖系列！" << endl;
	}
	if(strstr(Json, KEY.coldDrinkkey) != NULL){
		this->Intent = COLD_DRINK; 
		cout << "{coldDrink}: 冷饮系列！" << endl;
	}
	if(strstr(Json, KEY.noNeedkey) != NULL){
		this->Intent = NO_NEED; 
		cout << "{noNeed}: 拒绝点餐！" << endl;
	}
    	  	

/*
    //根据饮品名检查是否冷/热可选，在此基础上决定槽值是否填满
    if(this->slotFlag.DrinkName == true){
    	//温度存在选择，要求this->slotFlag.Temp == true
    	if(TempCheck(this->slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID))
    		this->slotFlag.Full = (this->slotFlag.Temp == true && this->slotFlag.CupNum == true) ? true : false;
    	//温度不存在选择，则忽略this->slotFlag.Temp
    	else
    		this->slotFlag.Full = (this->slotFlag.CupNum == true) ? true : false;
    }
*/
	// check 槽值是否填满, 并赋值this->slotFlag.Full
	_slotFlag_Full();

    if(this->AnswerType == CUSTOM)
    	return "Custom";

    else if(this->AnswerType == TURING)
    	return extractResult;

    else if(this->AnswerType == NO_ANSWER){
#ifdef NO_ANSWER_2_CUSTOM
		if(FLAG){
			cout << "NO_ANSWER --> CUSTOM" << endl;
			this->AnswerType = CUSTOM;
			FLAG = false;
		}
		else
			FLAG = true;
#endif
		return "NoAnswer";
	}
    
    else//(this->AnswerType==NO_SOUND)
    	return "NoSound";
}


//循环写入测试音频，每次写1278B
bool WriteAudioThread::threadLoop()
{
	char audio[1280];
	int len = mFileHelper->read(audio, 1280);

	if (len > 0)
	{
		Buffer* buffer = Buffer::alloc(len);//申请的内存会在sdk内部释放
		memcpy(buffer->data(), audio, len);

		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
			0, 0,  "data_type=audio,sample_rate=16000", buffer);	

		if (NULL != mAgent)
		{
			mAgent->sendMessage(writeMsg);
		}		
		writeMsg->destroy();
		usleep(40 * 1000);
	} else {
		if (mRepeat)
		{
			mFileHelper->rewindReadFile();
		} else {
			IAIUIMessage * stopWrite = IAIUIMessage::create(AIUIConstant::CMD_STOP_WRITE,
				0, 0, "data_type=audio,sample_rate=16000");

			if (NULL != mAgent)
			{
				mAgent->sendMessage(stopWrite);
			}
			stopWrite->destroy();

			mFileHelper->closeReadFile();
			mRun = false;
		}
	}

	return mRun;
}


void* WriteAudioThread::thread_proc(void * paramptr)
{
	WriteAudioThread * self = (WriteAudioThread *)paramptr;

	while (1) {
		if (! self->threadLoop())
			break;
	}
	return NULL;
}

WriteAudioThread::WriteAudioThread(IAIUIAgent* agent, const string& audioPath, bool repeat):
mAgent(agent), mAudioPath(audioPath), mRepeat(repeat), mRun(true), mFileHelper(NULL)
,thread_created(false)
{
	mFileHelper = new FileUtil::DataFileHelper("");
	mFileHelper->openReadFile(mAudioPath, false);
}

WriteAudioThread::~WriteAudioThread()
{
	if (NULL != mFileHelper)
	{
		delete mFileHelper;
		mFileHelper = NULL;
	}
}

void WriteAudioThread::stopRun()
{
    if (thread_created) {
        mRun = false;
        void * retarg;
        pthread_join(thread_id, &retarg);
        thread_created = false;
    }
}

bool WriteAudioThread::run()
{
    if (thread_created == false) {
        int rc = pthread_create(&thread_id, NULL, thread_proc, this);
        if (rc != 0) {
            exit(-1);
        }
        thread_created = true;
        return true;
    }

    return false;
}

string mSyncSid;

//事件回调接口，SDK状态，文本，语义结果等都是通过该接口抛出
void TestListener::onEvent(const IAIUIEvent& event) const
{
	switch (event.getEventType()) {
	//SDK 状态回调
	case AIUIConstant::EVENT_STATE:
		{
			switch (event.getArg1()) {
			case AIUIConstant::STATE_IDLE:
				{
					cout << "EVENT_STATE:" << "IDLE" << endl;
				} break;

			case AIUIConstant::STATE_READY:
				{
					cout << "EVENT_STATE:" << "READY" << endl;
				} break;

			case AIUIConstant::STATE_WORKING:
				{
					cout << "EVENT_STATE:" << "WORKING" << endl;
				} break;
			}
		} break;

	//唤醒事件回调
	case AIUIConstant::EVENT_WAKEUP:
		{
			cout << "EVENT_WAKEUP:" << event.getInfo() << endl;
		} break;

	//休眠事件回调
	case AIUIConstant::EVENT_SLEEP:
		{
			cout << "EVENT_SLEEP:arg1=" << event.getArg1() << endl;
		} break;

	//VAD事件回调，如找到前后端点
	case AIUIConstant::EVENT_VAD:
		{
			switch (event.getArg1()) {
			case AIUIConstant::VAD_BOS:
				{
					cout << "EVENT_VAD:" << "BOS" << endl;
				} break;

			case AIUIConstant::VAD_EOS:
				{
					cout << "EVENT_VAD:" << "EOS" << endl;
				} break;

			case AIUIConstant::VAD_VOL:
				{
					//						cout << "EVENT_VAD:" << "VOL" << endl;
				} break;
			}
		} break;

	//最重要的结果事件回调
	case AIUIConstant::EVENT_RESULT:
		{
			Json::Value bizParamJson;
			Json::Reader reader;
			
			if (!reader.parse(event.getInfo(), bizParamJson, false)) {
				cout << "parse error!" << endl << event.getInfo() << endl;
				break;
			}
			Json::Value data = (bizParamJson["data"])[0];
			Json::Value params = data["params"];
			Json::Value content = (data["content"])[0];
			string sub = params["sub"].asString();
			//sub字段表示结果的类别，如iat听写，nlp语义结果

			cout << "\n\n== 2# AIUI PROCESS =======================" << endl;
			cout << "EVENT_RESULT:" << sub << endl;

			if (sub == "nlp")
			{
				Json::Value empty;
				Json::Value contentId = content.get("cnt_id", empty);

				if (contentId.empty())
				{
					cout << "Content Id is empty" << endl;
					break;
				}	

				string cnt_id = contentId.asString();
				int dataLen = 0;
				const char* buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);
			
				string resultStr;
				char*  resultChar; 

				const string askHead = "您确定是要";
				const string askTail = "对吗？";

				if (NULL != buffer)
				{
					resultStr = string(buffer, dataLen);

					/** string converted to char* */
					resultChar = (char*)resultStr.data();   

					// cout << resultStr << endl;
			
					/** init the AIUI_Done flag */
					AIUI_Done = false;   

					OrderProcessManager.ExtractResult = OrderProcessManager.ExtractJson(resultChar); 

					
					/*=========================== CUSTOM =================================
					--------------------------------------------------------------------*/
					if(OrderProcessManager.AnswerType == OrderProcessManager::CUSTOM){

						/** EntryPoint of dialog */
						if(OrderProcessManager.Intent==OrderProcessManager.ENTRY){
							// OrderProcessManager.ExtractResult = "Entry";
							cout << "Local/CUSTOM--INITIAL: " << FixedResponse.Get("COFFEE") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("COFFEE");
						}

						/** Process of dialog */
						else if(OrderProcessManager.Intent==OrderProcessManager::PROCESS){

							/** slotValue is full */
							if(OrderProcessManager.slotFlag.Full==true){
								// cout << "Local:槽值已经填满了。" << endl;
								/** Something changes */
								if(OrderProcessManager.slotFlag.isChange==true){
									//OPT_HOT & OPT_COLD
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID)){
										cout << "Local/Custom:您是想换成" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.Temp << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您是想换成" + OrderProcessManager.slotValue.CupNum + OrderProcessManager.slotValue.Temp + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
										OrderProcessManager.slotFlag.isChange = false;
									}
									//OPT_HOT ONLY	
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_VALID)){
										cout << "Local/Custom:您是想换成" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您是想换成" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
										OrderProcessManager.slotFlag.isChange = false;
									}
									//OPT_COLD ONLY
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_COLD|OPT_VALID)){
										cout << "Local/Custom:您是想换成" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您是想换成" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
										OrderProcessManager.slotFlag.isChange = false;
									}
									//OPT_NONE
									else if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_NONE)){
										cout << "Local/Custom:您是想换成" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您是想换成" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
										OrderProcessManager.slotFlag.isChange = false;
									}
								}
								/** Nothing changes */
								else{
									//OPT_HOT & OPT_COLD
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.Temp << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum + OrderProcessManager.slotValue.Temp + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
									//OPT_HOT ONLY	
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_VALID)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
									//OPT_COLD ONLY
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_COLD|OPT_VALID)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
									//OPT_NONE
									else if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_NONE)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
								} 
							}

							//根据缺的槽值继续追问
							else if(OrderProcessManager.slotFlag.Full==false){
								if(OrderProcessManager.slotFlag.DrinkName==false){
									cout << "Local/Custom:可以先告诉我您要什么饮品吗？" << endl;
									OrderProcessManager.ExtractResult = "可以先告诉我您要什么饮品吗？";
								}
								
								else if(OrderProcessManager.slotFlag.CupNum==false){
									// cout << "Local/Custom:您想要几杯" << OrderProcessManager.slotValue.DrinkName << "呢？" << endl;
									// OrderProcessManager.ExtractResult = "您想要几杯" + OrderProcessManager.slotValue.DrinkName + "呢？";
									cout << "Local/Custom:您需要几杯呢？" << endl;
									OrderProcessManager.ExtractResult = "您需要几杯呢？";
								}
						
								
								else if(OrderProcessManager.slotFlag.Temp==false){
								 	//OPT_HOT & OPT_COLD
								 	if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID)){
										// cout << "Local/Custom:您喜欢热的还是冰的" << OrderProcessManager.slotValue.DrinkName << "呢？" << endl;
										// OrderProcessManager.ExtractResult = "您喜欢热的还是冰的" + OrderProcessManager.slotValue.DrinkName + "呢？";
										cout << "Local/Custom:您要热的还是冰的呢？" << endl;
										OrderProcessManager.ExtractResult = "您要热的还是冰的呢？";
									}
									//OPT_HOT ONLY	
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_VALID)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
									//OPT_COLD ONLY	
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_COLD|OPT_VALID)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
									//OPT_NONE
									else if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_NONE)){
										cout << "Local/Custom:您确定是要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "对吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定是要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "对吗？" ;
									}
								}
								
							}
						}

						/** EndPoint of dialog */
						else if(OrderProcessManager.Intent==OrderProcessManager::END){

							cout << "Local/Custom:" << FixedResponse.Get("END") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("END");
							
							msgToROS.OrderFinish = true;   /** set the flag, and write to fifo to notify ROS */
							msgTofrontEnd.OrderFinish = true;
						}

						//-----------------------新增功能在这里统一处理------------------------------
						else if(OrderProcessManager.Intent==OrderProcessManager::RECOMMENDATION){
							cout << "Local/Custom:" << FixedResponse.Get("RECOMMENDATION") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("RECOMMENDATION");
						}
						else if(OrderProcessManager.Intent==OrderProcessManager::COFFEE){
							cout << "Local/Custom:" << FixedResponse.Get("COFFEE") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("COFFEE");
						}
						else if(OrderProcessManager.Intent==OrderProcessManager::COCKTAIL){
							cout << "Local/Custom:" << FixedResponse.Get("COCKTAIL") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("COCKTAIL");
						}
						else if(OrderProcessManager.Intent==OrderProcessManager::MOCKTAIL){
							cout << "Local/Custom:" << FixedResponse.Get("MOCKTAIL") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("MOCKTAIL"); 
						}
						else if(OrderProcessManager.Intent==OrderProcessManager::LIQUEUR_COFFEE){
							cout << "Local/Custom:" << FixedResponse.Get("LIQUEUR_COFFEE") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("LIQUEUR_COFFEE"); 
						}
						else if(OrderProcessManager.Intent==OrderProcessManager::COLD_DRINK){
							cout << "Local/Custom:" << FixedResponse.Get("COLD_DRINK") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("COLD_DRINK");
						}
						else if(OrderProcessManager.Intent==OrderProcessManager::NO_NEED){
							cout << "Local/Custom:" << FixedResponse.Get("NO_NEED") << endl;
							OrderProcessManager.ExtractResult = FixedResponse.Get("NO_NEED");
							msgToROS.OrderFinish = true; 
							msgTofrontEnd.OrderFinish = true; 
						}

					}
					/*=========================== TURING =================================
					--------------------------------------------------------------------*/
					else if(OrderProcessManager.AnswerType == OrderProcessManager::TURING){
						cout << "Local/Turing: "   << "[ " << OrderProcessManager.ExtractResult << " ]" << endl; 
					}
					
					/*=========================== NO_ANSWER ==============================
					--------------------------------------------------------------------*/
					else if(OrderProcessManager.AnswerType == OrderProcessManager::NO_ANSWER){

						OrderProcessManager.n_NoAnswer++; 
						cout << "Local/NoAnswer: "  << "[ " << OrderProcessManager.ExtractResult << " x " << OrderProcessManager.n_NoAnswer << " ]" << endl;	

						//所有意图设置为入口。。。不再涉及退出技能的问题！
					

						//OrderProcessManager.slotValue is full
						if(OrderProcessManager.slotFlag.Full==true){
/*

							//cout << "Local:槽值已经填满了。" << endl;
							//OPT_HOT & OPT_COLD
							if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID)){
								cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.Temp << OrderProcessManager.slotValue.DrinkName << askTail<< endl;
								OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum + OrderProcessManager.slotValue.Temp + OrderProcessManager.slotValue.DrinkName + askTail;
							}
							//OPT_HOT ONLY	
							if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_VALID)){
								cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << askTail<< endl;
								OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + askTail ;
							}
							//OPT_COLD ONLY
							if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_COLD|OPT_VALID)){
								cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << askTail<< endl;
								OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + askTail ;
							}
							//OPT_NONE
							if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_NONE)){
								cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << askTail<< endl;
								OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + askTail ;
							}

*/
						}
						
						//根据缺的槽值继续追问
						else {
/*
							
							OrderProcessManager.n_NoAnswer++; 
							cout << "Local/NoAnswer: 没有听清" << endl;
							
*/

							// if(OrderProcessManager.slotFlag.DrinkName==false){
							// 	cout << "Local/NoAnswer:抱歉，您要喝什么呢？" << endl;
							// 	OrderProcessManager.ExtractResult = "抱歉，您要喝什么呢？";
							// }
							// else if(OrderProcessManager.slotFlag.CupNum==false){
							// 	cout << "Local/NoAnswer:抱歉，您需要几杯呢？" << endl;
							// 	OrderProcessManager.ExtractResult = "抱歉，您需要几杯呢？";
							// }
							// else if(OrderProcessManager.slotFlag.Temp==false){
							// 	//OPT_HOT & OPT_COLD
							//  	if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_HOT|OPT_COLD|OPT_VALID)){
							// 		cout << "Local/NoAnswer:抱歉，您想喝热的还是冰的呢？" << endl;
							// 		OrderProcessManager.ExtractResult = "抱歉，您想要喝热的还是冰的呢？";
							// 	}

								//实际不会再存在以下三种情形，前面extractJson()已做处理：温度不存在选择时，主动忽略OrderProcessManager.slotFlag.Temp,
								//即:{饮品名}{份数}填满时就执行OrderProcessManager.slotFlag.FUll=true

								/*
								
									//OPT_HOT ONLY								
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (	)){
										cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "吗？" ;
									}
									//OPT_COLD ONLY	
									if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_COLD|OPT_VALID)){
										cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "吗？" ;
									}
									//OPT_NONE
									else if(OrderProcessManager.TempCheck(OrderProcessManager.slotValue.DrinkName) == (OPT_NONE)){
										cout << "Local/NoAnswer:您确定要" << OrderProcessManager.slotValue.CupNum << OrderProcessManager.slotValue.DrinkName << "吗？"<< endl;
										OrderProcessManager.ExtractResult = "您确定要" + OrderProcessManager.slotValue.CupNum  + OrderProcessManager.slotValue.DrinkName + "吗？" ;
									}

								*/
							// }

						}

					}
					/*=========================== NO_SOUND ===============================
					--------------------------------------------------------------------*/
					// else if(OrderProcessManager.AnswerType == OrderProcessManager::NO_SOUND){
					else {

						msgToROS.generate_Json(msgFromROS.name, "puzzled", msgToROS.greetingDone, msgToROS.speechRecoResult, msgToROS.DrinkName, msgToROS.CupNum, msgToROS.CupType, msgToROS.Temp, msgToROS.OrderFinish, msgToROS.Json);
						int res = write(pipe_fd_wr, msgToROS.Json, strlen(msgToROS.Json));
						if(res==-1)  cout << "[ 'order' ] write err! " << endl;

						OrderProcessManager.n_NoSound++;
						cout << "Local/NoSound: "  << "[ " << OrderProcessManager.ExtractResult << " x " << OrderProcessManager.n_NoSound << " ]" << endl;
					}

					/** Extract AIUI Result done */
					AIUI_Done = true;   					 
				}

				else
				{
					cout << "buffer is NULL" << endl;
				}
			} 

			else if (sub == "tts")
			{
				cout << event.getInfo() << endl;

				string cnt_id = content["cnt_id"].asString();

				int dataLen = 0;
				const char* data = event.getData()->getBinary(cnt_id.c_str(), &dataLen);

				cout << "data_len=" << dataLen << endl;

				int dts = content["dts"].asInt();
				cout << "dts=" << dts << endl;

				string errorinfo = content["error"].asString();

				cout << "error info is " << errorinfo << endl;

				if (2 == dts && errorinfo == "AIUI DATA NULL")
				{
					//满足这两个条件的是空结果,不处理,直接丢弃
				}
				else if (3 == dts)
				{
					cout << "tts result receive dts = 3 result" << endl;
					mTtsFileHelper->createWriteFile("tts", ".pcm", false);   /* 指定生成音频格式？？ */
					mTtsFileHelper->write((const char*) data, dataLen, 0, dataLen);
					mTtsFileHelper->closeFile();
				}
				else
				{
					if (0 == dts)
					{
						mTtsFileHelper->createWriteFile("tts", ".pcm", false);
					}

					mTtsFileHelper->write((const char*) data, dataLen, 0, dataLen);

					if (2 == dts)
					{
						mTtsFileHelper->closeWriteFile();
					}
				}
			}

		}
		break;

		//上传资源数据的返回结果
	case AIUIConstant::EVENT_CMD_RETURN:
	{
		//cout << "onEvent --> EVENT_CMD_RETURN: arg1 is " << event.getArg1() << endl;
		if (AIUIConstant::CMD_SYNC == event.getArg1())
		{
			int retcode = event.getArg2();
			int dtype = event.getData()->getInt("sync_dtype", -1);

			//cout << "onEvent --> EVENT_CMD_RETURN: dtype is " << dtype << endl;

			switch (dtype)
			{
				case AIUIConstant::SYNC_DATA_STATUS:
					break;

				case AIUIConstant::SYNC_DATA_ACCOUNT:
					break;

				case AIUIConstant::SYNC_DATA_SCHEMA:
				{
					string sid = event.getData()->getString("sid", "");
					string tag = event.getData()->getString("tag", "");

					mSyncSid = sid;

					if (AIUIConstant::SUCCESS == retcode)
					{
						cout << "sync schema success." << endl;
					} else {
						cout << "sync schema error=" << retcode << endl;
					}

					cout << "sid=" << sid << endl;
					cout << "tag=" << tag << endl;
				} break;

				case AIUIConstant::SYNC_DATA_SPEAKABLE:
					break;

				case AIUIConstant::SYNC_DATA_QUERY://查询结果
				{
					if (AIUIConstant::SUCCESS == retcode)
					{
						cout << "sync status success" << endl;
					} else {
						cout << "sync status error=" << retcode << endl;
					}
				} break;
			}
		} else if (AIUIConstant::CMD_QUERY_SYNC_STATUS == event.getArg1()) {
			int syncType = event.getData()->getInt("sync_dtype", -1);
			if (AIUIConstant::SYNC_DATA_QUERY == syncType)
			{
				string result = event.getData()->getString("result", "");
				cout << "result:" << result << endl;

				if (0 == event.getArg2())
				{
					cout << "sync status:success." << endl;
				} else {
					cout << "sync status error:" << event.getArg2() << endl;
				}
			}
		} else if (AIUIConstant::CMD_BUILD_GRAMMAR == event.getArg1()) {
			if (event.getArg2() == 0)
			{
				cout << "build grammar success." << endl;
			}
			else
			{
				cout << "build grammar error, errcode = " << event.getArg2() << endl;
				cout << "error reasion is " << event.getInfo() << endl;
			}
		} else if (AIUIConstant::CMD_UPDATE_LOCAL_LEXICON == event.getArg1()) {
			if (event.getArg2() == 0)
			{
				cout << "update lexicon success" << endl;
			}
			else
			{
				cout << "update lexicon error, errcode = " << event.getArg2() << endl;
				cout << "error reasion is " << event.getInfo() << endl;
			}
		}
	} break;

	case AIUIConstant::EVENT_ERROR:
		{
			cout << "EVENT_ERROR:" << event.getArg1() << endl;
			cout << " ERROR info is " << event.getInfo() << endl;
		} break;
	}
}

AIUITester::AIUITester() : agent(NULL), writeThread(NULL)
{

}

AIUITester::~AIUITester()
{
	if (agent != NULL)
	{
		agent->destroy();
		agent = NULL;
	}
}

TestListener::TestListener()
{
	mTtsFileHelper = new FileUtil::DataFileHelper("");
}

TestListener::~TestListener()
{
	if (mTtsFileHelper != NULL)
	{
		delete mTtsFileHelper;
		mTtsFileHelper = NULL;
	}
}

void AIUITester::showIntroduction(bool detail)
{
	cout << "demo示例为输入命令，调用对应的函数，使用AIUI SDK完成文本理解，语义理解，文本合成等功能，如：" << endl;
	cout << "c命令，创建AIUI代理，与AIUI SDK交互都是通过代理发送消息的方式进行, 所以第一步必须是输入该命令；" << endl;
	cout << "w命令，发送外部唤醒命令唤醒AIUI，AIUI只有在唤醒过后才可以交互；" << endl;
	cout << "wr命令，单次读取本地pcm音频文件，写入sdk，sdk会返回云端识别的听写，语义结果；" << endl;
	cout << "wrt命令，字符串文本写入sdk，sdk会返回云端识别的语义结果；" << endl;
	cout << "stts命令，单合成示例，返回合成的音频，demo将音频保存为本地的pcm文件；" << endl;
	cout << "help命令，显示本demo提供的示例的介绍；" << endl;

	if (detail)
	{
		cout << "输入c命令后，正常情况返回结果为：" << endl;
		cout << "EVENT_STATE:READY" << endl;
		cout << "输入w命令后，正常情况返回结果为: " << endl;
		cout << "EVENT_WAKEUP:{\"CMScore\":-1,\"angle\":-1,\"beam\":0,\"channel\":-1,\"power\":-1}" << endl;
		cout << "EVENT_STATE:WORKING" << endl;

		cout << "听写，语义，合成等结果在onEvent函数，该函数是结果回调，请仔细研究。" << endl;
	}
}

//创建AIUI代理
void AIUITester::createAgent()
{
	string appid = "5d6e777f";
	Json::Value paramJson;
	Json::Value appidJson;

	appidJson["appid"] = appid;
	
	string fileParam = FileUtil::readFileAsString(CFG_FILE_PATH);
	Json::Reader reader;
	if(reader.parse(fileParam, paramJson, false))
	{
		paramJson["login"] = appidJson;

		//for ivw support
		string wakeup_mode = paramJson["speech"]["wakeup_mode"].asString();

		//如果在aiui.cfg中设置了唤醒模式为ivw唤醒，那么需要对设置的唤醒资源路径作处理，并且设置唤醒的libmsc.so的路径为当前路径
		if(wakeup_mode == "ivw")
		{
			//readme中有说明，使用libmsc.so唤醒库，需要调用MSPLogin()先登录
			//string lgiparams = "appid=5d6e777f,engine_start=ivw";
			//MSPLogin(NULL, NULL, lgiparams.c_str());
			string ivw_res_path = paramJson["ivw"]["res_path"].asString();
			if(!ivw_res_path.empty())
			{
				ivw_res_path = "fo|" + ivw_res_path;
				paramJson["ivw"]["res_path"] = ivw_res_path;
			}

			string ivw_lib_path = "libmsc.so";

			paramJson["ivw"]["msc_lib_path"] = ivw_lib_path;
		}
		//end

		Json::FastWriter writer;
		string paramStr = writer.write(paramJson);
		agent = IAIUIAgent::createAgent(paramStr.c_str(), &listener);
	}
	else
	{
		cout << "aiui.cfg has something wrong!" << endl;
	}
}

/*
	外部唤醒接口，通过发送CMD_WAKEUP命令对SDK进行外部唤醒，发送该命令后，SDK会进入working状态，用户就可以与SDK进行交互。
*/
void AIUITester::wakeup()
{
	if (NULL != agent)
	{
		IAIUIMessage * wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_WAKEUP);
		agent->sendMessage(wakeupMsg);
		wakeupMsg->destroy();
		// cout << "wakeup successfully!" << endl;
	}
}

//停止AIUI服务，此接口是与stop()对应，调用stop()之后必须调用此接口才能继续与SDK交互，如果你没有调用过stop(),就不需要调用该接口
void AIUITester::start()
{
	if (NULL != agent)
	{
		IAIUIMessage * startMsg = IAIUIMessage::create (AIUIConstant::CMD_START);
		agent->sendMessage(startMsg);
		startMsg->destroy();
	}
}

//停止AIUI服务
void AIUITester::stop()
{
	if (NULL != agent)
	{
		IAIUIMessage *stopMsg = IAIUIMessage::create (AIUIConstant::CMD_STOP);
		agent->sendMessage(stopMsg);
		stopMsg->destroy();
	}
}

//写入测试音频
void AIUITester::write(bool repeat)
{
	if (agent == NULL)
	{
		cout << "write anget == null" << endl;
		return;
	}

	if (writeThread == NULL) {
		writeThread = new WriteAudioThread(agent, TEST_AUDIO_PATH,  repeat);
		writeThread->run();
	}	
}



void AIUITester::stopWriteThread()
{
	if (writeThread) {
		writeThread->stopRun();
		delete writeThread;
		writeThread = NULL;
	}
}

void AIUITester::reset()
{
	if (NULL != agent)
	{
		IAIUIMessage * resetMsg = IAIUIMessage::create(AIUIConstant::CMD_RESET);
		agent->sendMessage(resetMsg);
		resetMsg->destroy();
	}
}

void AIUITester::buildGrammar()
{
	if (NULL != agent)
	{
		string grammarContent = FileUtil::readFileAsString(GRAMMAR_FILE_PATH);

		IAIUIMessage *grammarMsg = IAIUIMessage::create(AIUIConstant::CMD_BUILD_GRAMMAR, 0, 0, grammarContent.c_str());

		agent->sendMessage(grammarMsg);
		
		grammarMsg->destroy();
	}
}

void AIUITester::updateLocalLexicon()
{
	if (NULL != agent)
	{
		Json::Value paramJson;

		paramJson["name"] = "contact";
		paramJson["content"] = "张山\n李思\n刘得花\n";

		Json::FastWriter writer;
		string paramStr = writer.write(paramJson);

		IAIUIMessage *updateMsg = IAIUIMessage::create(AIUIConstant::CMD_UPDATE_LOCAL_LEXICON, 0, 0, paramStr.c_str(), NULL);     

		agent->sendMessage(updateMsg);

		updateMsg->destroy();
	}
}


//文本语义测试接口
void AIUITester::writeText(string text)
{
	if (NULL != agent)
	{
		// string text;

		// cout << "Please input chinese words:" << endl;
		// cin >> text;
		
		// textData内存会在Message在内部处理完后自动release掉
		Buffer* textData = Buffer::alloc(text.length());
		text.copy((char*) textData->data(), text.length());

		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
			0,0, "data_type=text", textData);	

		agent->sendMessage(writeMsg);
		writeMsg->destroy();		
	}
}


// //文本语义测试接口
// void AIUITester::writeText()
// {
// 	if (NULL != agent)
// 	{
// 		string text;

// 		cout << "Please input chinese words:" << endl;
// 		cin >> text;
		
// 		// textData内存会在Message在内部处理完后自动release掉
// 		Buffer* textData = Buffer::alloc(text.length());
// 		text.copy((char*) textData->data(), text.length());

// 		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
// 			0,0, "data_type=text", textData);	

// 		agent->sendMessage(writeMsg);
// 		writeMsg->destroy();		
// 	}
// }


string AIUITester::encode(const unsigned char* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;
}

//动态上传资源数据
void AIUITester::syncSchema()
{
	if (NULL != agent)
	{
		Json::Value paramsJson;
		paramsJson["tag"] = "tag_abc";

		Json::Value dataParamJson;
		dataParamJson["id_name"] = "uid";
		dataParamJson["res_name"] = "IFLYTEK.telephone_contact";

		string rawData = "{\"name\":\"程序员\", \"alias\":\"开发\"}\n";
		rawData.append("{\"name\":\"设计师\", \"alias\":\"设计\"}\n");
		rawData.append("{\"name\":\"产品经理\", \"alias\":\"产品\"}");

		string rawDataBase64 = encode((const unsigned char*) rawData.c_str(),
												rawData.length());

		Json::Value dataJson;
		dataJson["param"] = dataParamJson;
		dataJson["data"] = rawDataBase64;

		Json::FastWriter writer;

		string dataStr = writer.write(dataJson);
		string params = writer.write(paramsJson);

		Buffer* dataBuffer = Buffer::alloc(dataStr.length());
		dataStr.copy((char*) dataBuffer->data(), dataStr.length());

		IAIUIMessage * syncMsg = IAIUIMessage::create(AIUIConstant::CMD_SYNC,
			AIUIConstant::SYNC_DATA_SCHEMA, 0, params.c_str(), dataBuffer);

		agent->sendMessage(syncMsg);

		syncMsg->destroy();
	}
}

//查询数据同步状态
void AIUITester::querySyncStatus()
{
	if (NULL != agent)
	{
		Json::Value paramJson;

		//参数包含需要对应同步上传操作的sid
		cout << "query sid is " << mSyncSid << endl;
		paramJson["sid"] = mSyncSid;

		Json::FastWriter writer;
		string paramStr = writer.write(paramJson);

		IAIUIMessage * querySyncMsg = IAIUIMessage::create(AIUIConstant::CMD_QUERY_SYNC_STATUS,
			AIUIConstant::SYNC_DATA_SCHEMA, 0, paramStr.c_str(), NULL);

		agent->sendMessage(querySyncMsg);

		querySyncMsg->destroy();
	}
}

//语音单独合成示例，开始合成
void AIUITester::startTts()
{
	if (NULL != agent)
	{
		string text = "科大讯飞作为中国最大的智能语音技术提供商，在智能语音技术领域有着长期的研究积累，"
				"并在中文合成、语音识别、口语评测等多项技术上拥有国际领先的成果。科大讯飞是我多唯一以语音技"
				"术为产业化方向的“国家863计划成果产业基地”。";

		Buffer* textData = Buffer::alloc(text.length());
		text.copy((char*) textData->data(), text.length());

		/*
		 arg1取值说明：
			START	开始合成	合成发音人，语速语调等
			CANCEL	取消合成	无
			PAUSE	暂停播放	无
			RESUME	恢复播放	无
		*/

		/*
		合成参数示例：
		String params = "vcn=xiaoyan,speed=50,pitch=50,volume=50"

		参数字段说明：
			vcn	发音人，如xiaoyan
			speed	速度，0-100
			pitch	语调，0-100
			volume	音量，0-100
			ent	引擎，默认aisound，如果需要较好的效果，可设置成xtts
		*/
		string paramStr = "vcn=xiaoyan";
		paramStr += ",speed=50";
		paramStr += ",pitch=40";
		paramStr += ",volume=60";

		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::START, 0, paramStr.c_str(), textData);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

//暂停本次合成
void AIUITester::pauseTts()
{
	if (NULL != agent)
	{
		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::PAUSE, 0, "", NULL);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

//继续暂停的合成
void AIUITester::resumeTts()
{
	if (NULL != agent)
	{
		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::RESUME, 0, "", NULL);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

//取消本次合成
void AIUITester::cancelTts()
{
	if (NULL != agent)
	{
		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::CANCEL, 0, "", NULL);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

void AIUITester::destory()
{
	stopWriteThread();

	if (NULL != agent)
	{
		agent->destroy();
		agent = NULL;
	}
}

//接收用户输入命令，调用不同的测试接口
void AIUITester::readCmd()
{
	showIntroduction(false);

	cout << "input cmd:" << endl;

	string tmp;
	string cmd;
	while (true)
	{
		cin >> cmd;

		if (cmd == "c")
		{
			cout << "createAgent" << endl;
			createAgent();
		} else if (cmd == "w") {
			cout << "wakeup" << endl;
			wakeup();
		} else if (cmd == "s") {
			cout << "start" << endl;
			start();
		} else if (cmd == "st") {
			cout << "stop" << endl;
			stop();
		} else if (cmd == "d") {
			cout << "destroy" << endl;
			destory();
		} else if (cmd == "r") {
			cout << "reset" << endl;
			reset();
		} else if (cmd == "e") {
			cout << "exit" << endl;
			break;
		} else if (cmd == "wr") {
			cout << "write" << endl;
			write(false);                         //写入测试音频
		} else if (cmd == "wrr") {
			cout << "write repeatly" << endl;
			write(true);
		} else if (cmd == "swrt") {
			cout << "stopWriteThread" << endl;
			stopWriteThread();
		} else if (cmd == "wrt") {
			cout << "writeText" << endl;
			writeText(tmp);                        //文本语义测试接口
			// writeText();                        //文本语义测试接口
		} else if (cmd == "q") {
			destory();
			break;
		} else if (cmd == "ssm") {
			cout << "syncSchema" << endl;
			syncSchema();
		} else if (cmd == "qss") {
			cout << "querySyncStatus" << endl;
			querySyncStatus();
		} else if (cmd == "stts") {
			cout << "startTts" << endl;
			startTts();
		} else if (cmd == "ptts") {
			cout << "pauseTts" << endl;
			pauseTts();
		} else if (cmd == "rtts") {
			cout << "resumeTts" << endl;
			resumeTts();
		} else if (cmd == "ctts") {
			cout << "cancelTts" << endl;
			cancelTts();
		} else if (cmd == "help") {
			showIntroduction(true);
		} else {
			cout << "invalid cmd, input again." << endl;
		}
	}
}

void AIUITester::test()
{
	AIUISetting::setAIUIDir(TEST_ROOT_DIR);
	AIUISetting::initLogger(LOG_DIR);

	readCmd();
}
