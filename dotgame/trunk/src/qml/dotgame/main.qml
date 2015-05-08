// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//import QtItems 1.0
import Qt 4.7    // 4.8
//import com.nokia.symbian 1.8

Rectangle {
    id: main_rectangle
    width: 360
    height: 360
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#96d58989"
        }

        GradientStop {
            position: 1
            color: "#96000000"
        }
    }
    function updateDotLoc(location)    // slot
    {
        dot.x = location.x - (dot.width/2);
        dot.y = location.y - (dot.height/2);
    }
    signal screenClicked();
    signal setTimeLimit(int time);

    MouseArea {
        id: mousearea_main
        anchors.fill: parent
        //hoverEnabled: true        // makes containsMouse() accurate even without a mouse press
        onClicked: {
            //console.log("QML main clicked")
            if (containsMouse)
            {
                screenClicked()             // Pass the screen-click to Qt Widget
            }
        }
    }

    // Use a Rectangle with curved corners to create a circle, for the dot.
    Rectangle {
        id: dot
        width: 40
        height: 40
        color: "#0ef572"
        radius: width / 2
        border.color: "#000000"
        //onNewDotPos: console.log("NewDotPos")
    }

    Rectangle {
        id: ctrl_rect
        x: 160
        y: 99
        width: 100
        height: 200
        color: "#1a8fe2"
        anchors.right: parent.right
        anchors.rightMargin: 0
        opacity: 0.320
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            id: mousearea_ctrl
            anchors.fill: parent
            onClicked: {
                // do nothing.  Blocks clicks from triggering QWidget actions.
            }
        }

        Rectangle {
            id: timed_button
            x: 10
            y: 150
            width: 30
            height: 20
            color: "#d7ea00"
            opacity: 1

            Text {
                id: timed_button_text
                text: "Timed"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 6
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                anchors.fill: parent
            }

            MouseArea {
                id: timed_button_mousearea
                anchors.fill: parent
                onClicked: {
                    setTimeLimit(timed_button_input_text.text);
                }
            }
        } // timed_button rectangle

        Rectangle {
            id: timed_button_input
            x: timed_button.x + timed_button.width + 10
            y: timed_button.y
            width: 20
            height: timed_button.height
            color: "white"
            TextInput {
                id: timed_button_input_text
                width: 20
                height: 20
                text: "10"
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.fill: parent
                color: focus ? "black" : "gray"
            }
        }

        Rectangle {
            id: infinite_button
            x: 10
            y: 175
            width: 30
            height: 20
            color: "#d7ea00"
            opacity: 1

            Text {
                id: infinite_button_text
                text: "Infinite"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 6
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                anchors.fill: parent
            }

            MouseArea {
                id: infinite_button_mousearea
                anchors.fill: parent
                onClicked: {
                    setTimeLimit(0);  // 0 sec = INFINITE game
                }
            }
        } // infinite_button rectangle

    }

}
