/* =============================================================================
   FILE:    main.cpp          (the SERVER - the entry point of the program)

   WHAT THIS PROGRAM IS (say this first to the examiner):
     - A hospital system written in C++.
     - When started it opens a small web server on THIS computer
       (http://localhost:8080) and opens the browser.
     - The browser shows the screens (HTML/CSS/JavaScript). Every button the user
       presses sends a REQUEST to this file. This file checks the user, calls the
       HospitalSystem class (services/HospitalSystem.h), and sends back an ANSWER.
     - Data is stored in text files in the "data" folder.

   THE 3 LAYERS:
        Browser (web/ folder)  --request-->  main.cpp  --calls-->  HospitalSystem
        (screens)              <--answer---  (routes)             (rules + files)

   CODE MAP - search for the words "FEATURE n" to jump to the code:
     FEATURE 1   LOGIN and LOGOUT .................. routes /api/login, /api/logout, /api/me
     FEATURE 2   ROLE-BASED MENUS .................. sessionToJson() and pages() in models/User.h
     FEATURE 3   PATIENT REGISTRATION/MANAGEMENT ... routes /api/patients...
     FEATURE 4   APPOINTMENT REGISTRATION .......... routes /api/appointments...
     FEATURE 5   MEDICAL VISITS and HISTORY ........ route  /api/visits, /api/patients/../history
     FEATURE 6   TESTS and PROCEDURES .............. routes /api/services, /api/visits/../services
     FEATURE 7   BILLING ........................... routes /api/bills...
     FEATURE 8   REPORTS ........................... route  /api/reports
     FEATURE 9   STAFF MANAGEMENT .................. routes /api/staff...
     FEATURE 10  SAVING / LOADING DATA ............. services/HospitalSystem.h (load, save)
     FEATURE 11  SECURITY: ROLE CHECK .............. function route() below

   HOW EVERY ROUTE IS WRITTEN (read this once, then all routes look the same):
        svr.Post("/api/something", route(WHO_MAY_USE_IT, [](request, response, user) {
            ... check the input ...
            ... call HospitalSystem ...
            ... sendOk(response, ...)  or  sendError(response, code, "message") ...
        }));
     - 'svr.Get/Post' means: when the browser asks for this address, run this code.
     - route(...) is the security guard (FEATURE 11): it checks login and role first.
     - The part  [](...) { ... }  is just a small function without a name.
   ============================================================================= */
#include "httplib.h"                    // the small web-server library (third party)
#include "services/HospitalSystem.h"    // all data + rules
#include "web/Page.h"                   // the browser screens, stored as one text (kPage)
#include <mutex>
#include <map>
#include <random>
#include <functional>
#include <algorithm>

using namespace httplib;


/* =============================================================================
   GLOBAL OBJECTS (one of each for the whole program)
   ============================================================================= */
static HospitalSystem sys;               // ALL data and rules (loads the files at start)
static mutex mtx;                        // a "lock": only one request is handled at a time,
                                         // so two requests can never change data together
static map<string, string> sessions;     // logged-in users: token -> user ID
                                         // e.g.  "9f3a...c1" -> "D001"


/* =============================================================================
   PART A: SMALL HELPERS TO WRITE JSON
   The browser and this server talk using JSON text, for example:
        {"id":"P001","name":"Ram Sharma","age":36}
   These helpers build that text.
   ============================================================================= */

// jsonQuote(): puts text inside quotes and protects special characters.
//   he said "hi"   ->   "he said \"hi\""
static string jsonQuote(const string& text) {
    string out = "\"";
    for (unsigned char c : text) {
        if (c == '"')       out += "\\\"";     // a quote inside text must be escaped
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\t') out += "\\t";
        else if (c >= 0x20) out += (char)c;    // normal character: keep it
    }                                          // (other control characters are dropped)
    return out + "\"";
}

// JsonObject: builds one JSON object step by step.
// Example:   JsonObject().text("id", "P001").number("age", 36).build()
//            gives   {"id":"P001","age":36}
struct JsonObject {
    string body = "{";
    bool isEmpty = true;

    // raw(): add a key with a value that is ALREADY JSON (a number, list, object)
    JsonObject& raw(const string& key, const string& jsonValue) {
        if (!isEmpty) body += ",";          // comma between items
        isEmpty = false;
        body += jsonQuote(key) + ":" + jsonValue;
        return *this;                       // returning itself allows chaining .text().number()...
    }
    JsonObject& text(const string& key, const string& value) { return raw(key, jsonQuote(value)); }
    JsonObject& number(const string& key, long value)        { return raw(key, to_string(value)); }
    JsonObject& flag(const string& key, bool value)          { return raw(key, value ? "true" : "false"); }
    string build() const { return body + "}"; }
};

// jsonArray(): joins JSON items into a list:  ["a","b"] / [{...},{...}]
static string jsonArray(const vector<string>& items) {
    string out = "[";
    for (size_t i = 0; i < items.size(); i++) {
        if (i > 0) out += ",";
        out += items[i];
    }
    return out + "]";
}

// sendOk(): send a successful answer (HTTP status 200)
static void sendOk(Response& res, const string& json) {
    res.set_content(json, "application/json; charset=utf-8");
}

// sendError(): send an error answer with a status code and a message.
//   401 = not logged in,  403 = not allowed,  404 = not found,  400 = bad input
static void sendError(Response& res, int statusCode, const string& message) {
    res.status = statusCode;
    res.set_content(JsonObject().text("error", message).build(), "application/json; charset=utf-8");
}

// param(): read one value the browser sent in a form (e.g. "name").
//   - missing value -> ""
//   - spaces at the start/end are removed
//   - clean() removes characters that would damage the data files
static string param(const Request& req, const char* key) {
    string value = req.has_param(key) ? req.get_param_value(key) : "";
    size_t first = value.find_first_not_of(" \t\r\n");
    size_t last  = value.find_last_not_of(" \t\r\n");
    if (first == string::npos) return "";                       // only spaces
    return clean(value.substr(first, last - first + 1));
}


/* =============================================================================
   PART B: TURN OBJECTS INTO JSON
   One small function per kind of object. The browser receives this text.
   ============================================================================= */

static string patientToJson(const Patient& p) {
    return JsonObject()
        .text("id", p.getId()).text("name", p.getName()).text("dob", p.getDob())
        .number("age", p.age())                         // age is calculated, not stored
        .text("gender", p.getGender()).text("phone", p.getPhone()).text("address", p.getAddress())
        .text("blood", p.getBloodGroup()).text("emergency", p.getEmergency())
        .build();
}

static string appointmentToJson(const Appointment& a) {
    return JsonObject()
        .text("id", a.getId())
        .text("patientId", a.getPatientId()).text("patientName", sys.patientName(a.getPatientId()))
        .text("doctorId", a.getDoctorId()).text("doctorName", sys.doctorName(a.getDoctorId()))
        .text("date", a.getDate()).text("time", a.getTime())
        .text("reason", a.getReason()).text("status", a.getStatus())
        .build();
}

static string serviceToJson(const Service& s) {
    return JsonObject()
        .text("id", s.getId()).text("name", s.getName())
        .number("price", s.getPrice()).text("type", s.type())   // type() = "Test" / "Procedure"
        .build();
}

static string visitToJson(const MedicalVisit& v) {
    vector<string> serviceList;
    for (const string& serviceId : v.getServiceIds()) {
        Service* s = sys.findService(serviceId);
        if (s != nullptr) serviceList.push_back(serviceToJson(*s));
    }
    return JsonObject()
        .text("id", v.getId()).text("date", v.getDate())
        .text("doctorId", v.getDoctorId()).text("doctorName", sys.doctorName(v.getDoctorId()))
        .text("diagnosis", v.getDiagnosis()).text("notes", v.getNotes())
        .raw("services", jsonArray(serviceList))
        .build();
}

static string billToJson(const Bill& b) {
    vector<string> itemList;
    for (const BillItem& item : b.getItems()) {
        itemList.push_back(JsonObject().text("name", item.name).number("price", item.price).text("kind", item.kind).build());
    }
    return JsonObject()
        .text("id", b.getId())
        .text("patientId", b.getPatientId()).text("patientName", sys.patientName(b.getPatientId()))
        .text("visitId", b.getVisitId()).text("date", b.getDate())
        .number("consultation", b.getConsultationFee())
        .number("tests", b.testCharges()).number("procedures", b.procedureCharges())
        .number("other", b.getOtherCharges())
        .number("total", b.total())                     // calculated by Bill::total()
        .flag("paid", b.isPaid())
        .raw("items", jsonArray(itemList))
        .build();
}

// A staff member as JSON (a Doctor also shows specialization and fee)
static string userToJson(User* u) {
    JsonObject o;
    o.text("id", u->getId()).text("username", u->getUsername()).text("name", u->getName()).text("role", u->role());
    Doctor* d = dynamic_cast<Doctor*>(u);   // is this user a Doctor?
    if (d != nullptr) o.text("specialization", d->getSpecialization()).number("fee", d->getFee());
    return o.build();
}

/* FEATURE 2: ROLE-BASED MENUS
   sessionToJson() is the answer sent to the browser after login (and by /api/me).
   It contains the user's name and role and - most important - the MENU:
        .raw("pages", u->pages())
   u->pages() is POLYMORPHISM: for an Admin it returns the admin menu, for a
   Doctor the doctor menu, etc. (see models/User.h). The browser draws the left
   menu from this list (web/js/core.js, function showApp). */
static string sessionToJson(User* u, const string& token = "") {
    JsonObject o;
    o.text("id", u->getId()).text("name", u->getName()).text("role", u->role());
    o.raw("pages", u->pages());
    Doctor* d = dynamic_cast<Doctor*>(u);
    if (d != nullptr) o.number("fee", d->getFee());     // the doctor's form needs the fee
    if (!token.empty()) o.text("token", token);         // only sent at login time
    return o.build();
}


/* =============================================================================
   PART C: FEATURE 1 + FEATURE 11  -  WHO IS LOGGED IN, AND IS THE ROLE ALLOWED?
   ============================================================================= */

/* currentUser()  -  finds out who is sending this request.
   After login the browser sends its secret token in a header called X-Token on
   every request. We look the token up in 'sessions' to get the user ID, then
   find that user. Returns nullptr when the token is missing or unknown. */
static User* currentUser(const Request& req) {
    string token = req.get_header_value("X-Token");
    map<string, string>::iterator found = sessions.find(token);
    if (found == sessions.end()) return nullptr;     // unknown token = not logged in
    return sys.findUser(found->second);
}

// Which roles may use a route. An empty list would mean "everyone logged in".
static const vector<string> ANY_ROLE = {"Admin", "Receptionist", "Doctor"};
static const vector<string> ADMIN_OR_RECEPTIONIST = {"Admin", "Receptionist"};
static const vector<string> ADMIN_ONLY = {"Admin"};
static const vector<string> DOCTOR_ONLY = {"Doctor"};

// A "Handler" function receives the request, the response and the logged-in user.
using Handler = function<void(const Request&, Response&, User*)>;

/* FEATURE 11: route()  -  THE SECURITY GUARD placed around EVERY route.
   It returns a function which does these steps before the real code runs:
     1. Lock, so only one request is handled at a time.
     2. Find the user from the token.      -> none: answer 401 "Please log in."
     3. Check the user's role is allowed.  -> no:   answer 403 "not allowed"
     4. Only now run the real handler.
   Because of this, hiding a menu button is not the only protection: even if
   someone sends a request by hand, the server still refuses it. */
static Server::Handler route(vector<string> allowedRoles, Handler realHandler) {
    return [allowedRoles, realHandler](const Request& req, Response& res) {
        lock_guard<mutex> lock(mtx);                                 // step 1

        User* user = currentUser(req);                               // step 2
        if (user == nullptr) {
            sendError(res, 401, "Please log in.");
            return;
        }

        bool allowed = find(allowedRoles.begin(), allowedRoles.end(), user->role()) != allowedRoles.end();
        if (!allowed) {                                              // step 3
            sendError(res, 403, "Your role is not allowed to do this.");
            return;
        }

        realHandler(req, res, user);                                 // step 4
    };
}

/* newToken()  -  makes the secret login token: 32 random hexadecimal characters.
   The random generator is seeded with the clock as well, so every run differs. */
static string newToken() {
    static mt19937_64 generator((random_device())() ^ (unsigned long long)time(nullptr) ^ ((unsigned long long)clock() << 20));
    char buffer[33];
    snprintf(buffer, sizeof buffer, "%016llx%016llx", (unsigned long long)generator(), (unsigned long long)generator());
    return buffer;
}


/* =============================================================================
   PART D: READ A PATIENT FORM (used by "register" and "edit")
   Returns true when the form is valid. Otherwise returns false and puts the
   reason in 'error'.
   VALIDATION STEPS:
     1. every field must be filled in
     2. date of birth must look like YYYY-MM-DD
   ============================================================================= */
static bool readPatientForm(const Request& req, Patient& patient, string& error) {
    string name = param(req, "name");
    string dob = param(req, "dob");
    string gender = param(req, "gender");
    string phone = param(req, "phone");
    string address = param(req, "address");
    string blood = param(req, "blood");
    string emergency = param(req, "emergency");

    if (name.empty() || gender.empty() || phone.empty() || address.empty() || blood.empty() || emergency.empty()) {
        error = "Please fill in all fields.";                        // step 1
        return false;
    }
    if (!isDate(dob)) {
        error = "Date of birth must be YYYY-MM-DD.";                 // step 2
        return false;
    }
    patient.setName(name);
    patient.setDob(dob);
    patient.setGender(gender);
    patient.setPhone(phone);
    patient.setAddress(address);
    patient.setBloodGroup(blood);
    patient.setEmergency(emergency);
    return true;
}


/* =============================================================================
   main()  -  the program starts here.
   It (1) registers all the routes, then (2) starts the server and the browser.
   ============================================================================= */
int main() {
    Server svr;

    // The main page: when the browser opens http://localhost:8080 it receives the
    // whole interface (HTML/CSS/JavaScript) stored in kPage.
    svr.Get("/", [](const Request&, Response& res) {
        res.set_content(kPage, "text/html; charset=utf-8");
    });


    /* =========================================================================
       FEATURE 1: LOGIN AND LOGOUT
       ========================================================================= */

    /* LOGIN   POST /api/login   (anyone - this is how you become logged in)
       STEPS:
         1. Lock, so requests do not overlap.
         2. Read the username and password the user typed.
         3. Ask HospitalSystem::login() to check them.
         4. Wrong -> answer 401 "Invalid username or password."
         5. Correct -> create a random token, remember  token -> user ID  in
            'sessions', and send the browser: name, role, MENU and the token.
            (The browser will send the token with every later request.) */
    svr.Post("/api/login", [](const Request& req, Response& res) {
        lock_guard<mutex> lock(mtx);                                           // step 1

        string username = param(req, "username");                              // step 2
        string password = req.has_param("password") ? req.get_param_value("password") : "";

        User* user = sys.login(username, password);                            // step 3
        if (user == nullptr) {
            sendError(res, 401, "Invalid username or password.");              // step 4
            return;
        }

        string token = newToken();                                             // step 5
        sessions[token] = user->getId();
        sendOk(res, sessionToJson(user, token));
    });

    // WHO AM I   GET /api/me   (any role)
    // Used when the page is reloaded: if the saved token is still valid, the
    // browser gets the user's name, role and menu again and skips the login screen.
    svr.Get("/api/me", route(ANY_ROLE, [](const Request&, Response& res, User* user) {
        sendOk(res, sessionToJson(user));
    }));

    /* LOGOUT   POST /api/logout
       Removes the token from 'sessions'. After this the old token is unknown, so
       any request using it is answered with 401 (FEATURE 11). */
    svr.Post("/api/logout", [](const Request& req, Response& res) {
        lock_guard<mutex> lock(mtx);
        sessions.erase(req.get_header_value("X-Token"));
        sendOk(res, "{}");
    });


    /* =========================================================================
       DASHBOARD NUMBERS   GET /api/summary   (any role)
       The cards on the Dashboard page: patients, scheduled appointments ...
       A doctor only counts his or her OWN appointments.
       ========================================================================= */
    svr.Get("/api/summary", route(ANY_ROLE, [](const Request&, Response& res, User* user) {
        bool isDoctor = (user->role() == "Doctor");
        string todayDate = today();
        int scheduled = 0, scheduledToday = 0, unpaidBills = 0;

        for (Appointment& a : sys.allAppointments()) {
            if (isDoctor && a.getDoctorId() != user->getId()) continue;   // skip other doctors'
            if (a.getStatus() == "Scheduled") {
                scheduled++;
                if (a.getDate() == todayDate) scheduledToday++;
            }
        }
        for (Bill& b : sys.allBills()) {
            if (!b.isPaid()) unpaidBills++;
        }

        JsonObject o;
        o.number("patients", sys.allPatients().size());
        o.number("doctors", sys.doctors().size());
        o.number("scheduled", scheduled);
        o.number("today", scheduledToday);
        o.number("visits", sys.allVisits().size());
        if (!isDoctor) o.number("unpaid", unpaidBills);    // doctors do not see billing
        sendOk(res, o.build());
    }));


    /* =========================================================================
       FEATURE 3: PATIENT REGISTRATION AND MANAGEMENT
       ========================================================================= */

    /* LIST / SEARCH PATIENTS   GET /api/patients?q=text   (any role)
       No search text -> all patients. With text -> only matching patients
       (matching is done by HospitalSystem::searchPatients). */
    svr.Get("/api/patients", route(ANY_ROLE, [](const Request& req, Response& res, User*) {
        string searchText = param(req, "q");
        vector<string> list;
        if (searchText.empty()) {
            for (Patient& p : sys.allPatients()) list.push_back(patientToJson(p));
        } else {
            for (Patient* p : sys.searchPatients(searchText)) list.push_back(patientToJson(*p));
        }
        sendOk(res, jsonArray(list));
    }));

    /* REGISTER A PATIENT   POST /api/patients   (Admin, Receptionist)
       STEPS: 1. read and validate the form (readPatientForm)
              2. store it - HospitalSystem gives it the next ID (P001, P002 ...)
              3. answer with the new ID */
    svr.Post("/api/patients", route(ADMIN_OR_RECEPTIONIST, [](const Request& req, Response& res, User*) {
        Patient patient;
        string error;
        if (!readPatientForm(req, patient, error)) {
            sendError(res, 400, error);
            return;
        }
        string newId = sys.addPatient(patient);
        sendOk(res, JsonObject().text("id", newId).build());
    }));

    /* EDIT A PATIENT   POST /api/patients/<id>/update   (Admin, Receptionist)
       STEPS: 1. find the patient (404 if missing)
              2. read and validate the new form values into a temporary Patient
              3. copy the new values onto the stored patient, then save */
    svr.Post(R"(/api/patients/([^/]+)/update)", route(ADMIN_OR_RECEPTIONIST, [](const Request& req, Response& res, User*) {
        Patient* stored = sys.findPatient(req.matches[1]);     // matches[1] = the <id> in the address
        if (stored == nullptr) {
            sendError(res, 404, "Patient not found.");
            return;
        }
        Patient edited;
        string error;
        if (!readPatientForm(req, edited, error)) {
            sendError(res, 400, error);
            return;
        }
        stored->setName(edited.getName());
        stored->setDob(edited.getDob());
        stored->setGender(edited.getGender());
        stored->setPhone(edited.getPhone());
        stored->setAddress(edited.getAddress());
        stored->setBloodGroup(edited.getBloodGroup());
        stored->setEmergency(edited.getEmergency());
        sys.save();
        sendOk(res, "{}");
    }));

    /* DELETE A PATIENT   POST /api/patients/<id>/delete   (ADMIN ONLY)
       HospitalSystem::deletePatient also removes the patient's appointments,
       visits and bills, so no orphan records are left. */
    svr.Post(R"(/api/patients/([^/]+)/delete)", route(ADMIN_ONLY, [](const Request& req, Response& res, User*) {
        Patient* patient = sys.findPatient(req.matches[1]);
        if (patient == nullptr) {
            sendError(res, 404, "Patient not found.");
            return;
        }
        sys.deletePatient(patient->getId());
        sendOk(res, "{}");
    }));


    /* =========================================================================
       FEATURE 5: PATIENT HISTORY   GET /api/patients/<id>/history   (any role)
       Sends the patient's details plus all of the patient's visits; the browser
       shows them as a timeline.
       ========================================================================= */
    svr.Get(R"(/api/patients/([^/]+)/history)", route(ANY_ROLE, [](const Request& req, Response& res, User*) {
        Patient* patient = sys.findPatient(req.matches[1]);
        if (patient == nullptr) {
            sendError(res, 404, "Patient not found.");
            return;
        }
        vector<string> visitList;
        for (MedicalVisit* v : sys.visitsOf(patient->getId())) visitList.push_back(visitToJson(*v));
        sendOk(res, JsonObject().raw("patient", patientToJson(*patient)).raw("visits", jsonArray(visitList)).build());
    }));


    /* =========================================================================
       LISTS FOR DROP-DOWN BOXES (doctors and the price list)   (any role)
       ========================================================================= */
    svr.Get("/api/doctors", route(ANY_ROLE, [](const Request&, Response& res, User*) {
        vector<string> list;
        for (Doctor* d : sys.doctors()) list.push_back(userToJson(d));
        sendOk(res, jsonArray(list));
    }));

    // FEATURE 6: the price list of tests and procedures
    svr.Get("/api/services", route(ANY_ROLE, [](const Request&, Response& res, User*) {
        vector<string> list;
        for (const unique_ptr<Service>& s : sys.allServices()) list.push_back(serviceToJson(*s));
        sendOk(res, jsonArray(list));
    }));


    /* =========================================================================
       FEATURE 4: APPOINTMENT REGISTRATION (BOOKING) AND CANCELLATION
       ========================================================================= */

    /* LIST APPOINTMENTS   GET /api/appointments   (any role)
       Admin and Receptionist see ALL appointments.
       A Doctor sees ONLY his or her own (the 'if' below filters them). */
    svr.Get("/api/appointments", route(ANY_ROLE, [](const Request&, Response& res, User* user) {
        vector<string> list;
        for (Appointment& a : sys.allAppointments()) {
            bool isOtherDoctors = (user->role() == "Doctor" && a.getDoctorId() != user->getId());
            if (!isOtherDoctors) list.push_back(appointmentToJson(a));
        }
        sendOk(res, jsonArray(list));
    }));

    /* BOOK AN APPOINTMENT   POST /api/appointments   (Admin, Receptionist)
       This is the "appointment registration". VALIDATION STEPS, in order -
       the first problem found stops the booking with a message:
         1. the patient must exist
         2. the doctor must exist
         3. the date must look like YYYY-MM-DD
         4. the time must look like HH:MM
         5. a reason must be entered
         6. the doctor must be FREE at that date and time (no double booking)
       If all checks pass, the appointment is stored with status "Scheduled". */
    svr.Post("/api/appointments", route(ADMIN_OR_RECEPTIONIST, [](const Request& req, Response& res, User*) {
        Patient* patient = sys.findPatient(param(req, "patientId"));
        Doctor* doctor = sys.findDoctor(param(req, "doctorId"));
        string date = param(req, "date");
        string time = param(req, "time");
        string reason = param(req, "reason");

        if (patient == nullptr) { sendError(res, 400, "Select a patient."); return; }                   // 1
        if (doctor == nullptr)  { sendError(res, 400, "Select a doctor."); return; }                    // 2
        if (!isDate(date))      { sendError(res, 400, "Date must be YYYY-MM-DD."); return; }            // 3
        if (!isTime(time))      { sendError(res, 400, "Time must be HH:MM."); return; }                 // 4
        if (reason.empty())     { sendError(res, 400, "Enter a reason."); return; }                     // 5
        if (sys.slotTaken(doctor->getId(), date, time)) {                                               // 6
            sendError(res, 400, "That doctor already has an appointment at that time.");
            return;
        }

        Appointment appointment("", patient->getId(), doctor->getId(), date, time, reason);   // "" = ID is given by addAppointment
        string newId = sys.addAppointment(appointment);
        sendOk(res, JsonObject().text("id", newId).build());
    }));

    /* CANCEL AN APPOINTMENT   POST /api/appointments/<id>/cancel   (Admin, Receptionist)
       Only a "Scheduled" appointment can be cancelled. */
    svr.Post(R"(/api/appointments/([^/]+)/cancel)", route(ADMIN_OR_RECEPTIONIST, [](const Request& req, Response& res, User*) {
        Appointment* appointment = sys.findAppointment(req.matches[1]);
        if (appointment == nullptr) {
            sendError(res, 404, "Appointment not found.");
            return;
        }
        if (appointment->getStatus() != "Scheduled") {
            sendError(res, 400, "Only scheduled appointments can be cancelled.");
            return;
        }
        appointment->setStatus("Cancelled");
        sys.save();
        sendOk(res, "{}");
    }));


    /* =========================================================================
       FEATURE 5 + 6 + 7: MEDICAL VISIT, TESTS/PROCEDURES AND THE BILL
       ========================================================================= */

    /* SAVE A MEDICAL VISIT   POST /api/visits   (DOCTOR ONLY)
       This one request does a lot. STEPS:
         1. The patient must exist.
         2. Diagnosis and notes must be filled in.
         3. If an appointment was chosen it must be valid:
              - it exists
              - it belongs to THIS doctor and THIS patient
              - it is still "Scheduled"
         4. HospitalSystem::addVisit() saves the visit, CREATES THE BILL with the
            doctor's consultation fee, and marks the appointment "Completed".
         5. For every test/procedure the doctor ticked (a comma list such as
            "T001,T002") call addServiceToVisit(): the fixed price is looked up
            and added to the bill automatically.
         6. Answer with the visit ID and the finished bill. */
    svr.Post("/api/visits", route(DOCTOR_ONLY, [](const Request& req, Response& res, User* doctor) {
        Patient* patient = sys.findPatient(param(req, "patientId"));
        string diagnosis = param(req, "diagnosis");
        string notes = param(req, "notes");
        string appointmentId = param(req, "appointmentId");

        if (patient == nullptr) { sendError(res, 400, "Select a patient."); return; }                      // 1
        if (diagnosis.empty() || notes.empty()) {                                                          // 2
            sendError(res, 400, "Enter diagnosis and medical notes.");
            return;
        }
        if (!appointmentId.empty()) {                                                                      // 3
            Appointment* a = sys.findAppointment(appointmentId);
            bool valid = a != nullptr
                      && a->getDoctorId() == doctor->getId()
                      && a->getPatientId() == patient->getId()
                      && a->getStatus() == "Scheduled";
            if (!valid) { sendError(res, 400, "Invalid appointment selected."); return; }
        }

        MedicalVisit visit("", patient->getId(), doctor->getId(), appointmentId, today(), diagnosis, notes);
        string visitId = sys.addVisit(visit);                                                              // 4

        for (const string& serviceId : split(param(req, "services"), ',')) {                               // 5
            if (!serviceId.empty()) sys.addServiceToVisit(visitId, serviceId);
        }

        sendOk(res, JsonObject().text("visitId", visitId).raw("bill", billToJson(*sys.billForVisit(visitId))).build());   // 6
    }));

    /* SUGGEST A TEST/PROCEDURE LATER   POST /api/visits/<id>/services   (DOCTOR ONLY)
       Adds one more test/procedure to an EXISTING visit.
       Rule: a doctor may only change his or her OWN visits (else "not found").
       The price is added to the bill; a PAID bill refuses (message from
       addServiceToVisit). */
    svr.Post(R"(/api/visits/([^/]+)/services)", route(DOCTOR_ONLY, [](const Request& req, Response& res, User* doctor) {
        MedicalVisit* visit = sys.findVisit(req.matches[1]);
        if (visit == nullptr || visit->getDoctorId() != doctor->getId()) {
            sendError(res, 404, "Visit not found.");
            return;
        }
        string error = sys.addServiceToVisit(visit->getId(), param(req, "serviceId"));
        if (!error.empty()) {
            sendError(res, 400, error);
            return;
        }
        sendOk(res, billToJson(*sys.billForVisit(visit->getId())));
    }));


    /* =========================================================================
       FEATURE 7: BILLING   (Admin and Receptionist only - doctors cannot see bills)
       ========================================================================= */

    // LIST ALL BILLS   GET /api/bills
    svr.Get("/api/bills", route(ADMIN_OR_RECEPTIONIST, [](const Request&, Response& res, User*) {
        vector<string> list;
        for (Bill& b : sys.allBills()) list.push_back(billToJson(b));
        sendOk(res, jsonArray(list));
    }));

    // MARK PAID / UNPAID   POST /api/bills/<id>/paid   (send paid=1 or paid=0)
    svr.Post(R"(/api/bills/([^/]+)/paid)", route(ADMIN_OR_RECEPTIONIST, [](const Request& req, Response& res, User*) {
        Bill* bill = sys.findBill(req.matches[1]);
        if (bill == nullptr) {
            sendError(res, 404, "Bill not found.");
            return;
        }
        bill->setPaid(param(req, "paid") == "1");
        sys.save();
        sendOk(res, billToJson(*bill));
    }));

    /* ADD OTHER CHARGES   POST /api/bills/<id>/other   (for example a bed charge)
       STEPS: 1. bill must exist  2. bill must NOT be paid (paid bills are locked)
              3. amount must be a number above 0  4. add it to the other charges */
    svr.Post(R"(/api/bills/([^/]+)/other)", route(ADMIN_OR_RECEPTIONIST, [](const Request& req, Response& res, User*) {
        Bill* bill = sys.findBill(req.matches[1]);
        if (bill == nullptr) { sendError(res, 404, "Bill not found."); return; }                  // 1
        if (bill->isPaid())  { sendError(res, 400, "Bill is already paid."); return; }            // 2

        int amount = atoi(param(req, "amount").c_str());
        if (amount <= 0) { sendError(res, 400, "Enter a valid amount."); return; }                // 3

        bill->setOtherCharges(bill->getOtherCharges() + amount);                                  // 4
        sys.save();
        sendOk(res, billToJson(*bill));
    }));


    /* =========================================================================
       FEATURE 9: STAFF MANAGEMENT   (ADMIN ONLY)
       ========================================================================= */

    // LIST DOCTORS or RECEPTIONISTS   GET /api/staff/Doctor   or   /api/staff/Receptionist
    svr.Get(R"(/api/staff/([^/]+))", route(ADMIN_ONLY, [](const Request& req, Response& res, User*) {
        string role = req.matches[1];
        if (role != "Doctor" && role != "Receptionist") {
            sendError(res, 400, "Bad role.");
            return;
        }
        vector<string> list;
        for (User* u : sys.usersByRole(role)) list.push_back(userToJson(u));
        sendOk(res, jsonArray(list));
    }));

    /* ADD A DOCTOR OR RECEPTIONIST   POST /api/staff
       STEPS: 1. role must be Doctor or Receptionist
              2. name, username and password must be filled in
              3. the username must be new (usernames are unique)
              4. a Doctor also needs a specialization and a fee above 0
              5. create the right kind of object and store it (IDs D001 / R001 ...) */
    svr.Post("/api/staff", route(ADMIN_ONLY, [](const Request& req, Response& res, User*) {
        string role = param(req, "role");
        string username = param(req, "username");
        string password = param(req, "password");
        string name = param(req, "name");

        if (role != "Doctor" && role != "Receptionist") { sendError(res, 400, "Bad role."); return; }                       // 1
        if (username.empty() || password.empty() || name.empty()) { sendError(res, 400, "Please fill in all fields."); return; }   // 2
        if (sys.usernameTaken(username)) { sendError(res, 400, "Username already exists."); return; }                        // 3

        if (role == "Doctor") {
            string specialization = param(req, "specialization");
            int fee = atoi(param(req, "fee").c_str());
            if (specialization.empty() || fee <= 0) {                                                                         // 4
                sendError(res, 400, "Enter specialization and a valid fee.");
                return;
            }
            sys.addUser(make_unique<Doctor>(sys.nextUserId('D'), username, password, name, specialization, fee));            // 5
        } else {
            sys.addUser(make_unique<Receptionist>(sys.nextUserId('R'), username, password, name));
        }
        sendOk(res, "{}");
    }));

    // DELETE STAFF   POST /api/staff/<id>/delete
    // The Admin account can never be deleted here. A staff member with records
    // is refused by HospitalSystem::deleteUser (which returns the reason).
    svr.Post(R"(/api/staff/([^/]+)/delete)", route(ADMIN_ONLY, [](const Request& req, Response& res, User*) {
        User* staff = sys.findUser(req.matches[1]);
        if (staff == nullptr || staff->role() == "Admin") {
            sendError(res, 404, "Staff member not found.");
            return;
        }
        string error = sys.deleteUser(staff->getId());
        if (!error.empty()) {
            sendError(res, 400, error);
            return;
        }
        sendOk(res, "{}");
    }));

    // RESET A STAFF PASSWORD   POST /api/staff/<id>/password
    svr.Post(R"(/api/staff/([^/]+)/password)", route(ADMIN_ONLY, [](const Request& req, Response& res, User*) {
        User* staff = sys.findUser(req.matches[1]);
        string newPassword = param(req, "password");
        if (staff == nullptr || staff->role() == "Admin") {
            sendError(res, 404, "Staff member not found.");
            return;
        }
        if (newPassword.empty()) {
            sendError(res, 400, "Enter a password.");
            return;
        }
        staff->setPassword(newPassword);
        sys.save();
        sendOk(res, "{}");
    }));


    /* =========================================================================
       FEATURE 8: REPORTS   GET /api/reports   (ADMIN ONLY)
       Collects the numbers for the four reports:
         1. patients     - each patient and how many visits
         2. doctors      - each doctor with number of appointments and visits
         3. appointments - how many Scheduled / Completed / Cancelled
         4. billing      - bills count, money paid, money unpaid, total
       This route only READS data; it never changes anything.
       ========================================================================= */
    svr.Get("/api/reports", route(ADMIN_ONLY, [](const Request&, Response& res, User*) {
        // 1. patient report
        vector<string> patientRows;
        for (Patient& p : sys.allPatients()) {
            patientRows.push_back(JsonObject().text("id", p.getId()).text("name", p.getName())
                .number("visits", sys.visitsOf(p.getId()).size()).build());
        }

        // 2. doctor report (count this doctor's appointments and visits)
        vector<string> doctorRows;
        for (Doctor* d : sys.doctors()) {
            int appointmentCount = 0, visitCount = 0;
            for (Appointment& a : sys.allAppointments()) {
                if (a.getDoctorId() == d->getId()) appointmentCount++;
            }
            for (MedicalVisit& v : sys.allVisits()) {
                if (v.getDoctorId() == d->getId()) visitCount++;
            }
            doctorRows.push_back(JsonObject().text("id", d->getId()).text("name", d->getName())
                .text("specialization", d->getSpecialization())
                .number("appointments", appointmentCount).number("visits", visitCount).build());
        }

        // 3. appointment report (count by status)
        int scheduled = 0, completed = 0, cancelled = 0;
        for (Appointment& a : sys.allAppointments()) {
            if (a.getStatus() == "Scheduled") scheduled++;
            else if (a.getStatus() == "Completed") completed++;
            else cancelled++;
        }

        // 4. billing report (add up money of paid and unpaid bills)
        int paidAmount = 0, unpaidAmount = 0;
        for (Bill& b : sys.allBills()) {
            if (b.isPaid()) paidAmount += b.total();
            else            unpaidAmount += b.total();
        }

        string appointmentSummary = JsonObject().number("scheduled", scheduled)
            .number("completed", completed).number("cancelled", cancelled).build();
        string billingSummary = JsonObject().number("count", sys.allBills().size())
            .number("paid", paidAmount).number("unpaid", unpaidAmount)
            .number("total", paidAmount + unpaidAmount).build();

        sendOk(res, JsonObject()
            .raw("patients", jsonArray(patientRows))
            .raw("doctors", jsonArray(doctorRows))
            .raw("appointments", appointmentSummary)
            .raw("billing", billingSummary)
            .build());
    }));


    /* =========================================================================
       START THE SERVER
       STEPS:
         1. Try port 8080; if it is busy try 8081, 8082 ... up to 8099.
            "127.0.0.1" means: only THIS computer can connect (safer).
         2. Print the address and open the browser automatically.
         3. listen_after_bind() keeps the program running and answering requests
            until the window is closed.
       ========================================================================= */
    int port = 8080;
    while (port < 8100 && !svr.bind_to_port("127.0.0.1", port)) port++;      // step 1
    if (port >= 8100) {
        cerr << "Could not open a port.\n";
        return 1;
    }

    string url = "http://localhost:" + to_string(port);
    cout << "Hospital Management System running at " << url
         << "\nKeep this window open. Press Ctrl+C to stop." << endl;

#ifdef _WIN32
    system(("start " + url).c_str());                       // Windows: open the default browser
#elif __APPLE__
    system(("open " + url).c_str());                        // Mac
#else
    system(("xdg-open " + url + " >/dev/null 2>&1 &").c_str());   // Linux
#endif

    svr.listen_after_bind();                                // step 3
    return 0;
}
