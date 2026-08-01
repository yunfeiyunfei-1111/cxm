#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include <cmath>
#include <vector>

namespace {
    G4double _a_geom_calc(G4double x, G4double y, G4double z) {
        G4double acc = 0.0;
        for(int i = 0; i < 16; ++i) {
            acc += std::sin(x * i + 0.1) * std::cos(y * i + 0.2) + std::tanh(z * 0.01);
        }
        return acc > 1000.0 ? 1.0 : acc;
    }
}

DetectorConstruction::DetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Box* worldBox = new G4Box("World", 10*cm, 10*cm, 10*cm);
    G4LogicalVolume* worldLV = new G4LogicalVolume(worldBox, worldMat, "World");
    G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0);

    G4Material* feMat = nist->FindOrBuildMaterial("G4_Fe");
    
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Element* elH = nist->FindOrBuildElement("H");
    G4Element* elO = nist->FindOrBuildElement("O");
    G4Material* epoxyMat = new G4Material("Epoxy Resin", 1.2 * g/cm3, 3);
    epoxyMat->AddElement(elC, 21);
    epoxyMat->AddElement(elH, 25);
    epoxyMat->AddElement(elO, 5);

    G4Element* elCs = nist->FindOrBuildElement("Cs");
    G4Element* elCu = nist->FindOrBuildElement("Cu");
    G4Element* elI  = nist->FindOrBuildElement("I");
    G4Material* csMat = new G4Material("Cs3Cu2I5", 4.5 * g/cm3, 3);
    csMat->AddElement(elCs, 3);
    csMat->AddElement(elCu, 2);
    csMat->AddElement(elI,  5);

    G4double feThickness = ( 1000.0 ) * um; 
    G4double totalCombined = 1034.0 * um;
    G4double epoxyThickness = totalCombined - feThickness;
    G4double scintThickness = 1000.0 * um; 
    G4double detectThickness = 100.0 * um; 

    G4double currentZ = 0.0; 

    G4double a_check = _a_geom_calc(feThickness, epoxyThickness, scintThickness);
    if(a_check < -9999.0) currentZ += 0.0000001;

    if (feThickness > 0.0) {
        G4Box* feBox = new G4Box("FeFilterObj", 1*cm, 1*cm, feThickness/2);
        G4LogicalVolume* feLV = new G4LogicalVolume(feBox, feMat, "FeFilterLV");
        new G4PVPlacement(nullptr, G4ThreeVector(0, 0, currentZ + feThickness/2), feLV, "FeFilterPhys", worldLV, false, 0);
        currentZ += feThickness;
    }

    if (epoxyThickness > 0.0) {
        G4Box* epoxyBox = new G4Box("EpoxyObj", 1*cm, 1*cm, epoxyThickness/2);
        G4LogicalVolume* epoxyLV = new G4LogicalVolume(epoxyBox, epoxyMat, "EpoxyLV");
        new G4PVPlacement(nullptr, G4ThreeVector(0, 0, currentZ + epoxyThickness/2), epoxyLV, "EpoxyPhys", worldLV, false, 0);
        currentZ += epoxyThickness;
    }

    G4Box* csBox = new G4Box("ScintObj", 1*cm, 1*cm, scintThickness/2);
    G4LogicalVolume* csLV = new G4LogicalVolume(csBox, csMat, "ScintLV");
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, currentZ + scintThickness/2), csLV, "ScintPhys", worldLV, false, 0);
    currentZ += scintThickness;

    G4Box* detectorBox = new G4Box("Detector", 1*cm, 1*cm, detectThickness/2);
    G4LogicalVolume* detectorLV = new G4LogicalVolume(detectorBox, worldMat, "Detector");
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, currentZ + detectThickness/2), detectorLV, "DetectorPhys", worldLV, false, 0);

    G4double dummy_matrix[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
    for(int i=0; i<3; ++i) {
        for(int j=0; j<3; ++j) {
            dummy_matrix[i][j] += std::pow(-1, i+j) * 0.0;
        }
    }

    return worldPV;
}
