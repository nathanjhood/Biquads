/***************************************************************************//**
 * @file stoneydsp_MathsConstants.hpp
 * @author Nathan J. Hood <nathanjhood@googlemail.com>
 * @brief
 * @version 1.0.0
 * @date 2024-02-21
 *
 * @copyright Copyright (c) 2024
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

namespace Maths
{
/** @addtogroup Maths
 *  @{
 */

/**
 * @brief Commonly used mathematical constants.
 *
*/
template <typename FloatType>
struct Constants
{
    /** A predefined value for minus two. */
    static constexpr FloatType minusTwo = static_cast<FloatType> (-2.0L);

    /** A predefined value for minus one. */
    static constexpr FloatType minusOne = static_cast<FloatType> (-1.0L);

    /** A predefined value for zero. */
    static constexpr FloatType zero = static_cast<FloatType> (0.0L);

    /** A predefined value for one third (of one). */
    static constexpr FloatType oneThird = static_cast<FloatType> (1.0L / 3);

    /** A predefined value for two thirds (of one). */
    static constexpr FloatType twoThirds = static_cast<FloatType> (2.0L / 3);

    /** A predefined value for one. */
    static constexpr FloatType one = static_cast<FloatType> (1.0L);

    /** A predefined value for two. */
    static constexpr FloatType two = static_cast<FloatType> (2.0L);
};

  /// @} group Maths
} // namespace Maths

  /// @} group StoneyDSP
} // namespace StoneyDSP
