#include "windows.h"
#include <array>
#include <future>
#include <thread>


const char g_szClassName[] = "myWindowClass";
const int arraySize = 60;
std::array<int, arraySize> unsortedArray;
const int windowHeight = 600;
const int windowWidth = 800;
int drawableHeight;
int drawableWidth;

void drawArraySort(HDC hDC) {
	HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
	HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));

	for (size_t i = 0; i < unsortedArray.size(); i++) {
		int currentValue = unsortedArray[i];

		RECT rect = {
			(drawableWidth / (int)unsortedArray.size())*i,
			drawableHeight - currentValue,
			(drawableWidth / (int)unsortedArray.size())*i + (drawableWidth / (int)unsortedArray.size()),
			drawableHeight
		};

		FillRect(hDC, &rect, blueBrush);
		FrameRect(hDC, &rect, blackBrush);
	}
}

void swap(int *xp, int *yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void bubleSort(HWND windowHandler) {
	for (size_t i = 0; i < unsortedArray.size(); i++){
		for (size_t j = 0; j < unsortedArray.size() - 1; j++){
			if (unsortedArray[j] > unsortedArray[j + 1]) {
				swap(&unsortedArray[j], &unsortedArray[j + 1]);
				RedrawWindow(windowHandler, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
				Sleep(50);
			}
		}
	}
}

void fillArray() {
	int arrayMaxSize;
	for (size_t i = 0; i < unsortedArray.size(); i++) {
		unsortedArray[i] = rand() % 500 + 50;
	}
}

LRESULT CALLBACK WndProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(windowHandler);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
	{
		HDC hDC;
		PAINTSTRUCT ps;
		hDC = BeginPaint(windowHandler, &ps);

		RECT    rcCli;
		GetClientRect(WindowFromDC(hDC), &rcCli);
		drawableWidth = rcCli.right - rcCli.left;
		drawableHeight = rcCli.bottom - rcCli.top;

		drawArraySort(hDC);

		EndPaint(windowHandler, &ps);
		break;
	}
	default:
		return DefWindowProc(windowHandler, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
) {
	fillArray();

	WNDCLASSEX wc;
	HWND windowHandler;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	windowHandler = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Sorting Visualization",
		WS_OVERLAPPED | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);

	if (windowHandler == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(windowHandler, nCmdShow);

	auto future = std::async(bubleSort, windowHandler);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}