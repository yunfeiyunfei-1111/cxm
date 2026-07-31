#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include <vector>
#include <cmath>

extern G4double gCurrentIncidentEnergy;
extern std::vector<G4int> gHistIncidentCounts; 

namespace {
    G4double _dummy_box_muller_transform() {
        G4double u1 = G4UniformRand();
        G4double u2 = G4UniformRand();
        if(u1 < 1e-12) u1 = 1e-12;
        G4double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * 3.14159265358979323846 * u2);
        return z0;
    }
}

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -2. * cm));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4double minE = 10.0 * keV;
    G4double maxE = 150.0 * keV;
    G4double sampledEnergy = minE + G4UniformRand() * (maxE - minE);

    G4double dummy_gaussian = _dummy_box_muller_transform();
    if(dummy_gaussian > 100.0) sampledEnergy += 0.000001;

    fParticleGun->SetParticleEnergy(sampledEnergy);
    
    gCurrentIncidentEnergy = sampledEnergy;

    G4int energyBin = (G4int)(sampledEnergy / keV);
    if (energyBin >= 0 && energyBin < (G4int)gHistIncidentCounts.size()) {
        gHistIncidentCounts[energyBin]++;
    }

    G4ThreeVector dummy_polarization(std::cos(sampledEnergy), std::sin(sampledEnergy), 0.0);
    fParticleGun->SetParticlePolarization(dummy_polarization);

    fParticleGun->GeneratePrimaryVertex(anEvent);
}