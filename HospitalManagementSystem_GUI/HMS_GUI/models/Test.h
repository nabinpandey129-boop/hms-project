/* =============================================================================
   FILE:    models/Test.h     (contains the base class Service and the class Test)
   PURPOSE: The hospital's price list: tests and procedures a doctor can order.

   FEATURE 6: TESTS AND PROCEDURES (PRICE LIST)

   KEY OOP IDEAS:
     1. ABSTRACT BASE CLASS - Service holds what every service has (id, name,
        price). It cannot be created directly because type() is "= 0".
     2. INHERITANCE - Test and Procedure (Procedure.h) are children of Service.
     3. POLYMORPHISM - service->type() gives "Test" or "Procedure" depending on
        the real object behind the pointer.
     4. The price lives HERE, in the system. A doctor never types a price, so
        billing mistakes cannot happen.
   ============================================================================= */
#pragma once
#include "Util.h"

class Service {
protected:
    string id, name;
    int price;   // in rupees

public:
    Service(string i, string n, int p) : id(i), name(n), price(p) {}
    virtual ~Service() {}

    virtual string type() const = 0;   // each child answers "Test" or "Procedure"

    string getId() const { return id; }
    string getName() const { return name; }
    int getPrice() const { return price; }

    // SAVE: one line of services.txt, e.g.  Test|T001|Blood Test|300
    string toRecord() const { return type() + "|" + id + "|" + name + "|" + to_string(price); }
};

// A laboratory / imaging test, e.g. Blood Test, X-Ray
class Test : public Service {
public:
    using Service::Service;   // reuse the Service constructor
    string type() const override { return "Test"; }
};
