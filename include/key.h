#pragma once

#include "config.h"

struct Key {
	int row;
	int column;

	bool inline IsMainKey(void) {
		return (-1 < row && row < ROW_COUNT && -1 < column && column <COLUMN_COUNT);
	}
	bool inline IsValidKey(void) {
		return (-1 < row && row < ROW_COUNT && -1 < column && column <COLUMN_COUNT + 1);
	}
};

bool operator==(const Key& key1, const Key& key2);