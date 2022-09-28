#include "systemclass.h"

//��������� null������ ���� Shutdown�Լ��� �ʱ�ȭ���� �ʾҴµ� �ʱ�ȭ�Ǿ��ٰ� �Ǵ��ϰ� ��ü ��ȯ�� �����ʰ� �ϱ� ���� 
SystemClass::SystemClass() {
	m_Input = 0;
	m_Graphics = 0;
	m_Fps = 0; 
	m_Cpu = 0; 
	m_Timer = 0;
}

//������ ��������� ������ ��������� ���� �ڵ�
SystemClass::SystemClass(const SystemClass& other) {}

//�Ϻ� Window�Լ��� �Ҹ��ڸ� ����� �Ҹ��Ű�� ���� -> Shutdown�Լ����� �Ҹ��ų ��
SystemClass::~SystemClass() {}


//Initialize() �Լ��� APP�� �ʱ�ȭ�۾��� �����Ѵ�.
//InitializeWindows �Լ��� ȣ���Ͽ� ���÷����̼ǿ� ����� �����츦 �����Ѵ�.
//���� m_Input�� m_Graphhics ��ü�� �ʱ�ȭ�Ͽ� ������ �Է��� �޾Ƶ��̰� ȭ�鿡 �׷����� �׸� �� �ֵ��� �Ѵ�. 
bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	screenWidth = 0;
	screenHeight = 0;

	// �����츦 �ʱ�ȭ��Ų��.
	InitializeWindows(screenWidth, screenHeight);


	//Input Class�� �ʱ�ȭ��Ų��.
	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}
	m_Input->Initialize();


	//Graphics Class�� �ʱ�ȭ��Ų��.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) {
		return false;
	}
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	if (!result) {
		return false;
	}

	// FPS ��ü ����
	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}

	// FPS ��ü �ʱ�ȭ
	m_Fps->Initialize();

	// CPUClass ��ü ����
	m_Cpu = new CpuClass; 
	if (!m_Cpu) { 
		return false; 
	}

	// CPUClass ��ü �ʱ�ȭ
	m_Cpu->Initialize();

	// Ÿ�̸� ���� ����
	m_Timer = new TimerClass;
	if (!m_Timer){
		return false;
	}

	// Ÿ�̸� ��ü �ʱ�ȭ
	result = m_Timer->Initialize();
	if (!result){
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{
	// Ÿ�̸� ��ü��ȯ
	if (m_Timer){
		delete m_Timer;
		m_Timer = 0;
	}

	// CPU ��ü ��ȯ
	if (m_Cpu){
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// FPS ��ü��ȯ
	if (m_Fps){
		delete m_Fps;
		m_Fps = 0;
	}
	// Graphics ��ü ��ȯ 
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Input ��ü�� ��ȯ�Ѵ�.
	if (m_Input) {
		delete m_Input;
		m_Input = 0;
	}

	// Window â�� �����Ų��.
	ShutdownWindows();

	return;
}

//Run() �Լ��� ���α׷��� ����� ������ ������ ���鼭 ���ø����̼��� ��� �۾��� ó���Ѵ�.
//���ø����̼��� ��� �۾��� �������� �Ҹ��� Frame() �Լ����� ����ȴ�. �̰��� ���ø����̼��� �ٸ� �κ��� �ۼ��Ҷ��� ����ؾ��ϴ� �߿��� �����̴�. 
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//�޼��� ����ü�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done) {   //���α׷��� ����Ǿ���?
		// ������ �ý��� �޼����� Ȯ��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Ȯ���� �޼����� ó���Ѵ�. ���� ��û �޼����� �������� ��������
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			//  �� ���� �޼����� Frame()�Լ��� ó���Ѵ�.
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}
	return;
}

//Frame()�Լ��� ���ø����̼��� ��� �۾��� ó���Ǵ� ���̴�.
//���� ���ø����̼��� �Ը� Ŀ���ٸ� �� �κе� ���� Ŀ���� �ȴ�.
bool SystemClass::Frame()
{
	bool result;

	// ���� ������ ��ü�� �� �����Ӹ��� �ڽ��� ������ �Լ��� ȣ���Ѵ�.
	// �� ������� GraphicsClass�� ������ ���� �׷ó���.
	m_Timer->Frame(); 
	m_Fps->Frame();
	m_Cpu->Frame();

	// m_Input ������ ESC�� Ȯ���Ѵ�.
	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	// Graphcis��ü�� �۾��� ó���Ѵ�.
	result = m_Graphics->Frame(m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime());
	if (!result) {
		return false;
	}

	// �׷��� �������� �����Ѵ�.
	result = m_Graphics->Render();
	if (!result) {
		return false;
	}

	return true;
}


//MessageHandelr �Լ��� �������� �ý��� �޼����� ���޵Ǵ� ���̴�. �̷��� ������ �츮�� �����ִ� �� ���� �������� ���� �� �ִ�.
//���� �츮�� Ű�� ��������, ���������� �� �� �ְ� �� ������ m_Input�� �����Ѵ�. �ٸ� �������� �⺻ �޼��� ó���⿡ �����Ѵ�.
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg) {
		// Ű���尡 ���ȴ����� Ȯ���Ѵ�.
	case WM_KEYDOWN: {
		// Ű�� ���ȴٸ� input��ü�� �� ����� �����Ͽ� ����ϵ��� �Ѵ�. 
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

				   // Ű������ ���� Ű�� ���������� Ȯ���Ѵ�.
	case WM_KEYUP: {
		// Ű�� �������ٸ� input��ü�� �� ����� �����Ͽ� ����ϵ��� �Ѵ�.
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

				 // �ٸ� �޼������� ������� �����Ƿ� �⺻ �޼��� ó���⿡ �����Ѵ�.
	default: {
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

//������ â�� ���� ������ ������ ���� - ������ ��� - ������ ���� - ������ ���
//InitializeWindows �Լ��� �츮�� ������ �ϰ� �� �����츦 ����� �ڵ尡 ����. 
//�� �Լ��� ȣ���� �Լ�(Initialize)���Է� screenWidth�� ScreenHeight ���� �ٽ� �ǵ����ֹǷ� �� ���� ���ø����̼ǿ��� Ȱ���� �� �ִ�.
//����� ��İ� �ƹ� ��輱�� ���� �⺻ ���� �����츦 �����. 
//����� FULL_SCREEN�� ���� ���� ���� �����찡 �ɼ���, �ƴϸ� Ǯ ��ũ���� �����쵵 ���� �� �ִ�.(false�� 800x600�� â�� ������� ȭ�� ��� ��ġ�ȴ�.)
//FULL SCREEN ���������� graphicsclass.h ���� ������ִ�.
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// �ܺ� �����͸� �� ��ü�� �����Ѵ�.
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����´�.
	m_hinstance = GetModuleHandle(NULL);

	// ���ø����̼��� �̸��� �����Ѵ�.
	m_applicationName = L"Engine";

	// ������ Ŭ������ �⺻ �������� �����.
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

	// ������ Ŭ������ ����Ѵ�. 
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �˾ƿ´�.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Ǯ��ũ�� ��� ������ ���� ���� ȭ���� �����Ѵ�.
	if (FULL_SCREEN) {
		// ���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ�鿡 ���߰� ������ 32bit�� �����Ѵ�.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ���� �´� ���÷��̸� �����Ѵ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// �������� ��ġ�� ȭ���� ���� ���� �����.
		posX = posY = 0;
	}
	else {
		// ������ ����� 800x600�� ũ�⸦ ������ �Ѵ�.
		screenWidth = 800;
		screenHeight = 600;

		// â�� ����� �߾ӿ� �������Ѵ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ������ ���� ������ â�� ����� �� �ڵ��� �����´�.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//�����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ش�.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ���콺 Ŀ���� ǥ������ �ʴ´�.
	ShowCursor(false);

	return;
}

//ShutdownWindow()�� ȭ�� ������ �ǵ����� �� �ڵ���� ��ȯ�Ѵ�.
void SystemClass::ShutdownWindows() {
	// ���콺 Ŀ���� ǥ���Ѵ�.
	ShowCursor(true);

	// Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲ۴�.
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����Ѵ�.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���ø����̼� �ν��Ͻ��� �����Ѵ�.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �� Ŭ������ ���� �ܺ� ������ ������ �����Ѵ�.
	ApplicationHandle = NULL;

	return;
}

//WndProc �Լ��� ������ �ý��ۿ��� �޼����� ������ ���̴�.
//�̴� SystemClass���� �������� �޼����� ����ä�� ������ �ϰ�, �̰����� �ڵ� ���� �����ϰ� ������ش�.
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage) {
	//�����찡 ���ŵǾ����� Ȯ���Ѵ�.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �����찡 �������� Ȯ���Ѵ�.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �ٸ� ��� �޼������� systemŬ������ �޼��� ó���⿡ �����Ѵ�.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}