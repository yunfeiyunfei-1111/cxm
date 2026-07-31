// ----------- main.cc -----------
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4PhysListFactory.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
    G4UIExecutive* ui = nullptr;
    if (argc == 1) ui = new G4UIExecutive(argc, argv);

    auto runManager = new G4RunManager;

    // Set Detector Construction
    runManager->SetUserInitialization(new DetectorConstruction());

    // Set Physics List
    G4PhysListFactory factory;
    auto physicsList = factory.GetReferencePhysList("FTFT_BERT_LIV");
    runManager->SetUserInitialization(physicsList);

    // Set Action Initialization
    runManager->SetUserInitialization(new ActionInitialization());

    runManager->Initialize();

    auto UImanager = G4UImanager::GetUIpointer();

    if (ui) {
        // UI 模式启用可视化
        G4VisManager* visManager = new G4VisExecutive;
        visManager->Initialize();
        UImanager->ApplyCommand("/control/execute run.mac"); // Assuming run.mac exists for UI mode
        ui->SessionStart();
        delete visManager;
        delete ui;
    } else {
        // 批处理模式运行宏文件
        if (argc >= 2) {
            G4String command = "/control/execute ";
            G4String filename = argv[1];
            UImanager->ApplyCommand(command + filename);
        }
    }

    delete runManager;
    return 0;
}