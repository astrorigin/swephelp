#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Script used to build the geonames atlas into an sqlite database.

Works best on linuxes, for now.

"""

# CONFIGURATION

# where everything happens (empty writable directory)
_workdir = '/tmp'

# country codes
# this list must be up to date with geonames ftp
allcodes = [
    'AD',
    'AE',
    'AF',
    'AG',
    'AI',
    'AL',
    'AM',
    'AN',
    'AO',
    'AQ',
    'AR',
    'AS',
    'AT',
    'AU',
    'AW',
    'AZ',
    'BA',
    'BB',
    'BD',
    'BE',
    'BF',
    'BG',
    'BH',
    'BI',
    'BJ',
    'BL',
    'BM',
    'BN',
    'BO',
    'BQ',
    'BR',
    'BS',
    'BT',
    'BV',
    'BW',
    'BY',
    'BZ',
    'CA',
    'CC',
    'CD',
    'CF',
    'CG',
    'CH',
    'CI',
    'CK',
    'CL',
    'CM',
    'CN',
    'CO',
    'CR',
    'CS',
    'CU',
    'CV',
    'CW',
    'CX',
    'CY',
    'CZ',
    'DE',
    'DJ',
    'DK',
    'DM',
    'DO',
    'DZ',
    'EC',
    'EE',
    'EG',
    'EH',
    'ER',
    'ES',
    'ET',
    'FI',
    'FJ',
    'FK',
    'FM',
    'FO',
    'FR',
    'GA',
    'GB',
    'GD',
    'GE',
    'GF',
    'GG',
    'GH',
    'GI',
    'GL',
    'GM',
    'GN',
    'GP',
    'GQ',
    'GR',
    'GS',
    'GT',
    'GU',
    'GW',
    'GY',
    'HK',
    'HM',
    'HN',
    'HR',
    'HT',
    'HU',
    'ID',
    'IE',
    'IL',
    'IM',
    'IN',
    'IO',
    'IQ',
    'IR',
    'IS',
    'IT',
    'JE',
    'JM',
    'JO',
    'JP',
    'KE',
    'KG',
    'KH',
    'KI',
    'KM',
    'KN',
    'KP',
    'KR',
    'KW',
    'KY',
    'KZ',
    'LA',
    'LB',
    'LC',
    'LI',
    'LK',
    'LR',
    'LS',
    'LT',
    'LU',
    'LV',
    'LY',
    'MA',
    'MC',
    'MD',
    'ME',
    'MF',
    'MG',
    'MH',
    'MK',
    'ML',
    'MM',
    'MN',
    'MO',
    'MP',
    'MQ',
    'MR',
    'MS',
    'MT',
    'MU',
    'MV',
    'MW',
    'MX',
    'MY',
    'MZ',
    'NA',
    'NC',
    'NE',
    'NF',
    'NG',
    'NI',
    'NL',
    'NO',
    'NP',
    'NR',
    'NU',
    'NZ',
    'OM',
    'PA',
    'PE',
    'PF',
    'PG',
    'PH',
    'PK',
    'PL',
    'PM',
    'PN',
    'PR',
    'PS',
    'PT',
    'PW',
    'PY',
    'QA',
    'RE',
    'RO',
    'RS',
    'RU',
    'RW',
    'SA',
    'SB',
    'SC',
    'SD',
    'SE',
    'SG',
    'SH',
    'SI',
    'SJ',
    'SK',
    'SL',
    'SM',
    'SN',
    'SO',
    'SR',
    'ST',
    'SV',
    'SX',
    'SY',
    'SZ',
    'TC',
    'TD',
    'TF',
    'TG',
    'TH',
    'TJ',
    'TK',
    'TL',
    'TM',
    'TN',
    'TO',
    'TR',
    'TT',
    'TV',
    'TW',
    'TZ',
    'UA',
    'UG',
    'UM',
    'US',
    'UY',
    'UZ',
    'VA',
    'VC',
    'VE',
    'VG',
    'VI',
    'VN',
    'VU',
    'WF',
    'WS',
    'XK',
    'YE',
    'YT',
    'ZA',
    'ZM',
    'ZW' ]

# END CONFIG

import sys
import os.path
from sqlite3 import dbapi2 as sqlite


# Timezones

tzschema = """
CREATE TABLE Timezones
(
    _idx integer primary key,
    timezoneid varchar not null unique,
    gmtoffset numeric not null,
    dstoffset numeric not null,
    rawoffset numeric not null
);
"""

class TimeZone(object):
    def __init__(self, line):
        line = line.split('\t')
        #print(line)
        self.countrycode = line[0]
        self.timezoneid = line[1]
        self.gmtoffset = line[2]
        self.dstoffset = line[3]
        self.rawoffset = line[4]
    def insert(self, cur):
        print("... timezone [%s]" % self.timezoneid)
        sql = """INSERT INTO Timezones (timezoneid, gmtoffset, dstoffset,
            rawoffset) VALUES ( ?,?,?,? );"""
        try:
            cur.execute(sql, (self.timezoneid, self.gmtoffset,
                self.dstoffset, self.rawoffset))
        except:
            print('ERR='+self.timezoneid)
            raise
    @staticmethod
    def insertDummy(cur):
        print('... insert empty timezone')
        sql = """INSERT INTO Timezones (timezoneid, gmtoffset, dstoffset,
            rawoffset) VALUES ('?', 0, 0, 0);"""
        cur.execute(sql)
    @staticmethod
    def downloadFile():
        url = 'http://download.geonames.org/export/dump/timeZones.txt'
        if not os.path.exists('in/timeZones.txt'):
            os.system('cd in && wget %s && rm -f _timeZones.txt' % url)
        # sort file
        if not os.path.exists('in/_timeZones.txt'):
            os.system('cd in && tail -n +2 timeZones.txt | sort -uk 2 > _timeZones.txt')
    @staticmethod
    def parseFile():
        f = open('in/_timeZones.txt', 'r')
        lines = f.read()
        f.close()
        lines = lines.split('\n')
        ret = []
        for line in lines:
            if line != '':
                ret.append(TimeZone(line))
        return ret

# countries

ctyschema = """
CREATE TABLE CountryInfo
(
    _idx integer primary key,
    iso varchar unique,
    iso3 varchar,
    iso_numeric integer,
    fips varchar,
    country varchar,
    capital varchar,
    area varchar,
    population varchar,
    continent varchar,
    tld varchar,
    currencycode varchar,
    currencyname varchar,
    phone varchar,
    postalcodeformat varchar,
    postalcoderegex varchar,
    languages varchar,
    geonameid integer,
    neighbours varchar,
    equivalentfipscode varchar
);
"""

class CountryInfo(object):
    def __init__(self, line):
        words = line.split('\t')
        self.iso = words[0]
        self.iso3 = words[1]
        self.iso_numeric = int(words[2])
        self.fips = words[3]
        self.country = words[4]
        self.capital = words[5]
        self.area = words[6]
        self.population = words[7]
        self.continent = words[8]
        self.tld = words[9]
        self.currencycode = words[10]
        self.currencyname = words[11]
        self.phone = words[12]
        self.postalcodeformat = words[13]
        self.postalcoderegex = words[14]
        self.languages = words[15]
        self.geonameid = int(words[16])
        self.neighbours = words[17]
        self.equivalentfipscode = words[18]
    def insert(self, cur):
        print("... country [%s]" % self.iso)
        sql = '''INSERT INTO CountryInfo ( iso, iso3, iso_numeric, fips, country,
            capital, area, population, continent, tld, currencycode, currencyname,
            phone, postalcodeformat, postalcoderegex, languages, geonameid,
            neighbours, equivalentfipscode ) VALUES ( ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? );'''
        cur.execute(sql, (self.iso, self.iso3, self.iso_numeric, self.fips, self.country,
            self.capital, self.area, self.population, self.continent, self.tld, self.currencycode,
            self.currencyname, self.phone, self.postalcodeformat, self.postalcoderegex,
            self.languages, self.geonameid, self.neighbours, self.equivalentfipscode))
    @staticmethod
    def downloadFile():
        url = 'http://download.geonames.org/export/dump/countryInfo.txt'
        if not os.path.exists('in/countryInfo.txt'):
            os.system('cd in && wget %s' % url)
    @staticmethod
    def parseFile():
        f = open('in/countryInfo.txt', 'r')
        lines = f.read()
        f.close()
        lines = lines.split('\n')
        lines = lines[:-1]
        i = 0;
        ret = []
        while True:
            try:
                line = lines[i]
            except IndexError:
                return ret
            if not line.startswith('#'):
                ret.append(CountryInfo(line))
            i += 1
        #

# cities

citiesschema = """
CREATE TABLE GeoNames
(
    _idx integer primary key,
    geonameid integer default NULL,
    name varchar not null,
    asciiname varchar not null,
    alternatenames varchar not null,
    latitude numeric not null,
    longitude numeric not null,
    --feature_class varchar,
    --feature_code varchar,
    country integer not null,
    --cc2 varchar,
    --admin1_code varchar,
    --admin2_code varchar,
    --admin3_code varchar,
    --admin4_code varchar,
    --population integer,
    elevation integer default NULL,
    --dem integer,
    timezone integer not null,
    --modification_date varchar

    foreign key (country) references CountryInfo(_idx),
    foreign key (timezone) references Timezones(_idx)
);
"""

class GeoName(object):
    def __init__(self, line):
        line.strip()
        words = line.split('\t')
        self.geonameid = words[0]
        self.name = words[1]
        self.asciiname = words[2]
        self.alternatenames = words[3]
        self.latitude = words[4]
        self.longitude = words[5]
        self.feature_class = words[6]
        self.feature_code = words[7]
        self.country_code = words[8]
        self.cc2 = words[9]
        self.admin1_code = words[10]
        self.admin2_code = words[11]
        self.admin3_code = words[12]
        self.admin4_code = words[13]
        self.population = words[14]
        self.elevation = words[15]
        self.dem = words[16]
        self.timezone = words[17]
        if self.timezone == '': self.timezone = '?'
        self.modification_date = words[18]
    def insert(self, cur):
        #print(self.name)
        sql = """INSERT INTO GeoNames (geonameid,name,asciiname,alternatenames,
            latitude,longitude,country,elevation,timezone)
            VALUES ( ?,?,?,?,?,?,(SELECT _idx FROM CountryInfo WHERE iso = ?),
            ?,(SELECT _idx FROM Timezones WHERE timezoneid = ?));"""
        try: cur.execute(sql, (self.geonameid,self.name,self.asciiname,
            self.alternatenames,self.latitude,self.longitude,self.country_code,
            self.elevation,self.timezone))
        except sqlite.IntegrityError:
            print('(%s)' % self.timezone)
            raise
    @staticmethod
    def downloadFile(ctycode):
        if not os.path.exists('in/%s.txt' % ctycode):
            if not os.path.exists('in/%s.zip' % ctycode):
                os.system('cd in && wget http://download.geonames.org/export/dump/%s.zip' % ctycode)
            os.system('cd in && unzip -o %s.zip' % ctycode)
    @staticmethod
    def parseFile(ctycode):
        path = 'in/%s.txt' % ctycode
        f = open(path, 'r')
        lines = f.read()
        f.close()
        lines = lines.split('\n')
        lines = lines[:-1]
        ret = []
        if ctycode not in ['AN','BV','CS','HM']: # those have no P
            for l in lines:
                name = GeoName(l)
                if name.feature_class == 'P':
                    if name.population != '' and int(name.population) >= 1000:
                        ret.append(name)
        else:
            for l in lines:
                ret.append(GeoName(l))
        return ret
    @staticmethod
    def count(cur, code):
        if code:
            sql = "SELECT count(_idx) FROM GeoNames WHERE country_code = ?;"
            cur.execute(sql, (code,))
        else:
            sql = "SELECT count(_idx) FROM GeoNames;"
            cur.execute(sql)
        return int(cur.fetchone()[0])


def connectdb():
    cnx = sqlite.connect('out/atlas.db', isolation_level=None)
    cur = cnx.cursor()
    return cur

def makeTimeZones(cur):
    print('... making timezones table')
    TimeZone.downloadFile()
    cur.execute(tzschema)
    TimeZone.insertDummy(cur)
    zones = TimeZone.parseFile()
    cur.execute('begin;')
    for z in zones:
        z.insert(cur)
    cur.execute('end;')

def makeCountries(cur):
    print('... making countries table')
    CountryInfo.downloadFile()
    cur.execute(ctyschema)
    cties = CountryInfo.parseFile()
    cur.execute('begin;')
    for c in cties:
        c.insert(cur)
    cur.execute('end;')

def makeCountry(cur, ctycode):
    print('... adding locations [%s]' % ctycode)
    GeoName.downloadFile(ctycode)
    loc = GeoName.parseFile(ctycode)
    cur.execute('begin;')
    for l in loc:
        l.insert(cur)
    cur.execute('end;')

def main():
    os.chdir(_workdir)
    if not os.path.exists('in'):
        os.mkdir('in')
    if not os.path.exists('out'):
        os.mkdir('out')
    if os.path.exists('out/atlas.db'):
        print('error: file %s/out/atlas.db exists' % _workdir)
        sys.exit(1)
    cur = connectdb()
    makeTimeZones(cur)
    makeCountries(cur)
    print('... making cities table')
    cur.execute(citiesschema)
    for code in allcodes:
        makeCountry(cur, code)
        #os.system('rm -f in/%s.txt' % code)
    #tot = 0
    #print('... counting:')
    #for code in allcodes:
    #    cnt = GeoName.count(cur, code)
    #    print("++ [%s]: %d" % (code, cnt))
    #    tot += cnt
    #print('# Total count = %d' % tot)

if __name__ == '__main__':
    main()

# vi: set sw=4 ts=4 et
