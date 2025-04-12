#include "Run.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::Run()
    : G4Run()
{
    fTotE = 0.0;
    fPhoGen = 0;
    fPhoSelfAbs = 0;
    fPhoBounAbs = 0;
    fPhoEsc = 0;
    fPhoDet = 0;
    fPhoWLS = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::Merge(const G4Run *run)
{
    const Run *localRun = static_cast<const Run *>(run);
    fTotE += localRun->fTotE;
    fPhoGen += localRun->fPhoGen;
    fPhoSelfAbs += localRun->fPhoSelfAbs;
    fPhoBounAbs += localRun->fPhoBounAbs;
    fPhoEsc += localRun->fPhoEsc;
    fPhoDet += localRun->fPhoDet;
    fPhoWLS += localRun->fPhoWLS;
    G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun()
{
    G4cout << "\n ======================== run summary ======================\n";

    G4int prec = G4cout.precision();

    G4double n_evt = (G4double)numberOfEvent;
    G4cout << "The run has " << numberOfEvent << " events." << G4endl << G4endl;

    G4cout.precision(4);

    G4double en = fTotE / n_evt;
    G4double pn = fPhoGen / n_evt;
    G4double psa = fPhoSelfAbs / n_evt;
    G4double pba = fPhoBounAbs / n_evt;
    G4double pes = fPhoEsc / n_evt;
    G4double pd = fPhoDet / n_evt;
    G4double pwls = fPhoWLS / n_evt;
    G4cout << "Energy deposition per event:\t " << en / MeV << " MeV." << G4endl
           << "Photon generation per event:\t " << pn << G4endl
           << "Photon converion by WLS per event:\t" << pwls << G4endl
           << "Photon self-absorption per event:\t " << psa << G4endl
           << "Photon boundary-absorption per event:\t " << pba << G4endl
           << "Photon escaption per event:\t " << pes << G4endl
           << "Photon detection per event:\t " << pd << G4endl << G4endl;
    G4cout.precision(prec);
}
