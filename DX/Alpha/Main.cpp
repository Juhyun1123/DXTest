#include "stdafx.h"
#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdShow)
{
	SystemClass* system = new SystemClass();

	if (!system)
	{
		return 0;
	}

	if (system->Init())
	{
		system->Run();
	}

	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}
