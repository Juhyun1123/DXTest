#pragma once

#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
	struct KeyState
	{
		bool isKeyDown = false;
		bool previouseKeyDown = false;
	};

public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Init();

	bool GetKey(unsigned int keyCode);
	bool GetKeyDown(unsigned int keyCode);
	bool GetKeyUp(unsigned int keyCode);

private:
	void ProcessInput();
	void SavePreviouseKeyStates();

private:
	const int BINDING_KEY_COUNT = 256;

	KeyState keyStates[256];
	
};

#endif