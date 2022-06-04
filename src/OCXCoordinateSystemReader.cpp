//
// Created by cz on 04.06.22.
//

#include <GC_MakePlane.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Pln.hxx>
#include "OCXCoordinateSystemReader.h"
#include "OCXHelper.h"

OCXCoordinateSystemReader::OCXCoordinateSystemReader(OCXContext *ctx) {
    this->ctx = ctx;

}

Standard_Boolean OCXCoordinateSystemReader::ReadCoordinateSystem(LDOM_Element &vesselN) {
    std::string tag = std::string(vesselN.getTagName().GetString());
    std::size_t index = tag.find("Vessel");
    if (index == std::string::npos) {
        std::cout << "expected a Vessel element, got " << tag << std::endl;
        return false;
    }

    LDOM_Element cosysN = OCXHelper::GetFirstChild(vesselN, "CoordinateSystem");
    if (cosysN.isNull()) {
        std::cout << "could not find CoordinateSystem child node" << std::endl;
        return false;
    }

    // TODO: do we need to read the LocalCartesian ?
    // TODO: do we need to read the VesselGrid ?

    LDOM_Element frTblsN = OCXHelper::GetFirstChild(cosysN, "FrameTables");
    if (frTblsN.isNull()) {
        std::cout << "could not find CoordinateSystem/FrameTables child node"<<      std::endl;
        return false;
    }

    LDOM_Element xRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "XRefPlanes");
    if (xRefPlanesN.isNull()) {
        std::cout << "could not find CoordinateSystem/FrameTables/XRefPlanes child node"<<  std::endl;
    } else {
        ReadRefPlanes(xRefPlanesN);
    }

    LDOM_Element yRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "YRefPlanes");
    if (yRefPlanesN.isNull()) {
        std::cout << "could not find CoordinateSystem/FrameTables/YRefPlanes child node"<<        std::endl;
    } else {
        ReadRefPlanes(yRefPlanesN);
    }

    LDOM_Element zRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "ZRefPlanes");
    if (zRefPlanesN.isNull()) {
        std::cout << "could not find CoordinateSystem/FrameTables/ZRefPlanes child node"<<   std::endl;
    } else {
        ReadRefPlanes(zRefPlanesN);
    }


    return true;
}

Standard_Boolean OCXCoordinateSystemReader::ReadRefPlanes(LDOM_Element &refPlanesN) {

    std::cout << "ReadRefPlanes from " << refPlanesN.getTagName().GetString() << std::endl;

    // too lazy to define enumeration
    int axis = -1;
    if (strstr(refPlanesN.getTagName().GetString(), "XRefPlanes") ) {
        axis =0;
    } else if (strstr(refPlanesN.getTagName().GetString(), "YRefPlanes") ) {
        axis = 1;
    } else if (strstr(refPlanesN.getTagName().GetString(), "ZRefPlanes")) {
        axis = 2;
    } else {
        std::cout << "expected one element from XRefPlanes, YRefPlanes, or ZRefPlanes, got '" << refPlanesN.getTagName().GetString() << "'" << std::endl;
        return false;
    }

    int cntPlanes=0;
    LDOM_Node aChildN = refPlanesN.getFirstChild();
    while (!aChildN.isNull()) {
        const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();

        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) {
            break;
        }
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element refPlaneN = (LDOM_Element &) aChildN;

            if ("RefPlane" == OCXHelper::GetLocalTagName(refPlaneN)) {

                std::string guid = std::string( refPlaneN.getAttribute( ctx->OCXGUIDRef()).GetString());

                LDOM_Element refLocN = OCXHelper::GetFirstChild( refPlaneN, "ReferenceLocation");
                if ( refLocN.isNull()) {
                    std::cout << "could not RefPlane/ReferenceLocation where GUIDRef is '"<< guid << "', skip it" <<  std::endl;
                } else {
                    double location;
                    OCXHelper::GetDoubleAttribute( refLocN, "numericalValue", location);
                    std::string xUnit = std::string(refLocN.getAttribute("unit").GetString());
                    location *= ctx->LoopupFactor(xUnit);

                    // Now create an OCC Plane
                    gp_Pnt org;
                    gp_Dir direction;
                    if ( axis ==0) {
                        org = gp_Pnt(location,0,0);
                        direction = gp_Dir( 1,0,0);
                    }else if ( axis ==1) {
                        org = gp_Pnt(0,location,0);
                        direction = gp_Dir( 0,1,0);
                    }else if ( axis ==2) {
                        org = gp_Pnt(0,0,location);
                        direction = gp_Dir( 0,0,1);
                    }

                    gp_Pln plane =gp_Pln( org, direction);
                    TopoDS_Face refPlane = BRepBuilderAPI_MakeFace(plane);

                    ctx->RegisterRefplane(guid, refPlane);
                    cntPlanes++;
                }



            } else {
                std::cerr << "expected RefPlane element, got  " << refPlaneN.getTagName().GetString() << std::endl;
            }
        }

        aChildN = aChildN.getNextSibling();
    }
    std::cout << "    registered #"<< cntPlanes << " planes found in " << refPlanesN.getTagName().GetString() << std::endl;

    return true;
}
