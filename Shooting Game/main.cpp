#include "Scene/SceneMng.h"

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	lpSceneMng.Run();
	return 0;
}

