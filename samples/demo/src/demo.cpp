
/**
 * @Description: iFlytek: (iat + aiui + offline_tts) demo
 * @Author     : Kun Gan
 * @Date       : Sep. 2019
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>
#include "AIUITest.h"
#include "playWav.h"
#include "demo.h"
// #include "fifo.h"
#include <pthread.h>
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "iat_online.h"
#include "tts_offline.h"
#include "speech_recognizer.h"

#ifdef __cplusplus
} /* extern "C" */	
#endif /* C++ */



using namespace std;
using namespace aiui;

/** audio file path to be played, depends on the setting of SpeechSynthesis() */
const char * wavpath = "demo.wav";  
const char * Greeting[2]  	= {"您好!我是大眼睛点餐助手，您可以对我说, 我要一杯拿铁！",
							   "您好，需要喝点什么呢?"
							  };
const char * gender[2]   			= {"先生",
								       "女士"};

								

const char * Response_2_noAnswer[]   	= 
{
	"抱歉，可以说慢一些吗?",	
	"可以说大声一些吗？",
	"您可以靠近一些说吗？" 
};


const char * Response_2_noSound[]  	= 
{
	"您有在说话吗？",			
	"我都听不清您说的话了",
	"我好像没有听见你说话"
};

// 门禁
#define ENTRANCE_GUARD
#ifdef ENTRANCE_GUARD
/**
 * Range of Temp. considered： 30 ℃ ~ 40 ℃
 */
class EntranceGuard{
public:   
	char Json[512] = "{\"name\":'hebin',\"temperature\":36.9}";
	void generateResp(string& s){
		this->tempProcess();
		if(this->tempFloat <= 37.3)
			s = this->tempStr + this->normalRespTail;
		else
			s = this->tempStr + this->warningRespTail;
	}

private:
	void tempProcess(){
		this->extract_Json(this->keyTemperature, 
					this->Json, 
					this->tempChar);
		this->tempChar2Str();	
	}
	void extract_Json(const char* key, char* json, char* dest){
		char* start = NULL;
		char* end   = NULL;
		/** extract dest from Json */
		if(strstr(json, key)!=NULL){
			start = strstr(json, key);
			start += strlen(key);  
			for(end = start; *end != '\'' && *end!='}' && *end!=',' && *end!='\0'; ++end);
			memcpy(dest, start, end-start);
			dest[end-start] = 0;
		}
	}
	/** Example：tempChar("36.8") -> Chinese(三十六点八摄氏度) */
    void tempChar2Str(){
		this->tempStr = "三十";
		assert(strcmp(this->tempChar,""));
		this->tempFloat = atof(this->tempChar);
		assert(this->tempFloat > 30.0f && this->tempFloat < 40.0f);
		cout << left << setw(16) <<"\n>> tempFloat: " << this->tempFloat << endl;
        int gewei = (int(this->tempFloat * 10) % 100) / 10;
        int xiaoshu = int(this->tempFloat * 10) % 10;
		this->tempStr += digit2Char(gewei) + "点" + digit2Char(xiaoshu) + "摄氏度";
		cout << left << setw(15) << ">> tempStr: " << this->tempStr << endl;
    }
    string digit2Char(int n){
        switch(n)
		{
		case 0: return "零"; break;
        case 1: return "一"; break;
		case 2: return "二"; break;
        case 3: return "三"; break;
        case 4: return "四"; break;
        case 5: return "五"; break; 
        case 6: return "六"; break;
        case 7: return "七"; break;
        case 8: return "八"; break;
        case 9: return "九"; break; 

        default: return "零"; break; 
    	}
	}

private:
	const char* keyTemperature = "\"temperature\":";
	const string normalRespTail = "，体温正常，允许通行！";
	const string warningRespTail = "，体温过高警告！";
    char tempChar[10] = "";
	float tempFloat;
	string tempStr = "";
};
#endif 

int pipe_fd_wr = -1;
int pipe_fd_rd = -1;
int pipe_fd_wr_frontEnd = -1;
#ifdef ENTRANCE_GUARD
int pipe_fd_rd_entry = -1;
EntranceGuard entranceGuard;
#endif 

/*====================================================================================================================*/

#define TO_FRONT_END
// #define FRONT_END_SIMULATION


class MsgFromROS msgFromROS;			/** Msg from ROS */

class MsgToROS msgToROS;				/** Msg to ROS */

class MsgTofronEnd msgTofrontEnd;		/** Msg to frontEnd */

class AIUITester KEVIN;					/** AIUI Agent */

extern class OrderProcessManager OrderProcessManager;

extern bool AIUI_Done;

extern bool SpeakDone;


/****** msgFromROS Json format **********
*	{
*	  "command":"idle",
*	  "name":"甘坤",
*	  "gender":"male",
*	  "age":"23",
*	}
*****************************************/
void MsgFromROS::extract_Json(const char* key, char* json, char* dest){

	char* start = NULL;
	char* end   = NULL;

	/** extract dest from Json */
	if(strstr(json, key)!=NULL){
		start = strstr(json, key);
		start += strlen(key);  

		for(end = start; *end != '\"' && *end!='\0'; ++end);
    	memcpy(dest, start, end-start);
    	dest[end-start] = 0;
	}
}


/******* msgToROS Json format *********
*	{
*	  "name":"甘坤",
*	  "emotion":"puzzled",
*     "greetingDone":"false",
*	  "speechRecoResult":"",
*	  "OrderInfo":
*	   {
*	 	 "DrinkName":"",
*	 	 "CupNum":"",
*	 	 "CupType":"",
*	 	 "Temp":"",
*	 	 "OrderFinish":"0"
*	   } 
*	}
***************************************/
void MsgToROS::generate_Json(string name, string emotion, bool greetingDone,string speechRecoResult, string DrinkName, string CupNum, string CupType, string Temp, bool OrderFinish, char* destination){

	string json;

	json = "{\"name\":\"" + name + "\",\"emotion\":\"" + emotion + "\",\"greetingDone\":\"";

	greetingDone == true ? json += "true" : json += "false";

	json += "\",\"speechRecoResult\":\"" + speechRecoResult + "\",\"OrderInfo\":{\"DrinkName\":\"" + DrinkName + "\",\"CupNum\":\"" + CupNum + "\",\"CupType\":\"" + CupType + "\",\"Temp\":\"" + Temp + "\",\"OrderFinish\":\""; 

	OrderFinish == true ? json += "true\"}}" : json += "false\"}}";	   

	strcpy(destination,json.c_str());	   
}

/**** msgTofrontEnd Json format *****
*	{
*	  "pageStatus":"orderInfo",
*	  "name":"甘坤",
*	  "gender":"male",
*	  "age":"23",
*	  "speechRecoResult":"null",
*	  "OrderInfo":
*	   {
*	 	 "DrinkName":"拿铁"",
*	 	 "CupNum":"1",
*	 	 "CupType":"0",     //0->小杯  1->大杯
*	 	 "Temp":"0",        //0->冰的  1->热的
*	 	 "OrderFinish":"true"
*	   } 
*	}
**************************************/
string _CupNumFormat(string Cnum)
{
		if(Cnum == "一杯") return "1";
		if(Cnum == "两杯") return "2";
		if(Cnum == "三杯") return "3";
		if(Cnum == "四杯") return "4";
		if(Cnum == "五杯") return "5";
		if(Cnum == "六杯") return "6";
		if(Cnum == "七杯") return "7";
		if(Cnum == "八杯") return "8";
		if(Cnum == "九杯") return "9";
		return "1";
}

void MsgTofronEnd::generate_Json(string pageStatus, string name, string gender, string age, string speechRecoResult, string DrinkName, string CupNum, string CupType, string Temp, string orderFinish, char* destinaton){

    string json;

	/** CupNum */

	json = "{\"pageStatus\":\"" + pageStatus + 
			"\",\"name\":\"" + name + 
			"\",\"gender\":\"" + gender + 
			"\",\"age\":\"" + age + 
			"\",\"speechRecoResult\":\"" + speechRecoResult + 
			"\",\"OrderInfo\":{\"DrinkName\":\"" + DrinkName + 
			"\",\"CupNum\":\"" + _CupNumFormat(CupNum) +
			"\",\"CupType\":\"";

	/** CupType */
	if(CupType=="小杯")
		json += "0\",\"Temp\":\""; 
	else if(CupType=="大杯")
		json += "1\",\"Temp\":\""; 
	else
		json += "\",\"Temp\":\""; 
	
	/** Temp */
	if(Temp=="冰的")
		json += "0\",\"orderFinish\":\"" + (orderFinish) + "\"}}"; 
	else if(Temp=="热的")
		json += "1\",\"orderFinish\":\"" + (orderFinish) + "\"}}"; 
	else
		json += "\",\"orderFinish\":\"" + (orderFinish) + "\"}}"; 
	
    strcpy(destinaton,json.c_str());
}


void greetingGen(char * text, MsgFromROS rosmsg)
{
	strcpy(text,"");														/** greeting: empty the textStitching first */
	
		// if(0 != strcmp(rosmsg.name,"") && 0 != strcmp(rosmsg.name,"unknown")
		// 									&& 0!=strcmp(rosmsg.name,"unknown_d"))
		// {

		// 	strcat(text,rosmsg.name);										/** add the name to text */
		// }

	string name = rosmsg.name;
	if (name.find("--") != string::npos)
	{
		string real_name = name.substr(0, name.find("--"));
		if (real_name != "" && real_name.find("unknown") == string::npos)
		{
			strcat(text, real_name.data());
		}
	}
	
	if(0==strcmp(rosmsg.gender,"male"))											/** add the gender to text */
	{									
		strcat(text,gender[0]);
	}
	else if(0==strcmp(rosmsg.gender,"female"))
	{
		strcat(text,gender[1]);
	}
		
}


void _sendMsgToROS(string emotion)
{
	msgToROS.generate_Json(msgFromROS.name, emotion, msgToROS.greetingDone, msgToROS.speechRecoResult, 
					msgToROS.DrinkName, msgToROS.CupNum, msgToROS.CupType, msgToROS.Temp, 
					msgToROS.OrderFinish, msgToROS.Json);

	int res = write(pipe_fd_wr, msgToROS.Json, strlen(msgToROS.Json));
	if(res==-1) cout << "write err!" << endl;
}

/*============================================== ORDER THREAD ====================================================
----------------------------------------------------------------------------------------------------------------*/
void* _order(void* arg){

	char textStitching[512];	
	int n_conversation = 0;
	int n_idle = 0;
	int n_init = 0;
	// cout << "createAgent" << endl;
	KEVIN.createAgent();	
	// cout << "wakeup" << endl;
	KEVIN.wakeup();	

	while(1){
		/*----------------------------------------- WORKING ----------------------------------------------------*/
		if(0==strcmp(msgFromROS.cmd,"working"))
		{
			if(n_conversation==0){														/** First to serve someone */														
				AIUI_Done = false;
				KEVIN.writeText("开始点餐");				 									 /** Automatically start the skill */
				while(AIUI_Done==false);
				
				greetingGen(textStitching, msgFromROS);
				if (n_init == 0){
					strcat(textStitching,Greeting[0]);						
					n_init = 1;
				}
				else 
					strcat(textStitching,Greeting[1]);											/** add the common response text */

				SpeechSynthesis(textStitching);													/** synthesis greeting audio & play */
				playSound(wavpath);

				if(0!=strcmp(msgFromROS.cmd,"working")) 											/** !!!!check the cmd!!!! */
				{										
					/** specific response here */
					continue;
				}
			}
			/*---------------------------------------------------------------------------------------------------------*/
			SpeechRecognition();																/** 1# RECOGNITION PROCESS */
			msgTofrontEnd.speechRecoResult = g_result;	

/*			
			msgToROS.speechRecoResult = g_result;			
			msgToROS.generate_Json(msgFromROS.name, "normal", msgToROS.greetingDone, msgToROS.speechRecoResult, 
							   msgToROS.DrinkName, msgToROS.CupNum, msgToROS.CupType, msgToROS.Temp, 
							   msgToROS.OrderFinish, msgToROS.Json);

			int res = write(pipe_fd_wr, msgToROS.Json, strlen(msgToROS.Json));
			if(res==-1)  cout << "[ ORDER_THREAD ] write to ROS err! " << endl;
*/

			if(0!=strcmp(msgFromROS.cmd,"working")) 											/** !!!!check the cmd!!!! */
			{										
				/** specific response here */
				continue;
			}

			while(SpeakDone == false);															/** wait until speech recognition process finish */

			/*----------------------------------------------------------------------------------------------------------*/
			

			AIUI_Done = false;   							
			KEVIN.writeText(g_result);															/** 2# AIUI PROCESS */
			while(AIUI_Done == false); 															/** wait until AIUI result available */


			/** AIUI_Done == true */
			if( OrderProcessManager.AnswerType == OrderProcessManager::CUSTOM || OrderProcessManager.AnswerType == OrderProcessManager::TURING )
			{	
				SpeechSynthesis((char*)OrderProcessManager.ExtractResult.data());				/** 3# SYNTHESIS PROCESS */
				playSound(wavpath);																/** 4# PLAYING PROCESS */
				if(msgToROS.OrderFinish == true)
				{
					_sendMsgToROS("happy");
					
					msgToROS.greetingDone = false; 												/** reset the flag */
					msgToROS.OrderFinish  = false; 									
					msgTofrontEnd.OrderFinish  = "false"; 									
					
					strcpy(msgFromROS.cmd,"init");												/** order finish, change the cmd to 'init' */
				}
				OrderProcessManager.n_NoSound = 0;
				// OrderProcessManager.n_NoAnswer = 0;
			}
			else if( OrderProcessManager.AnswerType == OrderProcessManager::NO_ANSWER )
			{	
				OrderProcessManager.n_NoAnswer %= (sizeof(Response_2_noAnswer)/sizeof(Response_2_noAnswer[0]));
				SpeechSynthesis(Response_2_noAnswer[OrderProcessManager.n_NoAnswer]);
				playSound(wavpath);
			}
			else if( OrderProcessManager.AnswerType == OrderProcessManager::NO_SOUND)
			{	
				if(OrderProcessManager.n_NoSound % 2 == 0){
					OrderProcessManager.n_NoSound = OrderProcessManager.n_NoSound > 100 ? 0 : OrderProcessManager.n_NoSound;
					SpeechSynthesis(Response_2_noSound[OrderProcessManager.n_NoSound % (sizeof(Response_2_noSound)/sizeof(Response_2_noSound[0]))]);
					playSound(wavpath);	
				}
			}

			n_conversation++;	
		}
		/*------------------------------------------ IDLE/INIT ------------------------------------------------------*/
		else {
			n_conversation = 0;	
			if(0==strcmp(msgFromROS.cmd,"init"))
			{
				n_init = n_init == 0 ? 0 : 1;
				cout << "##__INIT__##" << endl;
			}
			else{
				n_init = 0;
				n_idle = n_idle > 36000 ? 1 : n_idle;
				if(0 == (++n_idle % 10)) cout << "## _IDLE_ ## : " << (n_idle / 600) << " min " << ((n_idle % 600) / 10) << " s" << endl;
			}
			usleep(1000*100);
		}
	}
}


/*=========================================== MSG FROM ROS THREAD ================================================
----------------------------------------------------------------------------------------------------------------*/
void* _extractJsonFromROS(void *arg){

	const char *fifo_name = "/home/aibar/intelligentvoiceordering/myfifo/ros_2_iFlytek";
	int open_mode = O_RDONLY;
	char buffer[PIPE_BUF+1];
	int bytes_read = 0; 

	memset(buffer, '\0', sizeof(buffer));
 
	pipe_fd_rd = open(fifo_name, open_mode);
	if(pipe_fd_rd == -1)   
		cout << " open ros_2_iFlytek err !" << endl;
	else 
		cout << "open ros_2_iFlytek successfully!" << endl;

	while(1){
 		/** read operation blocked, wait until ros:write_end */
		bytes_read = read(pipe_fd_rd, buffer, PIPE_BUF);

		if(bytes_read==-1){
			cout << "read err" << endl;
			close(pipe_fd_rd);

			pipe_fd_rd = open(fifo_name, open_mode);
			if(pipe_fd_rd == -1)   cout << " open err " << endl;
		}
		else if(bytes_read>0){
			buffer[bytes_read] = 0;
			strcpy(msgFromROS.Json,buffer); 
		}
		else
			;

		usleep(1000*10); 

		msgFromROS.extract_Json(msgFromROS.keyCmd, msgFromROS.Json,msgFromROS.cmd);
		// cout << "\t\t\t\t\t\t\t" << "[ 'MSG FROM CORE' ] " << left << setw(10) << "command: " << msgFromROS.cmd << endl;
	
		msgFromROS.extract_Json(msgFromROS.keyName, msgFromROS.Json,msgFromROS.name);
		msgTofrontEnd.name = msgFromROS.name;		
		// cout << "\t\t\t\t\t\t\t" << "[ 'MSG FROM CORE' ] " << left << setw(10) << "name: " << msgFromROS.name << endl;
		
		msgFromROS.extract_Json(msgFromROS.keyGender, msgFromROS.Json,msgFromROS.gender);
		msgTofrontEnd.gender = msgFromROS.gender;
		// cout << "\t\t\t\t\t\t\t" << "[ 'MSG FROM CORE' ] " << left << setw(10) << "gender: " << msgFromROS.gender << endl;
		
		msgFromROS.extract_Json(msgFromROS.keyAge, msgFromROS.Json,msgFromROS.age);
		msgTofrontEnd.age = msgFromROS.age;
		// cout << "\t\t\t\t\t\t\t" << "[ 'MSG FROM CORE' ] " << left << setw(10) << "age: " << msgFromROS.age << endl;
		
	}
}


/*=========================================== MSG TO FRONT END THREAD ================================================
--------------------------------------------------------------------------------------------------------------------*/
#ifdef TO_FRONT_END
void* _writeTofrontEnd(void* arg){

	const char *fifo_name = "/home/aibar/intelligentvoiceordering/myfifo/ros_2_frontEnd";
	int res = 0;
	int open_mode; 

#ifdef FRONT_END_SIMULATION
	open_mode = O_WRONLY;  //O_NONBLOCK 
#else
	open_mode = O_WRONLY | O_NONBLOCK;
#endif

	if(access(fifo_name, F_OK) == -1){
		res = mkfifo(fifo_name, 0777);
		if(res != 0){
			fprintf(stderr, "Could not create fifo %s\n", fifo_name);
			exit(EXIT_FAILURE);
		}
	}
	pipe_fd_wr_frontEnd = open(fifo_name, open_mode);

	if(pipe_fd_wr_frontEnd == -1)    
		cout << "[ WRITE_TO_FRONT_END ] open err " << endl;
	else 
		cout << "[ WRITE_TO_FRONT_END ] open successfully!" << endl;

#ifdef FRONT_END_SIMULATION

	/*------------------------------------------ SIMULATION ------------------------------------------------------*/
		msgTofrontEnd.pageStatus = "idle"; 
		msgTofrontEnd.name       = "甘坤";
		msgTofrontEnd.gender     = "male";
		msgTofrontEnd.age        = "18";

		msgTofrontEnd.DrinkName   = "卡布奇诺";
		msgTofrontEnd.CupNum      = "三杯";
		msgTofrontEnd.Temp        = "热的";
		msgTofrontEnd.CupType     = "大杯";
		msgTofrontEnd.OrderFinish = "true";

		while (1){
			
			/** 无服务对象，处于空闲状态 */
			/** idle:欢迎页面,只需要指定pageStatus即可，其余信息均为空 */
			msgTofrontEnd.generate_Json("idle", "unknown", "unknown", "0", "", "", "", "", "", "", msgTofrontEnd.Json);
			int res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*5000);
			
			/** 服务对象出现，页面状态切换为working，大眼睛语音问候开始,界面显示全部饮品菜单以及推荐饮品 */
			/** working：需发送顾客身份信息以及语音识别内容 */
			msgTofrontEnd.generate_Json("working", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
									"我要一杯卡布奇诺", msgTofrontEnd.DrinkName, "", "", "", "false", msgTofrontEnd.Json);
			res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*4000);
			
			msgTofrontEnd.generate_Json("working", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
									"冰的", "", "", "", "", "false", msgTofrontEnd.Json);
			res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*4000);
			
			msgTofrontEnd.generate_Json("working", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
									"还是换成热的吧", "", "", "", "", "false", msgTofrontEnd.Json);
			res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*4000);
			
			msgTofrontEnd.generate_Json("working", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
									"是的", "", "", "", "", "false", msgTofrontEnd.Json);
			res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*2000);

			/** 顾客确认订单信息后，则显示订单详情 */
			/** orerInfo:发送顾客身份信息，以及订单信息 */
			msgTofrontEnd.DrinkName   = "卡布奇诺";
			msgTofrontEnd.generate_Json("orderInfo", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
									"", msgTofrontEnd.DrinkName, msgTofrontEnd.CupNum, msgTofrontEnd.CupType, 
									msgTofrontEnd.Temp, "false", msgTofrontEnd.Json);
			res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*100);
			msgTofrontEnd.generate_Json("orderInfo", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
									"", msgTofrontEnd.DrinkName, msgTofrontEnd.CupNum, msgTofrontEnd.CupType, 
									msgTofrontEnd.Temp, "true", msgTofrontEnd.Json);
			res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
			cout << "\n================================================================================\n"
				 << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl;
			usleep(1000*5000);
	}
#else

	/*------------------------------------------ STATE  MACHINE ------------------------------------------------------*/
	//temp值初始化 --- DEBUG
	bool isChange = true;
	string pageStatus_temp 	= "idle"; 
	string name_temp      	= "";
	string gender_temp 	 	= "";
	string age_temp    		= "";
	char* g_result_temp     = NULL;

	string speechRecoResult_temp = "";
	string DrinkName_temp        = "";
	string CupNum_temp           = "";
	string CupType_temp          = "大杯";
	string Temp_temp             = "热的";
	string OrderFinish_temp      = "false";
	
	while(1){
		/**
				if(pageStatus_temp != msgTofrontEnd.pageStatus ||
				name_temp != msgTofrontEnd.name ||
				gender_temp != msgTofrontEnd.gender ||
				age_temp != msgTofrontEnd.age ||
				speechRecoResult_temp != msgTofrontEnd.speechRecoResult ||
				DrinkName_temp != msgTofrontEnd.DrinkName ||
				CupNum_temp != msgTofrontEnd.CupNum ||
				CupType_temp != msgTofrontEnd.CupType ||
				Temp_temp != msgTofrontEnd.Temp ||
				OrderFinish_temp != msgTofrontEnd.OrderFinish
				){
					isChange = true;
					cout << "---sth changes---" << endl;
				}   
		*/


		if(pageStatus_temp != msgTofrontEnd.pageStatus ||   \
		   speechRecoResult_temp != msgTofrontEnd.speechRecoResult ||  \
		   DrinkName_temp != msgTofrontEnd.DrinkName ||  \
		   CupNum_temp != msgTofrontEnd.CupNum ||   \
		   CupType_temp != msgTofrontEnd.CupType ||   \
		   Temp_temp != msgTofrontEnd.Temp) 
		{	
			// msgTofrontEnd.speechRecoResult = g_result;
			isChange = true;
		}

		/** PageStatus Transition Control："idle"->"workking"->"orderInfo" */ 
		if(0 != strcmp(msgFromROS.cmd,"working"))
		{
			msgTofrontEnd.pageStatus = "idle";
		}
		else if(msgTofrontEnd.pageStatus == "idle" && 0 == strcmp(msgFromROS.cmd,"working"))
		{
			msgTofrontEnd.pageStatus = "working";	
		}
		else if(msgTofrontEnd.pageStatus == "working" && msgToROS.OrderFinish == true)
		{
			msgTofrontEnd.pageStatus = "orderInfo";
		}

		if(isChange)
		{
			isChange = false;
			pageStatus_temp 		= msgTofrontEnd.pageStatus;
			name_temp 				= msgTofrontEnd.name;
			gender_temp 			= msgTofrontEnd.gender;
			age_temp 				= msgTofrontEnd.age;
			speechRecoResult_temp 	= msgTofrontEnd.speechRecoResult;
			DrinkName_temp 			= msgTofrontEnd.DrinkName;
			CupNum_temp 			= msgTofrontEnd.CupNum;
			CupType_temp 			= msgTofrontEnd.CupType;
			Temp_temp 				= msgTofrontEnd.Temp;
			OrderFinish_temp 		= msgTofrontEnd.OrderFinish;

			/** idle:显示welcompage；只需要指定pageStatus即可，其余信息为空 */
			if(msgTofrontEnd.pageStatus=="idle")
			{
				msgTofrontEnd.generate_Json("idle", "", "", "", "", "", "", "大杯", "热的", "", msgTofrontEnd.Json);
				int res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
				cout << "================================================================================\n" 
				     << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl << endl;
			}

			/** working：显示当日菜单及推荐；需发送顾客身份信息以及语音识别结果 */
			else if(msgTofrontEnd.pageStatus=="working")
			{
				msgTofrontEnd.generate_Json("working", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
								        msgTofrontEnd.speechRecoResult, msgTofrontEnd.DrinkName, msgTofrontEnd.CupNum, 
										msgTofrontEnd.CupType, msgTofrontEnd.Temp, "false", msgTofrontEnd.Json);
				res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
				cout << "================================================================================\n" 
				     << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl << endl;
			}

			/** orerInfo:页面显示订单信息；发送顾客身份信息，以及最终订单信息 */
			else if(msgTofrontEnd.pageStatus=="orderInfo")
			{
				msgTofrontEnd.generate_Json("orderInfo", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age, 
										"", msgTofrontEnd.DrinkName, msgTofrontEnd.CupNum,   
										msgTofrontEnd.CupType, msgTofrontEnd.Temp, "false", msgTofrontEnd.Json);
				res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
				cout << "================================================================================\n" 
				     << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl << endl;

				usleep(1000*100);
				
				msgTofrontEnd.generate_Json("orderInfo", msgTofrontEnd.name, msgTofrontEnd.gender, msgTofrontEnd.age,  
									    "", msgTofrontEnd.DrinkName, msgTofrontEnd.CupNum,   
										msgTofrontEnd.CupType, msgTofrontEnd.Temp, "true", msgTofrontEnd.Json);
				res = write(pipe_fd_wr_frontEnd, msgTofrontEnd.Json, strlen(msgTofrontEnd.Json));
				cout << "================================================================================\n" 
				     << "[ WRITE_TO_FRONT_END ] " << msgTofrontEnd.Json << endl << endl;
			}
			usleep(1000*100);
		}
	}
#endif
}
#endif


/*=========================================== ENTRANCE GUARD THREAD ==================================================
--------------------------------------------------------------------------------------------------------------------*/
#ifdef ENTRANCE_GUARD
void* _getTemperature(void *arg){

	// const char *fifo_name = "../../../myfifo/werewolfLu_2_iFlytek";
	// const char *fifo_name = "../../../myfifo/entryInfo";
	const char *fifo_name = "/home/aibar/tmp/entryInfo";
	int open_mode = O_RDONLY;
	char buffer[PIPE_BUF+1];
	int bytes_read = 0; 
	//Empty the buffer
	memset(buffer, '\0', sizeof(buffer));
	//RD_ONLY & BLOCKING
	cout << "# ENTRY MODE #\n" << "-Opening FIFO: entryInfo ..." << endl;
	pipe_fd_rd_entry = open(fifo_name, open_mode);
	if(pipe_fd_rd_entry == -1)   
		cout << "-open entryInfo err !" << endl;
	else 
		cout << "-open entryInfo successfully!" << endl;

	string Resp;
	while(1){
		
 		//read operation blocked, wait until ros:write_end.
		bytes_read = read(pipe_fd_rd_entry, buffer, PIPE_BUF);

		if(bytes_read==-1){
			cout << "-pipe_fd_rd_entry: read err" << endl;
			close(pipe_fd_rd_entry);
			pipe_fd_rd_entry = open(fifo_name, open_mode);
			if(pipe_fd_rd_entry == -1)   cout << "-pipe_fd_rd_entry: open err " << endl;
		}
		else if(bytes_read>0){
			buffer[bytes_read] = 0;
			strcpy(entranceGuard.Json, buffer); 
		}
		// usleep(1000*10); //10ms 
		cout << ">> Json: " << entranceGuard.Json << endl;

		entranceGuard.generateResp(Resp);
		cout << left << setw(15) << ">> Resp: " << Resp << endl;
		
        SpeechSynthesis((char*)Resp.data());
        playSound(wavpath);
	}
}
#endif


// #define TO_ROS

int main()
{
	string choice;
	// cout << "input choice :" << endl;
	
#ifdef TO_ROS
	/****************** Create FIFO: WR_ONLY & BLOCKING *****************/
	// const char *fifo_name = "/home/kevin/myfifo/iFlytek_2_ros";
	// const char *fifo_name = "../../../../myfifo/iFlytek_2_ros";
	const char *fifo_name = "/home/aibar/intelligentvoiceordering/myfifo/iFlytek_2_ros";
	int res = 0;
	const int open_mode = O_WRONLY;   

	if(access(fifo_name, F_OK) == -1)
	{
		res = mkfifo(fifo_name, 0777);
		if(res != 0)
		{
			fprintf(stderr, "Could not create fifo %s\n", fifo_name);
			exit(EXIT_FAILURE);
		}
	}
	cout << "### Open <iFlytek_2_ros> Blocked ###" << endl;
	/*以只写阻塞方式打开FIFO文件*/
	pipe_fd_wr = open(fifo_name, open_mode);
	cout << "### Open <iFlytek_2_ros> Successfully ###" << endl;

	if(pipe_fd_wr == -1)    
		cout << " Open iFlytek_2_ros err !" << endl;
	else 
		cout << "Open iFlytek_2_ros successfully!" << endl;
	/*******************************************************************/
#endif


	while(1){

		// cin >> choice;

		//default choice: multi-thread
		choice = "multithread";
	

		if(choice=="c"){
			cout << "createAgent" << endl;
			KEVIN.createAgent();
		}
		else if(choice=="w"){
			cout << "wakeup" << endl;
			KEVIN.wakeup();
		}
		else if(choice=="speak"){
			cout << "SpeechRecognition\n" << endl;
			/*Reconition result is saved in  g_result */
			SpeechRecognition();
		}
		else if(choice=="wrt"){
			cout << "writeText to Cloud" << endl;
			KEVIN.writeText("你好!"); //g_result
		}
		else if(choice=="syn"){
			cout << "SpeechSynthesis" << endl;
			SpeechSynthesis((char*)OrderProcessManager.ExtractResult.data());   
		}
		else if(choice=="play"){
			/*filepath of the audio to be played*/  
			playSound(wavpath);   
		}
		else if(choice=="demo"){

			cout << "createAgent" << endl;
			KEVIN.createAgent();	

			cout << "wakeup" << endl;
			KEVIN.wakeup();	

			while(1){

				string cmd = "working";
				// cout << cmd << endl;

				while(cmd=="working"){

					// g_result = "开始点餐";

					SpeechRecognition();
					KEVIN.writeText("开始点餐");

					/* wait until AIUI result available */
					while(AIUI_Done==false);  

					if( OrderProcessManager.AnswerType==OrderProcessManager::CUSTOM || OrderProcessManager.AnswerType==OrderProcessManager::TURING ){
						SpeechSynthesis((char*)OrderProcessManager.ExtractResult.data());
						playSound(wavpath);
					}
					else if( OrderProcessManager.AnswerType==OrderProcessManager::NO_ANSWER ){
						/*common voice response, save to local*/
						// SpeechSynthesis(Response_2_noAnswer); 
						playSound(wavpath);
					}
					else if( OrderProcessManager.AnswerType==OrderProcessManager::NO_SOUND ){
						/*common voice response, save to local*/
						// SpeechSynthesis(Response_2_noSound[1]);;
						playSound(wavpath);
					}
				
				}

			}
			
		}
		else if(choice=="multithread"){
		/*			
			pthread_t orderId, extractJsonFromROSId, writeTofrontEndId;
		*/
#ifdef ENTRANCE_GUARD
            pthread_t getTemperatureId;
#endif
			int ret;
		/*
	        ret = pthread_create(&orderId, NULL, _order, NULL);
	        if (ret != 0) 
	            printf("order pthread_create error: error_code = %d\n", ret);

	        ret = pthread_create(&extractJsonFromROSId, NULL, _extractJsonFromROS, NULL);
	        if (ret != 0) 
	            printf("getCmd pthread_create error: error_code = %d\n", ret);  

	        ret = pthread_create(&writeTofrontEndId, NULL, _writeTofrontEnd, NULL);
	        if (ret != 0) 
	            printf("getCmd pthread_create error: error_code = %d\n", ret);
		*/
#ifdef ENTRANCE_GUARD
            ret = pthread_create(&getTemperatureId, NULL, _getTemperature, NULL);
	        if (ret != 0) 
	            printf("getTemperature pthread_create error: error_code = %d\n", ret);
#endif

	        while(1);
		}

	}

    pthread_exit(NULL);

	return 0;

}

	
	
	
