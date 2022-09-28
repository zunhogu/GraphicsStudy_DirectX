#include "systemclass.h"

//멤버변수를 null값으로 만들어서 Shutdown함수가 초기화되지 않았는데 초기화되었다고 판단하고 객체 반환을 하지않게 하기 위함 
SystemClass::SystemClass() {
	m_Input = 0;
	m_Graphics = 0;
	m_Fps = 0; 
	m_Cpu = 0; 
	m_Timer = 0;
}

//묵시적 복사생성자 방지용 복사생성자 정의 코드
SystemClass::SystemClass(const SystemClass& other) {}

//일부 Window함수는 소멸자를 제대로 소멸시키지 않음 -> Shutdown함수에서 소멸시킬 것
SystemClass::~SystemClass() {}


//Initialize() 함수는 APP의 초기화작업을 진행한다.
//InitializeWindows 함수를 호출하여 어플레케이션에 사용할 윈도우를 생성한다.
//또한 m_Input과 m_Graphhics 객체를 초기화하여 유저의 입력을 받아들이고 화면에 그래픽을 그릴 수 있도록 한다. 
bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	screenWidth = 0;
	screenHeight = 0;

	// 윈도우를 초기화시킨다.
	InitializeWindows(screenWidth, screenHeight);


	//Input Class를 초기화시킨다.
	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}
	m_Input->Initialize();


	//Graphics Class를 초기화시킨다.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) {
		return false;
	}
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	if (!result) {
		return false;
	}

	// FPS 객체 생성
	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}

	// FPS 객체 초기화
	m_Fps->Initialize();

	// CPUClass 객체 생성
	m_Cpu = new CpuClass; 
	if (!m_Cpu) { 
		return false; 
	}

	// CPUClass 객체 초기화
	m_Cpu->Initialize();

	// 타이머 개게 생성
	m_Timer = new TimerClass;
	if (!m_Timer){
		return false;
	}

	// 타이머 객체 초기화
	result = m_Timer->Initialize();
	if (!result){
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{
	// 타이머 객체반환
	if (m_Timer){
		delete m_Timer;
		m_Timer = 0;
	}

	// CPU 객체 반환
	if (m_Cpu){
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// FPS 객체반환
	if (m_Fps){
		delete m_Fps;
		m_Fps = 0;
	}
	// Graphics 객체 반환 
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Input 객체를 반환한다.
	if (m_Input) {
		delete m_Input;
		m_Input = 0;
	}

	// Window 창을 종료시킨다.
	ShutdownWindows();

	return;
}

//Run() 함수는 프로그램이 종료될 때까지 루프를 돌면서 어플리케이션의 모든 작업을 처리한다.
//어플리케이션의 모든 작업은 루프마다 불리는 Frame() 함수에서 수행된다. 이것은 어플리케이션의 다른 부분을 작성할때도 고려해야하는 중요한 개념이다. 
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//메세지 구조체를 초기화한다.
	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done) {   //프로그램이 종료되었나?
		// 윈도우 시스템 메세지를 확인
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 확인한 메세지를 처리한다. 종료 요청 메세지면 루프문을 빠져나감
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			//  그 외의 메세지는 Frame()함수를 처리한다.
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}
	return;
}

//Frame()함수는 어플리케이션의 모든 작업이 처리되는 곳이다.
//만약 어플리케이션의 규모가 커진다면 이 부분도 같이 커지게 된다.
bool SystemClass::Frame()
{
	bool result;

	// 다음 세개의 객체는 매 프레임마다 자신의 프레임 함수를 호출한다.
	// 그 결과값을 GraphicsClass로 보내서 값을 그련낸다.
	m_Timer->Frame(); 
	m_Fps->Frame();
	m_Cpu->Frame();

	// m_Input 값으로 ESC를 확인한다.
	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	// Graphcis객체의 작업을 처리한다.
	result = m_Graphics->Frame(m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime());
	if (!result) {
		return false;
	}

	// 그래픽 렌더링을 수행한다.
	result = m_Graphics->Render();
	if (!result) {
		return false;
	}

	return true;
}


//MessageHandelr 함수는 윈도우의 시스템 메세지가 전달되는 곳이다. 이렇게 함으로 우리는 관심있는 몇 가지 정보들을 들을 수 있다.
//현재 우리가 키를 눌렀는지, 떼었는지를 알 수 있고 이 정보를 m_Input에 전달한다. 다른 정보들은 기본 메세지 처리기에 전달한다.
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg) {
		// 키보드가 눌렸는지를 확인한다.
	case WM_KEYDOWN: {
		// 키가 눌렸다면 input객체에 이 사실을 전달하여 기록하도록 한다. 
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

				   // 키보드의 눌린 키가 떼어졌는지 확인한다.
	case WM_KEYUP: {
		// 키가 떼어졌다면 input객체에 이 사실을 전달하여 기록하도록 한다.
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

				 // 다른 메세지들은 사용하지 않으므로 기본 메세지 처리기에 전달한다.
	default: {
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

//윈도우 창의 생성 순서는 윈도우 설정 - 윈도우 등록 - 윈도우 생성 - 윈도우 출력
//InitializeWindows 함수는 우리가 렌더링 하게 될 윈도우를 만드는 코드가 들어간다. 
//이 함수는 호출한 함수(Initialize)에게로 screenWidth와 ScreenHeight 값을 다시 되돌려주므로 이 값을 어플리케이션에서 활용할 수 있다.
//현재는 까맣고 아무 경계선이 없는 기본 설정 윈도우를 만든다. 
//현재는 FULL_SCREEN의 값에 따라 작은 윈도우가 될수도, 아니면 풀 스크린의 윈도우도 만들 수 있다.(false면 800x600의 창이 만들어져 화면 가운데 위치된다.)
//FULL SCREEN 전역변수는 graphicsclass.h 에서 선언되있다.
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// 외부 포인터를 이 객체로 설정한다.
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스를 가져온다.
	m_hinstance = GetModuleHandle(NULL);

	// 어플리케이션의 이름을 설정한다.
	m_applicationName = L"Engine";

	// 윈도우 클래스를 기본 설정으로 맞춘다.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스를 등록한다. 
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도를 알아온다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 풀스크린 모드 변수의 값에 따라 화면을 설정한다.
	if (FULL_SCREEN) {
		// 만약 풀스크린 모드라면 화면 크기를 데스크톱에 맞추고 색상을 32bit로 설정한다.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이를 설정한다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽 위로 맞춘다.
		posX = posY = 0;
	}
	else {
		// 윈도우 모드라면 800x600의 크기를 가지게 한다.
		screenWidth = 800;
		screenHeight = 600;

		// 창을 모니터 중앙에 오도록한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 설정한 것을 가지고 창을 만들고 그 핸들을 가져온다.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//윈도우를 화면에 표시하고 포커스를 준다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서는 표시하지 않는다.
	ShowCursor(false);

	return;
}

//ShutdownWindow()는 화면 설정을 되돌리고 그 핸들들을 반환한다.
void SystemClass::ShutdownWindows() {
	// 마우스 커서를 표시한다.
	ShowCursor(true);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꾼다.
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거한다.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 어플리케이션 인스턴스를 제거한다.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 이 클래스에 대한 외부 포인터 참조를 제거한다.
	ApplicationHandle = NULL;

	return;
}

//WndProc 함수는 윈도우 시스템에서 메세지를 보내는 곳이다.
//이는 SystemClass에서 윈도우의 메세지를 가로채는 역할을 하고, 이것으로 코드 역시 간단하게 만들어준다.
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage) {
	//윈도우가 제거되었는지 확인한다.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 윈도우가 닫혔는지 확인한다.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 다른 모든 메세지들은 system클래스의 메세지 처리기에 전달한다.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}