/*
 * AutoBuffer.cpp
 *
 *  Created on: 2017Äê5ÔÂ22ÈÕ
 *      Author: sucx
 */

#include <sutils/AutoBuffer.h>
#include <new> // for std::bad_alloc
#include <stdlib.h> // for malloc(), realloc() and free()
namespace vmodule {

AutoBuffer::AutoBuffer(size_t size) :
		p(0), s(0) {
	if (!size)
		return;

	p = malloc(size); // "malloc()" instead of "new" allow to use "realloc()"
	if (!p)
		throw std::bad_alloc();
	s = size;
}

AutoBuffer::~AutoBuffer() {
	free(p);
}

AutoBuffer& AutoBuffer::allocate(size_t size) {
	clear();
	if (size) {
		p = malloc(size);
		if (!p)
			throw std::bad_alloc();
		s = size;
	}
	return *this;
}

AutoBuffer& AutoBuffer::resize(size_t newSize) {
	if (!newSize)
		return clear();

	void* newPtr = realloc(p, newSize);
	if (!newPtr)
		throw std::bad_alloc();
	p = newPtr;
	s = newSize;
	return *this;
}

AutoBuffer& AutoBuffer::clear(void) {
	free(p);
	p = 0;
	s = 0;
	return *this;
}

AutoBuffer& AutoBuffer::attach(void* pointer, size_t size) {
	clear();
	if ((pointer && size) || (!pointer && !size)) {
		p = pointer;
		s = size;
	}
	return *this;
}

void* AutoBuffer::detach(void) {
	void* returnPtr = p;
	p = 0;
	s = 0;
	return returnPtr;
}

} /* namespace vmodule */
