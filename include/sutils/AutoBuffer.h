/*
 * AutoBuffer.h
 *
 *  Created on: 2017��5��22��
 *      Author: sucx
 */

#ifndef VMODULE_SUTILS_AUTOBUFFER_H_
#define VMODULE_SUTILS_AUTOBUFFER_H_
#include <stddef.h> // for size_t
namespace vmodule {

class AutoBuffer {
public:
	/**
	 * Create buffer with zero size
	 */
	AutoBuffer() :
			p(0), s(0) {
	}
	/**
	 * Create buffer with specified size
	 * @param size of created buffer
	 */
	explicit AutoBuffer(size_t size);

	~AutoBuffer();

	/**
	 * Allocate specified size for buffer, discarding current buffer content
	 * @param size of buffer to allocate
	 * @return reference to itself
	 */
	AutoBuffer& allocate(size_t size);
	/**
	 * Resize current buffer to new size. Buffer will be extended or truncated at the end.
	 * @param newSize of buffer
	 * @return reference to itself
	 */
	AutoBuffer& resize(size_t newSize);
	/**
	 * Reset buffer to zero size
	 * @return reference to itself
	 */
	AutoBuffer& clear(void);

	/**
	 * Get pointer to buffer content
	 * @return pointer to buffer content or NULL if buffer is zero size
	 */
	inline char* get(void) {
		return static_cast<char*>(p);
	}
	/**
	 * Get constant pointer to buffer content
	 * @return constant pointer to buffer content
	 */
	inline const char* get(void) const {
		return static_cast<char*>(p);
	}
	/**
	 * Get size of the buffer
	 * @return size of the buffer
	 */
	inline size_t size(void) const {
		return s;
	}
	/**
	 * Get size of the buffer
	 * @return size of the buffer
	 */
	inline size_t length(void) const {
		return s;
	}

	/**
	 * Attach malloc'ed pointer to the buffer, discarding current buffer content
	 * Pointer must be acquired by malloc() or realloc().
	 * Pointer will be automatically freed on destroy of the buffer.
	 * @param pointer to attach
	 * @param size of new memory region pointed by pointer
	 * @return reference to itself
	 */
	AutoBuffer& attach(void* pointer, size_t size);
	/**
	 * Detach current buffer content from the buffer, reset buffer to zero size
	 * Caller is responsible to free memory by calling free() for returned pointer
	 * when pointer in not needed anymore
	 * @return detached from buffer pointer to content
	 */
	void* detach(void);

private:
	AutoBuffer(const AutoBuffer& other); // disallow copy constructor
	AutoBuffer& operator=(const AutoBuffer& other); // disallow assignment

	void* p;
	size_t s;
};

} /* namespace vmodule */

#endif /* INCLUDE_SUTILS_AUTOBUFFER_H_ */
