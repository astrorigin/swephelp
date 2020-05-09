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

#include <assert.h>
#include <swephexp.h>

#include "swhformat.h"

void swh_degsplit(double pos, int *ret)
{
    pos = swe_degnorm(pos);
    ret[1] = (int) floor(pos/30.0);
    pos -= ret[1]*30;
    ret[0] = (int) floor(pos);
    pos -= ret[0];
    ret[2] = (int) floor(pos*60);
    pos -= ret[2]/60.0;
    ret[3] = (int) floor(pos*3600);
}

int swh_signtostr(int sign, char *str)
{
    assert(str);
    memset(str, 0, sizeof(char) * 12);
    switch (sign)
    {
    case 0: strcpy(str, "Aries"); return 0;
    case 1: strcpy(str, "Taurus"); return 0;
    case 2: strcpy(str, "Gemini"); return 0;
    case 3: strcpy(str, "Cancer"); return 0;
    case 4: strcpy(str, "Leo"); return 0;
    case 5: strcpy(str, "Virgo"); return 0;
    case 6: strcpy(str, "Libra"); return 0;
    case 7: strcpy(str, "Scorpio"); return 0;
    case 8: strcpy(str, "Sagittarius"); return 0;
    case 9: strcpy(str, "Capricorn"); return 0;
    case 10: strcpy(str, "Aquarius"); return 0;
    case 11: strcpy(str, "Pisces"); return 0;
    default: return -1;
    }
}

int swh_house_system_name(char hsys, char *str)
{
    assert(str);
    /* swe_house_name returns "Placidus" no matter what (as of v2.08)
     * check variable hsys for validity */
    switch (hsys) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'i':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
        memset(str, 0, 50);
        strncpy(str, swe_house_name(hsys), 49);
        return 0;
    default:
        return -1;
    }
}

int swh_house_system_id(char hsys)
{
    switch (hsys)
    {
    case 'P': return 0;
    case 'K': return 1;
    case 'R': return 2;
    case 'C': return 3;
    case 'B': return 4;
    case 'M': return 5;
    case 'O': return 6;
    case 'A': return 7;
    case 'E': return 7; /* same */
    case 'H': return 8;
    case 'V': return 9;
    case 'X': return 10;
    case 'G': return 11;
    case 'T': return 12;
    case 'U': return 13;
    case 'W': return 14;
    case 'Y': return 15;
    default: return -1;
    }
}

char swh_house_system_char(int hsys)
{
    switch (hsys)
    {
    case 0: return 'P';
    case 1: return 'K';
    case 2: return 'R';
    case 3: return 'C';
    case 4: return 'B';
    case 5: return 'M';
    case 6: return 'O';
    case 7: return 'A';
    /* case 7: return 'E'; / same */
    case 8: return 'H';
    case 9: return 'V';
    case 10: return 'X';
    case 11: return 'G';
    case 12: return 'T';
    case 13: return 'U';
    case 14: return 'W';
    case 15: return 'Y';
    default: return -1;
    }
}

int swh_sidereal_mode_flag(int sidmode)
{
    if (sidmode == 0) return 256; /* no western/tropical flag in swisseph */
    else if (sidmode == 22) return 255; /* user defined */
    else if (sidmode > 0 && sidmode < 22) return sidmode - 1;
    else return -1;
}

int swh_sidereal_mode_id(int sidmode)
{
    if (sidmode == 256) return 0; /* western/tropical */
    else if (sidmode == 255) return 22; /* user-defined */
    else if (sidmode > -1 && sidmode < 21) return sidmode + 1;
    else return -1;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
