#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "SteppingAction.hh"
#include "G4NistManager.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
	public:
		DetectorConstruction();
		virtual ~DetectorConstruction();

		virtual G4VPhysicalVolume *Construct();

	private:
		G4NistManager *fNistMan;
};

#endif
