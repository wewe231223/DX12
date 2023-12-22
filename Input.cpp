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
}

void Input::Init() {
	m_instance->m_keyboardState = new KEY_STATE[256];

	for (auto i = 0; i < 256; ++i) {
		m_instance->m_keyboardState[i] = KEY_STATE::NONE;
	}
}

Input* Input::GetInstance(){
	if (m_instance == nullptr) {
		m_instance = new Input();
	}

	return m_instance;

}


void Input::Update() {
	BYTE temp[256]{ };
	if (!GetKeyboardState(temp)) {
		exit(EXIT_FAILURE);
	}

	if (temp[VK_ESCAPE] & 0x80) {
		ProgramClose();
	}

	for (auto i = 0; i < 256; ++i) {
		if (temp[i] & 0x80) {
			if (m_keyboardState[i] == KEY_STATE::NONE or m_keyboardState[i] == KEY_STATE::RELEASE) {
				m_keyboardState[i] = KEY_STATE::DOWN;
			}
			else if (m_keyboardState[i] == KEY_STATE::DOWN) {
				m_keyboardState[i] = KEY_STATE::PRESS;
			}
		}
		else {
			if (m_keyboardState[i] == KEY_STATE::PRESS or m_keyboardState[i] == KEY_STATE::DOWN) {
				m_keyboardState[i] = KEY_STATE::RELEASE;
			}
			else if (m_keyboardState[i] == KEY_STATE::RELEASE) {
				m_keyboardState[i] = KEY_STATE::NONE;
			}
		}
	}

}

