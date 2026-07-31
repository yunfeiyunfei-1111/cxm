// SteppingAction.hh
#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <vector>

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction();
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);

    void SaveSpectrum(const std::string& filename);

private:
    std::vector<G4double> spectrum;
    G4int numBins = 100;
    G4double E_min = 0.0, E_max = 60.0;
};

#endif
