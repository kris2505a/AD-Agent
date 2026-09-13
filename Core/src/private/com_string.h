#pragma once
#include <comdef.h>
#include <string>

class ComString {
public:
	ComString(std::wstring_view value);

	~ComString();

	ComString(const ComString& other);
	ComString& operator=(const ComString& other);

	ComString(ComString&& other) noexcept;
	ComString& operator=(ComString&& other) noexcept;

	auto get() const -> BSTR;

private:
	BSTR mRawCOMString{};

};