#include "encoder.h"

#define SetBit(port, bit) port|= (1<<bit)
#define ClearBit(port, bit) port&= ~(1<<bit)

//��� ��� ����������� ����
#define b00000011 3
#define b11010010 210
#define b11100001 225

//������� �������������
//__________________________________________
void ENC_InitEncoder(void)
{
	ClearBit(DDR_Enc, Pin1_Enc); //����
	ClearBit(DDR_Enc, Pin2_Enc);
	SetBit(PORT_Enc, Pin1_Enc);//��� ������������� ��������
	SetBit(PORT_Enc, Pin2_Enc);
    ClearBit(DDR_EncT, Pin1_EncT); //����
    ClearBit(DDR_EncT, Pin2_EncT);
    SetBit(PORT_EncT, Pin1_EncT);//��� ������������� ��������
    SetBit(PORT_EncT, Pin2_EncT);
}

//������� ������ ��������
//___________________________________________
unsigned char ENC_PollEncoder(void)
{
	static unsigned char stateEnc; 	//������ ������������������ ��������� ��������
	unsigned char tmp;  
	unsigned char currentState = 0;

	//��������� ��������� ������� ����������������
	if ((PIN_Enc & (1<<Pin1_Enc))!= 0) {SetBit(currentState,0);}
	if ((PIN_Enc & (1<<Pin2_Enc))!= 0) {SetBit(currentState,1);}

	//���� ����� �����������, �� �������
	tmp = stateEnc;
	if (currentState == (tmp & b00000011)) return 0;

	//���� �� �����, �� �������� � ��������� � ���
	tmp = (tmp<<2)|currentState;
	stateEnc = tmp;

	//���������� ������������ ������������������
	if (tmp == b11100001) return LEFT_SPIN;
	if (tmp == b11010010) return RIGHT_SPIN;
	return 0;
}


unsigned char ENC_PollEncoderT(void)
{
	static unsigned char stateEncT; 	//������ ������������������ ��������� ��������
	unsigned char tmp;
	unsigned char currentState = 0;

	//��������� ��������� ������� ����������������
	if ((PIN_EncT & (1<<Pin1_EncT))!= 0) {SetBit(currentState,0);}
	if ((PIN_EncT & (1<<Pin2_EncT))!= 0) {SetBit(currentState,1);}

	//���� ����� �����������, �� �������
	tmp = stateEncT;
	if (currentState == (tmp & b00000011)) return 0;

	//���� �� �����, �� �������� � ��������� � ���
	tmp = (tmp<<2)|currentState;
	stateEncT = tmp;

	//���������� ������������ ������������������
	if (tmp == b11100001) return LEFT_SPIN;
	if (tmp == b11010010) return RIGHT_SPIN;
	return 0;
}

