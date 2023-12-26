#include "pch.h"
#include "Util.h"

DxException::DxException(HRESULT Hr, const std::wstring& FunctionName, const std::wstring& FileName, int LineNumber) :
	m_errorCode(Hr),
	m_functionName(FunctionName),
	m_fileName(FileName),
	m_lineNumber(LineNumber) {};

std::wstring DxException::ToString() const {
	_com_error  Err(m_errorCode);
	std::wstring msg = Err.ErrorMessage();

	return m_functionName + L" Failed in " + m_fileName + L";\n\n\nLine " + std::to_wstring(m_lineNumber) + L";\n\n\nError: " + msg;
};

std::wstring AnsiToWString(const std::string& Str) {
	WCHAR buffer[512];
	::MultiByteToWideChar(CP_ACP, 0, Str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

