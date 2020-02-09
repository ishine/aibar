#ifndef _DEMO_H
#define _DEMO_H

#include <limits.h>
#include <string.h>
#include <string>
using namespace std;
/****** msgFromROS Json format **********
*	{
*	  "command":"idle",
*	  "name":"甘坤",
*	  "gender":"male",
*	  "age":"23",
*	}
*****************************************/
class MsgFromROS
{

public:
	const char* keyCmd    = "\"command\":\""; 
	const char* keyName   = "\"name\":\"";
	const char* keyGender = "\"gender\":\"";
	const char* keyAge    = "\"age\":\"";

	char cmd[20]    = "idle";
	//format: name--user_id
	char name[100]   = "";
	char gender[20] = "";
	char age[20]    = "";	
	char Json[PIPE_BUF+1] = "";

	void extract_Json(const char* key, char* Json, char* dest);
};



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
class MsgToROS
{
public:

	string name 			= "";
	string emotion;
	bool greetingDone     	= false;
	string speechRecoResult = "";

	//orderInfo:
	string DrinkName = "";
	string CupNum    = "";
	string CupType   = "";
	string Temp      = "热的";
	bool OrderFinish = false;

	char Json[PIPE_BUF+1] = "";

	void generate_Json(string name, string emotion, bool greetingDone,string speechRecoResult, string DrinkName, string CupNum, string CupType, string Temp, bool OrderFinish, char* destination);

};


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
class MsgTofronEnd
{
public:

	string pageStatus = "idle"; // ROS中做逻辑判断
	string name     = "";
	string age    	= "";
	string gender   = "";

	string speechRecoResult = "";
	string DrinkName        = "";
	string CupNum           = "";
	string CupType          = "大杯";
	string Temp             = "热的";
	string OrderFinish      = "false";

	char Json[PIPE_BUF+1];

	void generate_Json(string pageStatus, string name, string gender, string age, string speechRecoResult, string DrinkName, string CupNum, string CupType, string Temp, string orderFinish, char* destinaton);
};


// struct OrderFlowControl
// {

// 	bool AIUI_Done = false;
// 	bool SpeakDone = false;
	
// };



#endif
