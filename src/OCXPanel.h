//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADER_OCXPANEL_H
#define OCXREADER_OCXPANEL_H


#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>

class OCXBaseType {

public:
    OCXBaseType( const TDF_Label& label);

    void SetShape(const TopoDS_Shape & shape);

    TopoDS_Shape GetShape();

private:
    TDF_Label m_root;
};

class OCXPanel : OCXBaseType {

};



#endif //OCXREADER_OCXPANEL_H
