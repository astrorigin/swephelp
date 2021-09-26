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

#ifndef SWHXX_H
#define SWHXX_H

#ifdef __cplusplus
extern "C"
{
#endif

int swhxx_has_error(void* o);

const char* swhxx_get_error(void* o);

void swhxx_clear_error(void* o);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* SWHXX_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
