/*
 * Exception.hpp
 *
 *  Created on: Jan 12, 2017
 *      Author: al1
 */

#ifndef SRC_INPUT_EXCEPTION_HPP_
#define SRC_INPUT_EXCEPTION_HPP_

#include "InputItf.hpp"

namespace Input {

/***************************************************************************//**
 * Exception generated by Drm.
 * @ingroup drm
 ******************************************************************************/
class Exception : public InputItf::Exception
{
public:
	using InputItf::Exception::Exception;
};

}

#endif /* SRC_INPUT_EXCEPTION_HPP_ */
