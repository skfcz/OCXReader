//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//


#include <STEPCAFControl_Reader.hxx>
#include "OCXCAFControl_Reader.h"
#include "OCXHelper.h"
#include "OCXCoordinateSystemReader.h"

#include <LDOM_DocumentType.hxx>
#include <LDOM_LDOMImplementation.hxx>
#include <LDOMParser.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>

#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>
#include <UnitsAPI.hxx>

#include <STEPControl_Writer.hxx>

#include <string>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <vector>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

OCXCAFControl_Reader::OCXCAFControl_Reader() {

}

OCXCAFControl_Reader::~OCXCAFControl_Reader() {

}

Standard_Boolean
OCXCAFControl_Reader::ReadFile(const Standard_CString filename) {
    // Load the OCX Document as DOM
    const Handle(OSD_FileSystem) &aFileSystem = OSD_FileSystem::DefaultFileSystem();
    opencascade::std::shared_ptr<std::istream> aFileStream = aFileSystem->OpenIStream(filename, std::ios::in);

    if (aFileStream.get() == NULL || !aFileStream->good()) {
        std::cerr << "cannot open '" << filename << "' for reading" << std::endl;
        return Standard_False;
    }

    LDOMParser aParser;
    if (aParser.parse(*aFileStream, Standard_True, Standard_False)) {
        TCollection_AsciiString aData;
        std::cerr << "failed to parse " << filename << std::endl;
        std::cerr << aParser.GetError(aData) << ": " << aData << std::endl;
        return Standard_False;
    }

    ocxDocEL = aParser.getDocument().getDocumentElement();


    // check we got the right root element
    std::cout << "root element " << ocxDocEL.getTagName().GetString() << std::endl;

    std::string rtn(ocxDocEL.getTagName().GetString());
    std::size_t pos = rtn.find(':');
    if (pos == std::string::npos) {
        std::cerr << "no : found in root element name <" << rtn << ">" << std::endl;
        return Standard_False;
    }
    nsPrefix = rtn.substr(0, pos);
    std::cout << "ocx prefix '" << nsPrefix << "'" << std::endl;

    if (rtn.compare(nsPrefix + ":ocxXML") != 0) {
        std::cerr << "expected root element <" << nsPrefix << ":ocxXML>, but got <" << rtn << ">" << std::endl;
        return Standard_False;
    }

    // check the version
    std::string schemaVersion(ocxDocEL.getAttribute("schemaVersion").GetString());
    if (schemaVersion.empty()) {
        std::cerr << "no schemaVersion attribute found in root element" << std::endl;
        return Standard_False;
    }
    if (schemaVersion != "2.8.5") {
        std::cerr << "expect schemaVersion 2.8.5, got " << schemaVersion << std::endl;
        return Standard_False;
    }


    // successfully parsed the file into DOM and made some preliminary checks
    ctx = new OCXContext(ocxDocEL, nsPrefix);

    return Standard_True;

}


Standard_Boolean OCXCAFControl_Reader::Perform(const Standard_CString filename,
                                               Handle(TDocStd_Document) &doc,
                                               const Message_ProgressRange &theProgress) {
    if (ReadFile(filename) == Standard_False) {
        return Standard_False;
    }
    return Transfer(doc, theProgress);
}

Standard_Boolean OCXCAFControl_Reader::Transfer(Handle(TDocStd_Document) &doc,
                                                const Message_ProgressRange &theProgress) {
    if (ocxDocEL == NULL) {
        std::cerr << "run ReadFile before Transfer" << std::endl;
        return Standard_False;
    }

    Handle(TDocStd_Application) app = new TDocStd_Application;
    app->NewDocument("XmlOcaf", doc);
    if (doc.IsNull()) {
        std::cerr << "Can not create OCAF document" << std::endl;
        return Standard_False;
    }

    std::string fullTagName = ocxDocEL.getTagName().GetString();
    std::size_t index = fullTagName.find(':');
    if (index == std::string::npos) {
        std::cerr << "root element tag does not contain a colon (:) '" << fullTagName << "'" << std::endl;
        return Standard_False;
    }
    nsPrefix = fullTagName.substr(0, index);

    std::string expectedTagName = nsPrefix + ":ocxXML";
    if (expectedTagName.compare(fullTagName) != 0) {
        std::cerr << "expect document to start with " << expectedTagName << ", but found " << fullTagName << "'"
                  << std::endl;
        return Standard_False;
    }

    ctx->PrepareUnits();

    LDOM_Element vesselN = OCXHelper::GetFirstChild(ocxDocEL, "Vessel");
    if (vesselN.isNull()) {
        std::cerr << "could not find ocxXML/Vessel element" << std::endl;
        return Standard_False;
    }


    OCXCoordinateSystemReader* cosysReader = new OCXCoordinateSystemReader(ctx);
    cosysReader->ReadCoordinateSystem(vesselN);



    TDF_Label vesselL = doc->Main();
    TopoDS_Shape shape = ParsePanels(vesselN, vesselL);

    std::string fileName = "vessel.stp";

    try {
        STEPControl_Writer writer;
        writer.Transfer(shape, STEPControl_AsIs);
        writer.Write(fileName.c_str());
    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred transferring geometry:" << exp << std::endl;
    }

    return Standard_True;
}



TopoDS_Shape OCXCAFControl_Reader::ParsePanels(LDOM_Element &vesselN, TDF_Label vesselL) {

    TopoDS_Compound compound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (compound);

    // Take the first child. If it doesn't match look for other ones in a loop
    LDOM_Node aChildNode = vesselN.getFirstChild();
    while (aChildNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element aNextElement = (LDOM_Element &) aChildNode;
            if ("Panel" == OCXHelper::GetLocalTagName(aNextElement)) {
                TopoDS_Shape shape = ParsePanel(aNextElement, vesselL);
                if ( !shape.IsNull()) {
                    aBuilder.Add(compound, shape);
                }


            }
        }
        aChildNode = aChildNode.getNextSibling();
    }

    return compound;

}

TopoDS_Shape OCXCAFControl_Reader::ParsePanel(LDOM_Element &panelN, TDF_Label vesselL) {

    std::string id = std::string(panelN.getAttribute("id").GetString());

    std::cout << "found Panel " << id << " " << panelN.getAttribute("name").GetString() << std::endl;

    TopoDS_Compound compound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (compound);


    LDOM_Element unboundedGeometryN = OCXHelper::GetFirstChild(panelN, "UnboundedGeometry");
    if (unboundedGeometryN.isNull()) {
        std::cerr << "could not find UnboundedGeometry for Panel " << id << std::endl;
        return TopoDS_Shape();
    }


    LDOM_Element outerContourN = OCXHelper::GetFirstChild(panelN, "OuterContour");
    if (outerContourN.isNull()) {
        std::cerr << "could not find OuterContour for Panel " << id << std::endl;
        return TopoDS_Shape();
    }
    try {
        TopoDS_Wire wire = ParseCurve(outerContourN);
        aBuilder.Add( compound, wire);




    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred transferring OuterContur from panel "
                  << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
    }
    TDF_Label panelL = TDF_TagSource::NewChild(vesselL);

    return compound;


}

TopoDS_Wire OCXCAFControl_Reader::ParseCurve(LDOM_Element &curveN) {

    LDOM_Node aCurveNode = curveN.getFirstChild();

    BRepBuilderAPI_MakeWire makeWire = BRepBuilderAPI_MakeWire();

    while (aCurveNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aCurveNode.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element aCurveElement = (LDOM_Element &) aCurveNode;
            if ("Ellipse3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "Ellipse3D" << std::endl;
            } else if ("CircumCircle3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "CircumCircle3D" << std::endl;
            } else if ("CircumArc3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "CircumArc3D" << std::endl;
            } else if ("Line3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "Line3D" << std::endl;
            } else if ("CompositeCurve3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "CompositeCurve3D" << std::endl;
            } else if ("PolyLine3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "PolyLine3D" << std::endl;
            } else if ("NURBS3D" == OCXHelper::GetLocalTagName(aCurveElement)) {
                std::cout << "NURBS3D" << std::endl;
                TopoDS_Edge edge = ParseNURBSCurve(aCurveElement);
                makeWire.Add(edge);
            } else {
                std::cerr << "found unsupported curve type " << aCurveElement.getTagName().GetString() << std::endl;
            }
        }
        aCurveNode = aCurveNode.getNextSibling();
    }

    BRepBuilderAPI_WireError error = makeWire.Error();
    if (error != BRepBuilderAPI_WireDone) {
        std::cout << "failed to read curve, wire has error" << error << std::endl;
    }
    TopoDS_Wire wire = makeWire.Wire();
    return wire;
}

TopoDS_Edge OCXCAFControl_Reader::ParseNURBSCurve(LDOM_Element &nurbs3DN) {

    std::string id = std::string(nurbs3DN.getAttribute("id").GetString());

    std::cout << "ParseNURBSCurve " << id << std::endl;

    LDOM_Element propsN = OCXHelper::GetFirstChild(nurbs3DN, "NURBSproperties");
    if (propsN.isNull()) {
        std::cout << "could not find NURBS3D/NURBSProperties in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    int degree;
    propsN.getAttribute("degree").GetInteger(degree);
    int numCtrlPts;
    propsN.getAttribute("numCtrlPts").GetInteger(numCtrlPts);
    int numKnots;
    propsN.getAttribute("numKnots").GetInteger(numKnots);
    std::string form = std::string(propsN.getAttribute("form").GetString());
    bool isRational = std::string(propsN.getAttribute("isRational").GetString()) == "true";

    std::cout << "degree " << degree << ", #ctr " << numCtrlPts << ", #knots " << numKnots << ", form '" << form <<
              (isRational ? "', rational" : "', irrational") << std::endl;

    // The number of knots is always equals to the number of control points + curve degree + one
    // == number of control points + curve degree



    LDOM_Element knotVectorN = OCXHelper::GetFirstChild(nurbs3DN, "KnotVector");
    if (knotVectorN.isNull()) {
        std::cout << "could not find NURBS3D/KnotVector in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    std::string knotVectorS = std::string(knotVectorN.getAttribute("value").GetString());
    std::cout << "knots[" << knotVectorS << "]" << std::endl;
    std::vector<std::string> out;
    OCXHelper::TokenizeBySpace(knotVectorS, out);

    if (numKnots != static_cast<int>(out.size())) {
        std::cerr << "failed to parse curve " << id << ", expected #" << numKnots <<
                  " knot values, but parsing [" << knotVectorS << "] resulted in #" << out.size() << " values"
                  << std::endl;
        return TopoDS_Edge();
    }
    //  https://github.com/tpaviot/pythonocc-core/issues/706
    // Typically you'll have in textbooks a knot vector like:     [0 0 0 0.3 0.5 0.7 1 1 1]
    // In OCCT, you have instead:
    //    Knots: [0 0.3 0.5 0.7 1]
    //    Mults [3 1 1 1 3]

    std::vector<double> knots0;
    std::vector<int> mults0;
    double lastKnot = -1;
    double currentMult = -1;
    for (int i = 0; i < out.size(); i++) {
        double currentKnot = std::stod(out[i]);
        if (i == 0) {
            lastKnot = currentKnot;
            currentMult = 1;
            continue;
        }
        if (abs(currentKnot - lastKnot) < 1e-3) {
            currentMult++;
        } else {
            // knot value changed from lastKnot to currentKnot
            knots0.push_back(lastKnot);
            mults0.push_back(currentMult);
            lastKnot = currentKnot;
            currentMult = 1;
        }
    }
    knots0.push_back(lastKnot);
    mults0.push_back(currentMult);

    // now we know the size of the knot and multiplicities vector and
    //  could create the OCC type
    TColStd_Array1OfReal knots(1, knots0.size());
    TColStd_Array1OfInteger mults(1, knots0.size());
    for (int i = 0; i < knots0.size(); i++) {
        std::cout << i << ", knot " << knots0[i] << ", mult " << mults0[i] << std::endl;
        knots.SetValue(i + 1, knots0[i]);
        mults.SetValue(i + 1, mults0[i]);
    }


    LDOM_Element controlPtListN = OCXHelper::GetFirstChild(nurbs3DN, "ControlPtList");
    if (controlPtListN.isNull()) {
        std::cout << "could not find NURBS3D/ControlPtList in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    // names of the elements we are looking for
    LDOMString controlPointT = LDOMString((nsPrefix + ":ControlPoint").c_str());
    LDOMString point3dT = LDOMString((nsPrefix + ":Point3D").c_str());
    LDOMString xT = LDOMString((nsPrefix + ":X").c_str());
    LDOMString yT = LDOMString((nsPrefix + ":Y").c_str());
    LDOMString zT = LDOMString((nsPrefix + ":Z").c_str());


    LDOM_Element controlPointN = controlPtListN.GetChildByTagName(controlPointT);
    if (controlPointN.isNull()) {
        std::cout << "could not find NURBS3D/ControlPtList/ControlPoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }

    TColgp_Array1OfPnt poles(1, numCtrlPts);
    TColStd_Array1OfReal weights(1, numCtrlPts);

    int i = 0;
    while (!controlPointN.isNull()) {

        double weight;
        OCXHelper::GetDoubleAttribute(controlPointN, "weight", weight);

        LDOM_Element pointN = controlPointN.GetChildByTagName(point3dT);
        LDOM_Element xN = pointN.GetChildByTagName(xT);
        LDOM_Element yN = pointN.GetChildByTagName(yT);
        LDOM_Element zN = pointN.GetChildByTagName(zT);

        double x;
        OCXHelper::GetDoubleAttribute(xN, "numericvalue", x);
        std::string xUnit = std::string(xN.getAttribute("unit").GetString());
        x *= ctx->LoopupFactor(xUnit);

        double y;
        OCXHelper::GetDoubleAttribute(yN, "numericvalue", y);
        std::string yUnit = std::string(yN.getAttribute("unit").GetString());
        y *= ctx->LoopupFactor(yUnit);

        double z;
        OCXHelper::GetDoubleAttribute(zN, "numericvalue", z);
        std::string zUnit = std::string(zN.getAttribute("unit").GetString());
        z *= ctx->LoopupFactor(zUnit);

        //        std::cout << controlPointN.getTagName().GetString() <<  " #" << i << " weight " << weight
        //            << ", [" << x << ", " << y << "," << z << "] " << xUnit  << std::endl;

        poles.SetValue(i + 1, gp_Pnt(x, y, z));
        weights.SetValue(i + 1, weight);

        controlPointN = controlPointN.GetSiblingByTagName();
        i++;
    }

    Handle(Geom_BSplineCurve) curve = new Geom_BSplineCurve(poles, weights, knots, mults, degree);

    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(curve);
    return edge;


}

void OCXCAFControl_Reader::ParseCoordinateSystem(LDOM_Element &coosysN) {



}






