#pragma once
enum class KEY_STATE {
	/// <summary>
	///  nothing state 
	/// </summary>
	NONE,

	/// <summary>
	///  pressed on time 
	/// </summary>
	DOWN,

	/// <summary>
	///  pressing during time 
	/// </summary>
	PRESS,

	/// <summary>
	///  released on time  
	/// </summary>
	RELEASE
};

class Input {
private:
	Input();
	~Input();

	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;

private:
	static Input* m_instance;

public:
	static Input* GetInstance();

private:

	KEY_STATE* m_keyboardState{ nullptr };


public:
	const KEY_STATE GetKey(int key) const;
public:
	void Init();
	void Update();

	bool m_notfired = true;
};

#define INPUT Input::GetInstance()