#ifndef Geometry_TrackingGeometryAligner_GeometryAligner_h
#define Geometry_TrackingGeometryAligner_GeometryAligner_h

#include <vector>
#include <algorithm>
#include <iterator>

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Geometry/CommonDetUnit/interface/DetPositioner.h"

#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignmentErrors.h"
#include "CondFormats/Alignment/interface/AlignTransform.h"
#include "CondFormats/Alignment/interface/AlignTransformError.h"
#include "CondFormats/Alignment/interface/AlignmentSurfaceDeformations.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "DataFormats/TrackingRecHit/interface/AlignmentPositionError.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/GlobalError.h"
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "Geometry/CommonTopologies/interface/SurfaceDeformationFactory.h"
#include "Geometry/CommonTopologies/interface/SurfaceDeformation.h"

//FIXME
#include "DataFormats/GeometryCommonDetAlgo/interface/ErrorFrameTransformer.h"
#include "DataFormats/DetId/interface/DetId.h"

#include <iostream>
#include <fstream>
#include <string>
#include "TRandom.h"


class Alignments;
class AlignmentSurfaceDeformations;

/// Class to update a given geometry with a set of alignments

class GeometryAligner : public DetPositioner { 

public:
  template<class C> 
  void applyAlignments( C* geometry,
			const Alignments* alignments,
			const AlignmentErrors* alignmentErrors,
			const AlignTransform& globalCoordinates );

  template<class C> 
  void attachSurfaceDeformations( C* geometry,
				  const AlignmentSurfaceDeformations* surfaceDeformations );

  inline void removeGlobalTransform( const Alignments* alignments,
                                     const AlignmentErrors* alignmentErrors,
                                     const AlignTransform& globalCoordinates,
                                     Alignments* newAlignments,
                                     AlignmentErrors* newAlignmentErrors );
};


template<class C>
void GeometryAligner::applyAlignments( C* geometry,
				       const Alignments* alignments,
				       const AlignmentErrors* alignmentErrors,
				       const AlignTransform& globalCoordinates )
{

  edm::LogInfo("Alignment") << "@SUB=GeometryAligner::applyAlignments" 
			    << "Starting to apply alignments.";

  // Preliminary checks (or we can't loop!)
  if ( alignments->m_align.size() != geometry->theMap.size() )
	throw cms::Exception("GeometryMismatch") 
	  << "Size mismatch between geometry (size=" << geometry->theMap.size() 
	  << ") and alignments (size=" << alignments->m_align.size() << ")";
  if ( alignments->m_align.size() != alignmentErrors->m_alignError.size() )
	throw cms::Exception("GeometryMismatch") 
	  << "Size mismatch between geometry (size=" << geometry->theMap.size() 
	  << ") and alignment errors (size=" << alignmentErrors->m_alignError.size() << ")";

  const AlignTransform::Translation &globalShift = globalCoordinates.translation();
  const AlignTransform::Rotation globalRotation = globalCoordinates.rotation(); // by value!
  const AlignTransform::Rotation inverseGlobalRotation = globalRotation.inverse();

  //FIXME test setup to read APEs from ASCIII file, if need be
  std::ifstream apeReadFile("/afs/cern.ch/user/a/asvyatko/public/APEList66.txt");

  //FIXME read in the APEs from ASCII file
  std::map<int,GlobalErrorExtended> apeDict;
  while (!apeReadFile.eof()) {
    int apeId=0; double xx,xy,xz,xphix,xphiy,xphiz,yy,yz,yphix,yphiy,yphiz,zz,zphix,zphiy,zphiz,phixphix,phixphiy,phixphiz,phiyphiy,phiyphiz,phizphiz;
    apeReadFile>>apeId>>xx>>xy>>xz>>xphix>>xphiy>>xphiz>>yy>>yz>>yphix>>yphiy>>yphiz>>zz>>zphix>>zphiy>>zphiz>>phixphix>>phixphiy>>phixphiz>>phiyphiy>>phiyphiz>>phizphiz>>std::ws;
    GlobalErrorExtended error_tmp(xx,xy,xz,xphix,xphiy,xphiz,yy,yz,yphix,yphiy,yphiz,zz,zphix,zphiy,zphiz,phixphix,phixphiy,phixphiz,phiyphiy,phiyphiz,phizphiz);
    apeDict[apeId] = error_tmp;
  }
  apeReadFile.close();


  // Parallel loop on alignments, alignment errors and geomdets
  std::vector<AlignTransform>::const_iterator iAlign = alignments->m_align.begin();
  std::vector<AlignTransformError>::const_iterator 
	iAlignError = alignmentErrors->m_alignError.begin();
  //copy  geometry->theMap to a real map to order it....
  std::map<unsigned int, GeomDet const *> theMap;
  std::copy(geometry->theMap.begin(), geometry->theMap.end(), std::inserter(theMap,theMap.begin()));
  unsigned int nAPE = 0;
  for ( auto iPair = theMap.begin(); 
	iPair != theMap.end(); ++iPair, ++iAlign, ++iAlignError )
	{
	  // Check DetIds
	  if ( (*iPair).first != (*iAlign).rawId() )
	    throw cms::Exception("GeometryMismatch") 
	      << "DetId mismatch between geometry (rawId=" << (*iPair).first
	      << ") and alignments (rawId=" << (*iAlign).rawId();
	  
	  if ( (*iPair).first != (*iAlignError).rawId() )
	    throw cms::Exception("GeometryMismatch") 
	      << "DetId mismatch between geometry (rawId=" << (*iPair).first
	      << ") and alignment errors (rawId=" << (*iAlignError).rawId();

	  // Apply global correction
	  CLHEP::Hep3Vector positionHep = globalRotation * CLHEP::Hep3Vector( (*iAlign).translation() ) + globalShift;
	  CLHEP::HepRotation rotationHep = CLHEP::HepRotation( (*iAlign).rotation() )  * inverseGlobalRotation;

	  // Define new position/rotation objects and apply
	  Surface::PositionType position( positionHep.x(), positionHep.y(), positionHep.z() );
	  Surface::RotationType rotation( rotationHep.xx(), rotationHep.xy(), rotationHep.xz(), 
					  rotationHep.yx(), rotationHep.yy(), rotationHep.yz(), 
					  rotationHep.zx(), rotationHep.zy(), rotationHep.zz() );
	  GeomDet* iGeomDet = const_cast<GeomDet*>((*iPair).second);

          Surface::PositionType posOld = iGeomDet->position(); 
          AlgebraicVector vOld(3,0);
          vOld[0] = posOld.x(); 
          vOld[1] = posOld.y();
          vOld[2] = posOld.z();

          Surface::RotationType rotOld = iGeomDet->rotation();
          AlgebraicMatrix amOld(3,3,0);
          amOld[0][0] = rotOld.xx(); amOld[0][1] = rotOld.xy(); amOld[0][2] = rotOld.xz();
          amOld[1][0] = rotOld.yx(); amOld[1][1] = rotOld.yy(); amOld[1][2] = rotOld.yz();
          amOld[2][0] = rotOld.zx(); amOld[2][1] = rotOld.zy(); amOld[2][2] = rotOld.zz();

	  this->setGeomDetPosition( *iGeomDet, position, rotation );

          Surface::PositionType posNew = iGeomDet->position();
          AlgebraicVector vNew(3,0);
          vNew[0] = posNew.x();
          vNew[1] = posNew.y();
          vNew[2] = posNew.z();

          Surface::RotationType rotNew = iGeomDet->rotation();
          AlgebraicMatrix amNew(3,3,0);
          amNew[0][0] = rotNew.xx(); amNew[0][1] = rotNew.xy(); amNew[0][2] = rotNew.xz();
          amNew[1][0] = rotNew.yx(); amNew[1][1] = rotNew.yy(); amNew[1][2] = rotNew.yz();
          amNew[2][0] = rotNew.zx(); amNew[2][1] = rotNew.zy(); amNew[2][2] = rotNew.zz();

          //Get the alignment parameters
          AlgebraicVector shifts(3,0);
          shifts = vNew - vOld; 
          AlgebraicMatrix am(3,3);
          am = amOld*amNew.T();
          //now phiz is 01
          //phiy is 02
          //phix is 21
          AlgebraicVector angles(3,0);
          angles[0] = am[2][1]; 
          angles[1] = am[0][2];
          angles[2] = am[0][1];

          int reference = (iGeomDet->geographicalId()).rawId();
          GlobalErrorExtended error = apeDict[reference];
          LocalErrorExtended newError = ErrorFrameTransformer().transform46(error,shifts,angles);

	  AlignmentPositionError ape( newError );
	  if (this->setAlignmentPositionError( *iGeomDet, ape ))
	    ++nAPE;

	}

        edm::LogInfo("Alignment") << "@SUB=GeometryAligner::applyAlignments" 
        			    << "Finished to apply " << theMap.size() << " alignments with "
        		    << nAPE << " non-zero APE.";
}


template<class C>
void GeometryAligner::attachSurfaceDeformations( C* geometry,
						 const AlignmentSurfaceDeformations* surfaceDeformations )
{
  edm::LogInfo("Alignment") << "@SUB=GeometryAligner::attachSurfaceDeformations" 
			    << "Starting to attach surface deformations.";

  //copy geometry->theMapUnit to a real map to order it....
  std::map<unsigned int, GeomDetUnit const*> theMap;
  std::copy(geometry->theMapUnit.begin(), geometry->theMapUnit.end(), std::inserter(theMap, theMap.begin()));
  
  unsigned int nSurfDef = 0;
  unsigned int itemIndex = 0;
  auto iPair = theMap.begin();
  for ( std::vector<AlignmentSurfaceDeformations::Item>::const_iterator iItem = surfaceDeformations->items().begin();
	iItem != surfaceDeformations->items().end();
	++iItem, ++iPair) {
    
    // Check DetIds
    // go forward in map of GeomDetUnits until DetId is found
    while ( (*iPair).first != (*iItem).m_rawId ) {

      // remove SurfaceDeformation from GeomDetUnit (i.e. set NULL pointer)
      GeomDetUnit* geomDetUnit = const_cast<GeomDetUnit*>((*iPair).second);
      this->setSurfaceDeformation( *geomDetUnit, 0 );

      ++iPair;
      if ( iPair==theMap.end() )
	throw cms::Exception("GeometryMismatch") 
	  << "GeomDetUnit with rawId=" << (*iItem).m_rawId
	  << " not found in geometry";
    }
    
    // get the parameters and put them into a vector
    AlignmentSurfaceDeformations::ParametersConstIteratorPair iteratorPair = surfaceDeformations->parameters(itemIndex);
    std::vector<double> parameters;
    std::copy(iteratorPair.first, iteratorPair.second, std::back_inserter(parameters));
    
    // create SurfaceDeformation via factory
    SurfaceDeformation * surfDef = SurfaceDeformationFactory::create( (*iItem).m_parametrizationType, parameters);
    GeomDetUnit* geomDetUnit = const_cast<GeomDetUnit*>((*iPair).second);
    this->setSurfaceDeformation( *geomDetUnit, surfDef );
    // delete is not needed since SurfaceDeformation is passed as a
    // DeepCopyPointerByClone which takes over ownership. Needs to be
    // cleaned up and checked once SurfaceDeformation are moved to
    // proxy topology classes
    //delete surfDef; 

    ++nSurfDef;
    
    ++itemIndex;
  }
  
  edm::LogInfo("Alignment") << "@SUB=GeometryAligner::attachSurfaceDeformations" 
			    << "Finished to attach " << nSurfDef << " surface deformations.";
}

void GeometryAligner::removeGlobalTransform( const Alignments* alignments,
                                             const AlignmentErrors* alignmentErrors,
                                             const AlignTransform& globalCoordinates,
                                             Alignments* newAlignments,
                                             AlignmentErrors* newAlignmentErrors )
{
  edm::LogInfo("Alignment") << "@SUB=GeometryAligner::removeGlobalTransform" 
			    << "Starting to remove global position from alignments and errors";
  
  if ( alignments->m_align.size() != alignmentErrors->m_alignError.size() )
    throw cms::Exception("GeometryMismatch") 
      << "Size mismatch between alignments (size=" << alignments->m_align.size()
      << ") and alignment errors (size=" << alignmentErrors->m_alignError.size() << ")";
  
  const AlignTransform::Translation &globalShift = globalCoordinates.translation();
  const AlignTransform::Rotation globalRotation = globalCoordinates.rotation(); // by value!
  const AlignTransform::Rotation inverseGlobalRotation = globalRotation.inverse();
  
  AlignTransform::Translation newPosition;
  AlignTransform::Rotation newRotation;
  
  std::vector<AlignTransform>::const_iterator iAlign = alignments->m_align.begin();
  std::vector<AlignTransformError>::const_iterator iAlignError = alignmentErrors->m_alignError.begin();
  unsigned int nAPE = 0;
  for ( iAlign = alignments->m_align.begin();
        iAlign != alignments->m_align.end();
        ++iAlign, ++iAlignError ) {
    
    // Remove global position transformation from alignment
    newPosition = inverseGlobalRotation * ( (*iAlign).translation() - globalShift );
    newRotation = (*iAlign).rotation() * globalRotation;
    
    newAlignments->m_align.push_back( AlignTransform(newPosition,
                                                     newRotation,
                                                     (*iAlign).rawId()) );
    
    // Don't remove global position transformation from APE
    // as it wasn't applied. Just fill vector with original
    // values
    GlobalError error( asSMatrix<3>((*iAlignError).matrix()) );
    newAlignmentErrors->m_alignError.push_back( AlignTransformError( (*iAlignError).matrix(),
								     (*iAlignError).rawId() ) );
    if ( error.cxx() || error.cyy() || error.czz() ||
	 error.cyx() || error.czx() || error.czy() ) {
      ++nAPE;
    }

    // Code that removes the global postion transformation
    // from the APE.
    //     
    //AlgebraicSymMatrix as(3,0);
    //as[0][0] = error.cxx();
    //as[1][0] = error.cyx(); as[1][1] = error.cyy();
    //as[2][0] = error.czx(); as[2][1] = error.czy(); as[2][2] = error.czz();
    
    //AlgebraicMatrix am(3,3);
    //am[0][0] = inverseGlobalRotation.xx(); am[0][1] = inverseGlobalRotation.xy(); am[0][2] = inverseGlobalRotation.xz();
    //am[1][0] = inverseGlobalRotation.yx(); am[1][1] = inverseGlobalRotation.yy(); am[1][2] = inverseGlobalRotation.yz();
    //am[2][0] = inverseGlobalRotation.zx(); am[2][1] = inverseGlobalRotation.zy(); am[2][2] = inverseGlobalRotation.zz();
    //as = as.similarityT( am );
    
    //GlobalError newError( as );
    //newAlignmentErrors->m_alignError.push_back( AlignTransformError( newError.matrix(),
    //                                                                 (*iAlignError).rawId() ) );
    //++nAPE;
  }

  edm::LogInfo("Alignment") << "@SUB=GeometryAligner::removeGlobalTransform" 
			    << "Finished to remove global transformation from " 
			    << alignments->m_align.size() << " alignments with "
			    << nAPE << " non-zero APE.";
}

#endif
