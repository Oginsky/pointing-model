#ifndef STYLESHEETS_H
#define STYLESHEETS_H

#include <QString>

class StyleSheets
{
public:
    StyleSheets();

    static QString getPlayButtonStyleSheet();
    static QString getStopButtonStyleSheets();
    static QString getRightStepButtonStyleSheet();
    static QString getLeftStepButtonStyleSheet();
};

#endif // STYLESHEETS_H
