#include "task.h"
#include <stdio.h>
#include <string.h>
//{"header":{"messageId":6,"messageName":"UPDATE_VALUE","messageType":"IPEK_CHINA_GUI"},"payload":{"value":"50%","what":"laserIntensity"}}
// "{\"header\":{\"messageId\":102,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":{\"angle\":57,\"power\":1},\"what\":\"roverJoystick\"}}"	//�Ҳ���ݸˣ�����С����
//"{\"header\":{\"messageId\":5,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":1,\"what\":\"auxiliaryLights\"}}"	//
char DecodeRECBuff[500] = "{\"header\":{\"messageId\":6,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":\"50%\",\"what\":\"laserIntensity\"}}"; // �������ݴ����
char CodeRECBuff[500] = {0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x58, 0x39, 0x08, 0x40};

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
