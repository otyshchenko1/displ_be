/*
 * DumbZCopyBack.hpp
 *
 *  Created on: Jan 20, 2017
 *      Author: al1
 */

#ifndef SRC_DRM_DUMBZCOPYBACK_HPP_
#define SRC_DRM_DUMBZCOPYBACK_HPP_

#include <xen/be/XenGnttab.hpp>

#include "DisplayItf.hpp"

namespace Drm {

class DumbZCopyBack : public DisplayItf::DisplayBuffer
{
public:

	/**
	 * @param fd     DRM file descriptor
	 * @param width  dumb width
	 * @param height dumb height
	 * @param bpp    bits per pixel
	 */
	DumbZCopyBack(int drmFd, int mapFd,
				  uint32_t width, uint32_t height, uint32_t bpp,
				  domid_t domId, DisplayItf::GrantRefs& refs);

	~DumbZCopyBack();

	/**
	 * Returns dumb size
	 */
	size_t getSize() const override { return mSize; }

	/**
	 * Returns pointer to the dumb buffer
	 */
	void* getBuffer() const override { return mBuffer; }

	/**
	 * Get stride
	 */
	virtual uint32_t getStride() const override { return mStride; }

	/**
	 * Get handle
	 */
	virtual uintptr_t getHandle() const override { return mHandle; }

	/**
	 * Gets name
	 */
	uint32_t readName() override;

	/**
	 * Copies data from associated grant table buffer
	 */
	void copy() override;

private:

	int mDrmFd;
	int mMappedFd;
	uint32_t mHandle;
	uint32_t mMappedHandle;
	uint32_t mStride;
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mName;
	size_t mSize;
	void* mBuffer;
	XenBackend::Log mLog;

	void createDumb(uint32_t bpp);
	void mapDumb();

	void getGrantRefs(domid_t domId, DisplayItf::GrantRefs& refs);

	void init(uint32_t bpp, domid_t domId, DisplayItf::GrantRefs& refs);
	void release();
};

}

#endif /* SRC_DRM_DUMBZCOPYBACK_HPP_ */