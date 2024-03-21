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

#define __STDOUT_GAP_40 std::setw(40) << std::setfill(' ')

#define __STDOUT_USAGE_LINE(args, info) \
  std::cout << __STDOUT_GAP_40 << args << "  "; std::cout << info << std::endl;

/**
 * @brief The ConsoleApplication Class.
 *
*/
class ConsoleApplication : public StoneyDSP::Application::Application
{
public:

    /**
     * @brief Construct a new Console Application object.
     *
     */
    ConsoleApplication()
    : _application_name("ConsoleApplication")
    {}

    ConsoleApplication(std::string_view application_name)
    : _application_name(application_name)
    {}

    ConsoleApplication(std::string application_name)
    : _application_name(application_name)
    {}

    ~ConsoleApplication()
    {
        // _application_name.clear(); not supported for string_view!
    }

    void print_usage()
    {
        std::cout << "usage: " << _application_name.data() << " [OPTION]... [FILE]..." << std::endl;
    }

    void print_version()
    {
        std::cout << _application_name.data() << " v." << "1.0.0" << std::endl;
    }

    void print_err_exception(const std::exception& x)
    {
        std::cerr << x.what() << '\n';
        std::cerr << "usage: " << _application_name.data() << " [OPTION]... [FILE]...\n";
    }

    void print_help()
    {
        print_usage();
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "" << std::endl;
        __STDOUT_USAGE_LINE("[-v|--version]",   "Show program version and exit.")
        __STDOUT_USAGE_LINE("[-h|--help]",      "Show program commands and exit.")
        __STDOUT_USAGE_LINE("[--usage]",        "Show program options and exit.")
    }

private:

    std::string_view                        _application_name;

    static std::vector<std::string_view>    _input_args;

    static inline bool                      _show_version   = false;
    static inline bool                      _show_help      = false;
    static inline bool                      _show_usage     = false;
};

  /// @} group Application
} // namespace Application

  /// @} group StoneyDSP
} // namespace StoneyDSP
