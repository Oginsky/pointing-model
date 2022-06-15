#include "stylesheets.h"

StyleSheets::StyleSheets()
{

}


QString StyleSheets::getPlayButtonStyleSheet() {
    return "QToolButton { "
           "image: url(:/resource/resource/img/play.png);"
           "icon-size: 24px;"
           "border: none;"
           "}";
}
QString StyleSheets::getStopButtonStyleSheets() {
    return "QToolButton { "
    "image: url(:/resource/resource/img/stop.png);"
    "icon-size: 24px;"
    "border: none;"
    "};";
}
QString StyleSheets::getRightStepButtonStyleSheet() {
    return "";
}
QString StyleSheets::getLeftStepButtonStyleSheet() {
    return "";
}

QString getLabelStyleSheets() {
    return "QLabel {"
            "background: rgb(255, 255, 255);"
            "color: rgb(0, 0, 0);"
            "}";
}
