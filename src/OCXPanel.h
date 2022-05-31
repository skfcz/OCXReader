//
// Created by cz on 26.05.22.
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
