import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import RumQuick 1.0
import Rum 1.0

import "rum/target/qt-exp.js" as RUM;

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Column {
        RumRender {
            id: rumRender
            Component.onCompleted: {
                RUM.app.core.render_main_BANG_(rumRender);
            }
        }
    }

}
