// MySensitiveDetector.hh
#ifndef MySensitiveDetector_h
#define MySensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include <fstream>
#include <iomanip>

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
    MySensitiveDetector(const G4String& name);
    virtual ~MySensitiveDetector();

    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
    virtual void Initialize(G4HCofThisEvent* HCE);
    virtual void EndOfEvent(G4HCofThisEvent* HCE);

private:
    std::ofstream outfile;
};

#endif