/*
 * Dumb.cpp
 *
 *  Created on: Dec 9, 2016
 *      Author: al1
 */

#include "Dumb.hpp"

#include <fcntl.h>

#include <xf86drm.h>

#include <xen-drm-map.h>

#include <xen/be/Log.hpp>

#include "Exception.hpp"

using std::exception;

Dumb::Dumb(int mapFd, int drmFd, int domId,
		   const std::vector<uint32_t>& refs,
		   uint32_t width, uint32_t height, uint32_t bpp) :
	mFd(drmFd),
	mMappedFd(mapFd),
	mHandle(0),
	mMappedHandle(0),
	mStride(0),
	mWidth(width),
	mHeight(height),
	mSize(0),
	mBuffer(nullptr)
{
	try
	{
		DLOG("Dumb", DEBUG) << "Create dumb, mapFd: " << mapFd << ", drmFd: "
							<< drmFd;

		drm_mode_create_dumb createReq {0};

		createReq.width = width;
		createReq.height = height;
		createReq.bpp = bpp;

		auto ret = drmIoctl(mMappedFd, DRM_IOCTL_MODE_CREATE_DUMB, &createReq);

		if (ret < 0)
		{
			throw DrmMapException("Cannot create dumb buffer");
		}

		mStride = createReq.pitch;
		mSize = createReq.size;
		mMappedHandle = createReq.handle;

		xendrmmap_ioctl_map_dumb mapReq;

		mapReq.handle = mMappedHandle;
		mapReq.otherend_id = domId;
		mapReq.grefs = const_cast<uint32_t*>(refs.data());
		mapReq.num_grefs = refs.size();

		ret = drmIoctl(mMappedFd, DRM_IOCTL_XENDRM_MAP_DUMB, &mapReq);

		if (ret < 0)
		{
			throw DrmMapException("Cannot map dumb");
		}


		drm_prime_handle prime {0};

		prime.handle = mMappedHandle;

		prime.flags = DRM_CLOEXEC;

		DLOG("Dumb", DEBUG) << "DRM_IOCTL_PRIME_HANDLE_TO_FD";

		ret = drmIoctl(mMappedFd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime);

		if (ret < 0)
		{
			DLOG("Dumb", ERROR) << "DRM_IOCTL_PRIME_HANDLE_TO_FD";

			throw DrmMapException("Cannot export prime buffer.");
		}

		prime.flags = DRM_CLOEXEC;

		DLOG("Dumb", DEBUG) << "DRM_IOCTL_PRIME_FD_TO_HANDLE";

		ret = drmIoctl(mFd, DRM_IOCTL_PRIME_FD_TO_HANDLE, &prime);

		if (ret < 0)
		{
			DLOG("Dumb", ERROR) << "DRM_IOCTL_PRIME_FD_TO_HANDLE";

			throw DrmMapException("Cannot import prime buffer.");
		}

		mHandle = prime.handle;

		mBuffer = nullptr;

		DLOG("Dumb", DEBUG) << "Create dumb, handle: " << mHandle << ", size: "
						   << mSize << ", stride: " << mStride;

	}
	catch(const exception& e)
	{
		release();

		throw;
	}
}

Dumb::~Dumb()
{
	release();

	DLOG("Dumb", DEBUG) << "Delete dumb, handle: " << mHandle;
}

void Dumb::release()
{
	if (mMappedHandle != 0)
	{
		drm_mode_destroy_dumb dreq {0};

		dreq.handle = mMappedHandle;

		if (drmIoctl(mMappedFd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq) < 0)
		{
			DLOG("Dumb" , ERROR) << "Cannot destroy dumb";
		}
	}
}
