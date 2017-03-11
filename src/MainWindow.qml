import QtQuick 2.4
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

ApplicationWindow {
	title: qsTr("FS stat")
	height: 480
	width: 640
	visible: true

	ColumnLayout {
		id: mainLayout
		anchors.fill: parent

		RowLayout {
			id: headerLayout
			Layout.margins: 5
			Layout.fillHeight: false

			TextField {
				id: pathField
				placeholderText: qsTr("Path")
				Layout.fillWidth: true
				text: mainModel.directoryPath
				enabled: !mainModel.isScanning

				Binding {
					target: mainModel
					property: "directoryPath"
					value: pathField.text
				}
			}

			Button {
				id: startStopButton
				text: mainModel.isScanning ? qsTr("Stop") : qsTr("Start")
				Layout.preferredWidth: 100
				style: ButtonStyle {
					background: Rectangle {
						color: mainModel.isScanning ? "#ffaaaa" : "#aaffaa"
						border.width: 1
						border.color: "green"
						radius: 3
					}
				}
				onClicked: mainModel.startStopScan()
			}
		}

		SplitView {
			id: splitView
			Layout.fillHeight: true
			Layout.fillWidth: true
			orientation: Qt.Horizontal

			TreeView {
				id: fileSystemView
				model: mainModel.fileSystemModel
				Layout.fillHeight: true
				Layout.preferredWidth: 250
				headerVisible: false

				TableViewColumn {
					role: "fileName"
				}

				onClicked: function() {
					if (this.currentIndex.row >= 0) {
						// magic numbers are bad practice but I had to use the one here:
						// a small hack to pass QFileSystemModel::FilePathRole = Qt::UserRole + 1 = 257
						mainModel.directoryPath = mainModel.fileSystemModel.data(this.currentIndex, 257)
					}
				}
			}

			ColumnLayout {
				id: statisticsLayout

				TableView {
					id: fileStatisticsView
					model: mainModel.statisticsModel
					Layout.fillHeight: true
					Layout.fillWidth: true

					TableViewColumn {
						role: "extension"
						title: "Extension"
						width: 100
						horizontalAlignment: Text.AlignLeft
					}

					TableViewColumn {
						role: "count"
						title: "Count"
						width: 70
						horizontalAlignment: Text.AlignRight
					}

					TableViewColumn {
						role: "total_size"
						title: "Total size"
						width: 120
						horizontalAlignment: Text.AlignRight
					}

					TableViewColumn {
						role: "average_size"
						title: "Average size"
						width: 120
						horizontalAlignment: Text.AlignRight
					}
				}

				Label {
					id: errorMessage
					text: qsTr("Error: ") + mainModel.errorText
					Layout.fillWidth: true
					Layout.preferredHeight: 20
					leftPadding: 7
					color: "#ff5555"
					font.bold: true
					visible: mainModel.errorText != ""
				}

				Label {
					id: subdirectoriesCount
					text: qsTr("Subdirectories: ") + mainModel.subdirectoriesCount
					Layout.fillWidth: true
					Layout.preferredHeight: 20
					leftPadding: 7
				}

				Label {
					id: fileCount
					text: qsTr("Total files: ") + mainModel.fileCount
					Layout.fillWidth: true
					Layout.preferredHeight: 20
					leftPadding: 7
				}

				Label {
					id: fileSize
					text: qsTr("Total files size: ") + mainModel.fileSizeString
					Layout.fillWidth: true
					Layout.preferredHeight: 20
					leftPadding: 7
				}
			}
		}
	}
}
