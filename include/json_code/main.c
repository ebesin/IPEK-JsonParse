#include "task.h"
#include <stdio.h>
#include <string.h>
//{"header":{"messageId":6,"messageName":"UPDATE_VALUE","messageType":"IPEK_CHINA_GUI"},"payload":{"value":"50%","what":"laserIntensity"}}
// "{\"header\":{\"messageId\":102,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":{\"angle\":57,\"power\":1},\"what\":\"roverJoystick\"}}"	//�Ҳ���ݸˣ�����С����
//"{\"header\":{\"messageId\":5,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":1,\"what\":\"auxiliaryLightsValueInPercent\"}}"	//
//{\"header\":{\"messageId\":36,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":30,\"what\":\"autoAngleMainLightsValueInDegrees\"}}
//{\"header\":{\"messageId\":641,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":1,\"what\":\"cableReelPower\"}}
//{\"header\":{\"messageId\":648,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":-13,\"what\":\"cableReelSpeed\"}}
//{\"header\":{\"messageId\":649,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":\"manual\",\"what\":\"cableReelType\"}}
// {\"header\":{\"messageId\":665,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":{\"angle\":-60.945395900922854,\"power\":0.46330335634441494},\"what\":\"cameraJoystick\"}}
//{\"header\":{\"messageId\":664,\"messageName\":\"ACTION\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"action\":\"reset\",\"what\":\"camera\"}}



//{\"header\":{\"messageId\":701,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":true,\"what\":\"cruiseControlStatus\"}}
//{\"header\":{\"messageId\":704,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":36,\"what\":\"cruiseControlValue\"}}
//{\"header\":{\"messageId\":711,\"messageName\":\"EMERGENCY_STOP\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{}}

//{\"header\":{\"messageId\":50,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":77,\"what\":\"lowBeamMainLightsValueInPercent\"}}

//{\"header\":{\"messageId\":55,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":true,\"what\":\"clutchEnabled\"}}
//{\"header\":{\"messageId\":668,\"messageName\":\"ACTION\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"action\":\"incrementing_started\",\"what\":\"zoom\"}}
//{\"header\":{\"messageId\":707,\"messageName\":\"ACTION\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"action\":\"incrementing_started\",\"what\":\"elevator\"}}
//{"header":{"messageId":673,"messageName":"ACTION","messageType":"IPEK_CHINA_GUI"},"payload":{"action":"incrementing_started","what":"focus"}}
//{\"header\":{\"messageId\":678,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":\"manual\",\"what\":\"focusType\"}}
//{\"header\":{\"messageId\":681,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":\"rear\",\"what\":\"cameraChosen\"}}
//{\"header\":{\"messageId\":682,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":2,\"what\":\"rearCameraIdx\"}}
//{\"header\":{\"messageId\":696,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":{\"angle\":-98.10724803565803,\"power\":1},\"what\":\"roverJoystick\"}}
char DecodeRECBuff[500] = "{\"header\":{\"messageId\":682,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":1,\"what\":\"rearCameraIdx\"}}"; // �������ݴ����
char CodeRECBuff[500] = {0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x58, 0x39, 0x08, 0x40};
/*---------https://www.yuque.com/alipaylextv5b0d6/ps8gp4/kgtemy2mo33s7elo---------------*/
int main()
{
	cJSON *Str_Json;

	Str_Json = cJSON_Parse(DecodeRECBuff); // ����JSON�������󣬷���JSON��ʽ�Ƿ���ȷ

#if DEBUG
	printf("%s \r\n", DecodeRECBuff);
#endif
	Scheduler_Decode(Str_Json);
	Scheduler_Code(CodeRECBuff);
	cJSON_Delete(Str_Json); // �ͷ��ڴ�
}
