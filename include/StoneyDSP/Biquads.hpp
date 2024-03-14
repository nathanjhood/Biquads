/**
 * @file Biquads.hpp
 * @author  ()
 * @brief
 * @version 0.1
 * @date 2024-03-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#define STONEYDSP_BIQUADS_HPP_INCLUDED

#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <stoneydsp_core/stoneydsp_core.h>
#include <stoneydsp_audio/stoneydsp_audio.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>


#if JUCE_TARGET_HAS_BINARY_DATA
 #include "BinaryData.h"
#endif

// #if ! DONT_SET_USING_JUCE_NAMESPACE
//  // If your code uses a lot of JUCE classes, then this will obviously save you
//  // a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
//  using namespace juce;
// #endif

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

/**
 * @brief The ```StoneyDSP::Biquads``` namespace.
 *
 */
namespace Biquads
{
/** @addtogroup Biquads
 *  @{
 */


namespace ProjectInfo
{
    const char* const  projectName    = "Biquads";
    const char* const  companyName    = "StoneyDSP";
    const char* const  versionString  = "1.2.0";
    const int          versionNumber  =  0x10200;
}

// class AudioPluginAudioProcessor;

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

// #include "Biquads/Processor.hpp"
// #include "Biquads/Wrapper.hpp"
// #include "Biquads/Parameters.hpp"
