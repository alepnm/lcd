#ifndef _BUTTON_H
#define _BUTTON_H


#define NUMKEYMAX 	32

typedef struct 
{	
	unsigned char   bsHoldCounter	: 8;	// ��� �������� �����������
	unsigned char   bsJitter		: 2;		// ��� ���������� ��������
	
										// ���������
	unsigned bsPressed 				:1;		// ��������� ������
	unsigned bsHold					:1;		// ��������� ����������� �����������
	unsigned bsShort				:1;		// �������� ������� � ����������
	unsigned bsJustPressed			:1;		// ������ ������ ��� ������
	unsigned bsJustReleased			:1;		// ������ ������ ��� ��������
	unsigned bsChanged				:1;		// ��������� ������ ����������
		
									// �������:

} TBUTTON;

extern void buttonControl(TBUTTON *, unsigned int _pKeyIn, unsigned int _numKey );
extern void initButton(TBUTTON * _but, unsigned int _numButton);

// ������ �� tester
// http://www.microchip.su/showpost.php?p=76612&postcount=12




#endif
