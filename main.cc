#include "G4MTRunManager.hh"
#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "TrackingAction.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"

#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

namespace
{
  const G4String ECALVisManagerVerboseLevel("errors");
}

struct ECALTerminalData
{
  ECALTerminalData()
      : isInteractive(false), executeable_macro(""), output_file("ECAL.root"),
        N_threads(std::min(G4Threading::G4GetNumberOfCores(), 8)),
        use_optical(false) {}
  G4bool isInteractive;
  G4String executeable_macro;
  G4String output_file;
  G4int N_threads;
  G4bool use_optical;
};

void printUsage(void)
{
  const G4String progName("ECAL");
  G4cout << "\nUsage: ./" << progName <<
#ifdef G4UI_USE_QT
      " [-i] "
#endif
      "[-e executeable_marco] [-o output_file] [-n N_threads] [-op]"
         << G4endl;
  G4cout << "or     ./" << progName << " [--help | -h]" << G4endl;
#ifdef G4UI_USE_QT
  G4cout << "         -i          - Run in interactive mode" << G4endl;
#endif
  G4cout << "         -e          - Execute specified marco file " << G4endl;
  G4cout << "         -o          - Specify the name of output file (default: "
            "ECAL.root )"
         << G4endl;
  G4cout << "         -n          - Number of threads to use (default: 8 )"
         << G4endl;
  G4cout << "         -op         - Use PhysicsList with optics" << G4endl;
  G4cout << "         --help | -h - Print this message and exit " << G4endl;
}

G4bool parseArgs(int argc, char **argv, ECALTerminalData &cmd)
{
  if (argc < 2)
    return false;
  for (G4int i = 1; i < argc; ++i)
  {
    do
    {
      if (G4String(argv[i]) == "--help" || G4String(argv[i]) == "-h")
        return false;
      if (G4String(argv[i], 3) == "-op")
      {
        cmd.use_optical = true;
        G4cout << "Use Physicslist with optics" << G4endl;
      }
#ifdef G4UI_USE_QT
      if (G4String(argv[i], 2) == "-i")
      {
        cmd.isInteractive = true;
        G4cout << "Start under interactive mode" << G4endl;
        break;
      }
#endif
      if (G4String(argv[i], 2) == "-e")
      {
        G4String tmp = argv[i] + 2;
        G4cout << tmp << G4endl;
        if (tmp == "")
        {
          if (++i >= argc)
          {
            return false;
          }
          cmd.executeable_macro = argv[i];
        }
        else
          cmd.executeable_macro = tmp;
        G4cout << "Executeable_macro name is:   " << cmd.executeable_macro
               << G4endl;
        break;
      }
      if (G4String(argv[i], 2) == "-o" && G4String(argv[i], 3) != "-op")
      {
        G4String tmp = argv[i] + 2;
        if (tmp == "")
        {
          if (++i < argc)
          {
            cmd.output_file = argv[i];
          }
        }
        else
        {
          cmd.output_file = tmp;
        }
        G4cout << "    Output file's name is:   " << cmd.output_file << G4endl;
        break;
      }
#ifdef G4MULTITHREADED
      if (G4String(argv[i], 2) == "-n")
      {
        if (G4String(argv[i] + 2) == "")
        {
          if (++i < argc)
          {
            cmd.N_threads = G4UIcommand::ConvertToInt(argv[i]);
          }
        }
        else
        {
          cmd.N_threads = G4UIcommand::ConvertToInt(argv[i] + 2);
        }
        G4cout << "     Number of threads is:   " << cmd.N_threads << G4endl;
        break;
      }
#endif
    } while (false);
  }
  return true;
}

int main(int argc, char **argv)
{
  ECALTerminalData cmd;
  {
    if (!parseArgs(argc, argv, cmd))
    {
      printUsage();
      return 0;
    }
  }

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  G4long seed;
  int fd;
  fd = open("/dev/random", 0);
  read(fd, &seed, sizeof(seed));
  seed += time(NULL);
  CLHEP::HepRandom::setTheSeed(seed);
  CLHEP::HepRandom::showEngineStatus();
#ifdef G4MULTITHREADED
  G4MTRunManager *runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(cmd.N_threads);
  G4cout
      << "Multi-Threads Mode \033[47m \033[32m ON \033[0m ; Number of Threads: "
      << cmd.N_threads << G4endl;
#else
  G4RunManager *runManager = new G4RunManager;
  G4cout << "Multi-Threads Mode \033[47m \033[32m OFF \033[0m" << G4endl;
#endif

  DetectorConstruction *detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Physicslist
  G4VModularPhysicsList *physicsList = new QGSP_BERT(0);
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4(0));
  G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();
  if (cmd.use_optical)
  {
    opticalPhysics->SetWLSTimeProfile("exponential");
    opticalPhysics->SetScintillationYieldFactor(1.0);
    opticalPhysics->SetMaxNumPhotonsPerStep(100);
    opticalPhysics->SetMaxBetaChangePerStep(10.0);
    opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
    opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);
    physicsList->RegisterPhysics(opticalPhysics);
    G4cout << "Register optical physics successfully" << G4endl;
  }
  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new ActionInitialization(cmd.output_file));

  runManager->Initialize();
  G4UImanager *UI = G4UImanager::GetUIpointer();

  // #ifdef G4VIS_USE
  G4VisManager *visManager = new G4VisExecutive;
  visManager->Initialize();
  // #endif

  if (cmd.executeable_macro != "")
  {
    G4String command = "/control/execute ";
    G4String macname = cmd.executeable_macro;
    UI->ApplyCommand(command + macname);
  }
  else
  {
    // #ifdef G4UI_USE
    G4UIExecutive *ui = new G4UIExecutive(argc, argv);
    // #ifdef G4VIS_USE
    UI->ApplyCommand("/control/execute vis.mac");
    // #endif
    ui->SessionStart();
    delete ui;
    // #endif
  }
  // #ifdef G4VIS_USE
  G4cout << "delete visManager" << G4endl;
  delete visManager;
  // #endif
  G4cout << "delete runManager" << G4endl;
  delete runManager;
  G4cout << "time consumption: " << std::clock() / CLOCKS_PER_SEC / cmd.N_threads << "s." << G4endl;
  return 0;
}
