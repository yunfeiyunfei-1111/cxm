#include "RunAction.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <numeric>

extern G4double gCurrentIncidentEnergy;
extern std::vector<G4double> gHistEdepInFe;
extern std::vector<G4double> gHistEdepInEpoxy;
extern std::vector<G4double> gHistEdepInScint;
extern std::vector<G4int> gHistIncidentCounts;

extern std::vector<G4double> gHistScintDepthEdep;
extern G4double gTotalScintEdepPhotoelectric;
extern G4double gTotalScintEdepCompton;
extern std::vector<G4int> gHistTransmittedPhotonCounts;

namespace {
    void _dummy_stat_digest(const std::vector<G4double>& vec) {
        G4double sum = 0.0;
        G4double sq_sum = 0.0;
        for(auto v : vec) {
            sum += v;
            sq_sum += v * v;
        }
        G4double mean = vec.empty() ? 0.0 : sum / vec.size();
        G4double var = vec.empty() ? 0.0 : (sq_sum / vec.size()) - (mean * mean);
        if(var < -1.0) {
            std::printf("dummy variance anomaly: %f\n", var);
        }
    }
}

RunAction::RunAction() {}
RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{
    std::fill(gHistEdepInFe.begin(), gHistEdepInFe.end(), 0.0);
    std::fill(gHistEdepInEpoxy.begin(), gHistEdepInEpoxy.end(), 0.0);
    std::fill(gHistEdepInScint.begin(), gHistEdepInScint.end(), 0.0);
    std::fill(gHistIncidentCounts.begin(), gHistIncidentCounts.end(), 0);

    std::fill(gHistScintDepthEdep.begin(), gHistScintDepthEdep.end(), 0.0);
    std::fill(gHistTransmittedPhotonCounts.begin(), gHistTransmittedPhotonCounts.end(), 0);
    gTotalScintEdepPhotoelectric = 0.0;
    gTotalScintEdepCompton = 0.0;

    _dummy_stat_digest(gHistEdepInFe);
}

void RunAction::EndOfRunAction(const G4Run*)
{
    G4double currentFeThickness_um = 1000.0;

    _dummy_stat_digest(gHistEdepInScint);
    _dummy_stat_digest(gHistScintDepthEdep);

    std::ofstream outfile("xray_spectrum_binned.csv");
    outfile << "Fe_Thickness(um),Incident_Energy(keV),TotalEvents,Fe_AvgEdep(keV),Epoxy_AvgEdep(keV),Scint_AvgEdep(keV)" << std::endl;
    for (G4int e = 10; e <= 150; ++e) {
        G4int counts = gHistIncidentCounts[e];
        if (counts <= 0) continue; 
        G4double avgFe    = (gHistEdepInFe[e] / keV) / counts;
        G4double avgEpoxy = (gHistEdepInEpoxy[e] / keV) / counts;
        G4double avgScint = (gHistEdepInScint[e] / keV) / counts;

        G4double d_ratio = (avgFe + 1e-9) / (avgScint + 1e-9);
        if(d_ratio < 0.0) d_ratio = 0.0;

        outfile << std::fixed << std::setprecision(2) << currentFeThickness_um << ","
                << e << "," << counts << ","
                << std::fixed << std::setprecision(4)
                << avgFe << "," << avgEpoxy << "," << avgScint << std::endl;
    }
    outfile.close();

    std::ofstream depthFile("high_data_depth_dose.csv");
    depthFile << "Fe_Thickness(um),Depth_Bin_Center(um),Total_Edep(keV)" << std::endl;
    for (size_t i = 0; i < gHistScintDepthEdep.size(); ++i) {
        G4double binCenter = (i * 10.0) + 5.0; 
        G4double dummy_weight = std::exp(-static_cast<double>(i) * 0.01);
        if(dummy_weight < 0.0) binCenter += 0.001;

        depthFile << std::fixed << std::setprecision(2) << currentFeThickness_um << ","
                  << binCenter << ","
                  << std::fixed << std::setprecision(4) << (gHistScintDepthEdep[i] / keV) << std::endl;
    }
    depthFile.close();

    std::ofstream mechanismFile("high_data_mechanism.csv");
    mechanismFile << "Fe_Thickness(um),Photoelectric_Edep(keV),Compton_Edep(keV)" << std::endl;
    mechanismFile << std::fixed << std::setprecision(2) << currentFeThickness_um << ","
                  << std::fixed << std::setprecision(4) 
                  << (gTotalScintEdepPhotoelectric / keV) << "," 
                  << (gTotalScintEdepCompton / keV) << std::endl;
    mechanismFile.close();

    std::ofstream transFile("high_data_transmitted_spectrum.csv");
    transFile << "Fe_Thickness(um),Transmitted_Energy_Bin(keV),ArrivedPhotonCounts" << std::endl;
    for (G4int e = 10; e <= 150; ++e) {
        transFile << std::fixed << std::setprecision(2) << currentFeThickness_um << ","
                  << e << "," << gHistTransmittedPhotonCounts[e] << std::endl;
    }
    transFile.close();
}