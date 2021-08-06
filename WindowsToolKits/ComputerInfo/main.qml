import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Window {
    visible: true;
    width: 640;
    height: 450;
    property int fontsize: 18;
    property var memInfoJson: null;
    property var diskInfoJson :null;
    property var gpuMemJson: null;

    onHeightChanged: console.log("height"+height);
    onWidthChanged: console.log("width"+width);
    GroupBox{
        visible: true;
        id:cpuRectangle
        width:parent.width*0.5;
        height: parent.height*0.3;
        title: "CPU Information"
        font.pixelSize: 15;
        anchors.left:parent.left;


        Grid{
            rows: 4;
            columns: 2;
            rowSpacing:5;
            Text {
                  font.pixelSize: fontsize;
                  text: qsTr("cpu temperature:")
             }
            Text {
                  id:cpuTemp;
                  font.pixelSize: fontsize;
                  text: qsTr("1111")
             }
             Text {
                   font.pixelSize: fontsize;
                   text: qsTr("cpu Load:")
              }
             Text {
                   id:cpuLoad;
                   font.pixelSize: fontsize;
                   text: qsTr("2222")
              }
             Text {
                 font.pixelSize: fontsize;
                 text: qsTr("总内存容量:")
             }
             Text {
                   id:phyTotalMem;
                   font.pixelSize: fontsize;
                   text: qsTr("3333")
              }
             Text {
                 font.pixelSize: fontsize;
                 text: qsTr("空闲内存容量:")
             }
             Text {
                   id:phyFreeMem;
                   font.pixelSize: fontsize;
                   text: qsTr("4444")
              }
        }
    }


    GroupBox{

        visible: true;
        id:gpuRectangle;
        title:"GPU Information"
        font.pixelSize: 15;
        width:parent.width*0.5;
        height: parent.height*0.3;
        anchors.left: cpuRectangle.right;
        Grid{
            rows: 4;
            columns: 2;
            rowSpacing:5;
            Text {
                  font.pixelSize: fontsize;
                  text: qsTr("Gpu temperature(℃):")
             }
            Text {
                  id:gpuTemp;
                  font.pixelSize: fontsize;
                  text: qsTr("1111")
             }
             Text {
                   font.pixelSize: fontsize;
                   text: qsTr("Gpu Load:")
              }
             Text {
                   id:gpuLoad;
                   font.pixelSize: fontsize;
                   text: qsTr("2222")
              }
             Text {
                 font.pixelSize: fontsize;
                 text: qsTr("总内存容量:")
             }
             Text {
                   id:gpuTotalMem;
                   font.pixelSize: fontsize;
                   text: qsTr("3333M")
              }
             Text {
                 font.pixelSize: fontsize;
                 text: qsTr("空闲内存容量:")
             }
             Text {
                   id:gpuFreeMem;
                   font.pixelSize: fontsize;
                   text: qsTr("4444")
              }
        }
    }

    GroupBox{
        id:diskRectangle;
        width: parent.width;
        height: parent.height*0.7;
        anchors.top: cpuRectangle.bottom;
        title: "磁盘信息";
        font.pixelSize: 15;
        property int space: 50;

        GridLayout{   // 展示所有磁盘的信息
            id:showDiskGrid
            rows: 3;
            columns: 4;
            rowSpacing: 30;
            columnSpacing: 20;
            property int count: 0;
            Repeater{
                id:rep;
                model:0;
                delegate:GridLayout{
                    rows: 3;
                    columns: 2;
                    Text {
                        font.pixelSize: fontsize;
                        text: qsTr("盘符:");
                    }
                    Text {
                        font.pixelSize: fontsize;
                        text: qsTr("1111");
                    }
                    Text {
                        font.pixelSize: fontsize;
                        text: qsTr("总容量:");
                    }
                    Text {
                        font.pixelSize: fontsize;
                        text: qsTr("2222");
                    }
                    Text{
                        font.pixelSize: fontsize;
                        text:qsTr("空闲容量:");
                    }
                    Text{
                        font.pixelSize: fontsize;
                        text:qsTr("3333");
                    }

                }
            }

        }
    }

    Timer{
        id:startTimer;
        interval: 1000;
        repeat: true;

        onTriggered: {
            if(ComputerInfo.IsCpuLoad){
                cpuTemp.text = ComputerInfo.cpuTemp.toFixed(2)+"(℃)";
                cpuLoad.text = ComputerInfo.usedRate.toFixed(2)+"%";
                memInfoJson = JSON.parse(ComputerInfo.getMemInfo2Json());
                phyTotalMem.text = (memInfoJson.totalMem/1024/1024/1024).toFixed(2)+"G";
                phyFreeMem.text  = (memInfoJson.freeMem/1024/1024/1024).toFixed(2)+"G";
            }
            if(ComputerInfo.IsGpuLoad){
                gpuMemJson=JSON.parse(ComputerInfo.getGpuMemInfo2Json());
                gpuTemp.text = ComputerInfo.gpuTemp+"(℃)";
                gpuLoad.text = ComputerInfo.gpuLoad+"%";
                gpuTotalMem.text = gpuMemJson.totalMem/1024+"M";
                gpuFreeMem.text  = (gpuMemJson.freeMem/1024).toFixed(2)+"M";
            }

			diskInfoJson = JSON.parse(ComputerInfo.getDiskInfo2Json());
			rep.model=diskInfoJson.disk.length;
			for(var index=0; index < rep.count; index++){
				rep.itemAt(index).children[1].text = qsTr(diskInfoJson.disk[index].diskName);
				rep.itemAt(index).children[3].text = qsTr((diskInfoJson.disk[index].capacity[0]/1024/1024/1024).toFixed(2)+"G");
				rep.itemAt(index).children[5].text = qsTr((diskInfoJson.disk[(index)].capacity[1]/1024/1024/1024).toFixed(2)+"G");
			}
        }
    }

    Component.onCompleted: {
        // 启动定时器
        startTimer.start();
      
    }
}
