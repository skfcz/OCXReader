#include <BRepPrimAPI_MakeBox.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDataStd_Integer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp_Explorer.hxx>
#include <XmlDrivers.hxx>

#include "ocx/ocx-caf-control-reader.h"

int main() {
  // Initialize the application
  Handle(TDocStd_Application) app = new TDocStd_Application;

  // Initialize the document
  Handle(TDocStd_Document) doc;
  app->NewDocument("XmlOcaf", doc);
  if (doc.IsNull()) {
    std::cerr << "Can not create OCAF document" << std::endl;
    return 1;
  }

  OCXCAFControl_Reader reader;
  // NAPA-D-BULKER-MID_V286.3docx, Aveva-OHCM-MidShip_V285.3docx, ERHULLV2.3docx
  if (!reader.ReadFile("test2.3docx")) {
    std::cerr << "failed to load" << std::endl;
    app->Close(doc);
    return 1;
  }

  // TODO: discuss if parsing should happen inside Reader::Transfer
  if (!reader.Transfer(doc)) {
    std::cerr << "failed to parse" << std::endl;
    app->Close(doc);
    return 1;
  }

  // Save the document
  XmlDrivers::DefineFormat(app);
  if (app->SaveAs(doc, "vessel.xml") != PCDM_SS_OK) {
    app->Close(doc);

    std::cout << "Cannot write OCAF document." << std::endl;
    return 1;
  }

  app->Close(doc);

  return 0;
}
