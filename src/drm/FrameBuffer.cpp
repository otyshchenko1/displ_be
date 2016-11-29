/*
 *  FrameBuffer class
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Copyright (C) 2016 EPAM Systems Inc.
 *
 */

#include "FrameBuffer.hpp"

#include <xen/be/Log.hpp>

#include "Device.hpp"
#include "Dumb.hpp"

using std::shared_ptr;
using std::string;

namespace Drm {

extern const uint32_t cInvalidId;

/*******************************************************************************
 * FrameBuffer
 ******************************************************************************/

FrameBuffer::FrameBuffer(shared_ptr<Dumb> dumb,
						 uint32_t width, uint32_t height,
						 uint32_t pixelFormat) :
	mDumb(dumb),
	mId(cInvalidId)
{
	uint32_t handles[4], pitches[4], offsets[4] = {0};

	handles[0] = mDumb->mHandle;
	pitches[0] = mDumb->mStride;

	auto ret = drmModeAddFB2(mDumb->mFd, width, height, pixelFormat,
							 handles, pitches, offsets, &mId, 0);

	DLOG("FrameBuffer", DEBUG) << "Create frame buffer, handle: " << handles[0]
							  << ", id: " << mId;

	if (ret)
	{
		throw DrmException ("Cannot create frame buffer: " +
							string(strerror(errno)));
	}

}

FrameBuffer::~FrameBuffer()
{
	if (mId != cInvalidId)
	{
		DLOG("FrameBuffer", DEBUG) << "Delete frame buffer, id: " << mId;

		drmModeRmFB(mDumb->mFd, mId);
	}
}

/*******************************************************************************
 * Public
 ******************************************************************************/

/*******************************************************************************
 * Private
 ******************************************************************************/

}

