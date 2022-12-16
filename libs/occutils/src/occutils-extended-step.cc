#include "occutils/occutils-extended-step.h"

#include <NCollection_Vector.hxx>
#include <Quantity_Color.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <vector>

namespace OCCUtils::STEP {

/**
 * Internal state
 */
struct _ExtendedSTEPExporterInternals {
  _ExtendedSTEPExporterInternals() {
    // Create document
    application = XCAFApp_Application::GetApplication();
    application->NewDocument("MDTV-XCAF", document);
    // Get tools
    shapeTool = XCAFDoc_DocumentTool::ShapeTool(document->Main());
    colorTool = XCAFDoc_DocumentTool::ColorTool(document->Main());
    materialTool = XCAFDoc_DocumentTool::MaterialTool(document->Main());
    layerTool = XCAFDoc_DocumentTool::LayerTool(document->Main());
    dimTolTool = XCAFDoc_DocumentTool::DimTolTool(document->Main());
    // Reserve some space in the labels to prevent frequent reallocation's
    shapeLabels.reserve(25);
  }
  // Base attributes
  Handle(TDocStd_Document) document;
  Handle(TDocStd_Application) application;
  // Tools
  Handle(XCAFDoc_ShapeTool) shapeTool;
  Handle(XCAFDoc_ColorTool) colorTool;
  Handle(XCAFDoc_MaterialTool) materialTool;
  Handle(XCAFDoc_LayerTool) layerTool;
  Handle(XCAFDoc_DimTolTool) dimTolTool;
  // Internal storage for labels
  std::vector<TDF_Label> shapeLabels;
};

ExtendedSTEPExporter::ExtendedSTEPExporter() {
  internals = std::make_unique<_ExtendedSTEPExporterInternals>();
}

ExtendedSTEPExporter::~ExtendedSTEPExporter() = default;

size_t ExtendedSTEPExporter::AddShape(const TopoDS_Shape& shape) {
  TDF_Label shapeLabel = internals->shapeTool->NewShape();
  internals->shapeTool->SetShape(shapeLabel, shape);
  // Add to internal labels
  size_t idx = internals->shapeLabels.size();
  internals->shapeLabels.push_back(shapeLabel);
  return idx;
}

size_t ExtendedSTEPExporter::AddShapeWithColor(const TopoDS_Shape& shape,
                                               const Quantity_Color& color,
                                               XCAFDoc_ColorType colorType) {
  TDF_Label shapeLabel = internals->shapeTool->NewShape();
  internals->shapeTool->SetShape(shapeLabel, shape);
  internals->colorTool->SetColor(shape, color, colorType);
  // Add to internal labels
  size_t idx = internals->shapeLabels.size();
  internals->shapeLabels.push_back(shapeLabel);
  return idx;
}

void ExtendedSTEPExporter::Write(const std::string& filename) {
  STEPCAFControl_Writer writer;
  writer.SetMaterialMode(true);
  writer.SetDimTolMode(true);
  writer.SetLayerMode(true);
  writer.SetPropsMode(true);
  writer.SetColorMode(true);
  writer.Perform(internals->document, filename.c_str());
}

}  // namespace OCCUtils::STEP
