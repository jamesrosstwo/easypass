#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <sstream>
#include <map>
#include <CommCtrl.h>


// The main window class name.
static TCHAR szWindowClass[] = _T("Source");
static int frameWidth = 1400;
static int frameHeight = 750;
static std::map<LPCWSTR, COLORREF> colourScheme;
static LPCWSTR defaultFontType = L"Segoe UI";

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Mnemonic Passwords");
HFONT defaultFont;
HWND parentWindow;
HWND genPasswordButton;
HWND languageSelector;
HWND numWordsSelector;
HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


TCHAR Language[12][11] =
{
	TEXT("English"), TEXT("French"), TEXT("Italian"), TEXT("Spanish"),
	TEXT("German"), TEXT("Portuguese"), TEXT("Polish"), TEXT("Dutch"),
	TEXT("Dutch"), TEXT("Finnish"), TEXT("Danish"), TEXT("Norwegian")
};

HWND createLanguageSelector() {
	int xpos = 100;            // Horizontal position of the window.
	int ypos = 100;            // Vertical position of the window.
	int nwidth = 200;          // Width of the window
	int nheight = 200;         // Height of the window

	HWND hWndComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		xpos, ypos, nwidth, nheight, parentWindow, NULL, hInst,
		NULL);
	return hWndComboBox;
}

void initColourScheme() {
	COLORREF lightBlue = 0xffa670;
	COLORREF midBlue = 0xf48642;
	COLORREF midCharcoal = 0x3d3d3d;
	COLORREF darkCharcoal = 0x262626;
	COLORREF accentGold = 0x49f2ff;
	COLORREF white = 0xf2f2f2;

	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"lightBlue", lightBlue));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"midBlue", midBlue));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"midCharcoal", midCharcoal));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"darkCharcoal", darkCharcoal));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"accentGold", accentGold));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"bg", midCharcoal));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"white", white));
}

HFONT getFont(LPCWSTR fontChoice) {
	return CreateFont(-17, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, fontChoice);
}

HFONT setFont(HDC hdc, LPCWSTR fontChoice) {
	HFONT hf;
	long lfHeight;
	lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, fontChoice);
	SelectObject(hdc, hf);
	return hf;
}

void setFont(HWND hw, LPCWSTR fontChoice) {
	SendMessage(hw, WM_SETFONT, (WPARAM)getFont(fontChoice), TRUE);
}

HFONT setFont(HDC hdc, long fontSize, LPCWSTR fontChoice, bool isBold, bool isItalic) {
	HFONT hf;
	UINT weight = FW_DONTCARE;
	if (isBold) {
		weight = FW_SEMIBOLD;
	}

	hf = CreateFont(fontSize, 0, 0, 0, weight, FALSE, isItalic, 0, 0, 0, 0, 0, 0, fontChoice);
	SelectObject(hdc, hf);
	return hf;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	initColourScheme();

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(colourScheme.at(L"bg"));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	parentWindow = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		frameWidth, frameHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	setFont(parentWindow, defaultFontType);
	defaultFont = CreateFont(-17, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, defaultFontType);

	languageSelector = createLanguageSelector();

	genPasswordButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"Generate",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		500,         // x position 
		650,         // y position 
		400,        // Button width
		50,        // Button height
		parentWindow,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLong(parentWindow, GWL_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(genPasswordButton, WM_SETFONT, (WPARAM)defaultFont, TRUE);

	if (!parentWindow)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(parentWindow,
		nCmdShow);
	UpdateWindow(parentWindow);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HFONT g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	COLORREF g_rgbText = RGB(0, 0, 0);
	TCHAR frameTitle[] = _T("Mnemonic Password Generator");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		setFont(hdc, 30, defaultFontType, true, false);
		SetBkColor(hdc, colourScheme.at(L"bg"));
		SetTextColor(hdc, colourScheme.at(L"white"));
		TextOut(hdc,
			534,20,
			frameTitle, _tcslen(frameTitle));
		// End application-specific layout section.

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}