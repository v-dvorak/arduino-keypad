#pragma once

struct StringInfo {
	const char* str;
	int length;

	inline bool IsSingleCharacter(void) {
		return length == 1;
	}
	inline char GetValue(int index) {
		return str[index % length];
	}
};