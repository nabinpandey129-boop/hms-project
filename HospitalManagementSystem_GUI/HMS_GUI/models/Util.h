/* =============================================================================
   FILE:    models/Util.h
   PURPOSE: Small helper functions that the whole project uses.
            Nothing here is about hospitals - they are general text/date tools.

   FUNCTIONS IN THIS FILE (say this to the examiner):
     split()    - cuts one line of a data file into its fields
     clean()    - removes characters that would damage the data files
     lower()    - makes text lower-case (used for searching)
     pad()      - 7 -> "007" (used to build IDs like P007)
     today()    - today's date as text, e.g. "2026-10-08"
     isDate()   - checks a date looks like YYYY-MM-DD
     isTime()   - checks a time looks like HH:MM
     calcAge()  - works out age from a date of birth
   ============================================================================= */
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>
using namespace std;   // lets us write "string" instead of "std::string"


/* -----------------------------------------------------------------------------
   split()
   Our data files store one record per line, fields separated by the | symbol:
        P001|Ram Sharma|1990-05-10|Male|...
   split() turns that line back into a list of separate fields.

   HOW IT WORKS (steps):
     1. Walk through the line one character at a time.
     2. While the character is NOT the separator, add it to the current field.
     3. When we meet the separator, save the current field and start a new one.
     4. After the last character, save the final field.
     5. If the line had fewer fields than expected, add empty ones so the
        caller can safely read f[0] ... f[minimumFields-1] without crashing.
   ----------------------------------------------------------------------------- */
inline vector<string> split(const string& text, char separator = '|', size_t minimumFields = 0) {
    vector<string> fields;
    string current;
    for (char c : text) {
        if (c == separator) {
            fields.push_back(current);   // step 3: finished one field
            current.clear();
        } else {
            current += c;                // step 2: still inside a field
        }
    }
    fields.push_back(current);           // step 4: the last field
    while (fields.size() < minimumFields) fields.push_back("");   // step 5
    return fields;
}


/* -----------------------------------------------------------------------------
   clean()
   WHY: if a user typed the | symbol inside a name, the saved line would have an
        extra field and the record would be read wrongly next time.
   WHAT: every character that is special in our file format (| ~ ; and line
         breaks) is replaced by a space, BEFORE the text is saved.
   ----------------------------------------------------------------------------- */
inline string clean(string text) {
    for (char& c : text) {
        if (c == '|' || c == '\n' || c == '\r' || c == '~' || c == ';') c = ' ';
    }
    return text;
}


// lower(): "Ram SHARMA" -> "ram sharma". Used so searching ignores capital letters.
inline string lower(string text) {
    for (char& c : text) c = tolower(c);
    return text;
}


// pad(): adds zeros in front. pad(7, 3) -> "007". Used to build IDs such as P007.
inline string pad(int number, int width) {
    string s = to_string(number);
    while ((int)s.size() < width) s = "0" + s;
    return s;
}


// today(): returns today's date from the computer clock as "YYYY-MM-DD".
inline string today() {
    time_t now = time(nullptr);              // seconds since 1970
    tm* local = localtime(&now);             // split into year, month, day ...
    char buffer[11];
    strftime(buffer, sizeof buffer, "%Y-%m-%d", local);
    return buffer;
}


// digitsOnly(): true if every character of s from position 'from' up to (not
// including) 'to' is a digit 0-9. Helper for isDate() and isTime().
inline bool digitsOnly(const string& s, int from, int to) {
    for (int i = from; i < to; i++) {
        if (!isdigit((unsigned char)s[i])) return false;
    }
    return true;
}


/* -----------------------------------------------------------------------------
   isDate()  -  accepts only the format YYYY-MM-DD, for example 2026-10-08
   CHECKS (in order):
     1. exactly 10 characters long
     2. the 5th and 8th characters are '-'
     3. all other characters are digits
     4. month is 1..12 and day is 1..31
   ----------------------------------------------------------------------------- */
inline bool isDate(const string& s) {
    if (s.size() != 10) return false;
    if (s[4] != '-' || s[7] != '-') return false;
    if (!digitsOnly(s, 0, 4) || !digitsOnly(s, 5, 7) || !digitsOnly(s, 8, 10)) return false;
    int month = stoi(s.substr(5, 2));
    int day   = stoi(s.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}


/* -----------------------------------------------------------------------------
   isTime()  -  accepts only HH:MM in 24-hour style, for example 09:30 or 14:05
   CHECKS: 5 characters, ':' in the middle, digits elsewhere, hour < 24, minute < 60
   ----------------------------------------------------------------------------- */
inline bool isTime(const string& s) {
    if (s.size() != 5 || s[2] != ':') return false;
    if (!digitsOnly(s, 0, 2) || !digitsOnly(s, 3, 5)) return false;
    int hour   = stoi(s.substr(0, 2));
    int minute = stoi(s.substr(3, 2));
    return hour < 24 && minute < 60;
}


/* -----------------------------------------------------------------------------
   calcAge()  -  age in years from a date of birth ("YYYY-MM-DD")
   STEPS:
     1. age = this year - birth year
     2. if the birthday has not happened yet this year, subtract 1
   ----------------------------------------------------------------------------- */
inline int calcAge(const string& dateOfBirth) {
    if (!isDate(dateOfBirth)) return 0;
    int birthYear  = stoi(dateOfBirth.substr(0, 4));
    int birthMonth = stoi(dateOfBirth.substr(5, 2));
    int birthDay   = stoi(dateOfBirth.substr(8, 2));

    time_t now = time(nullptr);
    tm* local = localtime(&now);
    int thisYear  = local->tm_year + 1900;   // tm_year counts from 1900
    int thisMonth = local->tm_mon + 1;       // tm_mon counts from 0
    int thisDay   = local->tm_mday;

    int age = thisYear - birthYear;                                        // step 1
    bool birthdayNotYet = thisMonth < birthMonth || (thisMonth == birthMonth && thisDay < birthDay);
    if (birthdayNotYet) age--;                                             // step 2
    return age < 0 ? 0 : age;
}
