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

#ifndef SWHDBXX_HPP
#define SWHDBXX_HPP

#include <string>

#include "swhxx.hpp"

using namespace std;

namespace swh {
namespace db {

class User
    :
    public swh::ErrorBase
{
public:

    User();

    unsigned long idx() const;
    int idx(unsigned long x);

    const char* name() const;
    int name(const char* s);

    const char* pswd() const;
    int pswd(const char* s);

    const char* mail() const;
    int mail(const char* s);

    const char* info() const;
    int info(const char* s);

    int drop();

    int save();

    static bool infoIsValid(const char* s);

    static bool mailIsValid(const char* s);

    static bool nameIsValid(const char* s);

    static bool pswdIsValid(const char* s);

    static int root(User** p, char err[512]);

    static int select(unsigned long uidx, User** p, char err[512]);

    static int select(const char* name, User** p, char err[512]);

protected:

    unsigned long m_idx;
    string  m_name;
    string  m_pswd;
    string  m_mail;
    string  m_info;
};

class Data
    :
    public swh::ErrorBase
{
public:

    Data();

    unsigned long idx() const;
    int idx(unsigned long x);

    unsigned long useridx() const;
    int useridx(unsigned long x);

    const char* title() const;
    int title(const char* s);

    double jd() const;
    int jd(double t);

    double latitude() const;
    int latitude(double d);

    double longitude() const;
    int longitude(double d);

    long altitude() const;
    int altitude(long i);

    const char* datetime() const;
    int datetime(const char* s);

    const char* timezone() const;
    int timezone(const char* s);

    long isdst() const;
    int isdst(long i);

    const char* location() const;
    int location(const char* s);

    const char* country() const;
    int country(const char* s);

    int owner(swh::db::User** p, char err[512]) const;

protected:

    unsigned long m_idx;
    unsigned long m_useridx;
    string  m_title;
    double  m_jd;
    double  m_latitude;
    double  m_longitude;
    long    m_altitude;
    string  m_datetime;
    string  m_timezone;
    long    m_isdst;
    string  m_location;
    string  m_country;
};


} // end namespace db
} // end namespace swh

#endif // SWHDBXX_HPP
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
