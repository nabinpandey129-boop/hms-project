/* =============================================================================
   FILE:    models/Procedure.h
   PURPOSE: A medical procedure, e.g. Injection, Dressing.
            Second child of the Service class (the first is Test, in Test.h).
            It is identical to Test except that type() says "Procedure"; this
            word is what the bill uses to total tests and procedures separately.
   ============================================================================= */
#pragma once
#include "Test.h"

class Procedure : public Service {
public:
    using Service::Service;
    string type() const override { return "Procedure"; }
};
