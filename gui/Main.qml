pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

ApplicationWindow {
    id: window
    width: 1440; height: 980
    minimumWidth: 1000; minimumHeight: 720
    visible: true
    title: "wikiruiner — Wikipedia Path Finder"
    color: "white"
    font.family: "Segoe UI"
    font.pixelSize: 13
    palette.window: "#ffffff"
    palette.windowText: "#202b38"
    palette.text: "#202b38"
    palette.base: "#ffffff"
    palette.button: "#f7f8fa"
    palette.buttonText: "#202b38"
    palette.highlight: "#336bd4"
    palette.highlightedText: "#ffffff"
    readonly property color ink: "#202b38"
    readonly property color blue: "#336bd4"
    readonly property color muted: "#66768b"
    readonly property color line: "#bfc7d1"
    property var recentRoutes: []

    function showInfo(heading: string, description: string) {
        information.title = heading
        information.description = description
        information.open()
    }
    function findRoute() { backend.search(startInput.text, targetInput.text, algorithmBox.currentIndex) }
    function linkCountText(count: int): string { return count + (count === 1 ? " link" : " links") }

    SearchController { id: backend; objectName: "searchController" }
    Connections {
        target: backend
        function onSearchCompleted() {
            if (backend.route.length > 0) {
                const entry = { from: backend.route[0], to: backend.route[backend.route.length - 1],
                    algorithmIndex: algorithmBox.currentIndex, links: backend.route.length - 1 }
                window.recentRoutes = [entry].concat(window.recentRoutes).slice(0, 8)
            }
        }
    }

    component Heading: Text {
        color: window.ink; font.family: "Georgia"; font.pixelSize: 26
        renderType: Text.NativeRendering
    }
    component Caption: Text { color: window.muted; font.pixelSize: 10; font.letterSpacing: 0.8 }
    component Divider: Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: window.line }
    component NavLink: ItemDelegate {
        id: link
        property bool selected: false
        Layout.fillWidth: true; implicitHeight: 30; leftPadding: 12; rightPadding: 8
        contentItem: Text {
            text: link.text; color: window.blue; font.pixelSize: 12; font.bold: link.selected
            verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
        }
        background: Rectangle { color: link.selected ? "#eaf1ff" : link.hovered ? "#f3f6fa" : "transparent" }
        Accessible.name: text
    }
    component ActionButton: Button {
        id: action
        leftPadding: 16; rightPadding: 16; topPadding: 11; bottomPadding: 11
        font.pixelSize: 11; font.bold: true; opacity: enabled ? 1 : 0.55
        contentItem: Text {
            text: action.text; color: window.blue; font: action.font
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: action.down ? "#dfebff" : action.hovered ? "#edf3ff" : "white"
            border.color: "#aac0ef"; radius: 2
        }
        Accessible.name: text
    }
    component ArticleField: TextField {
        id: field
        Layout.fillWidth: true; implicitHeight: 43; padding: 12
        color: window.ink; placeholderTextColor: "#8591a3"; font.pixelSize: 13; selectByMouse: true
        background: Rectangle {
            color: "white"; radius: 2; border.width: field.activeFocus ? 2 : 1
            border.color: field.activeFocus ? "#b8d1ff" : "#e3e9f2"
        }
        onAccepted: window.findRoute()
    }
    component StatRow: RowLayout {
        id: stat
        property string label
        property string value
        Layout.fillWidth: true; spacing: 12
        Text { text: stat.label; color: window.muted; font.pixelSize: 11; Layout.fillWidth: true }
        Text { text: stat.value; color: window.ink; font.pixelSize: 11; font.bold: true }
    }

    Rectangle {
        id: sidebar
        width: 194; anchors.top: parent.top; anchors.bottom: parent.bottom; color: "white"
        ColumnLayout {
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 22; topMargin: 95 }
            spacing: 2
            Text { text: "Contents"; color: window.ink; font.family: "Georgia"; font.pixelSize: 14; Layout.bottomMargin: 12 }
            NavLink { text: "Home"; selected: true; onClicked: pageScroll.ScrollBar.vertical.position = 0 }
            Text { text: "Pathfinding"; color: window.ink; font.pixelSize: 11; font.bold: true; Layout.topMargin: 10 }
            NavLink {
                text: "New Search"; enabled: !backend.busy
                onClicked: { backend.clearRoute(); startInput.clear(); targetInput.clear(); startInput.forceActiveFocus() }
            }
            NavLink { text: "Recent Routes"; onClicked: recentDialog.open() }
            Text { text: "Algorithms"; color: window.ink; font.pixelSize: 11; font.bold: true; Layout.topMargin: 12 }
            NavLink { text: "Breadth-First Search"; enabled: !backend.busy; onClicked: { algorithmBox.currentIndex = 0; backend.clearRoute() } }
            NavLink { text: "Dijkstra"; enabled: !backend.busy; onClicked: { algorithmBox.currentIndex = 1; backend.clearRoute() } }
            Text { text: "Data"; color: window.ink; font.pixelSize: 11; font.bold: true; Layout.topMargin: 12 }
            NavLink {
                text: "Article Index"
                onClicked: window.showInfo("Article index", "The names database maps Wikipedia article titles to integer IDs. Enter a full title in either search field; title matching ignores letter case. An article must also appear in the loaded graph to be searchable.")
            }
            NavLink {
                text: "Graph Statistics"
                onClicked: window.showInfo("Graph statistics", backend.ready
                    ? Number(backend.nodeCount).toLocaleString(Qt.locale(), 'f', 0) + " articles\n" + Number(backend.edgeCount).toLocaleString(Qt.locale(), 'f', 0) + " directed links\n\nThese counts describe the graph actually loaded from graphdata_small.bin."
                    : backend.status)
            }
            NavLink { text: "Full Dataset ↗"; onClicked: Qt.openUrlExternally("https://snap.stanford.edu/data/enwiki-2013.html") }
            Text { text: "Project"; color: window.ink; font.pixelSize: 11; font.bold: true; Layout.topMargin: 12 }
            NavLink {
                text: "How It Works"
                onClicked: window.showInfo("How it works", "Each article is a node. Each hyperlink is a directed edge. Choose two titles and an algorithm to find a shortest route through the loaded graph.\n\nBoth algorithms treat each link as one step. A path from A to B does not guarantee a path from B to A. Click a result article to open it on Wikipedia.")
            }
            NavLink { text: "Team WAS"; onClicked: window.showInfo("Team WAS", "wikiruiner\nWikipedia Is an Academic Source\n\nA DSA project comparing Breadth-First Search and Dijkstra on a Wikipedia hyperlink graph.") }
            NavLink { text: "Documentation"; onClicked: window.showInfo("Search guide", "1. Wait for the graph to load.\n2. Enter two full Wikipedia article titles.\n3. Choose BFS or Dijkstra and click Find Path.\n4. Explore the returned articles and compare search times.\n\nThe graph is loaded once. Recent Routes keeps the last eight successful searches for this session.") }
        }
        ColumnLayout {
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom; margins: 26 }
            spacing: 6
            Divider { Layout.bottomMargin: 8 }
            Caption { text: "LEARN"; font.pixelSize: 9 }
            Text { text: "Wikipedia Graphs · Shortest Paths"; color: window.blue; font.pixelSize: 9 }
            Caption { text: "DATA"; font.pixelSize: 9 }
            Text { text: "SNAP Dataset · SQLite Index"; color: window.blue; font.pixelSize: 9 }
            Caption { text: "PROJECT"; font.pixelSize: 9 }
            Text { text: "Built by Team WAS"; color: window.muted; font.pixelSize: 9 }
        }
    }

    ScrollView {
        id: pageScroll
        anchors { left: sidebar.right; right: parent.right; top: parent.top; bottom: parent.bottom }
        clip: true; contentWidth: availableWidth
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        Item {
            width: pageScroll.availableWidth; height: page.height + 64
            Rectangle {
                x: 0; y: 295; width: Math.min(parent.width, 1080); height: 830; radius: width / 2
                gradient: Gradient {
                    GradientStop { position: 0; color: "#e4ecfd" }
                    GradientStop { position: 0.65; color: "#eef3ff" }
                    GradientStop { position: 1; color: "#ffffff" }
                }
            }
            ColumnLayout {
                id: page
                x: 28; y: 28; width: Math.min(parent.width - 64, 1230); spacing: 17
                RowLayout {
                    Layout.fillWidth: true; Layout.bottomMargin: 5; spacing: 5
                    Heading { text: "wikiruiner"; font.pixelSize: 34; font.bold: true }
                    Rectangle {
                        Layout.alignment: Qt.AlignTop; Layout.topMargin: 4
                        implicitWidth: 28; implicitHeight: 12; color: window.blue; radius: 1
                        Text { anchors.centerIn: parent; text: "BETA"; color: "white"; font.pixelSize: 7; font.bold: true }
                    }
                    Item { Layout.fillWidth: true }
                    Text { text: "W"; color: window.muted; font.family: "Georgia"; font.pixelSize: 19; Layout.rightMargin: 10 }
                    ActionButton {
                        text: "About Project"
                        onClicked: window.showInfo("About wikiruiner", "Discover connections between Wikipedia articles using graph search.\n\nCompare BFS and Dijkstra on the same directed, unweighted dataset. Search timings measure the algorithm itself; database lookups and drawing are excluded.\n\nSource: SNAP enwiki-2013. Article names reflect the historical dataset and may differ from today's Wikipedia.")
                    }
                }
                Divider { }
                ColumnLayout {
                    spacing: 5
                    Heading { text: "Find a Wikipedia Path"; font.pixelSize: 34 }
                    Text {
                        Layout.fillWidth: true
                        text: "Discover the shortest route from one Wikipedia article to another using only hyperlinks."
                        color: window.muted; font.pixelSize: 13; wrapMode: Text.WordWrap
                    }
                }
                Divider { }
                GridLayout {
                    id: explorerGrid
                    Layout.fillWidth: true; columns: page.width >= 950 ? 2 : 1; columnSpacing: 18; rowSpacing: 24
                    ColumnLayout {
                        Layout.fillWidth: true; Layout.minimumWidth: 490; spacing: 12
                        RowLayout {
                            Layout.fillWidth: true
                            Heading { text: "Route Explorer" }
                            Item { Layout.fillWidth: true }
                            Caption { text: "WIKIPEDIA / EN" }
                        }
                        Divider { }
                        Rectangle {
                            Layout.fillWidth: true; implicitHeight: searchForm.implicitHeight + 44
                            gradient: Gradient {
                                orientation: Gradient.Horizontal
                                GradientStop { position: 0; color: "#2861c4" }
                                GradientStop { position: 1; color: "#477bd5" }
                            }
                            ColumnLayout {
                                id: searchForm
                                anchors { fill: parent; margins: 22 }
                                spacing: 16
                                RowLayout {
                                    Layout.fillWidth: true
                                    ColumnLayout {
                                        spacing: 5
                                        Heading { text: "Search the graph"; color: "white"; font.pixelSize: 27 }
                                        Text { text: "Choose two article titles and we’ll map the shortest connection."; color: "#edf3ff"; font.pixelSize: 11; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                                    }
                                    Item { Layout.fillWidth: true }
                                    Rectangle {
                                        implicitWidth: 38; implicitHeight: 38; radius: 2; color: "#22ffffff"
                                        Text { anchors.centerIn: parent; text: "W"; font.family: "Georgia"; font.bold: true; font.pixelSize: 24; color: "#d9e6ff" }
                                    }
                                }
                                RowLayout {
                                    Layout.fillWidth: true; spacing: 12
                                    ColumnLayout {
                                        Layout.fillWidth: true; Layout.preferredWidth: 1; spacing: 7
                                        Caption { text: "STARTING ARTICLE"; color: "#c4d9ff"; font.bold: true }
                                        ArticleField { id: startInput; objectName: "startInput"; placeholderText: "e.g. Computer science"; enabled: !backend.busy; Accessible.name: "Starting article" }
                                    }
                                    ToolButton {
                                        id: swapButton
                                        text: "⇄"; font.pixelSize: 22; Layout.alignment: Qt.AlignBottom; Layout.bottomMargin: 5
                                        enabled: !backend.busy; Accessible.name: "Swap starting and destination articles"
                                        contentItem: Text { text: swapButton.text; color: "#dce9ff"; font: swapButton.font }
                                        background: Rectangle { color: "transparent" }
                                        onClicked: { const oldStart = startInput.text; startInput.text = targetInput.text; targetInput.text = oldStart }
                                        ToolTip.visible: hovered; ToolTip.text: "Swap articles"
                                    }
                                    ColumnLayout {
                                        Layout.fillWidth: true; Layout.preferredWidth: 1; spacing: 7
                                        Caption { text: "DESTINATION ARTICLE"; color: "#c4d9ff"; font.bold: true }
                                        ArticleField { id: targetInput; objectName: "targetInput"; placeholderText: "e.g. Philosophy"; enabled: !backend.busy; Accessible.name: "Destination article" }
                                    }
                                }
                                RowLayout {
                                    Layout.fillWidth: true; spacing: 16
                                    ColumnLayout {
                                        Layout.fillWidth: true; Layout.preferredWidth: 210; spacing: 7
                                        Caption { text: "ALGORITHM"; color: "#c4d9ff"; font.bold: true }
                                        ComboBox {
                                            id: algorithmBox
                                            objectName: "algorithmBox"
                                            Layout.fillWidth: true; implicitHeight: 41
                                            model: ["Breadth-First Search", "Dijkstra"]
                                            enabled: !backend.busy; font.pixelSize: 12
                                            onActivated: backend.clearRoute()
                                            Accessible.name: "Search algorithm"
                                            background: Rectangle { color: "white"; radius: 2; border.color: "#d9e4f4" }
                                        }
                                    }
                                    ActionButton {
                                        id: findButton
                                        objectName: "findButton"
                                        Layout.alignment: Qt.AlignBottom
                                        text: backend.busy ? "PLEASE WAIT…" : "FIND PATH  →"
                                        enabled: backend.ready && !backend.busy; implicitHeight: 41
                                        onClicked: window.findRoute()
                                    }
                                }
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true; spacing: 10
                            BusyIndicator { running: backend.busy; visible: running; implicitWidth: 22; implicitHeight: 22 }
                            Text {
                                objectName: "statusText"
                                Layout.fillWidth: true; text: backend.status
                                color: !backend.ready && !backend.busy ? "#a34435" : window.muted
                                font.pixelSize: 12; wrapMode: Text.WordWrap
                                Accessible.role: Accessible.StaticText
                            }
                            ActionButton { text: "Retry"; visible: !backend.ready && !backend.busy; onClicked: backend.loadData() }
                        }
                        RowLayout {
                            Layout.fillWidth: true; Layout.topMargin: 9
                            Heading { text: "Shortest Path" }
                            Item { Layout.fillWidth: true }
                            Text {
                                text: backend.route.length > 0 ? window.linkCountText(backend.route.length - 1) + " · " + backend.route.length + " articles" : "Your route appears here"
                                color: window.muted; font.pixelSize: 10
                            }
                        }
                        Divider { }
                        Rectangle {
                            Layout.fillWidth: true; implicitHeight: 390
                            color: "#fcfdff"; border.color: window.line; clip: true
                            Column {
                                anchors.centerIn: parent; width: parent.width - 70; spacing: 12
                                visible: backend.route.length === 0
                                Text { anchors.horizontalCenter: parent.horizontalCenter; text: "○   →   ○   →   ○"; color: "#c0d0ec"; font.pixelSize: 37 }
                                Heading {
                                    width: parent.width; text: backend.busy ? "Finding connections…" : "Every article is a starting point."
                                    horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap; font.pixelSize: 23
                                }
                                Text {
                                    width: parent.width
                                    text: backend.busy ? "The graph search runs in the background." : "Enter two titles above to explore the links between them."
                                    color: window.muted; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
                                }
                            }
                            Flickable {
                                id: pathView
                                objectName: "pathView"
                                anchors.fill: parent; anchors.margins: 16; visible: backend.route.length > 0; clip: true
                                contentWidth: Math.max(width, routeRow.width); contentHeight: height
                                boundsBehavior: Flickable.StopAtBounds
                                ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }
                                Row {
                                    id: routeRow
                                    objectName: "routeRow"
                                    x: Math.max(0, (pathView.width - width) / 2)
                                    anchors.top: parent.top
                                    anchors.topMargin: 24
                                    Repeater {
                                        model: backend.route
                                        delegate: Item {
                                            id: routeNode
                                            required property int index
                                            required property string modelData
                                            width: 136; height: 148
                                            Rectangle {
                                                anchors.horizontalCenter: parent.horizontalCenter
                                                y: 12; width: 45; height: 45; radius: 23
                                                color: nodeHover.containsMouse ? "#eaf1ff" : "white"
                                                border.color: "#487cf0"; border.width: 2
                                                Text { anchors.centerIn: parent; text: routeNode.index + 1; font.pixelSize: 16; font.bold: true; color: window.blue }
                                            }
                                            Text { x: parent.width - 8; y: 20; visible: routeNode.index < backend.route.length - 1; text: "→"; color: "#8996a9"; font.pixelSize: 23 }
                                            Column {
                                                y: 72; width: parent.width - 12; anchors.horizontalCenter: parent.horizontalCenter; spacing: 7
                                                Caption {
                                                    width: parent.width; horizontalAlignment: Text.AlignHCenter
                                                    text: routeNode.index === 0 ? "START" : routeNode.index === backend.route.length - 1 ? "DESTINATION" : "ARTICLE"
                                                    font.pixelSize: 8
                                                }
                                                Text {
                                                    width: parent.width; text: routeNode.modelData; color: window.ink
                                                    font.family: "Georgia"; font.pixelSize: 14; wrapMode: Text.Wrap
                                                    maximumLineCount: 3; elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
                                                }
                                            }
                                            MouseArea {
                                                id: nodeHover
                                                anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                                onClicked: Qt.openUrlExternally("https://en.wikipedia.org/wiki/" + encodeURIComponent(routeNode.modelData.replace(/ /g, "_")))
                                            }
                                            Accessible.role: Accessible.Link
                                            Accessible.name: routeNode.modelData
                                            Accessible.onPressAction: Qt.openUrlExternally("https://en.wikipedia.org/wiki/" + encodeURIComponent(routeNode.modelData.replace(/ /g, "_")))
                                            ToolTip.visible: nodeHover.containsMouse
                                            ToolTip.text: "Open " + routeNode.modelData + " on Wikipedia"
                                        }
                                    }
                                }
                            }
                        }
                        ToolButton {
                            id: routeExplanation
                            text: checked ? "▾  About this route" : "▸  About this route"
                            checkable: true; font.pixelSize: 11
                            contentItem: Text { text: routeExplanation.text; color: window.muted; font: routeExplanation.font }
                            background: Rectangle { color: "transparent" }
                        }
                        Text {
                            visible: routeExplanation.checked; Layout.fillWidth: true
                            text: "Each arrow represents a directed hyperlink in the historical dataset. Every link has equal cost, so both BFS and Dijkstra find a minimum-link route. Several equally short routes may exist. Scroll horizontally for longer paths."
                            color: window.muted; font.pixelSize: 12; wrapMode: Text.WordWrap
                        }
                    }
                    Rectangle {
                        id: detailsCard
                        Layout.alignment: Qt.AlignTop
                        Layout.preferredWidth: explorerGrid.columns === 2 ? 285 : -1
                        Layout.fillWidth: explorerGrid.columns === 1
                        implicitHeight: detailsColumn.implicitHeight
                        color: "#f7f8fa"; border.color: "#aeb8c6"
                        ColumnLayout {
                            id: detailsColumn
                            width: parent.width; spacing: 0
                            Rectangle {
                                Layout.fillWidth: true; implicitHeight: 48
                                color: "#e9ecf1"; border.color: "#aeb8c6"
                                Heading { anchors.centerIn: parent; text: "Search Details"; font.pixelSize: 22; font.bold: true }
                            }
                            RowLayout {
                                Layout.fillWidth: true; Layout.margins: 17; spacing: 12
                                Rectangle {
                                    implicitWidth: 42; implicitHeight: 42; radius: 6; color: "#eaf1ff"
                                    Text { anchors.centerIn: parent; text: "W"; color: window.blue; font.family: "Georgia"; font.pixelSize: 22; font.bold: true }
                                }
                                ColumnLayout {
                                    spacing: 6
                                    Text { text: "enwiki 2013"; color: window.ink; font.family: "Georgia"; font.bold: true; font.pixelSize: 14 }
                                    Text {
                                        text: "View dataset information ↗"; color: window.blue; font.pixelSize: 10
                                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: Qt.openUrlExternally("https://snap.stanford.edu/data/enwiki-2013.html") }
                                    }
                                }
                            }
                            Divider { }
                            ColumnLayout {
                                Layout.fillWidth: true; Layout.margins: 17; spacing: 17
                                Caption { text: "ROUTE SUMMARY"; Layout.bottomMargin: 6 }
                                StatRow { label: "Articles in graph"; value: backend.ready ? Number(backend.nodeCount).toLocaleString(Qt.locale(), 'f', 0) : "—" }
                                Divider { color: "#e2e6ed" }
                                StatRow { label: "Search time"; value: backend.elapsedMs < 0 ? "—" : backend.elapsedMs.toFixed(2) + " ms" }
                                Divider { color: "#e2e6ed" }
                                StatRow { label: "Path length"; value: backend.route.length > 0 ? window.linkCountText(backend.route.length - 1) : "—" }
                            }
                            Divider { }
                            ColumnLayout {
                                Layout.fillWidth: true; Layout.margins: 17; spacing: 12
                                RowLayout {
                                    Layout.fillWidth: true
                                    Caption { text: "ALGORITHM" }
                                    Item { Layout.fillWidth: true }
                                    Text { text: algorithmBox.currentIndex === 0 ? "BFS" : "DIJKSTRA"; color: window.muted; font.pixelSize: 9 }
                                }
                                Rectangle {
                                    Layout.fillWidth: true; implicitHeight: algorithmDescription.implicitHeight + 28
                                    color: "white"; border.color: "#cbd2dc"
                                    ColumnLayout {
                                        id: algorithmDescription
                                        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 14 }
                                        spacing: 7
                                        Text { text: algorithmBox.currentText; color: window.ink; font.family: "Georgia"; font.bold: true; font.pixelSize: 13 }
                                        Text {
                                            Layout.fillWidth: true
                                            text: algorithmBox.currentIndex === 0 ? "Explores the graph level by level to find the fewest links." : "Uses a priority queue to expand the lowest-cost route. Every link has cost one."
                                            wrapMode: Text.WordWrap; color: window.muted; font.pixelSize: 10
                                        }
                                    }
                                }
                                Text { Layout.fillWidth: true; text: "Algorithm time only. Both searches use the same unweighted graph."; color: window.muted; font.pixelSize: 10; wrapMode: Text.WordWrap }
                            }
                            Divider { }
                            RowLayout {
                                Layout.fillWidth: true; Layout.margins: 17; spacing: 7
                                Rectangle { implicitWidth: 7; implicitHeight: 7; radius: 4; color: backend.busy ? "#d7a542" : backend.ready ? "#159378" : "#bb6254" }
                                Text { text: backend.busy ? "Working…" : backend.ready ? "Graph ready" : "Data needed"; color: window.muted; font.pixelSize: 10 }
                                Item { Layout.fillWidth: true }
                                Text { text: backend.ready ? Number(backend.edgeCount).toLocaleString(Qt.locale(), 'f', 0) + " links" : "Offline"; color: window.ink; font.bold: true; font.pixelSize: 9 }
                            }
                        }
                    }
                }
                Divider { Layout.topMargin: 30 }
                Text { text: "Built by Team WAS — Wikipedia Is an Academic Source"; color: window.muted; font.pixelSize: 10 }
            }
        }
    }
    Dialog {
        id: information
        property string description: ""
        anchors.centerIn: parent; width: Math.min(window.width - 80, 540)
        modal: true; standardButtons: Dialog.Ok
        contentItem: Text { text: information.description; wrapMode: Text.WordWrap; color: window.ink; font.pixelSize: 14; lineHeight: 1.4 }
    }
    Dialog {
        id: recentDialog
        title: "Recent routes"; anchors.centerIn: parent; width: 560
        modal: true; standardButtons: Dialog.Close
        contentItem: ColumnLayout {
            spacing: 10
            Text { visible: window.recentRoutes.length === 0; text: "Your successful searches will appear here for this session."; color: window.muted; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Repeater {
                model: window.recentRoutes
                delegate: ItemDelegate {
                    id: recentItem
                    required property var modelData
                    Layout.fillWidth: true; enabled: !backend.busy
                    text: modelData.from + " → " + modelData.to + "   (" + window.linkCountText(modelData.links) + ")"
                    contentItem: Text { text: recentItem.text; color: window.blue; elide: Text.ElideRight; font.pixelSize: 13; verticalAlignment: Text.AlignVCenter }
                    onClicked: {
                        startInput.text = modelData.from
                        targetInput.text = modelData.to
                        algorithmBox.currentIndex = modelData.algorithmIndex
                        recentDialog.close()
                        window.findRoute()
                    }
                }
            }
        }
    }
}
