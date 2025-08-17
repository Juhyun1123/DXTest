#include "stdafx.h"
#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Init()
{
	//memset()
}

bool InputClass::GetKey(unsigned int keyCode) 
{ 
	return keyStates[keyCode].isKeyDown; 
}

bool InputClass::GetKeyDown(unsigned int keyCode) 
{ 
	return !keyStates[keyCode].previouseKeyDown && keyStates[keyCode].isKeyDown; 
}

bool InputClass::GetKeyUp(unsigned int keyCode) 
{ 
	return keyStates[keyCode].previouseKeyDown && !keyStates[keyCode].isKeyDown; 
}

void InputClass::ProcessInput()
{
	for (int i = 0; i < BINDING_KEY_COUNT; ++i) { keyStates[i].isKeyDown = (GetAsyncKeyState(i) & 0x8000); }
}

void InputClass::SavePreviouseKeyStates()
{
	for (int i = 0; i < BINDING_KEY_COUNT; ++i) { keyStates[i].previouseKeyDown = keyStates[i].isKeyDown; }
}