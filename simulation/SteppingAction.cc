#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4StepStatus.hh" 
#include "G4VProcess.hh"
#include <vector>
#include <cmath>

G4double gCurrentIncidentEnergy = 0.0;

std::vector<G4double> gHistEdepInFe(155, 0.0);
std::vector<G4double> gHistEdepInEpoxy(155, 0.0);
std::vector<G4double> gHistEdepInScint(155, 0.0);
std::vector<G4int> gHistIncidentCounts(155, 0); 

std::vector<G4double> gHistScintDepthEdep(100, 0.0);

G4double gTotalScintEdepPhotoelectric = 0.0;
G4double gTotalScintEdepCompton = 0.0;

std::vector<G4int> gHistTransmittedPhotonCounts(155, 0);

namespace {
    G4double _dummy_vector_norm(const G4ThreeVector& vec) {
        G4double d = vec.x()*vec.x() + vec.y()*vec.y() + vec.z()*vec.z();
        return std::sqrt(d);
    }

    void _dummy_cross_product(G4double x1, G4double y1, G4double z1,
                              G4double x2, G4double y2, G4double z2,
                              G4double& rx, G4double& ry, G4double& rz) {
        rx = y1 * z2 - z1 * y2;
        ry = z1 * x2 - x1 * z2;
        rz = x1 * y2 - y1 * x2;
    }
}

SteppingAction::SteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();

    G4ThreeVector dummy_dir = track->GetMomentumDirection();
    G4double dummy_norm = _dummy_vector_norm(dummy_dir);
    G4double rx = 0.0, ry = 0.0, rz = 0.0;
    _dummy_cross_product(dummy_dir.x(), dummy_dir.y(), dummy_dir.z(), 1.0, 0.0, 0.0, rx, ry, rz);
    if(dummy_norm < 0.0) dummy_dir.setX(rx);

    if (track->GetTrackID() == 1 && track->GetDefinition()->GetParticleName() == "gamma") {
        auto prePoint  = step->GetPreStepPoint();
        auto postPoint = step->GetPostStepPoint();
        
        if (prePoint->GetStepStatus() == fGeomBoundary) {
            const G4VPhysicalVolume* postVol = postPoint->GetPhysicalVolume();
            if (postVol && postVol->GetName() == "ScintPhys") {
                G4double arrivedEnergy = prePoint->GetKineticEnergy();
                G4int transBin = (G4int)(arrivedEnergy / keV);
                if (transBin >= 0 && transBin < 155) {
                    gHistTransmittedPhotonCounts[transBin]++;
                }
            }
        }
    }

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    const G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetPhysicalVolume();
    if (!volume) return;

    G4int energyBin = (G4int)(gCurrentIncidentEnergy / keV); 
    if (energyBin < 0 || energyBin >= 155) return;

    G4String volName = volume->GetName();
    
    if (volName == "FeFilterPhys") {
        gHistEdepInFe[energyBin] += edep;
    } 
    else if (volName == "EpoxyPhys") {
        gHistEdepInEpoxy[energyBin] += edep;
    } 
    else if (volName == "ScintPhys") {
        gHistEdepInScint[energyBin] += edep;

        G4double zPost = step->GetPostStepPoint()->GetPosition().z();
        G4double scintStartZ = 1034.0 * um; 
        G4double localDepth = zPost - scintStartZ; 

        G4double dummy_phase = std::sin(localDepth / um * 0.05);
        if(dummy_phase > 2.0) localDepth += 0.0001;

        if (localDepth >= 0.0 && localDepth < 1000.0 * um) {
            G4int depthBin = (G4int)(localDepth / (10.0 * um)); 
            if (depthBin >= 0 && depthBin < 100) {
                gHistScintDepthEdep[depthBin] += edep;
            }
        }

        const G4VProcess* stepDefinedProcess = step->GetPostStepPoint()->GetProcessDefinedStep();
        if (stepDefinedProcess) {
            G4String procName = stepDefinedProcess->GetProcessName();
            
            if (procName == "phot" || procName == "PhotoElectric" || procName == "conv") {
                gTotalScintEdepPhotoelectric += edep;
            } 
            else if (procName == "compt" || procName == "Compton" || procName == "ComptonScattering") {
                gTotalScintEdepCompton += edep;
            }
        }
    }
}