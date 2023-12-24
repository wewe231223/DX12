#include "pch.h"
#include "Input.h"
Input* Input::m_instance{ nullptr };


Input::Input() {
}

Input::~Input(){
	if (m_keyboardState) {
		delete[] m_keyboardState;
	}
	m_keyboardState = nullptr;

	if (m_pmouseDevice) {
		m_pmouseDevice->Unacquire();
		m_pmouseDevice->Release();
		m_pmouseDevice = NULL;
	}

	if (m_pkeyDevice) {
		m_pkeyDevice->Unacquire();
		m_pkeyDevice->Release();
		m_pkeyDevice = NULL;
	}


	if (m_pdirectInput) {
		m_pdirectInput->Release();
		m_pdirectInput = NULL;
	}

}

const KEY_STATE Input::GetKey(int key) const
{
	return KEY_STATE();
}

void Input::Init(HWND hWnd, HINSTANCE Instance) {
	m_instance->m_keyboardState = new KEY_STATE[256];

	for (auto i = 0; i < 256; ++i) {
		m_instance->m_keyboardState[i] = KEY_STATE::NONE;
	}





	HRESULT hr = S_OK;

	if (FAILED(hr = DirectInput8Create(Instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pdirectInput, NULL))) {
		printf("FAILED : Cannot Initialize DirectInput\n");
		exit(-1);
	}


	if (FAILED(hr = m_pdirectInput->CreateDevice(GUID_SysKeyboard, &m_pkeyDevice, NULL))) {
		printf("FAILED : Cannot Initialize Keyboard Device\n");
		exit(-1);
	}
	m_pkeyDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr = m_pkeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY))) {
		printf("FAILED : Cannot Initialize Keyboard Device\n");
		exit(-1);
	}
	while (m_pkeyDevice->Acquire() == DIERR_INPUTLOST);


	if (FAILED(hr = m_pdirectInput->CreateDevice(GUID_SysMouse, &m_pmouseDevice, NULL))) {
		printf("FAILED : Cannot Initialize Mouse Device\n");
		exit(-1);
	}
	m_pmouseDevice->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr = m_pmouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ))) {
		printf("FAILED : Cannot Initialize Mouse Device\n");
		exit(-1);
	}
	while (m_pmouseDevice->Acquire() == DIERR_INPUTLOST);
}


Input* Input::GetInstance(){
	if (m_instance == nullptr) {
		m_instance = new Input();
	}

	return m_instance;
	
}


void Input::Update() {


	BYTE Keystate[256]{};
	DIMOUSESTATE Mousestate{};
	HRESULT hr{};

	if (FAILED(hr = m_pkeyDevice->GetDeviceState(256, Keystate))) {
		while (m_pkeyDevice->Acquire() == DIERR_INPUTLOST);
	}

	if (FAILED(hr = m_pmouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &Mousestate))) {
		while (m_pmouseDevice->Acquire() == DIERR_INPUTLOST);
	}



	if (Keystate[DIK_ESCAPE] & 0x80) {
		ProgramClose();
	}







	//BYTE temp[256]{ };
	//if (!GetKeyboardState(temp)) {
	//	exit(EXIT_FAILURE);
	//}

	//if (temp[VK_ESCAPE] & 0x80) {
	//	ProgramClose();
	//}


	// 


	//for (auto i = 0; i < 256; ++i) {
	//	if (temp[i] & 0x80) {
	//		if (m_keyboardState[i] == KEY_STATE::NONE or m_keyboardState[i] == KEY_STATE::RELEASE) {
	//			m_keyboardState[i] = KEY_STATE::DOWN;
	//		}
	//		else if (m_keyboardState[i] == KEY_STATE::DOWN) {
	//			m_keyboardState[i] = KEY_STATE::PRESS;
	//		}
	//	}
	//	else {
	//		if (m_keyboardState[i] == KEY_STATE::PRESS or m_keyboardState[i] == KEY_STATE::DOWN) {
	//			m_keyboardState[i] = KEY_STATE::RELEASE;
	//		}
	//		else if (m_keyboardState[i] == KEY_STATE::RELEASE) {
	//			m_keyboardState[i] = KEY_STATE::NONE;
	//		}
	//	}
	//}

}

