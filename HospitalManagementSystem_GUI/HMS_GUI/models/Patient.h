/* =============================================================================
   FILE:    models/Patient.h
   PURPOSE: One patient record.

   FEATURE 3: PATIENT REGISTRATION AND MANAGEMENT  (data part)

   KEY POINTS:
     1. ENCAPSULATION - all fields are private. Other code reads them with
        getters (getName()) and changes them with setters (setName()).
     2. toRecord() / fromRecord() convert between the object in memory and ONE
        line of text in data/patients.txt (this is how data is saved/loaded):
            P001|Ram Sharma|1990-05-10|Male|9801234567|Kathmandu|O+|Sita 9811
     3. The age is NOT stored. It is calculated from the date of birth each
        time, so it is always correct.
   ============================================================================= */
#pragma once
#include "Util.h"

class Patient {
private:
    string id, name, dob, gender, phone, address, bloodGroup, emergency;

public:
    Patient() {}   // empty patient (used when reading a form)
    Patient(string i, string n, string d, string g, string ph, string a, string b, string e)
        : id(i), name(n), dob(d), gender(g), phone(ph), address(a), bloodGroup(b), emergency(e) {}

    // ---- getters: read the private data ----
    string getId() const { return id; }
    string getName() const { return name; }
    string getDob() const { return dob; }
    string getGender() const { return gender; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }
    string getBloodGroup() const { return bloodGroup; }
    string getEmergency() const { return emergency; }
    int age() const { return calcAge(dob); }   // calculated, not stored

    // ---- setters: change the private data ----
    void setId(const string& v) { id = v; }
    void setName(const string& v) { name = v; }
    void setDob(const string& v) { dob = v; }
    void setGender(const string& v) { gender = v; }
    void setPhone(const string& v) { phone = v; }
    void setAddress(const string& v) { address = v; }
    void setBloodGroup(const string& v) { bloodGroup = v; }
    void setEmergency(const string& v) { emergency = v; }

    // SAVE: object -> one line of text (fields joined with |)
    string toRecord() const {
        return id + "|" + name + "|" + dob + "|" + gender + "|" + phone + "|" + address + "|" + bloodGroup + "|" + emergency;
    }

    // LOAD: one line of text -> object. 'static' means we can call it without
    // having a Patient yet:  Patient p = Patient::fromRecord(line);
    static Patient fromRecord(const string& line) {
        vector<string> f = split(line, '|', 8);   // 8 fields expected
        return Patient(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
    }
};
