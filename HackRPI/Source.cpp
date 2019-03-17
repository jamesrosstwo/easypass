#include <objidl.h>
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
#include <gdiplus.h>
#include <fstream>
#include "PasswordGenerator.h"
#include "StringConverter.h"
#include "Account.h"
#include "Blowfish.h"

#define IDC_MAIN_EDIT 101


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 

// The main window class name.
static TCHAR szWindowClass[] = _T("Source");
static int frameWidth = 1400;
static int frameHeight = 750;
int optionYOffset = 120;
int buttonYOffset = 20;
static std::map<LPCWSTR, COLORREF> colourScheme;
static LPCWSTR defaultFontType = L"Segoe UI";

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("EasyPass v1.0");
HFONT defaultFont;
HWND parentWindow;
HWND genPasswordButton;
HWND savePasswordButton;
HWND languageSelector;
HWND titleBox;
HWND usernameBox;
HWND numWordsSelector;
HWND passwordLengthSelector;

std::vector<HWND> deleteButtons;

HINSTANCE hInst;



std::string currentLang = "english";
std::wstring currentTitle;
std::wstring currentUsername;
std::wstring currentPass;
int passwordLength = 20;

std::vector<Account> accounts;

BLOWFISH * bf;


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR Languages[12][11] =
{
	TEXT("English"), TEXT("French"), TEXT("Italian"), TEXT("Spanish"),
	TEXT("German"), TEXT("Portuguese"), TEXT("Polish"), TEXT("Dutch"),
	TEXT("Finnish"), TEXT("Danish"), TEXT("Norwegian")
};

void addDeleteButton(int yVal) {
	HWND delButton = CreateWindowW(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"X",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		1300,         // x position 
		yVal,        // y position 
		20,        // Button width
		20,        // Button height
		parentWindow,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLong(parentWindow, GWL_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(delButton, WM_SETFONT, (WPARAM)defaultFont, TRUE);
	deleteButtons.push_back(delButton);
}

const std::string address = "C:\\Users\\james.ross\\Desktop\\pass.txt";
std::vector<Account> decrypt(BLOWFISH  enc) {
	std::wifstream fileIn(address);
	std::vector<Account> vec;
	while (fileIn.peek() != std::wifstream::traits_type::eof()) {
		std::wstring next;
		std::getline(fileIn, next);
		std::string s = WidestringToString(next);
		std::string delimiter = " ";
		Account nextAcc;
		size_t pos = 0;
		pos = s.find(delimiter);
		std::string token;
		token = s.substr(0, pos);

		nextAcc.title = StringToWidestring(enc.Decrypt_CBC(token));
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		nextAcc.username = StringToWidestring(enc.Decrypt_CBC(token));
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		nextAcc.password = StringToWidestring(enc.Decrypt_CBC(s));

		vec.push_back(nextAcc);


	}
	return vec;
}

void encrypt(BLOWFISH dec, std::vector<Account> input) {

	std::ofstream fileOut(address);
	for (Account next : input) {
		fileOut << dec.Encrypt_CBC(WidestringToString(next.title));
		fileOut << " ";
		fileOut << dec.Encrypt_CBC(WidestringToString(next.username));
		fileOut << " ";
		fileOut << dec.Encrypt_CBC(WidestringToString(next.password)) << std::endl;
	}

}

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
	colourScheme.insert(std::pair<LPCWSTR, COLORREF>(L"bg", darkCharcoal));
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
	int ypos = optionYOffset;            // Vertical position of the window.
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
	int y = 65 + optionYOffset;
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
	int y = 130 + optionYOffset;
	int w = 250;
	int h = 30;

	HWND hw = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		x, y, w, h, parentWindow, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
	setFont(hw, defaultFontType);
	return hw;
}

HWND createLengthSelector() {
	int x = 300;
	int y = 195 + optionYOffset;
	int w = 250;
	int h = 25;

	long iMin = 4;
	long iMax = 60;
	long defaultPos = 20;

	HWND hw = CreateWindowExW(0, TRACKBAR_CLASS, L"Trackbar Control",
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_BOTTOM,
		x, y, w, h,
		parentWindow, NULL, hInst, NULL);

	SendMessage(hw, TBM_SETRANGE,
		(WPARAM)TRUE,                   // redraw flag 
		(LPARAM)MAKELONG(iMin, iMax));

	SendMessage(hw, TBM_SETTICFREQ,
		(WPARAM)5,
		(LPARAM)0);

	SendMessage(hw, TBM_SETPAGESIZE,
		0, (LPARAM)4);

	//SendMessage(hw, TBM_SETSEL,
	//	(WPARAM)FALSE,                  // redraw flag 
	//	(LPARAM)MAKELONG(iSelMin, iSelMax));

	SendMessage(hw, TBM_SETPOS,
		(WPARAM)TRUE,                   // redraw flag 
		(LPARAM)defaultPos);


	return hw;
}

std::wstring getTextFromTextBox(HWND tBox) {
	int length = GetWindowTextLengthW(tBox);
	std::wstring buf(length + 1, '\000'); // + 1 for extra null GetWindowTextW wants to write
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
	std::wstring mPassword;
	std::wifstream wStream("C:\\Users\\james.ross\\Desktop\\masterpass.txt");
	getline(wStream, mPassword);
	OutputDebugString(mPassword.c_str());
	bf = new BLOWFISH(WidestringToString(mPassword));

	accounts = decrypt(*bf);
	addDeleteButton(optionYOffset - 5 + 2 * 25);


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
	passwordLengthSelector = createLengthSelector();
	int iter = 0;
	for (Account acc : accounts) {
		addDeleteButton(optionYOffset - 5 + iter * 25);
		iter++;
	}

	genPasswordButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"Generate",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		150,         // x position 
		650 - buttonYOffset,         // y position 
		400,        // Button width
		50,        // Button height
		parentWindow,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLong(parentWindow, GWL_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(genPasswordButton, WM_SETFONT, (WPARAM)defaultFont, TRUE);

	savePasswordButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"Save",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		150,         // x position 
		580 - buttonYOffset,         // y position 
		400,        // Button width
		50,        // Button height
		parentWindow,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLong(parentWindow, GWL_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(savePasswordButton, WM_SETFONT, (WPARAM)defaultFont, TRUE);

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
	TCHAR frameTitle[] = _T("EasyPass");
	switch (message)
	{
	case WM_HSCROLL:
	{
		if (LOWORD(wParam) == SB_THUMBPOSITION || LOWORD(wParam) == SB_THUMBTRACK) {
			passwordLength = HIWORD(wParam);
		}
		else if (LOWORD(wParam) == SB_ENDSCROLL) {
			InvalidateRect(hWnd, 0, TRUE);
		}
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		HBRUSH navbarBrush = CreateSolidBrush(colourScheme.at(L"midBlue"));
		RECT navbar;
		navbar.left = 0;
		navbar.right = frameWidth;
		navbar.top = 0;
		navbar.bottom = 80;

		FillRect(hdc, &navbar, navbarBrush);



		HBRUSH lineBrush = CreateSolidBrush(colourScheme.at(L"accentGold"));
		RECT divider;
		divider.top = 100;
		divider.left = 609;
		divider.right = 610;
		divider.bottom = 700;
		FillRect(hdc, &divider, lineBrush);

		setFont(hdc, 45, defaultFontType, FW_DONTCARE, false);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, colourScheme.at(L"white"));
		TextOut(hdc,
			635, 15,
			frameTitle, _tcslen(frameTitle));


		SetTextColor(hdc, colourScheme.at(L"lightBlue"));
		setFont(hdc, 28, defaultFontType, FW_THIN, false);

		TextOut(hdc,
			150, optionYOffset,
			L"Language", _tcslen(L"Language"));

		TextOut(hdc,
			150, 65 + optionYOffset,
			L"Title", _tcslen(L"Title"));

		TextOut(hdc,
			150, 130 + optionYOffset,
			L"Username", _tcslen(L"Username"));

		std::wstringstream wss;
		std::wstring ws;

		wss << passwordLength;
		wss >> ws;
		std::wstring t = L"# of Letters: " + ws;
		TextOut(hdc,
			150, 190 + optionYOffset,
			t.c_str(), _tcslen(t.c_str()));

		SetTextColor(hdc, colourScheme.at(L"accentGold"));

		UINT len = _tcslen(currentPass.c_str());
		if (len == 0) {
			len = 1;
		}
		setFont(hdc, 16 + 200 / len, defaultFontType, FW_THIN, false);
		TextOut(hdc,
			150, 400 + len / 15,
			currentPass.c_str(), len
		);

		SetTextColor(hdc, colourScheme.at(L"white"));
		int iter = 0;
		for (Account acc : accounts) {
			std::wstring txt = acc.title + _T(" - ") + acc.username.c_str() + L": " + acc.password.c_str();
			setFont(hdc, 21, defaultFontType, FW_EXTRALIGHT, false);
			TextOut(hdc,
				650, optionYOffset - 5 + iter * 25,
				txt.c_str(), _tcslen(txt.c_str()));
			iter++;
		}
		// End application-specific layout section.

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
		if ((HWND)lParam == genPasswordButton) {
			currentPass = genPass(passwordLength, currentLang);

			InvalidateRect(hWnd, 0, TRUE);
		}
		else if ((HWND)lParam == savePasswordButton) {
			Account acc;
			std::wstring title = getTextFromTextBox(titleBox);
			std::wstring username = getTextFromTextBox(usernameBox);
			if (title.empty()) {
				MessageBox(parentWindow,
					_T("Missing Title"),
					_T("Password Generation"),
					NULL);
			}
			else if (username.empty()) {
				MessageBox(parentWindow,
					_T("Missing Username"),
					_T("Password Generation"),
					NULL);
			}
			else if (currentPass.empty()) {
				MessageBox(parentWindow,
					_T("Missing Password"),
					_T("Password Generation"),
					NULL);
			}
			else {
				acc.title = getTextFromTextBox(titleBox);
				acc.username = getTextFromTextBox(usernameBox);
				acc.password = currentPass;
				accounts.push_back(acc);
				SetWindowTextW(usernameBox, L"");
				SetWindowTextW(titleBox, L"");
				currentPass = L"";
				InvalidateRect(hWnd, 0, TRUE);
				encrypt(*bf, accounts);
				addDeleteButton(optionYOffset - 5 + (deleteButtons.size() - 1) * 25);
			}
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
		else {
			int i = 0;
			for (HWND dButton : deleteButtons) {
				if ((HWND)lParam == dButton)
					if (i > 0) {
						accounts.erase(accounts.begin() + i - 1);
					}
					else {
						accounts.erase(accounts.begin());
					}
				DestroyWindow(dButton);
				i++;
			}
			deleteButtons.clear();
			int iter = 0;
			for (Account acc : accounts) {
				addDeleteButton(optionYOffset - 5 + iter * 25);
				iter++;
			}
			encrypt(*bf, accounts);
			InvalidateRect(hWnd, 0, TRUE);
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