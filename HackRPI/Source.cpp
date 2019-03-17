#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <sstream>
#include <map>
#include <CommCtrl.h>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <vector>
#include "PasswordGenerator.h"

#define IDC_MAIN_EDIT	101

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
HWND titleBox;
HWND usernameBox;
HWND numWordsSelector;
HINSTANCE hInst;

std::string currentLang = "english";
std::wstring currentTitle;
std::wstring currentUsername;
std::wstring currentPass;

struct Account {
	std::wstring title;
	std::wstring username;
	std::wstring password;
};
std::vector<Account> accounts;


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

std::string WidestringToString(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

std::wstring StringToWidestring(const std::string &str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

TCHAR Languages[12][11] =
{
	TEXT("English"), TEXT("French"), TEXT("Italian"), TEXT("Spanish"),
	TEXT("German"), TEXT("Portuguese"), TEXT("Polish"), TEXT("Dutch"),
	TEXT("Finnish"), TEXT("Danish"), TEXT("Norwegian")
};

void initColourScheme() {
	COLORREF lightBlue = 0xffa670;
	COLORREF midBlue = 0xf48642;
	COLORREF darkBlue = 0xfc271b;
	COLORREF midCharcoal = 0x3d3d3d;
	COLORREF darkCharcoal = 0x262626;
	COLORREF accentGold = 0x49f2ff;
	COLORREF white = 0xf2f2f2;

	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"lightBlue", lightBlue));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"midBlue", midBlue));
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"darkBlue", darkBlue));
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

HFONT setFont(HDC hdc, long fontSize, LPCWSTR fontChoice, UINT weight, bool isItalic) {
	HFONT hf;

	hf = CreateFont(fontSize, 0, 0, 0, weight, FALSE, isItalic, 0, 0, 0, 0, 0, 0, fontChoice);
	SelectObject(hdc, hf);
	return hf;
}

HWND createLanguageSelector() {
	int xpos = 300;            // Horizontal position of the window.
	int ypos = 100;            // Vertical position of the window.
	int nwidth = 250;          // Width of the window
	int nheight = 300;         // Height of the window

	HWND hWndComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		xpos, ypos, nwidth, nheight, parentWindow, NULL, hInst,
		NULL);
	setFont(hWndComboBox, defaultFontType);
	TCHAR A[24];
	int i = 0;
	memset(&A, 0, sizeof(A));
	for (i = 0; i <= 10; i += 1)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)Languages[i]);
		// Add string to combobox.
		SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	return hWndComboBox;
}

HWND createTitleBox() {
	int x = 300;
	int y = 165;
	int w = 250;
	int h = 30;

	HWND hw = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		x, y, w, h, parentWindow, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
	setFont(hw, defaultFontType);
	return hw;
}

HWND createUsernameBox() {
	int x = 300;
	int y = 230;
	int w = 250;
	int h = 30;

	HWND hw = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		x, y, w, h, parentWindow, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
	setFont(hw, defaultFontType);
	return hw;
}

std::wstring getTextFromTextBox(HWND tBox) {
    int length = GetWindowTextLengthW(tBox);
    std::wstring buf (length+1, '\000'); // + 1 for extra null GetWindowTextW wants to write
    GetWindowTextW(tBox, buf.data(), buf.size());
    buf.resize(length); // throw away extra null, std::(w)string already has one
    return buf;
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

	titleBox = createTitleBox();
	usernameBox = createUsernameBox();

	genPasswordButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"Generate",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		150,         // x position 
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
//  WM_CREATE   - Create the window
//  WM_COMMAND  - React to button press, events, etc
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
	{
		hdc = BeginPaint(hWnd, &ps);
		setFont(hdc, 45, defaultFontType, FW_DONTCARE, false);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, colourScheme.at(L"white"));
		TextOut(hdc,
			534, 20,
			frameTitle, _tcslen(frameTitle));


		SetTextColor(hdc, colourScheme.at(L"lightBlue"));
		setFont(hdc, 28, defaultFontType, FW_THIN, false);

		TextOut(hdc,
			150, 100,
			L"Language", _tcslen(L"Language"));

		TextOut(hdc,
			150, 164,
			L"Title", _tcslen(L"Title"));

		TextOut(hdc,
			150, 230,
			L"Username", _tcslen(L"Username"));

		int iter = 0;
		for (Account acc : accounts) {
			OutputDebugString(L"Working");
			std::wstring txt = acc.title + _T(" - ") + acc.username.c_str() + L": " + acc.password.c_str();
			TextOut(hdc,
				650, 94 + iter * 40,
				txt.c_str(), _tcslen(txt.c_str()));
			iter++;
		}
		// End application-specific layout section.

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
		if ((HWND)lParam == genPasswordButton) {
			Account acc;
			acc.title = getTextFromTextBox(titleBox);
			acc.username = getTextFromTextBox(usernameBox);
			acc.password = genPass(30, currentLang);;
			accounts.push_back(acc);

			for (Account a : accounts) {
				OutputDebugString(a.title.c_str());
			}

			InvalidateRect(hWnd, 0, TRUE);
		}
		else if ((HWND)lParam == languageSelector && HIWORD(wParam) == CBN_SELCHANGE) {
			int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			TCHAR  ListItem[256];
			(TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
				(WPARAM)ItemIndex, (LPARAM)ListItem);

			int length = 12;
			std::wstring wChoice((LPCWSTR)ListItem);
			std::string choice = WidestringToString(wChoice);
			std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
			currentLang = choice;
		}
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