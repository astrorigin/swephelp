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

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "swhxx.h"
#include "swhxx.hpp"

swh::ErrorBase::ErrorBase()
    :
    m_error(NULL)
{
}

swh::ErrorBase::~ErrorBase()
{
    if (m_error)
        delete m_error;
}

const char* swh::ErrorBase::error() const
{
    return m_error ? m_error->c_str() : NULL;
}

const char* swhxx_get_error(void* o)
{
    return ((swh::ErrorBase*)o)->error();
}

void swh::ErrorBase::error(const char* s)
{
    if (!s)
        return clearError();
    if (!m_error) {
        m_error = new (std::nothrow) string(s);
        if (!m_error) {
            fputs("nomem", stderr);
            exit(1);
        }
    }
    else
        *m_error = s;
}

void swh::ErrorBase::errorFormat(const char* fmt, ...)
{
    va_list ap;
    char err[512];
    memset(err, 0, 512);

    assert(fmt);
    va_start(ap, fmt);
    vsnprintf(err, 511, fmt, ap);
    va_end(ap);
    error(err);
}

bool swh::ErrorBase::hasError() const
{
    return m_error ? true : false;
}

int swhxx_has_error(void* o)
{
    return ((swh::ErrorBase*)o)->hasError() == true ? 1 : 0;
}

void swh::ErrorBase::clearError()
{
    if (m_error) {
        delete m_error;
        m_error = NULL;
    }
}

void swhxx_clear_error(void* o)
{
    ((swh::ErrorBase*)o)->clearError();
}

string swh::replaceAll(string str, const string& from, const string& to)
{
    size_t pos = 0;
    while((pos = str.find(from, pos)) != string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
