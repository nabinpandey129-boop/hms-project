/* =============================================================================
   FILE:    models/MedicalVisit.h
   PURPOSE: One consultation: what the doctor found and which tests/procedures
            were suggested.

   FEATURE 5: MEDICAL VISITS AND PATIENT HISTORY  (data part)

   KEY POINTS:
     1. A visit belongs to one patient and one doctor.
     2. appointmentId is optional: it links the visit to the appointment that
        was booked (empty text "" when there was no appointment).
     3. serviceIds is a LIST of IDs of the tests/procedures used in this visit,
        e.g. T001 (Blood Test), PR001 (Injection).
     4. In visits.txt the list is saved as comma-separated text in the last field:
            V001|P001|D001|AP001|2026-10-02|Viral Fever|Rest|T001,T002
   ============================================================================= */
#pragma once
#include "Util.h"

class MedicalVisit {
private:
    string id, patientId, doctorId, appointmentId, date, diagnosis, notes;
    vector<string> serviceIds;   // IDs of the tests / procedures of this visit

public:
    MedicalVisit() {}
    MedicalVisit(string i, string p, string d, string a, string dt, string diag, string n)
        : id(i), patientId(p), doctorId(d), appointmentId(a), date(dt), diagnosis(diag), notes(n) {}

    // getters
    string getId() const { return id; }
    string getPatientId() const { return patientId; }
    string getDoctorId() const { return doctorId; }
    string getAppointmentId() const { return appointmentId; }
    string getDate() const { return date; }
    string getDiagnosis() const { return diagnosis; }
    string getNotes() const { return notes; }
    const vector<string>& getServiceIds() const { return serviceIds; }

    // setters / changers
    void setId(const string& v) { id = v; }
    void addService(const string& serviceId) { serviceIds.push_back(serviceId); }

    // SAVE: object -> one line of text.
    // The service list is joined with commas: "T001,T002"
    string toRecord() const {
        string services;
        for (size_t i = 0; i < serviceIds.size(); i++) {
            if (i > 0) services += ",";       // comma between items, not before the first
            services += serviceIds[i];
        }
        return id + "|" + patientId + "|" + doctorId + "|" + appointmentId + "|" + date + "|" + diagnosis + "|" + notes + "|" + services;
    }

    // LOAD: one line of text -> object (the last field is split on commas)
    static MedicalVisit fromRecord(const string& line) {
        vector<string> f = split(line, '|', 8);
        MedicalVisit visit(f[0], f[1], f[2], f[3], f[4], f[5], f[6]);
        if (!f[7].empty()) {
            vector<string> ids = split(f[7], ',');
            for (const string& serviceId : ids) visit.addService(serviceId);
        }
        return visit;
    }
};
