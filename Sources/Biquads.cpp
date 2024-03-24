/***************************************************************************//**
 * @file Biquads.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.3.155
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ******************************************************************************/

// #ifdef STONEYDSP_BIQUADS_HPP_INCLUDED
//  #error "Incorrect usage of 'Biquads.cpp'!"
// #endif

#include "Biquads.hpp"

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

namespace ProjectInfo
{
    extern const char* const  projectName    = "Biquads";
    extern const char* const  companyName    = "StoneyDSP";
    extern const char* const  versionString  = "1.2.3.167";
    extern const int          versionNumber  =  0x10203a7;
}

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

// #ifdef STONEYDSP_BIQUADS_MODULE
 #include "Parameters/Parameters.cpp"
 #include "Wrapper/Wrapper.cpp"
 #include "Processor/Processor.cpp"
 #include "Editor/Editor.cpp"
// #endif

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StoneyDSP::Biquads::AudioPluginAudioProcessor();
}
