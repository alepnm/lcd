#include "button.h"

void initButton(TBUTTON * _but, unsigned int _numButton)
{
	int i;
	if (_numButton > NUMKEYMAX) _numButton = 0; 
	for (i = 0; i < _numButton; i++)
	{
		_but[i].bsChanged = 0;
		_but[i].bsHoldCounter = 0;
		_but[i].bsHold = 0;
		_but[i].bsJitter = 0;
		_but[i].bsJustPressed = 0;
		_but[i].bsJustReleased = 0;
		_but[i].bsPressed = 0;
		_but[i].bsShort = 0;
	}		
}	

void buttonControl(TBUTTON * _button, unsigned int _pKeyIn, unsigned int _numButton )
{
	unsigned int i;
	if (_numButton > NUMKEYMAX) _numButton = 0; 
	for(i = 0; i < _numButton; i++)
	{
		
		//------------------------------------------------------------------------------
		//  ������ ��� �� ������
		//------------------------------------------------------------------------------
		if (!_button[i].bsPressed) 
		{
			_button[i].bsJitter++;
			// �������� ������� 0
			if ((_pKeyIn & (1<<i))) _button[i].bsJitter = 0;  // ��������� �������
		
			if (_button[i].bsJitter == 3)				// ��������� ����������
			{
				_button[i].bsPressed = 1;			 	// ������
				_button[i].bsJustPressed = 1;		 	// ������ ������ ��� (������������ �������)
				_button[i].bsHold = 0;					// �� ������������
				_button[i].bsShort = 0;			   		// �� �������� �������
				_button[i].bsHoldCounter = 0;
			}   // if jitter
		} 
		else
		{	// if button pressed
		
			//------------------------------------------------------------------------------
			//  ������ ������, ��������� �����������
			//------------------------------------------------------------------------------
		
			_button[i].bsJitter++;
			// �������� ������� 0
			if (!(_pKeyIn & (1<<i))) _button[i].bsJitter = 0; // ��������� �����
		
			if (_button[i].bsJitter == 3)			 // ������ ������ ��� ���������
			{
				_button[i].bsPressed = 0;
				if (!_button[i].bsHold)			   	// ���� �� ���� ����������������� 1 ���,
					_button[i].bsShort = 1;		   	// �� ���������� ���� "��������� �������"
		
				_button[i].bsHold = 0;
				_button[i].bsHoldCounter = 0;
		
			}   // if jitter
		
			_button[i].bsHoldCounter++;
		
			if (_button[i].bsHoldCounter >= 50)	   // ��������� �����������
				_button[i].bsHold = 1;
		
		}   // if button pressed	
	}
}	

