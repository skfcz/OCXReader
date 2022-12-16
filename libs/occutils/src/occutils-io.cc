#include "occutils/occutils-io.hxx"

#include <IGESControl_Reader.hxx>
#include <STEPControl_Reader.hxx>
#include <boost/algorithm/string.hpp>

#include "occutils/occutils-exceptions.h"

namespace OCCUtils::IO {

TopoDS_Shape Read(const std::string& filename) {
  auto reader = Reader::STEPorIGESReader(filename);
  Reader::ReadFile(reader, filename);
  return Reader::ReadOneShape(reader);
}

namespace Reader {

std::shared_ptr<XSControl_Reader> STEPorIGESReader(
    const std::string& filename) {
  std::shared_ptr<XSControl_Reader> reader;
  // Automatically determine filename
  std::string lowercaseFilename = boost::algorithm::to_lower_copy(filename);
  if (boost::algorithm::ends_with(lowercaseFilename, ".step") ||
      boost::algorithm::ends_with(lowercaseFilename, ".stp")) {
    reader = std::shared_ptr<XSControl_Reader>(
        dynamic_cast<XSControl_Reader*>(new STEPControl_Reader()));
  } else if (boost::algorithm::ends_with(lowercaseFilename, ".iges") ||
             boost::algorithm::ends_with(lowercaseFilename, ".igs")) {
    reader = std::shared_ptr<XSControl_Reader>(
        dynamic_cast<XSControl_Reader*>(new IGESControl_Reader()));
  } else {
    throw OCCIOException(
        "Unknown file extension (.stp/.step or .igs/.iges expected): " +
        filename);
  }
  return reader;
}

std::shared_ptr<XSControl_Reader> STEPReader() {
  return std::shared_ptr<XSControl_Reader>(
      dynamic_cast<XSControl_Reader*>(new STEPControl_Reader()));
}

std::shared_ptr<XSControl_Reader> IGESReader() {
  return std::shared_ptr<XSControl_Reader>(
      dynamic_cast<XSControl_Reader*>(new IGESControl_Reader()));
}

/**
 * Convert a IFSelect_ReturnStatus code to string
 */
static std::string _IFSelectReturnStatusToString(IFSelect_ReturnStatus code) {
  switch (code) {
    case IFSelect_RetVoid:
      return "Void";
    case IFSelect_RetDone:
      return "Done";
    case IFSelect_RetError:
      return "Error";
    case IFSelect_RetFail:
      return "Fail";
    case IFSelect_RetStop:
      return "Stop";
    default:
      return "Unknown";
  }
}

void ReadFile(const std::shared_ptr<XSControl_Reader>& reader,
              const std::string& filename) {
  auto readStat = reader->ReadFile(filename.c_str());
  if (readStat != IFSelect_ReturnStatus::IFSelect_RetDone) {
    throw OCCIOException("Failed to read file, error code: " +
                         _IFSelectReturnStatusToString(readStat));
  }
}

TopoDS_Shape ReadOneShape(const std::shared_ptr<XSControl_Reader>& reader) {
  // Check if there is anything to convert
  auto numroots = reader->NbRootsForTransfer();
  if (numroots < 1) {
    throw OCCIOException(
        "Failed to read file: No roots to transfer are present");
  }
  // Convert STEP to shape
  if (reader->TransferRoots() < 1) {
    throw OCCIOException("Failed to read file: Failed to transfer any roots");
  }
  return reader->OneShape();
}

}  // namespace Reader

}  // namespace OCCUtils::IO
