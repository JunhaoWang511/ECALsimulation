//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#include "G4VUserTrackInformation.hh"
#include "globals.hh"

#ifndef TrackInformation_h
#define TrackInformation_h 1

enum TrackStatus
{
    active,
    detected,
    absorbed,
    boundaryAbsorbed,
    escaped
};

/*TrackStatus of optical photons at the end of its track:
  active: still being tracked
  detected: stopped by being detected in an APD
  absorbed: stopped by being absorbed with G4OpAbsorbtion
  boundaryAbsorbed: stopped by being aborbed with Absorption in G4OpBoundaryProcessStatus
  escaped: stopped by being escaped from world volume
*/

class TrackInformation : public G4VUserTrackInformation
{
public:
    TrackInformation();
    virtual ~TrackInformation();

    // Sets the track status to s
    void SetTrackStatusFlag(int s) { fStatus = s; }
    int GetTrackStatus() const { return fStatus; }
    // Global life time of an optical photon
    void SetTrackTime(G4double time) { fTime = time; }
    G4double GetTrackTime() { return fTime; }

private:
    int fStatus;
    G4double fTime;
};

#endif
