#include "key.h"

bool operator==(const Key& key1, const Key& key2) {
	return (key1.column == key2.column && key1.row == key2.row);
}