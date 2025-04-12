#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4HEPEvtInterface.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4GeneralParticleSource.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction(), fGParticleSource(), fParticleGun()
{
	fGParticleSource = new G4GeneralParticleSource();
	fParticleGun = new G4ParticleGun();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete fGParticleSource;
	delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
	fGParticleSource->GeneratePrimaryVertex(anEvent);
}
