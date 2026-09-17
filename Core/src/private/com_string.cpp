#include "com_string.h"
#include "types.h"

ComString::ComString(std::wstring_view value) {
	mRawCOMString = SysAllocStringLen(value.data(), static_cast<UINT>(value.size()));
}

ComString::~ComString() {
	SysFreeString(mRawCOMString);
}

ComString::ComString(const ComString& other) {
	mRawCOMString = SysAllocString(other.mRawCOMString);
}

ComString& ComString::operator=(const ComString& other) {
	if (this == &other) {
		return *this;
	}

	BSTR newString = SysAllocString(other.mRawCOMString);

	SysFreeString(this->mRawCOMString);
	this->mRawCOMString = newString;

	return *this;
}

ComString::ComString(ComString&& other) noexcept {
	mRawCOMString = other.mRawCOMString;
	other.mRawCOMString = nullptr;
}

ComString& ComString::operator=(ComString&& other) noexcept {
	if (this == &other) {
		return *this;
	}

	SysFreeString(mRawCOMString);
	mRawCOMString = other.mRawCOMString;
	other.mRawCOMString = nullptr;

	return *this;
}


auto ComString::get() const -> BSTR {
	return mRawCOMString;
}

auto ComString::getStr() const -> std::string {
	return toNarrow(mRawCOMString);
}

