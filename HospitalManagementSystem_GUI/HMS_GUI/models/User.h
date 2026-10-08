/* =============================================================================
   FILE:    models/User.h
   PURPOSE: The people who log in: Admin, Receptionist and Doctor.

   FEATURE 2: ROLE-BASED MENUS  (search "FEATURE 2" to find it)
   FEATURE 9: STAFF ACCOUNTS    (Doctor keeps specialization + consultation fee)

   KEY OOP IDEAS (say this to the examiner):
     1. INHERITANCE   - Admin, Receptionist and Doctor all "are a" User.
                        They share id, username, password and name from User.
     2. ABSTRACT CLASS - User has "pure virtual" functions (= 0). You cannot
                        create a plain User; you must create one of the roles.
     3. POLYMORPHISM  - the server just calls  user->pages().  Each role has
                        its own pages(), so the correct menu is chosen
                        automatically depending on who logged in.
     4. ENCAPSULATION - the data members are private/protected; other code uses
                        the getter functions (getName(), getId() ...).
   ============================================================================= */
#pragma once
#include "Util.h"


/* -----------------------------------------------------------------------------
   class User  -  the common part of every staff account
   ----------------------------------------------------------------------------- */
class User {
protected:   // 'protected' = the child classes (Admin, Doctor ...) can use these
    string id, username, password, name;

public:
    User(string i, string u, string p, string n) : id(i), username(u), password(p), name(n) {}

    // virtual destructor: makes sure the right destructor runs when a User is
    // deleted through a User pointer (needed because we store Admin/Doctor
    // objects in a list of User pointers).
    virtual ~User() {}

    // "= 0" means: every child class MUST write its own version of these.
    virtual string role() const = 0;    // "Admin", "Receptionist" or "Doctor"
    virtual string pages() const = 0;   // the menu of this role (see FEATURE 2)

    // Two extra values that only some roles use (a Doctor has a specialization
    // and a fee). They are written to users.txt; other roles leave them empty.
    virtual string extra1() const { return ""; }
    virtual string extra2() const { return ""; }

    // getters (read-only access to the private data)
    string getId() const { return id; }
    string getUsername() const { return username; }
    string getName() const { return name; }

    // LOGIN CHECK: true when the typed password equals the stored password.
    bool checkPassword(const string& typed) const { return password == typed; }
    void setPassword(const string& newPassword) { password = newPassword; }

    // toRecord(): turns this user into one line of text for users.txt
    // Example:  Doctor|D001|dr.sharma|doc123|Dr. Sharma|General Physician|500
    string toRecord() const {
        return role() + "|" + id + "|" + username + "|" + password + "|" + name + "|" + extra1() + "|" + extra2();
    }
};


/* -----------------------------------------------------------------------------
   FEATURE 2: ROLE-BASED MENUS
   Each role below overrides pages(). It returns the menu as JSON text:
        id    = which screen to open (the browser uses this word)
        label = the text written on the menu button
   The browser receives this list right after login and draws the left menu from
   it (see web/js/core.js, function showApp). So changing a role's menu only
   needs a change in the role's pages() function.
   ----------------------------------------------------------------------------- */

// ADMIN: can use every screen, including staff management and reports.
class Admin : public User {
public:
    using User::User;    // reuse the User constructor
    string role() const override { return "Admin"; }
    string pages() const override {
        return R"([
            {"id":"dashboard","label":"Dashboard"},
            {"id":"patients","label":"Patients"},
            {"id":"doctors","label":"Doctors"},
            {"id":"receptionists","label":"Receptionists"},
            {"id":"appointments","label":"Appointments"},
            {"id":"bills","label":"Bills"},
            {"id":"reports","label":"Reports"}
        ])";
    }
};

// RECEPTIONIST: front-desk work. No delete, no reports, no staff management.
class Receptionist : public User {
public:
    using User::User;
    string role() const override { return "Receptionist"; }
    string pages() const override {
        return R"([
            {"id":"dashboard","label":"Dashboard"},
            {"id":"patients","label":"Patients"},
            {"id":"appointments","label":"Appointments"},
            {"id":"history","label":"Patient History"},
            {"id":"bills","label":"Billing"}
        ])";
    }
};

// DOCTOR: medical work only. Adds a specialization and a consultation fee.
class Doctor : public User {
private:
    string specialization;   // e.g. "General Physician"
    int fee;                 // consultation fee in rupees; goes on every bill

public:
    Doctor(string i, string u, string p, string n, string spec, int f)
        : User(i, u, p, n), specialization(spec), fee(f) {}

    string role() const override { return "Doctor"; }
    string extra1() const override { return specialization; }   // saved in users.txt
    string extra2() const override { return to_string(fee); }   // saved in users.txt
    string getSpecialization() const { return specialization; }
    int getFee() const { return fee; }

    string pages() const override {
        return R"([
            {"id":"dashboard","label":"Dashboard"},
            {"id":"appointments","label":"My Appointments"},
            {"id":"patients","label":"Patients"},
            {"id":"history","label":"Patient History"},
            {"id":"visit","label":"Add Medical Visit"}
        ])";
    }
};
