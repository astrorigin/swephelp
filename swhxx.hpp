/*

    Swephelp

    Copyright 2007-2020 Stanislas Marquis <stan@astrorigin.com>

    Swephelp is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    Swephelp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Swephelp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SWHXX_HPP
#define SWHXX_HPP

#include <string>

using namespace std;

namespace swh {

class ErrorBase
{
public:

    ErrorBase();

    ~ErrorBase();

    const char* error() const;

    void error(const char* s);

    void errorFormat(const char* fmt, ...);

    bool hasError() const;

    void clearError();

private:

    string* m_error;
};

string replaceAll(string str, const string& from, const string& to);

} // end namespace swh

#endif // SWHXX_HPP

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
