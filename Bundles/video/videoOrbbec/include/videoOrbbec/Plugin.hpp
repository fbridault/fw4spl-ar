/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoOrbbec/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace videoOrbbec
{
/**
 * @brief   This class is started when the bundle is loaded.
 */
struct VIDEOORBBEC_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOORBBEC_API void start();

    // Overrides
    VIDEOORBBEC_API void stop() noexcept;

};

} // namespace videoOrbbec
