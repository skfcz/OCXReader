#include "occutils/occutils-step-export.h"

#include <Interface_Static.hxx>
#include <STEPControl_Writer.hxx>
#include <stdexcept>

namespace OCCUtils::STEP {

void ExportSTEP(const TopoDS_Shape& shape, const std::string& filename,
                const std::string& unit) {
  if (shape.IsNull()) {
    throw new std::invalid_argument("Can't export null shape to STEP");
  }

  STEPControl_Writer writer;
  Interface_Static::SetCVal("xstep.cascade.unit", unit.c_str());
  Interface_Static::SetCVal("write.step.unit", unit.c_str());
  Interface_Static::SetIVal("write.step.nonmanifold", 1);
  // "Transfer" = convert
  IFSelect_ReturnStatus transferStatus =
      writer.Transfer(shape, STEPControl_AsIs);

  if (transferStatus != IFSelect_RetDone) {
    throw std::logic_error("Error while transferring shape to STEP");
  }
  // Write transferred structure to STEP file
  IFSelect_ReturnStatus writeStatus = writer.Write(filename.c_str());

  // Return previous locale
  if (writeStatus != IFSelect_RetDone) {
    throw std::logic_error(
        "Error while writing transferred shape to STEP file");
  }
}

}  // namespace OCCUtils::STEP
