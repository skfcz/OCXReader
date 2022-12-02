//
// Created by cz on 02.12.22.
//

#ifndef OCXREADER_DISTRIBUTION_PANELREADER_H
#define OCXREADER_DISTRIBUTION_PANELREADER_H

#include <LDOM_Element.hxx>


namespace shipxml {


  class PanelReader {
   public:
    void ReadPanels(LDOM_Element const &vesselN, ShipSteelTransfer &sst) const;
    Panel ReadPanel(LDOM_Element panelO, const char* id,
                    const char* guid) const;

    void ReadLimits(LDOM_Element limitedByO, Panel panelX) const;
  };

}

#endif  // OCXREADER_DISTRIBUTION_PANELREADER_H
