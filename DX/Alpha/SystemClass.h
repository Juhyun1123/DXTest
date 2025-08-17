#pragma once

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Init();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	InputClass* input;
	GraphicsClass* graphics;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;

#endif