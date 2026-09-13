#pragma once
#include <variant>
#include <string>
#include <cstdint>
#include <comdef.h>
#include "com_string.h"
#include <stdexcept>

using VariantValue = std::variant<ComString, int32_t, int64_t, uint32_t, uint64_t, bool>;

class ComVariant {
public:
	ComVariant() {
		VariantInit(&mRawVariant);
	}

	ComVariant(VariantValue value) {
		VariantInit(&mRawVariant);


		std::visit([this](const auto& value) {

			using NonConstType = std::remove_cvref_t<decltype(value)>;

			if constexpr (std::is_same_v<NonConstType, ComString>) {
				mRawVariant.vt = VT_BSTR;
				mRawVariant.bstrVal = SysAllocString(value.get());
			}

			else if constexpr (std::is_same_v<NonConstType, int32_t>) {
				mRawVariant.vt = VT_I4;
				mRawVariant.lVal = value;
			}

			else if constexpr (std::is_same_v<NonConstType, int64_t>) {
				mRawVariant.vt = VT_I8;
				mRawVariant.llVal = value;
			}

			else if constexpr (std::is_same_v<NonConstType, uint32_t>) {
				mRawVariant.vt = VT_UI4;
				mRawVariant.ulVal = value;
			}

			else if constexpr (std::is_same_v<NonConstType, uint64_t>) {
				mRawVariant.vt = VT_UI8;
				mRawVariant.ullVal = value;
			}

			else if constexpr (std::is_same_v<NonConstType, bool>) {
				mRawVariant.vt = VT_BOOL;
				mRawVariant.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
			}
		}, value);
	}

	auto getValue() const -> VariantValue {
		switch (mRawVariant.vt) {
			case VT_BSTR:
				return ComString{ mRawVariant.bstrVal };
			
			case VT_I4:
				return static_cast<int32_t>(mRawVariant.lVal);
			
			case VT_I8:
				return static_cast<int64_t>(mRawVariant.llVal);
			
			case VT_UI4:
				return static_cast<uint32_t>(mRawVariant.ulVal);
			
			case VT_UI8:
				return static_cast<uint64_t>(mRawVariant.ullVal);
			
			case VT_BOOL:
				return mRawVariant.boolVal == VARIANT_TRUE;
			
			default:
			throw std::runtime_error("Unsupported VARIANT type");
		}
	}

	auto get() const -> const VARIANT& {
		return mRawVariant;
	}

	auto get() -> VARIANT& {
		return mRawVariant;
	}

	~ComVariant() {
		VariantClear(&mRawVariant);
	}

private:
	VARIANT mRawVariant;
};