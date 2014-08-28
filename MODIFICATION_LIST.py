#!/usr/bin/env python

from subprocess import Popen
import os

#tagList = ['Alignment/CommonAlignment',
tagList = [
'Alignment/ReferenceTrajectories',
'Alignment/TrackerAlignment',
'CalibTracker/SiStripESProducers',
'CalibTracker/SiStripLorentzAngle',
'DataFormats/TrackerRecHit2D',
'DataFormats/TrackingRecHit',
'DataFormats/GeometryCommonDetAlgo',
'DataFormats/GeometrySurface',
'Geometry/TrackingGeometryAligner',
'FastSimulation/TrackingRecHitProducer',
'FastSimulation/TrajectoryManager',
'Fireworks/Geometry',
'Geometry/CSCGeometry',
'Geometry/CommonDetUnit',
'Geometry/DTGeometry',
'Geometry/GEMGeometry',
'Geometry/RPCGeometry',
'Geometry/TrackerGeometryBuilder',
'RecoLocalTracker/SiStripRecHitConverter',
'RecoMuon/TransientTrackingRecHit',
'RecoTracker/DebugTools',
'RecoTracker/MeasurementDet',
'RecoTracker/TransientTrackingRecHit',
'TrackingTools/TransientTrackingRecHit']

for subsystem in tagList:
    #Popen('git cms-addpkg '+subsystem,shell=True).wait()
    #Popen('cp -r /afs/cern.ch/cms/sw/ReleaseCandidates/vol0/slc6_amd64_gcc481/cms/cmssw-patch/CMSSW_7_2_X_2014-08-23-0200/src/'+subsystem+' .',shell=True).wait()
    print 'git cms-addpkg '+subsystem

fileModList = [
'DataFormats/GeometryCommonDetAlgo/interface/GlobalErrorBaseExtended.h',
'DataFormats/GeometryCommonDetAlgo/interface/GlobalError.h',
'DataFormats/GeometrySurface/interface/LocalErrorBaseExtended.h',
'DataFormats/GeometryCommonDetAlgo/interface/LocalError.h',
'DataFormats/GeometryCommonDetAlgo/interface/ErrorFrameTransformer.h',
'DataFormats/TrackingRecHit/interface/AlignmentPositionError.h',
'Geometry/TrackingGeometryAligner/interface/GeometryAligner.h']

INIT_PATH = '/afs/cern.ch/user/a/asvyatko/APEStudyDev_Repo/CMSSW_7_1_0_pre9/src/'

for file in fileModList:
    Popen('cp '+INIT_PATH+file+' '+file,'shell=True').wait()
    #print 'cp '+INIT_PATH+file+' '+file

#RecoMuon/TransientTrackingRecHit/src/MuonTransientTrackingRecHit.cc
#Geometry/CommonDetUnit/src/GeomDet.cc
