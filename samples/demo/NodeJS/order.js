

AIUI.create("v2.1",  async function(aiui,  err){
    var Num = Math.ceil(Math.random()*10);
    var requestObject = aiui.getRequest().getObject();
    var response = aiui.getResponse();
    var updatedIntent = aiui.getUpdatedIntent();
    // 判断请求类型
    var requestType =requestObject.request.type;
    console.log("技能请求类型为:" + requestType);
    if(requestType === "LaunchRequest"){
        // 会话保持活动状态
        response.withShouldEndSession(false);
        response.setOutputSpeech("很高兴又见面啦！");
    } else if(requestType === "IntentRequest"){
        // 会话保持活动状态
        response.withShouldEndSession(false);
        // 获取当前意图名
        intentName = requestObject.request.intent.name;
        console.log("本次意图来自:" + intentName);
        switch(intentName){

            case 'StartSkill':

                response.setOutputSpeech(">>>Entry: Do Nothing！");
                break;
                
            case 'OrderProcess':
                //获取当前对话的填槽状态
                var dialogState= requestObject.request.dialogState;
                //判断填槽状态是否已完成
                if(dialogState != null && dialogState != "COMPLETED") {
                    //系统默认按照开发者在平台填写的信息进行追问反问－>托管
                    // response.addDelegateDirective();
                    var DrinkName = updatedIntent.getSlotValue('DrinkNameSlot');
                    var CupNum = updatedIntent.getSlotValue('CupNumSlot');
                    var Temp = updatedIntent.getSlotValue('TempSlot');

                    // if (DrinkName == null){
                    //  response.addElicitSlotDirective("DrinkNameSlot");
                    //  response.setOutputSpeech(">>>请问你要喝什么饮品呢？");

                    // }
                    // else if (CupNum == null){
                    //  response.addElicitSlotDirective("CupNumSlot");
                    //  response.setOutputSpeech(">>>可以告诉我您要几份吗？");
                        
                    // }
                    // else if (Temp == null){
                    //  response.addElicitSlotDirective("TempSlot");
                    //  response.setOutputSpeech(">>>你喜欢热的还是凉的呢？");
                        
                    // }
                    response.setOutputSpeech(">>>Process: Do Nothing！");

                }else{ 
                    var DrinkName = updatedIntent.getSlotValue('DrinkNameSlot');
                    var CupNum = updatedIntent.getSlotValue('CupNumSlot');
                    var Temp = updatedIntent.getSlotValue('TempSlot');

                    response.setOutputSpeech(">>>Process: Do Nothing！");
                
                    
                }
                break;
            
            case 'EndSignal':
                response.setOutputSpeech(">>>EndSignal: Do Nothing！");  
                response.withShouldEndSession(true);
                break;   

            default:
                response.setOutputSpeech("这是一条来自IntentRequest未知意图的 answer");
                break;
        }
    } else if(requestType === "SessionEndedRequest"){
        response.withShouldEndSession(true);
        response.setOutputSpeech("退出技能！");
    }
    aiui.commit();
})