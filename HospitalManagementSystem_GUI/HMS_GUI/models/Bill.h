/* =============================================================================
   FILE:    models/Bill.h
   PURPOSE: The bill of one medical visit.

   FEATURE 7: BILLING  (data and calculation part)

   HOW A BILL IS BUILT (explain this to the examiner):
     1. When a doctor saves a visit, a Bill is created that already contains the
        doctor's CONSULTATION FEE.
     2. Each test/procedure the doctor adds becomes a BillItem (name + price).
     3. The total is CALCULATED, never typed:
            total = consultation fee + test charges + procedure charges
                    + other charges
     4. Example: fee 500 + Blood Test 300 + X-Ray 800 = Rs. 1600.
     5. Once the bill is marked PAID it is locked (see HospitalSystem.h).

   WHY BillItem COPIES THE NAME AND PRICE: if the price list changes later, old
   bills must not change. So each item stores its own copy of the price.
   ============================================================================= */
#pragma once
#include "Util.h"

// One line on the bill, e.g. "X-Ray", 800, "Test"
struct BillItem {
    string name;
    int price;
    string kind;   // "Test" or "Procedure"
};

class Bill {
private:
    string id, patientId, visitId, date;
    int consultationFee;       // the doctor's fee, set when the bill is created
    int otherCharges;          // extra amounts added by reception (e.g. bed charge)
    bool paid;                 // false = Unpaid, true = Paid
    vector<BillItem> items;    // the tests and procedures

public:
    Bill() : consultationFee(0), otherCharges(0), paid(false) {}
    Bill(string i, string p, string v, string d, int fee)
        : id(i), patientId(p), visitId(v), date(d), consultationFee(fee), otherCharges(0), paid(false) {}

    // getters
    string getId() const { return id; }
    string getPatientId() const { return patientId; }
    string getVisitId() const { return visitId; }
    string getDate() const { return date; }
    int getConsultationFee() const { return consultationFee; }
    int getOtherCharges() const { return otherCharges; }
    bool isPaid() const { return paid; }
    const vector<BillItem>& getItems() const { return items; }

    // setters / changers
    void setPaid(bool value) { paid = value; }
    void setOtherCharges(int amount) { otherCharges = amount; }
    // addItem(): put one test/procedure on the bill (name is cleaned for the file)
    void addItem(const string& name, int price, const string& kind) {
        BillItem item = { clean(name), price, kind };
        items.push_back(item);
    }

    // ---- THE CALCULATION ----
    // Add up the prices of all items of one kind
    int testCharges() const {
        int sum = 0;
        for (const BillItem& item : items) {
            if (item.kind == "Test") sum += item.price;
        }
        return sum;
    }
    int procedureCharges() const {
        int sum = 0;
        for (const BillItem& item : items) {
            if (item.kind == "Procedure") sum += item.price;
        }
        return sum;
    }
    // The final amount to pay
    int total() const {
        return consultationFee + testCharges() + procedureCharges() + otherCharges;
    }

    // SAVE: object -> one line of bills.txt.
    // The items are written as  name~price~kind  and separated by ;
    //   B001|P001|V001|2026-10-02|500|0|0|Blood Test~300~Test;X-Ray~800~Test
    //   (the "0" before the items = unpaid; "1" would mean paid)
    string toRecord() const {
        string itemText;
        for (size_t i = 0; i < items.size(); i++) {
            if (i > 0) itemText += ";";
            itemText += items[i].name + "~" + to_string(items[i].price) + "~" + items[i].kind;
        }
        return id + "|" + patientId + "|" + visitId + "|" + date + "|" + to_string(consultationFee) + "|" +
               to_string(otherCharges) + "|" + (paid ? "1" : "0") + "|" + itemText;
    }

    // LOAD: one line of text -> object
    static Bill fromRecord(const string& line) {
        vector<string> f = split(line, '|', 8);
        Bill bill(f[0], f[1], f[2], f[3], atoi(f[4].c_str()));
        bill.otherCharges = atoi(f[5].c_str());
        bill.paid = (f[6] == "1");
        if (!f[7].empty()) {
            vector<string> itemTexts = split(f[7], ';');          // one text per item
            for (const string& itemText : itemTexts) {
                vector<string> part = split(itemText, '~', 3);    // name, price, kind
                BillItem item = { part[0], atoi(part[1].c_str()), part[2] };
                bill.items.push_back(item);
            }
        }
        return bill;
    }
};
