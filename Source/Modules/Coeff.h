/**
  ==============================================================================

    Coeff.h
    Created: 26 Jul 2022 13:27:39pm
    Author:  Nathan J. Hood
    Website: github.com/StoneyDSP
    email:   nathanjhood@googlemail.com

  ==============================================================================
*/

#pragma once

#include <assert.h>
#include <atomic>

//template <typename SampleType>
class Coefficient
{
public:
    //==========================================================================

    //==========================================================================
    //
    //  CONSTRUCTORS
    //
    //==========================================================================

    //==========================================================================
    /** Default Constructor. */
    Coefficient() : value(0.0F) 
    {
        assertion();
    }

    /** Initialized Constructor. */
    Coefficient(float initialValue) : value(initialValue)
    {
        assertion();
    }

    /** Initialized Constructor. */
    Coefficient(float* initialValue) : value(*(initialValue))
    {
        assertion();
    }

    /** Copy Constructor. */
    Coefficient(Coefficient& newValue) : value(newValue)
    {
        assertion();
    }

    /** Copy Constructor (const). */
    Coefficient(const Coefficient& newValue) : value(newValue)
    {
        assertion();
    }

    /** Move Constructor. */
    Coefficient(Coefficient&& sourceValue) : value(0.0F)
    {

        // Assign the class data members from the source object to the 
        // object that is being constructed;
        // Assign the data members of the source object to default values. 
        // This prevents the destructor from freeing resources (such as memory) 
        // multiple times:
        value.store(sourceValue);

        // Assign the class data members from the source object to the 
        // object that is being constructed;
        sourceValue.value.store(0.0);

        assertion();
    }

    /** Destructor. */
    ~Coefficient() noexcept
    {
        value.store(0.0);

        static_assert (this->value.is_always_lock_free,
            "Coefficient requires a lock-free std::atomic<float>");
    }

    //==========================================================================
    /** Assertions to check Constructor succeeded */
    void assertion()
    {
        assert(this);
    }
    //==========================================================================

    //==========================================================================
    //
    //  COPY & MOVE ASSIGNMENT OPERATORS
    //
    //==========================================================================

    //==========================================================================
    /** Copy Assignment Operator [=]. */
    float& operator=(float& sourceValue)
    {
        // Copy the data from the source object.
        value.store(sourceValue);
        
        auto result = value.load();

        return result;
    }

    /** Copy Assignment Operator (const) [=]. */
    Coefficient& operator=(const float& sourceValue)
    {
        // Copy the data from the source object.
        value.store(sourceValue);

        return *this;
    }

    /** Move Assignment Operator [=]. */
    Coefficient& operator=(Coefficient&& sourceValue)
    {
        //* Performs no operation if you try to assign the object to itself. */
        if (this != &sourceValue)
        {
            // Copy the data from the source object.
            value.store(sourceValue);

            // Release the data from the source object so that
            // the destructor does not free the memory multiple times.
            sourceValue.value.store(0.0);
        }

        auto result = value.load();

        return *this;
    }
    //==========================================================================

    //==========================================================================
    //
    //  UNARY ARITHMETIC OPERATORS
    //
    //==========================================================================

    //==========================================================================
    /** Increment Prefix Operator [++]. */
    Coefficient& operator++()
    {
        // do actual increment
        (*this)++;

        return *this;
    }

    /** Increment Postfix Operator [++]. */
    Coefficient operator++(int)
    {
        return value.fetch_add(1);
    }

    /** Decrement Prefix Operator [++]. */
    Coefficient& operator--()
    {
        // do actual decrement
        (*this)--;

        return *this;
    }

    /** Decrement Postfix Operator [--]. */
    Coefficient operator--(int)
    {
        return value.fetch_sub(1);
    }
    //==========================================================================

    //==========================================================================
    //
    //  BINARY ARITHMETIC OPERATORS
    //
    //==========================================================================

    //==========================================================================
    /** Addition Assignment Operator [+=]. */
    Coefficient& operator+=(const Coefficient& rhs)
    {
        /** actual addition of rhs to *this. */
        value.fetch_add(rhs);

        return *this;
    }

    /** Subtraction Assignment Operator [-=]. */
    Coefficient& operator-=(const Coefficient& rhs)
    {
        /** actual subtraction of rhs from *this */
        value.fetch_sub(rhs);

        return *this;
    }

    /** Multiplication Assignment Operator [*=]. */
    Coefficient& operator*=(const Coefficient& rhs)
    {
        /** Actual multiplication of rhs with *this */
        *this *= rhs;

        return *this;
    }

    /** Division Assignment Operator [/=]. */
    Coefficient& operator/=(const Coefficient& rhs)
    {
        /** actual division of rhs by *this */
        *this /= rhs;

        return *this;
    }
    //==========================================================================

    //==========================================================================
    //
    //  TYPE CONVERSION OPERATORS
    //
    //==========================================================================

    //==========================================================================
    /** Opertor int(). */
    operator int() const noexcept
    {
        return value.load();
    }

    /** Operator bool(). */
    operator bool() const noexcept
    {
        return value.load();
    }

    /** Operator float(). */
    operator float() const noexcept
    {
        return value.load();
    }

    /** Operator double(). */
    operator double() const noexcept
    {
        return value.load();
    }

//protected:
    //==========================================================================
    //
    //  DATA MEMBERS
    //
    //==========================================================================
    /** Value. */
    //float value{ 0.0F };

    std::atomic<float> value;
};

//template class Coefficient<int>;
//template class Coefficient<bool>;
//template class Coefficient<float>;
//template class Coefficient<double>;

//==============================================================================

//==============================================================================
//
//  BINARY ARITHMETIC OPERATOR NON-MEMBER FUNCTIONS
//
//==============================================================================

//==============================================================================
/** Addition Allocation Operator [+]. */
inline float operator+(Coefficient lhs, const Coefficient& rhs)
{
    /** Call to class member function */
    lhs += rhs;
    return lhs;
}

/** Subtraction Allocation Operator [-]. */
inline Coefficient operator-(Coefficient lhs, const Coefficient& rhs)
{
    /** Call to class member function */
    lhs -= rhs;
    return lhs;
}

/** Multiplication Allocation Operator [*]. */
inline Coefficient operator*(Coefficient lhs, const Coefficient& rhs)
{
    /** Call to class member function */
    lhs *= rhs;
    return lhs;
}

/** Division Allocation Operator [/]. */
inline Coefficient operator/(Coefficient lhs, const Coefficient& rhs)
{
    /** Call to class member function */
    lhs *= rhs;
    return lhs;
}
//==============================================================================

//==============================================================================
//
//  COMPARISON OPERATOR NON-MEMBER FUNCTIONS
//
//==============================================================================

//==============================================================================
/** Equality Comparison Operator [==]. */
inline bool operator==(const Coefficient& lhs, const Coefficient& rhs)
{
    /* do actual comparison */
    return (lhs == rhs) ? true : false;
}

/** Inequality Comparison Operator [!=]. */
inline bool operator!=(const Coefficient& lhs, const Coefficient& rhs)
{
    return !operator==(lhs, rhs);
}

/** Less-Than Comparison Operator [<]. */
inline bool operator< (const Coefficient& lhs, const Coefficient& rhs)
{
    /* do actual comparison */
    return (lhs < rhs) ? true : false;
}

/** Greater-Than Comparison Operator [>]. */
inline bool operator> (const Coefficient& lhs, const Coefficient& rhs)
{
    return  operator< (rhs, lhs);
}

/** Less-Than or Equal-To Comparison Operator [<=]. */
inline bool operator<=(const Coefficient& lhs, const Coefficient& rhs)
{
    return !operator> (lhs, rhs);
}

/** Greater-Than or Equal-To Comparison Operator [>=]. */
inline bool operator>=(const Coefficient& lhs, const Coefficient& rhs)
{
    return !operator< (lhs, rhs);
}
//==============================================================================
