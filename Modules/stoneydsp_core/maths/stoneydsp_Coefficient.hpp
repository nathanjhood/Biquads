/***************************************************************************//**
 * @file stoneydsp_Coefficient.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-09
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

template <typename SampleType>
class Coefficient
{
public:
    /** Construct a new Coefficient object. */
    Coefficient() noexcept;
    /** Construct a new Coefficient object. */
    Coefficient(SampleType init) noexcept;
    /** Construct a new Coefficient object. */
    Coefficient(const Coefficient<SampleType>& other) noexcept;
    /** Destroy the Coefficient object. */
    ~Coefficient() noexcept;

    /** Returns the Coefficient object's current value. */
    SampleType get() const noexcept { return value.load(); }

    /** Returns the Coefficient object's current value. */
    operator SampleType() const noexcept { return value.load(); }

    Coefficient<SampleType>& operator=(const Coefficient<SampleType>&) noexcept;

    /** Changes the Coefficient object's current value. */
    Coefficient& operator= (SampleType newValue) noexcept;

protected:
    /** Override this method if you are interested in receiving callbacks
        when the parameter value changes. */
    void valueChanged(SampleType newValue) noexcept;

private:
    SampleType getValue() const noexcept;
    void setValue(SampleType newValue) noexcept;

private:
    std::atomic<SampleType> value;
    // SampleType value;
};

  /// @} group Maths
} // namespace Maths

  /// @} group StoneyDSP
} // namespace StoneyDSP
