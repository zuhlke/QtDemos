/** ****************************************************************************
 * @file main.cpp
 * @author Jeremy Hannon <hje@zuhlke.com>
 * @version 0.4
 * @date July 11th, 2012
 * Created: June 19th, 2012
 *
 * @section LICENSE
 *     All rights reserved, Zuhlke Engineering Ltd.  Copyright 2012.
 *
 * @section DESCRIPTION
 * The DotGame is a Qt application involving a randomly walking dot that the
 * user tries to closely follow with their cursor to achieve a high m_score.
 *
 * This file contains the 'main' subroutine, which simply instantiates the
 * MainWindow with all of the game's activity.
 *
 ******************************************************************************/

#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "mainwindow.h"
#include <QDeclarativeView>
#include <QtGui/QSlider>
#include <string>

/// Main Function for Dot Game application.
Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    //qmlRegisterType<QSlider>("QtItems", 1, 0, "QSlider");

    MainWindow w;
    w.show();

    // test code to demonstrate command-line arguments successfully passes in from Qt Creator.
    std::string debugStr = "DEBUG_ON=";
    //qDebug("arguments: %s %s", argv[0], argv[1]);
    if ((debugStr.compare(0,9,argv[1])))
    {
        //qDebug("correctly compared.");
    }

    return app->exec();
}
