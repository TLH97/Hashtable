#include <stdio.h>

/*
 * data - key to compute hash
 * len - sizeof of the data
 * range - the number of buckets
 * Return the index of the bucket
 */
unsigned int hash_f(const void *data, size_t len, size_t range) {
	unsigned int hash = 0u;
	size_t idx = 0;
	size_t cof = 1;
	size_t rate = 2;
	const char *d = (const char*)data;
	for(; idx < len; ++idx) {
		hash += d[idx] * cof;
		cof *= rate;
	}
	return hash % range;
}

