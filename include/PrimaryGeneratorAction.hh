#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VPrimaryGenerator.hh"
#include "G4Event.hh"
#include "DetectorConstruction.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
	public:
		PrimaryGeneratorAction();
		virtual ~PrimaryGeneratorAction();
		virtual void GeneratePrimaries(G4Event*);
		G4GeneralParticleSource *GetParticleGun(){ return fGParticleSource; };
		void SetOptPhotonPolar();
		void GetOptPhotonPolar();
		void SetOptPhotonPolar(G4double);

	private:
		G4ParticleGun *fParticleGun;
		G4GeneralParticleSource *fGParticleSource;
};

#endif
