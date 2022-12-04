//
// Created by cz on 02.12.22.
//

#include "../include/PanelReader.h"

#include "../include/shipxml-log.h"

void shipxml::PanelReader::ReadPanels(const LDOM_Element& vesselN,
                                      shipxml::ShipSteelTransfer& sst) const {

  LDOM_Node aChildNode = vesselN.getFirstChild();

  std::list<Panel> list;

  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      if (ShipXMLHelper::GetLocalTagName(aElement) == "Panel") {
        char const *id = aElement.getAttribute("id").GetString();
        char const *guid = ShipXMLHelper::ReadGUID( aElement);

        Panel panel = ReadPanel(aElement, id, guid);
        list.push_back(panel);
        cout << "Read Read, added # " <<list.size() << " : " << panel.Name() << "\n";
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  cout << "Finished ReadPanels, added # " <<list.size() << " panels\n";

  sst.Structure().Panels(list);

  cout << "Finished ReadPanels, added # " <<sst.Structure().Panels().size() << " panels\n";

}
shipxml::Panel shipxml::PanelReader::ReadPanel(LDOM_Element panelO,
                                               const char *id,
                                               const char *guid) const {

  auto name = panelO.getAttribute("name").GetString();

  Panel panel(name);
  cout << "ReadPanel " << name << " (" <<guid << ")" << panelO.getTagName().GetString() << "\n";

  auto attrs = panelO.GetAttributesList();
  for( int i =0; i < attrs.getLength();i++) {
    LDOM_Node node = attrs.item(i);
    panel.Properties().Add( string(node.getNodeName().GetString()), string(node.getNodeValue().GetString()));
  }
  auto descrO = ShipXMLHelper::GetFirstChild( panelO, "Description");
  if (! descrO.isNull()) {
    panel.Properties().Add( "description", descrO.getNodeValue().GetString());
  }

  // the support
  auto unboundedGeometryO = ShipXMLHelper::GetFirstChild( panelO, "UnboundedGeometry");
  if (! unboundedGeometryO.isNull()) {
    // TODO: look up in context
  } else {
    SHIPXML_WARN("no UnboundedGeometry found in Panel {} ({})",id, guid );
  }

  // the outer contour
  auto outerContourO = ShipXMLHelper::GetFirstChild( panelO, "OuterContour");
  if (! outerContourO.isNull()) {
    // TODO: look up in context
  } else {
    SHIPXML_WARN("no OuterContour found in Panel {} ({})",id, guid );
  }

  // the limits
  auto limitedByO = ShipXMLHelper::GetFirstChild( panelO, "LimitedBy");
  if (! limitedByO.isNull()) {
    ReadLimits( limitedByO, panel);

  } else {
    SHIPXML_WARN("no LimitedBy found in Panel {} ({})",id, guid );
  }

  return panel;
}
void shipxml::PanelReader::ReadLimits(LDOM_Element limitedByO, Panel  panelX) const {

  cout << "ReadLimits from  " << panelX.Name()<< "\n";

  std::list<Limit> list;

  LDOM_Node aChildNode = limitedByO.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      //cout << "    limit " << aElement.getTagName().GetString()<< "\n";

      if (ShipXMLHelper::GetLocalTagName(aElement) == "FreeEdgeCurve3D") {

        auto id = ShipXMLHelper::GetAttribute( aElement, "id");
        auto name = ShipXMLHelper::GetAttribute( aElement, "name");
        auto guidRef = ShipXMLHelper::GetAttribute( aElement, "GUIDRef");

        Limit limitX(id);
        limitX.Feature(name);
        limitX.LimitType(CURVE);
        list.emplace_back( limitX);
        // TODO: Get limit curve and write to an limit

      } else if ( (ShipXMLHelper::GetLocalTagName(aElement) == "OcxItemPtr") ||
                 ShipXMLHelper::GetLocalTagName(aElement) == "GridRef" )  {

        auto localRef = ShipXMLHelper::GetAttribute( aElement, "localRef");
        auto guidRef = ShipXMLHelper::GetAttribute( aElement, "GUIDRef");
        auto refType = ShipXMLHelper::GetAttribute( aElement, "refType");

        Limit limitX( localRef);
        limitX.Feature(guidRef);
        if ( refType.find("GridRef") != std::string::npos) {
           limitX.LimitType( PLANE);
        } else if  ( refType.find("Panel") != std::string::npos) {
            limitX.LimitType( PANEL);
        } else {
          cerr << "found unsupported refType in OcxItemPtr  " << localRef << " (" <<guidRef << ") of Panel "
               <<  panelX.Name() << endl;
        }

        list.emplace_back( limitX);
        // TODO: Get limit curve and write to an limit

      } else {
        cerr << "found unsupported child element " << ShipXMLHelper::GetLocalTagName(aElement) << " in limitedBy of Panel "
            <<  panelX.Name() << endl;
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  panelX.Limits(list);
  cout  << "read #" << panelX.Limits().size()<< "limits\n";



}
