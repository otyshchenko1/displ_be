/*
 *  Command handler
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
 */

#ifndef SRC_COMMANDHANDLER_HPP_
#define SRC_COMMANDHANDLER_HPP_

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <xen/be/RingBufferBase.hpp>
#include <xen/be/XenGnttab.hpp>
#include <xen/be/Log.hpp>

#include <xen/io/displif.h>

#include "DisplayItf.hpp"

/***************************************************************************//**
 * Ring buffer used to send events to the frontend.
 * @ingroup displ_be
 ******************************************************************************/
class ConEventRingBuffer : public XenBackend::RingBufferOutBase<
										xendispl_event_page, xendispl_evt>
{
public:
	/**
	 * @param id        connector id
	 * @param domId     frontend domain id
	 * @param port      event channel port number
	 * @param ref       grant table reference
	 * @param offset    start of the ring buffer inside the page
	 * @param size      size of the ring buffer
	 */
	ConEventRingBuffer(int id, int domId, int port,
					   int ref, int offset, size_t size);

private:
	int mId;
	XenBackend::Log mLog;
};

/***************************************************************************//**
 * Ring buffer used to send events to the frontend.
 * @ingroup displ_be
 ******************************************************************************/
class BuffersStorage
{
public:

private:
	XenBackend::Log mLog;
};


/**
 * Handles commands received from the frontend.
 * @ingroup displ_be
 */
class CommandHandler
{
public:
	/**
	 * @param display     display
	 * @param conId       connector id
	 * @param domId       domain id
	 * @param eventBuffer event ring buffer
	 */
	CommandHandler(std::shared_ptr<DisplayItf> display, uint32_t conId,
				   int domId, std::shared_ptr<ConEventRingBuffer> eventBuffer);
	~CommandHandler();

	/**
	 * Processes commands received from the frontend.
	 * @param req frontend request
	 * @return
	 */
	uint8_t processCommand(const xendispl_req& req);

private:
	typedef void(CommandHandler::*CommandFn)(const xendispl_req& req);

	static std::unordered_map<int, CommandFn> sCmdTable;

	int mDomId;
	std::shared_ptr<ConEventRingBuffer> mEventBuffer;
	std::shared_ptr<DisplayItf> mDisplay;
	std::shared_ptr<ConnectorItf> mConnector;

	XenBackend::Log mLog;

	static std::unordered_map<uint32_t, uint32_t> drmToWlPixelFormat;

	struct LocalDisplayBuffer
	{
		std::shared_ptr<DisplayBufferItf> displayBuffer;
		std::unique_ptr<XenBackend::XenGnttabBuffer> buffer;
	};

	static std::unordered_map<uint64_t, std::shared_ptr<FrameBufferItf>> mFrameBuffers;
	static std::unordered_map<uint64_t, LocalDisplayBuffer> mDisplayBuffers;

	void pageFlip(const xendispl_req& req);
	void createDisplayBuffer(const xendispl_req& req);
	void destroyDisplayBuffer(const xendispl_req& req);
	void attachFrameBuffer(const xendispl_req& req);
	void detachFrameBuffer(const xendispl_req& req);
	void setConfig(const xendispl_req& req);

	void getBufferRefs(grant_ref_t startDirectory, uint32_t size,
					   std::vector<grant_ref_t>& refs);

	uint32_t getLocalConnectorId();
	static std::shared_ptr<DisplayBufferItf> getLocalDb(uint64_t cookie);
	static std::shared_ptr<FrameBufferItf> getLocalFb(uint64_t cookie);
	void copyBuffer(uint64_t cookie);
	void sendFlipEvent(uint8_t conIdx, uint64_t fb_id);
};

#endif /* SRC_COMMANDHANDLER_HPP_ */
