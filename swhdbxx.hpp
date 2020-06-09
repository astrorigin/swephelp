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

    User(
        int idx=0,
        const char* name="?",
        const char* pswd="",
        const char* mail="",
        const char* info="");

    int idx() const;
    int idx(int x);

    const char* name() const;
    int name(const char* s);

    const char* pswd() const;
    int pswd(const char* s);

    const char* mail() const;
    int mail(const char* s);

    const char* info() const;
    int info(const char* s);

    int drop(char err[512]);

    int save(char err[512]);

    static int root(User** p, char err[512]);

    static int select(const char* name, User** p, char err[512]);

protected:

    int     m_idx;
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

    Data(
        int idx=0,
        int uidx=1,
        const char* title="?",
        double jd=0,
        double lat=0,
        double lon=0,
        int alt=0,
        const char* datetime="",
        const char* timezone="",
        const char* location="",
        const char* country="");

    int idx() const;
    int idx(int x);

    int uidx() const;
    int uidx(int x);

    const char* title() const;
    int title(const char* s);

    double jd() const;
    int jd(double t);

    double lat() const;
    int lat(double d);

    double lon() const;
    int lon(double d);

    int alt() const;
    int alt(int i);

    const char* datetime() const;
    int datetime(const char* s);

    const char* timezone() const;
    int timezone(const char* s);

    const char* location() const;
    int location(const char* s);

    const char* country() const;
    int country(const char* s);

protected:

    int     m_idx;
    int     m_uidx;
    string  m_title;
    double  m_jd;
    double  m_lat;
    double  m_lon;
    int     m_alt;
    string  m_datetime;
    string  m_timezone;
    string  m_location;
    string  m_country;
};


} // end namespace db
} // end namespace swh

#endif // SWHDBXX_HPP
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
