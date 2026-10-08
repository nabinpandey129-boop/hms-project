/* =============================================================================
   FILE:    models/Appointment.h
   PURPOSE: One appointment = one patient + one doctor + a date and time.

   FEATURE 4: APPOINTMENT REGISTRATION (BOOKING) AND CANCELLATION  (data part)

   KEY POINTS:
     1. An appointment only stores the IDs of the patient and the doctor
        (patientId, doctorId). The names are looked up when needed.
     2. status has three possible values:
            "Scheduled"  - booked, not yet done
            "Completed"  - the doctor saved a visit linked to it
            "Cancelled"  - cancelled by Admin/Receptionist
     3. Saved in data/appointments.txt, one line per appointment:
            AP001|P001|D001|2026-10-10|10:00|Fever|Scheduled
   ============================================================================= */
#pragma once
#include "Util.h"

class Appointment {
private:
    string id, patientId, doctorId, date, time, reason, status;

public:
    Appointment() {}
    // a new appointment starts as "Scheduled" unless told otherwise
    Appointment(string i, string p, string d, string dt, string t, string r, string s = "Scheduled")
        : id(i), patientId(p), doctorId(d), date(dt), time(t), reason(r), status(s) {}

    // getters
    string getId() const { return id; }
    string getPatientId() const { return patientId; }
    string getDoctorId() const { return doctorId; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    string getReason() const { return reason; }
    string getStatus() const { return status; }

    // setters (only the two values that ever change)
    void setId(const string& v) { id = v; }
    void setStatus(const string& v) { status = v; }

    // SAVE: object -> one line of text
    string toRecord() const {
        return id + "|" + patientId + "|" + doctorId + "|" + date + "|" + time + "|" + reason + "|" + status;
    }

    // LOAD: one line of text -> object
    static Appointment fromRecord(const string& line) {
        vector<string> f = split(line, '|', 7);
        return Appointment(f[0], f[1], f[2], f[3], f[4], f[5], f[6]);
    }
};
