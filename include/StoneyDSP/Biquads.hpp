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

#define STONEYDSP_BIQUADS_HPP

namespace StoneyDSP {
namespace Biquads {

namespace ProjectInfo
{
    const char* const  projectName    = "Biquads";
    const char* const  companyName    = "StoneyDSP";
    const char* const  versionString  = "1.1.0";
    const int          versionNumber  =  0x10100;
}

} // namespace StoneyDSP
} // namespace Biquads

#include "Biquads/PluginProcessor.hpp"
#include "Biquads/PluginWrapper.hpp"
#include "Biquads/PluginParameters.hpp"
