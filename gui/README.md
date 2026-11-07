# wikiruiner Qt interface

Use the build scripts that are supplied in this folder and explained in the root's README.

The app looks for `dataset/graphdata_small.bin` and `database/names.db` in the
repository root, independently of Qt Creator's working directory. You can point
to another directory with the same layout using the `WIKIRUINER_DATA_DIR`
environment variable. Both files stay outside the executable and are not
uploaded or modified by the app.

The QML interface follows the supplied Figma screenshot. Search is connected to
the existing `Graph::BFS` and `Graph::dij` implementations. A GUI-specific adapter
reads the names database using bound, read-only SQLite queries. The original
algorithm and database source files are unchanged.

Loading and search run on a worker thread. The app displays actual graph counts,
algorithm elapsed time, and the returned route. Click route articles to open
Wikipedia; longer paths scroll horizontally. Recent routes are kept for the
current session. The smaller dataset does not contain every title in names.db.

The reference's page-limit slider is disabled because the existing graph
algorithms do not accept a limit. The summary shows articles in the loaded graph
rather than an invented explored-node count; the existing algorithms do not
expose that metric. The Article Index sidebar entry explains title lookup rather
than opening a full database browser.

## Verification

Enable `WIKIRUINER_BUILD_TESTS` in CMake to build `wikiruiner_tests`. Tests cover
both searches, unreachable routes, destination-only nodes, identical endpoints,
invalid titles, graph membership, missing data and retry, and the QML search
button. They render the window at desktop and narrower sizes. If the real
dataset is installed, an additional test loads it and searches a known edge.

On Windows, run tests through the selected Qt kit's environment so its DLLs are
available. For offscreen rendering, set `QT_QPA_FONTDIR` to `C:/Windows/Fonts`.
`WIKIRUINER_TEST_CAPTURE_DIR` optionally selects a directory for preview PNGs.
