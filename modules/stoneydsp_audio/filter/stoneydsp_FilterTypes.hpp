/**
 * @file stoneydsp_FilterTypes.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 1.1.0
 * @date 2024-03-13
 *
 * @copyright Copyright (c) 2024
 *
 */

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

namespace Audio
{
/** @addtogroup Audio
 *  @{
 */

/**
 * @brief A list of the types of the filter.
 *
 */
enum struct FilterType
{
    lowPass2 = 0,
    lowPass1 = 1,
    highPass2 = 2,
    highPass1 = 3,
    bandPass = 4,
    bandPassQ = 5,
    lowShelf2 = 6,
    lowShelf1 = 7,
    lowShelf1C = 8,
    highShelf2 = 9,
    highShelf1 = 10,
    highShelf1C = 11,
    peak = 12,
    notch = 13,
    allPass = 14
};

  /// @} group Audio
} // namespace Audio

  /// @} group StoneyDSP
} // namespace StoneyDSP
