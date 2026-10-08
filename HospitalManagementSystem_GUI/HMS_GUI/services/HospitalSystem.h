/* =============================================================================
   FILE:    services/HospitalSystem.h
   PURPOSE: The "brain" of the project. It
              (a) keeps ALL data in memory (lists of patients, appointments ...)
              (b) loads/saves that data from/to the text files in the data folder
              (c) contains the BUSINESS RULES (no double booking, billing, ...)

   FEATURES IN THIS FILE (search the word to jump to the code):
     FEATURE 1  LOGIN               -> login()
     FEATURE 3  PATIENTS            -> addPatient(), searchPatients(), deletePatient()
     FEATURE 4  APPOINTMENTS        -> slotTaken(), addAppointment()
     FEATURE 5  VISITS + HISTORY    -> addVisit(), visitsOf()
     FEATURE 6  TESTS/PROCEDURES    -> findService(), addServiceToVisit()
     FEATURE 7  BILLING             -> billForVisit(), (bill rules in addServiceToVisit)
     FEATURE 9  STAFF               -> addUser(), deleteUser()
     FEATURE 10 SAVING / LOADING    -> load(), save()

   HOW TO EXPLAIN THE DESIGN:
     - main.cpp (the server) never touches the files. It only calls the functions
       of this class. This keeps the rules in ONE place.
     - Every change calls save(), so the files are always up to date.
   ============================================================================= */
#pragma once
#include <fstream>
#include <memory>
#include <filesystem>
#include "../models/User.h"
#include "../models/Patient.h"
#include "../models/Appointment.h"
#include "../models/MedicalVisit.h"
#include "../models/Test.h"
#include "../models/Procedure.h"
#include "../models/Bill.h"

class HospitalSystem {
private:
    // ---------------- THE DATA (kept in memory while the program runs) --------
    // users is a list of POINTERS to User. A pointer can point to an Admin, a
    // Receptionist or a Doctor object - this is what makes polymorphism work.
    vector<unique_ptr<User>> users;
    vector<Patient> patients;
    vector<Appointment> appts;          // "appts" = appointments
    vector<MedicalVisit> visits;
    vector<unique_ptr<Service>> services;   // tests and procedures (price list)
    vector<Bill> bills;

    const string dir = "data/";         // folder that holds the text files


    /* =========================================================================
       FEATURE 10: SAVING AND LOADING DATA  (private helper functions)
       ========================================================================= */

    // readLines(): reads a text file and returns its non-empty lines.
    // If the file does not exist yet, it simply returns an empty list.
    static vector<string> readLines(const string& file) {
        ifstream in(file);
        vector<string> lines;
        string line;
        while (getline(in, line)) {
            if (!line.empty()) lines.push_back(line);
        }
        return lines;
    }

    // makeUser(): turns one line of users.txt into the correct kind of object.
    // Line format:  role|id|username|password|name|extra1|extra2
    static unique_ptr<User> makeUser(const string& line) {
        vector<string> f = split(line, '|', 7);
        string role = f[0];
        if (role == "Admin")  return make_unique<Admin>(f[1], f[2], f[3], f[4]);
        if (role == "Doctor") return make_unique<Doctor>(f[1], f[2], f[3], f[4], f[5], atoi(f[6].c_str()));
        return make_unique<Receptionist>(f[1], f[2], f[3], f[4]);
    }

    /* -------------------------------------------------------------------------
       load()  -  runs once, when the program starts.
       STEPS:
         1. Read each text file and turn every line into an object.
         2. If there are no users at all (very first run), create 4 default
            accounts so someone can log in.
         3. If there is no price list yet, create the 6 default services.
         4. Save, so the files now exist on disk.
       ------------------------------------------------------------------------- */
    void load() {
        // step 1
        for (const string& line : readLines(dir + "users.txt"))        users.push_back(makeUser(line));
        for (const string& line : readLines(dir + "patients.txt"))     patients.push_back(Patient::fromRecord(line));
        for (const string& line : readLines(dir + "appointments.txt")) appts.push_back(Appointment::fromRecord(line));
        for (const string& line : readLines(dir + "visits.txt"))       visits.push_back(MedicalVisit::fromRecord(line));
        for (const string& line : readLines(dir + "bills.txt"))        bills.push_back(Bill::fromRecord(line));
        for (const string& line : readLines(dir + "services.txt")) {
            vector<string> f = split(line, '|', 4);     // type|id|name|price
            int price = atoi(f[3].c_str());
            if (f[0] == "Test") services.push_back(make_unique<Test>(f[1], f[2], price));
            else                services.push_back(make_unique<Procedure>(f[1], f[2], price));
        }

        // step 2: default accounts (first run only)
        if (users.empty()) {
            users.push_back(make_unique<Admin>("A001", "admin01", "admin123", "Admin"));
            users.push_back(make_unique<Receptionist>("R001", "recep01", "recep123", "Sita Reception"));
            users.push_back(make_unique<Doctor>("D001", "dr.sharma", "doc123", "Dr. Sharma", "General Physician", 500));
            users.push_back(make_unique<Doctor>("D002", "dr.rai", "doc123", "Dr. Rai", "Pediatrician", 600));
        }

        // step 3: default price list (first run only)
        if (services.empty()) {
            services.push_back(make_unique<Test>("T001", "Blood Test", 300));
            services.push_back(make_unique<Test>("T002", "X-Ray", 800));
            services.push_back(make_unique<Test>("T003", "Urine Test", 200));
            services.push_back(make_unique<Procedure>("PR001", "Injection", 150));
            services.push_back(make_unique<Procedure>("PR002", "Dressing", 250));
            services.push_back(make_unique<Procedure>("PR003", "Minor Procedure", 1000));
        }

        // step 4
        save();
    }


    /* =========================================================================
       ID GENERATION  -  how P001, P002 ... are created
       RULE: new ID = (highest number already used) + 1, with leading zeros.
       Using the highest number (not the count) means IDs are never reused,
       even after something is deleted.
       ========================================================================= */

    // numberOf("P007", "P") -> 7   (cuts the prefix off and reads the number)
    static int numberOf(const string& id, const string& prefix) {
        return atoi(id.substr(prefix.size()).c_str());
    }

    string nextPatientId() {
        int highest = 0;
        for (Patient& p : patients) {
            int n = numberOf(p.getId(), "P");
            if (n > highest) highest = n;
        }
        return "P" + pad(highest + 1, 3);
    }
    string nextAppointmentId() {
        int highest = 0;
        for (Appointment& a : appts) {
            int n = numberOf(a.getId(), "AP");
            if (n > highest) highest = n;
        }
        return "AP" + pad(highest + 1, 3);
    }
    string nextVisitId() {
        int highest = 0;
        for (MedicalVisit& v : visits) {
            int n = numberOf(v.getId(), "V");
            if (n > highest) highest = n;
        }
        return "V" + pad(highest + 1, 3);
    }
    string nextBillId() {
        int highest = 0;
        for (Bill& b : bills) {
            int n = numberOf(b.getId(), "B");
            if (n > highest) highest = n;
        }
        return "B" + pad(highest + 1, 3);
    }


public:
    /* -------------------------------------------------------------------------
       Constructor: runs when the program starts -> creates the data folder and
       loads everything.   Destructor: runs when the program ends -> saves.
       ------------------------------------------------------------------------- */
    HospitalSystem() {
        filesystem::create_directories(dir);
        load();
    }
    ~HospitalSystem() { save(); }

    /* -------------------------------------------------------------------------
       FEATURE 10: save()  -  writes ALL data back to the text files.
       Called after every change. Each object writes itself with toRecord().
       ------------------------------------------------------------------------- */
    void save() {
        ofstream usersFile(dir + "users.txt");
        for (unique_ptr<User>& u : users) usersFile << u->toRecord() << "\n";

        ofstream servicesFile(dir + "services.txt");
        for (unique_ptr<Service>& s : services) servicesFile << s->toRecord() << "\n";

        ofstream patientsFile(dir + "patients.txt");
        for (Patient& p : patients) patientsFile << p.toRecord() << "\n";

        ofstream appointmentsFile(dir + "appointments.txt");
        for (Appointment& a : appts) appointmentsFile << a.toRecord() << "\n";

        ofstream visitsFile(dir + "visits.txt");
        for (MedicalVisit& v : visits) visitsFile << v.toRecord() << "\n";

        ofstream billsFile(dir + "bills.txt");
        for (Bill& b : bills) billsFile << b.toRecord() << "\n";
    }


    /* =========================================================================
       FEATURE 1: LOGIN
       ========================================================================= */

    /* login()
       STEPS:
         1. Go through every user.
         2. If the username matches AND the password matches -> return that user.
         3. If nobody matches -> return nullptr (meaning "login failed").
       The caller (main.cpp) then creates a session token for the user. */
    User* login(const string& username, const string& password) {
        for (unique_ptr<User>& u : users) {
            if (u->getUsername() == username && u->checkPassword(password)) {
                return u.get();
            }
        }
        return nullptr;
    }


    /* =========================================================================
       FEATURE 9: STAFF (users, doctors, receptionists)
       ========================================================================= */

    // All users of one role, e.g. usersByRole("Doctor")
    vector<User*> usersByRole(const string& role) {
        vector<User*> result;
        for (unique_ptr<User>& u : users) {
            if (u->role() == role) result.push_back(u.get());
        }
        return result;
    }

    // All doctors. dynamic_cast asks: "is this User really a Doctor?"
    // It gives a Doctor pointer if yes, or nullptr if it is another role.
    vector<Doctor*> doctors() {
        vector<Doctor*> result;
        for (unique_ptr<User>& u : users) {
            Doctor* d = dynamic_cast<Doctor*>(u.get());
            if (d != nullptr) result.push_back(d);
        }
        return result;
    }

    Doctor* findDoctor(const string& id) {
        for (Doctor* d : doctors()) {
            if (d->getId() == id) return d;
        }
        return nullptr;   // not found
    }

    // Name of a doctor for display; "Unknown" if the ID does not exist
    string doctorName(const string& id) {
        Doctor* d = findDoctor(id);
        return d ? d->getName() : "Unknown";
    }

    User* findUser(const string& id) {
        for (unique_ptr<User>& u : users) {
            if (u->getId() == id) return u.get();
        }
        return nullptr;
    }

    // Is this username already used? (usernames must be unique)
    bool usernameTaken(const string& username) {
        for (unique_ptr<User>& u : users) {
            if (u->getUsername() == username) return true;
        }
        return false;
    }

    // Next staff ID: prefix 'D' -> D001, D002 ...   'R' -> R001 ...
    string nextUserId(char prefix) {
        int highest = 0;
        for (unique_ptr<User>& u : users) {
            if (u->getId()[0] == prefix) {
                int n = atoi(u->getId().substr(1).c_str());
                if (n > highest) highest = n;
            }
        }
        return string(1, prefix) + pad(highest + 1, 3);
    }

    // addUser(): store a new staff account. move() hands the object over to the
    // list (a unique_ptr cannot be copied, only moved).
    void addUser(unique_ptr<User> user) {
        users.push_back(move(user));
        save();
    }

    /* deleteUser()  -  returns "" if deleted, otherwise the reason it failed.
       RULE: a staff member who already has appointments or visits cannot be
             deleted, because those records would point to nobody. */
    string deleteUser(const string& id) {
        for (Appointment& a : appts) {
            if (a.getDoctorId() == id) return "This staff member has appointment records.";
        }
        for (MedicalVisit& v : visits) {
            if (v.getDoctorId() == id) return "This staff member has visit records.";
        }
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i]->getId() == id) {
                users.erase(users.begin() + i);   // remove from the list
                save();
                return "";
            }
        }
        return "Not found.";
    }


    /* =========================================================================
       FEATURE 3: PATIENT REGISTRATION AND MANAGEMENT
       ========================================================================= */

    vector<Patient>& allPatients() { return patients; }

    // findPatient(): lookup by ID. lower() on both sides = "p001" also works.
    Patient* findPatient(const string& id) {
        for (Patient& p : patients) {
            if (lower(p.getId()) == lower(id)) return &p;
        }
        return nullptr;
    }

    string patientName(const string& id) {
        Patient* p = findPatient(id);
        return p ? p->getName() : "Unknown";
    }

    /* searchPatients()  -  the search box on the Patients page.
       A patient matches when the typed text appears ANYWHERE inside the ID, the
       name (ignoring capitals) or the phone number. */
    vector<Patient*> searchPatients(const string& term) {
        vector<Patient*> result;
        string t = lower(term);
        for (Patient& p : patients) {
            bool inId    = lower(p.getId()).find(t) != string::npos;
            bool inName  = lower(p.getName()).find(t) != string::npos;
            bool inPhone = p.getPhone().find(term) != string::npos;
            if (inId || inName || inPhone) result.push_back(&p);
        }
        return result;
    }

    // addPatient(): give the patient the next ID (P001, P002 ...), store, save.
    string addPatient(Patient p) {
        p.setId(nextPatientId());
        patients.push_back(p);
        save();
        return p.getId();
    }

    /* deletePatient()  -  removes the patient AND everything that belongs to
       the patient (appointments, visits, bills), so no "orphan" records remain.
       Each loop goes BACKWARDS (from the last item to the first). Reason: when
       an item is erased the items after it shift one place to the left; going
       backwards means we never skip an item. */
    void deletePatient(const string& id) {
        for (size_t i = appts.size(); i-- > 0;) {
            if (lower(appts[i].getPatientId()) == lower(id)) appts.erase(appts.begin() + i);
        }
        for (size_t i = visits.size(); i-- > 0;) {
            if (lower(visits[i].getPatientId()) == lower(id)) visits.erase(visits.begin() + i);
        }
        for (size_t i = bills.size(); i-- > 0;) {
            if (lower(bills[i].getPatientId()) == lower(id)) bills.erase(bills.begin() + i);
        }
        for (size_t i = 0; i < patients.size(); i++) {
            if (lower(patients[i].getId()) == lower(id)) {
                patients.erase(patients.begin() + i);
                break;   // IDs are unique, so we can stop
            }
        }
        save();
    }


    /* =========================================================================
       FEATURE 4: APPOINTMENT REGISTRATION (BOOKING) AND CANCELLATION
       ========================================================================= */

    vector<Appointment>& allAppointments() { return appts; }

    Appointment* findAppointment(const string& id) {
        for (Appointment& a : appts) {
            if (lower(a.getId()) == lower(id)) return &a;
        }
        return nullptr;
    }

    /* slotTaken()  -  THE DOUBLE-BOOKING RULE.
       Returns true if this doctor already has a SCHEDULED appointment at exactly
       this date and time. A cancelled or completed appointment does not block
       the time slot, because the doctor is free again. */
    bool slotTaken(const string& doctorId, const string& date, const string& time) {
        for (Appointment& a : appts) {
            bool sameDoctor = a.getDoctorId() == doctorId;
            bool sameDate   = a.getDate() == date;
            bool sameTime   = a.getTime() == time;
            bool stillOpen  = a.getStatus() == "Scheduled";
            if (sameDoctor && sameDate && sameTime && stillOpen) return true;
        }
        return false;
    }

    // addAppointment(): give it the next ID (AP001 ...), store, save.
    string addAppointment(Appointment a) {
        a.setId(nextAppointmentId());
        appts.push_back(a);
        save();
        return a.getId();
    }


    /* =========================================================================
       FEATURE 5: MEDICAL VISITS AND PATIENT HISTORY
       ========================================================================= */

    vector<MedicalVisit>& allVisits() { return visits; }

    MedicalVisit* findVisit(const string& id) {
        for (MedicalVisit& v : visits) {
            if (lower(v.getId()) == lower(id)) return &v;
        }
        return nullptr;
    }

    // visitsOf(): the patient's history = all visits of that patient
    vector<MedicalVisit*> visitsOf(const string& patientId) {
        vector<MedicalVisit*> result;
        for (MedicalVisit& v : visits) {
            if (lower(v.getPatientId()) == lower(patientId)) result.push_back(&v);
        }
        return result;
    }

    /* addVisit()  -  called when the doctor presses "Save visit".
       STEPS:
         1. Give the visit the next ID (V001 ...).
         2. Find the doctor, to know the consultation fee.
         3. Create the BILL for this visit, starting with that fee.   <-- billing
         4. If the visit is linked to an appointment, mark it "Completed".
         5. Store the visit, save, and return the visit ID. */
    string addVisit(MedicalVisit v) {
        v.setId(nextVisitId());                                              // step 1

        Doctor* doctor = findDoctor(v.getDoctorId());                        // step 2
        int fee = doctor ? doctor->getFee() : 0;

        bills.push_back(Bill(nextBillId(), v.getPatientId(), v.getId(), v.getDate(), fee));   // step 3

        if (!v.getAppointmentId().empty()) {                                 // step 4
            Appointment* a = findAppointment(v.getAppointmentId());
            if (a != nullptr) a->setStatus("Completed");
        }

        visits.push_back(v);                                                 // step 5
        save();
        return v.getId();
    }


    /* =========================================================================
       FEATURE 6: TESTS AND PROCEDURES (the price list)
       ========================================================================= */

    const vector<unique_ptr<Service>>& allServices() { return services; }

    Service* findService(const string& id) {
        for (unique_ptr<Service>& s : services) {
            if (lower(s->getId()) == lower(id)) return s.get();
        }
        return nullptr;
    }

    /* addServiceToVisit()  -  THE LINK BETWEEN DOCTOR AND BILLING.
       Called when a doctor ticks a test/procedure. Returns "" on success or an
       error message.
       STEPS:
         1. Find the visit, the service and the visit's bill.
            If any of them is missing -> return an error message.
         2. If the bill is already PAID -> refuse (a paid bill is locked).
         3. Otherwise:  a) remember the service in the visit's service list
                        b) add a bill item with the name and FIXED price
            The bill total updates by itself, because Bill::total() adds all
            items every time it is asked.
         4. Save. */
    string addServiceToVisit(const string& visitId, const string& serviceId) {
        MedicalVisit* visit = findVisit(visitId);          // step 1
        Service* service = findService(serviceId);
        Bill* bill = billForVisit(visitId);
        if (visit == nullptr)   return "Visit not found.";
        if (service == nullptr) return "Service not found.";
        if (bill == nullptr)    return "Bill not found.";

        if (bill->isPaid()) return "Bill is already paid; cannot add services.";   // step 2

        visit->addService(service->getId());                                       // step 3a
        bill->addItem(service->getName(), service->getPrice(), service->type());   // step 3b
        save();                                                                    // step 4
        return "";
    }


    /* =========================================================================
       FEATURE 7: BILLING (lookups; the calculation itself is in models/Bill.h)
       ========================================================================= */

    vector<Bill>& allBills() { return bills; }

    Bill* findBill(const string& id) {
        for (Bill& b : bills) {
            if (lower(b.getId()) == lower(id)) return &b;
        }
        return nullptr;
    }

    // Each visit has exactly one bill; this finds it.
    Bill* billForVisit(const string& visitId) {
        for (Bill& b : bills) {
            if (b.getVisitId() == visitId) return &b;
        }
        return nullptr;
    }
};
