/***************************************************************************//**
 * @file stoneydsp_Application.hpp
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

#pragma once

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

namespace Application
{
/** @addtogroup Application
 *  @{
 */

/**
 * @brief The Application Class.
 *
*/
class Application
{
public:

    /**
     * @brief Construct a new Application object.
     *
     */
    Application()
    : _application_name("Application")
    {}

    /**
     * @brief Construct a new Application object with a given name.
     *
     * @param application_name
     */
    Application(std::string_view application_name)
    : _application_name(application_name)
    {}

    /**
     * @brief Construct a new Application object with a given name.
     *
     * @param application_name
     */
    Application(std::string application_name)
    : _application_name(application_name)
    {}

    /**
     * @brief Construct a new Application object with a given name.
     *
     * @param application_name
     */
    Application(const char application_name[])
    : _application_name(application_name)
    {}

    /**
     * @brief Destroy the Application object.
     *
     */
    ~Application()
    {
        // _application_name.clear(); not supported for string_view!
    }

private:
    std::string_view                        _application_name;
};

  /// @} group Application
} // namespace Application

  /// @} group StoneyDSP
} // namespace StoneyDSP
