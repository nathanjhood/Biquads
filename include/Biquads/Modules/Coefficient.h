/**
 * @file Coefficient.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_MODULES_COEFFICIENT_H__

#include <atomic>

template <typename SampleType>
class Coefficient
{
public:
    Coefficient(SampleType init);
    ~Coefficient();

    /** Returns the coefficient's current value. */
    SampleType get() const noexcept { return value; }

    /** Returns the coefficient's current value. */
    operator SampleType() const noexcept { return value; }

    /** Changes the coefficient's current value. */
    Coefficient& operator= (SampleType newValue);

protected:
    /** Override this method if you are interested in receiving callbacks
        when the parameter value changes.
    */
    void valueChanged(SampleType newValue);

private:
    //==============================================================================
    SampleType getValue() const;
    void setValue(SampleType newValue);

    std::atomic<SampleType> value;
};
