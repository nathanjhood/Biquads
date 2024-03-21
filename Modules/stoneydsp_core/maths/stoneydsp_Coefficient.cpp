/***************************************************************************//**
 * @file stoneydsp_Coefficient.cpp
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

namespace StoneyDSP::Maths
{

template <typename SampleType>
Coefficient<SampleType>::Coefficient() noexcept
: value()
{
}

template <typename SampleType>
Coefficient<SampleType>::Coefficient(SampleType init) noexcept
: value(init)
{
}

template <typename SampleType>
Coefficient<SampleType>::Coefficient(const Coefficient<SampleType>& other) noexcept
: value (other.get())
{}

template <typename SampleType>
Coefficient<SampleType>::~Coefficient() noexcept
{
    #if __cpp_lib_atomic_is_always_lock_free
    static_assert (std::atomic<SampleType>::is_always_lock_free,
                    "This class can only be used for lock-free types");
    #endif
}

template <typename SampleType>
SampleType Coefficient<SampleType>::getValue() const noexcept
{
    return value.load();
}

template <typename SampleType>
void Coefficient<SampleType>::setValue(SampleType newValue) noexcept
{
    value = newValue;
    valueChanged(get());
}

template <typename SampleType>
void Coefficient<SampleType>::valueChanged(SampleType) noexcept
{
}

template <typename SampleType>
Coefficient<SampleType>& Coefficient<SampleType>::operator= (const Coefficient<SampleType>& other) noexcept
{
    value = other.value.load();
    valueChanged(get());
    return *this;
}

template <typename SampleType>
Coefficient<SampleType>& Coefficient<SampleType>::operator= (SampleType other) noexcept
{
    value = other;
    valueChanged(get());
    return *this;
}

template class Coefficient<float>;
template class Coefficient<double>;

} // namespace StoneyDSP::Maths

    /** Creates a new value, initialised to zero. */

    /** Creates a new value, with a given initial value. */

    /** Copies another value (atomically). */

    /** Destructor. */


    // /** Atomically reads and returns the current value. */
    // Type get() const noexcept               { return value.load(); }

    // /** Atomically sets the current value. */
    // void set (Type newValue) noexcept       { value = newValue; }

    // /** Atomically sets the current value, returning the value that was replaced. */
    // Type exchange (Type newValue) noexcept  { return value.exchange (newValue); }

    // /** Copies another value into this one (atomically). */
    // Atomic<Type>& operator= (const Atomic& other) noexcept
    // {
    //     value = other.value.load();
    //     return *this;
    // }

    // /** Copies another value into this one (atomically). */
    // Atomic<Type>& operator= (Type newValue) noexcept
    // {
    //     value = newValue;
    //     return *this;
    // }

    // /** Atomically adds a number to this value, returning the new value. */
    // Type operator+= (DiffType amountToAdd) noexcept { return value += amountToAdd; }

    // /** Atomically subtracts a number from this value, returning the new value. */
    // Type operator-= (DiffType amountToSubtract) noexcept { return value -= amountToSubtract; }

    // /** Atomically increments this value, returning the new value. */
    // Type operator++() noexcept { return ++value; }

    // /** Atomically decrements this value, returning the new value. */
    // Type operator--() noexcept { return --value; }
