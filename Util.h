#pragma once
// Util.h : 전역함수 전역클래스 등을 정의합니다
//			이외에 시스템과 무관한 전역 매크로, 전역 상수 등도 정의합니다 

// Exeptions
class DxException {
public:
	DxException() = default;
	DxException(HRESULT Hr, const std::wstring& FunctionName, const std::wstring& FileName, int LineNumber);
public:
	HRESULT m_errorCode = S_OK;
	std::wstring m_functionName{};
	std::wstring m_fileName{};
	int m_lineNumber = -1;
public:
	std::wstring ToString() const;
};
std::wstring AnsiToWString(const std::string& Str);
#ifndef ThrowIfFailed
#define ThrowIfFailed(x){												\
    HRESULT hr__ = (x);													\
    std::wstring wfn = AnsiToWString(__FILE__);							\
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }	\
}
#endif

