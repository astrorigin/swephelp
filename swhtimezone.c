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
#include <ctype.h>
#include <string.h>

#include "swhtimezone.h"

/* Sources:
 * https://en.wikipedia.org/wiki/List_of_time_zone_abbreviations
 */
const struct swh_tzabbr swh_tzabbrlist[SWH_TZABBR_NUM+1] =
{
{"ACDT", "Australian Central Daylight Saving Time", "UTC+10:30", 10, 30},
{"ACST", "Australian Central Standard Time", "UTC+09:30", 9, 30},
{"ACT", "Acre Time", "UTC−05", -5, 0},
{"ADT", "Atlantic Daylight Time", "UTC−03", -3, 0},
{"AEDT", "Australian Eastern Daylight Saving Time", "UTC+11", 11, 0},
{"AEST", "Australian Eastern Standard Time", "UTC+10", 10, 0},
/*{"AET", "Australian Eastern Time", "UTC+10/UTC+11", 0, 0}, see AEST or AEDT */
{"AFT", "Afghanistan Time", "UTC+04:30", 4, 30},
{"AKDT", "Alaska Daylight Time", "UTC−08", -8, 0},
{"AKST", "Alaska Standard Time", "UTC−09", -9, 0},
{"ALMT", "Alma-Ata Time", "UTC+06", 6, 0},
{"AMST", "Amazon Summer Time (Brazil)",	"UTC−03", -3, 0},
{"AMT", "Amazon Time (Brazil)",	"UTC−04", -4, 0}, /* dupl */
{"AMT", "Armenia Time", "UTC+04", 4, 0}, /* dupl */
{"ANAT", "Anadyr Time",	"UTC+12", 12, 0},
{"AQTT", "Aqtobe Time", "UTC+05", 5, 0},
{"ART", "Argentina Time", "UTC−03", -3, 0},
{"AST", "Arabia Standard Time", "UTC+03", 3, 0}, /* dupl */
{"AST", "Atlantic Standard Time", "UTC−04", -4, 0}, /* dupl */
{"AWST", "Australian Western Standard Time", "UTC+08", 8, 0},
{"AZOST", "Azores Summer Time", "UTC±00", 0, 0},
{"AZOT", "Azores Standard Time", "UTC−01", -1, 0},
{"AZT", "Azerbaijan Time", "UTC+04", 4, 0},
{"BDT", "Brunei Time", "UTC+08", 8, 0},
{"BIOT", "British Indian Ocean Time", "UTC+06", 6, 0},
{"BIT", "Baker Island Time", "UTC−12", -12, 0},
{"BOT", "Bolivia Time", "UTC−04", -4, 0},
{"BRST", "Brasília Summer Time", "UTC−02", -2, 0},
{"BRT", "Brasília Time", "UTC−03", -3, 0},
{"BST", "Bangladesh Standard Time", "UTC+06", 6, 0}, /* tripl */
{"BST", "Bougainville Standard Time", "UTC+11", 11, 0}, /* tripl */
{"BST", "British Summer Time (British Standard Time from Feb 1968 to Oct 1971)", "UTC+01", 1, 0}, /* tripl */
{"BTT", "Bhutan Time", "UTC+06", 6, 0},
{"CAT", "Central Africa Time", "UTC+02", 2, 0},
{"CCT", "Cocos Islands Time", "UTC+06:30", 6, 30},
{"CDT", "Central Daylight Time (North America)", "UTC−05", -5, 0}, /* dupl */
{"CDT", "Cuba Daylight Time", "UTC−04", -4, 0}, /* dupl */
{"CEST", "Central European Summer Time", "UTC+02", 2, 0},
{"CET", "Central European Time", "UTC+01", 1, 0},
{"CHADT", "Chatham Daylight Time", "UTC+13:45", 13, 45},
{"CHAST", "Chatham Standard Time", "UTC+12:45", 12, 45},
{"CHOT", "Choibalsan Standard Time", "UTC+08", 8, 0},
{"CHOST", "Choibalsan Summer Time", "UTC+09", 9, 0},
{"CHST", "Chamorro Standard Time", "UTC+10", 10, 0},
{"CHUT", "Chuuk Time", "UTC+10", 10, 0},
{"CIST", "Clipperton Island Standard Time", "UTC−08", -8, 0},
{"CIT", "Central Indonesia Time", "UTC+08", 8, 0},
{"CKT", "Cook Island Time", "UTC−10", -10, 0},
{"CLST", "Chile Summer Time", "UTC−03", -3, 0},
{"CLT", "Chile Standard Time", "UTC−04", -4, 0},
{"COST", "Colombia Summer Time", "UTC−04", -4, 0},
{"COT", "Colombia Time", "UTC−05", -5, 0},
{"CST", "Central Standard Time (North America)", "UTC−06", -6, 0}, /* tripl */
{"CST", "China Standard Time", "UTC+08", 8, 0}, /* tripl */
{"CST", "Cuba Standard Time", "UTC−05", -5, 0}, /* tripl */
{"CT", "China Time", "UTC+08", 8, 0},
{"CVT", "Cape Verde Time", "UTC−01", -1, 0},
{"CWST", "Central Western Standard Time (Australia) unofficial", "UTC+08:45", 8, 45},
{"CXT", "Christmas Island Time", "UTC+07", 7, 0},
{"DAVT", "Davis Time", "UTC+07", 7, 0},
{"DDUT", "Dumont d'Urville Time", "UTC+10", 10, 0},
{"DFT", "AIX-specific equivalent of Central European Time", "UTC+01", 1, 0},
{"EASST", "Easter Island Summer Time", "UTC−05", -5, 0},
{"EAST", "Easter Island Standard Time", "UTC−06", -6, 0},
{"EAT", "East Africa Time", "UTC+03", 3, 0},
{"ECT", "Eastern Caribbean Time", "UTC−04", -4, 0}, /* dupl */
{"ECT", "Ecuador Time", "UTC−05", -5, 0}, /* dupl */
{"EDT", "Eastern Daylight Time (North America)", "UTC−04", -4, 0},
{"EEST", "Eastern European Summer Time", "UTC+03", 3, 0},
{"EET", "Eastern European Time", "UTC+02", 2, 0},
{"EGST", "Eastern Greenland Summer Time", "UTC±00", 0, 0},
{"EGT", "Eastern Greenland Time", "UTC−01", -1, 0},
{"EIT", "Eastern Indonesian Time", "UTC+09", 9, 0},
{"EST", "Eastern Standard Time (North America)", "UTC−05", -5, 0},
{"FET", "Further-eastern European Time", "UTC+03", 3, 0},
{"FJT", "Fiji Time", "UTC+12", 12, 0},
{"FKST", "Falkland Islands Summer Time", "UTC−03", -3, 0},
{"FKT", "Falkland Islands Time", "UTC−04", -4, 0},
{"FNT", "Fernando de Noronha Time", "UTC−02", -2, 0},
{"GALT", "Galápagos Time", "UTC−06", -6, 0},
{"GAMT", "Gambier Islands Time", "UTC−09", -9, 0},
{"GET", "Georgia Standard Time", "UTC+04", 4, 0},
{"GFT", "French Guiana Time", "UTC−03", -3, 0},
{"GILT", "Gilbert Island Time", "UTC+12", 12, 0},
{"GIT", "Gambier Island Time", "UTC−09", -9, 0},
{"GMT", "Greenwich Mean Time", "UTC±00", 0, 0},
{"GST", "South Georgia and the South Sandwich Islands Time", "UTC−02", -2, 0}, /* dupl */
{"GST", "Gulf Standard Time", "UTC+04", 4, 0}, /* dupl */
{"GYT", "Guyana Time", "UTC−04", -4, 0},
{"HDT", "Hawaii–Aleutian Daylight Time", "UTC−09", -9, 0},
{"HAEC", "Heure Avancée d'Europe Centrale French-language name for CEST", "UTC+02", 2, 0},
{"HST", "Hawaii–Aleutian Standard Time", "UTC−10", -10, 0},
{"HKT", "Hong Kong Time", "UTC+08", 8, 0},
{"HMT", "Heard and McDonald Islands Time", "UTC+05", 5, 0},
{"HOVST", "Hovd Summer Time (not used from 2017-present)", "UTC+08", 8, 0},
{"HOVT", "Hovd Time", "UTC+07", 7, 0},
{"ICT", "Indochina Time", "UTC+07", 7, 0},
{"IDLW", "International Day Line West time zone", "UTC−12", -12, 0},
{"IDT", "Israel Daylight Time", "UTC+03", 3, 0},
{"IOT", "Indian Ocean Time", "UTC+03", 3, 0},
{"IRDT", "Iran Daylight Time", "UTC+04:30", 4, 30},
{"IRKT", "Irkutsk Time", "UTC+08", 8, 0},
{"IRST", "Iran Standard Time", "UTC+03:30", 3, 30},
{"IST", "Indian Standard Time", "UTC+05:30", 5, 30}, /* tripl */
{"IST", "Irish Standard Time", "UTC+01", 1, 0}, /* tripl */
{"IST", "Israel Standard Time", "UTC+02", 2, 0}, /* tripl */
{"JST", "Japan Standard Time", "UTC+09", 9, 0},
{"KALT", "Kaliningrad Time", "UTC+02", 2, 0},
{"KGT", "Kyrgyzstan Time", "UTC+06", 6, 0},
{"KOST", "Kosrae Time", "UTC+11", 11, 0},
{"KRAT", "Krasnoyarsk Time", "UTC+07", 7, 0},
{"KST", "Korea Standard Time", "UTC+09", 9, 0},
{"LHST", "Lord Howe Standard Time", "UTC+10:30", 10, 30}, /* dupl */
{"LHST", "Lord Howe Summer Time", "UTC+11", 11, 0}, /* dupl */
{"LINT", "Line Islands Time", "UTC+14", 14, 0},
{"MAGT", "Magadan Time", "UTC+12", 12, 0},
{"MART", "Marquesas Islands Time", "UTC−09:30", -9, 30},
{"MAWT", "Mawson Station Time", "UTC+05", 5, 0},
{"MDT", "Mountain Daylight Time (North America)", "UTC−06", -6, 0},
{"MET", "Middle European Time Same zone as CET", "UTC+01", 1, 0},
{"MEST", "Middle European Summer Time Same zone as CEST", "UTC+02", 2, 0},
{"MHT", "Marshall Islands Time", "UTC+12", 12, 0},
{"MIST", "Macquarie Island Station Time", "UTC+11", 11, 0},
{"MIT", "Marquesas Islands Time", "UTC−09:30", -9, 30},
{"MMT", "Myanmar Standard Time", "UTC+06:30", 6, 30},
{"MSK", "Moscow Time", "UTC+03", 3, 0},
{"MST", "Malaysia Standard Time", "UTC+08", 8, 0}, /* dupl */
{"MST", "Mountain Standard Time (North America)", "UTC−07", -7, 0}, /* dupl */
{"MUT", "Mauritius Time", "UTC+04", 4, 0},
{"MVT", "Maldives Time", "UTC+05", 5, 0},
{"MYT", "Malaysia Time", "UTC+08", 8, 0},
{"NCT", "New Caledonia Time", "UTC+11", 11, 0},
{"NDT", "Newfoundland Daylight Time", "UTC−02:30", -2, 30},
{"NFT", "Norfolk Island Time", "UTC+11", 11, 0},
{"NOVT", "Novosibirsk Time", "UTC+07", 7, 0},
{"NPT", "Nepal Time", "UTC+05:45", 5, 45},
{"NST", "Newfoundland Standard Time", "UTC−03:30", -3, 30},
{"NT", "Newfoundland Time", "UTC−03:30", -3, 30},
{"NUT", "Niue Time", "UTC−11", -11, 0},
{"NZDT", "New Zealand Daylight Time", "UTC+13", 13, 0},
{"NZST", "New Zealand Standard Time", "UTC+12", 12, 0},
{"OMST", "Omsk Time", "UTC+06", 6, 0},
{"ORAT", "Oral Time", "UTC+05", 5, 0},
{"PDT", "Pacific Daylight Time (North America)", "UTC−07", -7, 0},
{"PET", "Peru Time", "UTC−05", -5, 0},
{"PETT", "Kamchatka Time", "UTC+12", 12, 0},
{"PGT", "Papua New Guinea Time", "UTC+10", 10, 0},
{"PHOT", "Phoenix Island Time", "UTC+13", 13, 0},
{"PHT", "Philippine Time", "UTC+08", 8, 0},
{"PKT", "Pakistan Standard Time", "UTC+05", 5, 0},
{"PMDT", "Saint Pierre and Miquelon Daylight Time", "UTC−02", -2, 0},
{"PMST", "Saint Pierre and Miquelon Standard Time", "UTC−03", -3, 0},
{"PONT", "Pohnpei Standard Time", "UTC+11", 11, 0},
{"PST", "Pacific Standard Time (North America)", "UTC−08", -8, 0}, /* dupl */
{"PST", "Philippine Standard Time", "UTC+08", 8, 0}, /* dupl */
{"PYST", "Paraguay Summer Time", "UTC−03", -3, 0},
{"PYT", "Paraguay Time", "UTC−04", -4, 0},
{"RET", "Réunion Time", "UTC+04", 4, 0},
{"ROTT", "Rothera Research Station Time", "UTC−03", -3, 0},
{"SAKT", "Sakhalin Island Time", "UTC+11", 11, 0},
{"SAMT", "Samara Time", "UTC+04", 4, 0},
{"SAST", "South African Standard Time", "UTC+02", 2, 0},
{"SBT", "Solomon Islands Time", "UTC+11", 11, 0},
{"SCT", "Seychelles Time", "UTC+04", 4, 0},
{"SDT", "Samoa Daylight Time", "UTC−10", -10, 0},
{"SGT", "Singapore Time", "UTC+08", 8, 0},
{"SLST", "Sri Lanka Standard Time", "UTC+05:30", 5, 30},
{"SRET", "Srednekolymsk Time", "UTC+11", 11, 30},
{"SRT", "Suriname Time", "UTC−03", -3, 0},
{"SST", "Samoa Standard Time", "UTC−11", -11, 0}, /* dupl */
{"SST", "Singapore Standard Time", "UTC+08", 8, 0}, /* dupl, see SGT */
{"SYOT", "Showa Station Time", "UTC+03", 3, 0},
{"TAHT", "Tahiti Time", "UTC−10", -10, 0},
{"THA", "Thailand Standard Time", "UTC+07", 7, 0},
{"TFT", "French Southern and Antarctic Time", "UTC+05", 5, 0},
{"TJT", "Tajikistan Time", "UTC+05", 5, 0},
{"TKT", "Tokelau Time", "UTC+13", 13, 0},
{"TLT", "Timor Leste Time", "UTC+09", 9, 0},
{"TMT", "Turkmenistan Time", "UTC+05", 5, 0},
{"TRT", "Turkey Time", "UTC+03", 3, 0},
{"TOT", "Tonga Time", "UTC+13", 13, 0},
{"TVT", "Tuvalu Time", "UTC+12", 12, 0},
{"ULAST", "Ulaanbaatar Summer Time", "UTC+09", 9, 0},
{"ULAT", "Ulaanbaatar Standard Time", "UTC+08", 8, 0},
{"UTC", "Coordinated Universal Time", "UTC±00", 0, 0},
{"UYST", "Uruguay Summer Time", "UTC−02", -2, 0},
{"UYT", "Uruguay Standard Time", "UTC−03", -3, 0},
{"UZT", "Uzbekistan Time", "UTC+05", 5, 0},
{"VET", "Venezuelan Standard Time", "UTC−04", -4, 0},
{"VLAT", "Vladivostok Time", "UTC+10", 10, 0},
{"VOLT", "Volgograd Time", "UTC+04", 4, 0},
{"VOST", "Vostok Station Time", "UTC+06", 6, 0},
{"VUT", "Vanuatu Time", "UTC+11", 11, 0},
{"WAKT", "Wake Island Time", "UTC+12", 12, 0},
{"WAST", "West Africa Summer Time", "UTC+02", 2, 0},
{"WAT", "West Africa Time", "UTC+01", 1, 0},
{"WEST", "Western European Summer Time", "UTC+01", 1, 0},
{"WET", "Western European Time", "UTC±00", 0, 0},
{"WIT", "Western Indonesian Time", "UTC+07", 7, 0},
{"WGST", "West Greenland Summer Time", "UTC−02", -2, 0},
{"WGT", "West Greenland Time", "UTC−03", -3, 0},
{"WST", "Western Standard Time", "UTC+08", 8, 0},
{"YAKT", "Yakutsk Time", "UTC+09", 9, 0},
{"YEKT", "Yekaterinburg Time", "UTC+05", 5, 0},
{NULL, NULL, NULL, 0, 0}
};

int swh_tzabbr_find(const char* tz, struct swh_tzabbr* ret[4])
{
    struct swh_tzabbr *z = (struct swh_tzabbr*) swh_tzabbrlist;
    char str[6];
    char* p = (char*) tz;
    char* q = str;
    size_t sz;
    char c;

    assert(tz);
    assert(ret);

    ret[0] = ret[1] = ret[2] = ret[3] = NULL;
    sz = strlen(tz);
    if (sz < 2 || sz > 5)
        return 1;
    do
        *q++ = toupper(*p++);
    while (*p);
    *q = '\0';
    c = *str;
    do
        if (z->name[0] == c)
            break;
    while ((++z)->name);
    if (!z->name)
        return 1;
    for (;;) {
        if (!strcmp(z->name, str))
            break;
        if (!(++z)->name || z->name[0] != c)
            return 1;
    }
    ret[0] = z++;
    if (z->name && !strcmp(z->name, str)) {
        ret[1] = z++;
        if (z->name && !strcmp(z->name, str)) {
            ret[2] = z;
        }
    }
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
