#include "SearchController.h"
#include "../../lib/sqlite3.h"

#include <QDir>
#include <QFile>
#include <QFont>
#include <QGuiApplication>
#include <QImage>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class IntegrationTests : public QObject {
    Q_OBJECT
    QTemporaryDir m_fixture;

private slots:
    void initTestCase() {
        QVERIFY(m_fixture.isValid());
        QDir root(m_fixture.path());
        QVERIFY(root.mkpath("dataset"));
        QVERIFY(root.mkpath("database"));
        QFile file(root.filePath("dataset/graphdata_small.bin"));
        QVERIFY(file.open(QIODevice::WriteOnly));
        // A diamond, a destination-only node, and a disconnected component.
        const int edges[][2] = {{1, 2}, {1, 3}, {2, 4}, {3, 4}, {4, 5}, {6, 7}};
        QCOMPARE(file.write(reinterpret_cast<const char*>(edges), sizeof(edges)), qint64(sizeof(edges)));
        file.close();
        sqlite3* database = nullptr;
        QCOMPARE(sqlite3_open(root.filePath("database/names.db").toUtf8().constData(), &database), SQLITE_OK);
        const char* sql = "CREATE TABLE names(id INTEGER PRIMARY KEY, name TEXT NOT NULL);"
                          "INSERT INTO names VALUES(0,'Zero Article'),(1,'Alpha Article'),"
                          "(2,'Beta Article'),(3,'Gamma Article'),(4,'Delta Article'),"
                          "(5,'End Article'),(6,'Island Article'),(7,'Island End'),(8,'Outside Article');";
        const int code = sqlite3_exec(database, sql, nullptr, nullptr, nullptr);
        sqlite3_close(database);
        QCOMPARE(code, SQLITE_OK);
        qputenv("WIKIRUINER_DATA_DIR", m_fixture.path().toUtf8());
    }

    void searchesAndErrors() {
        SearchController controller;
        QTRY_VERIFY_WITH_TIMEOUT(controller.ready(), 10000);
        QCOMPARE(controller.nodeCount(), 7.0);
        QCOMPARE(controller.edgeCount(), 6.0);
        QSignalSpy finished(&controller, &SearchController::searchCompleted);
        for (int algorithm = 0; algorithm < 2; ++algorithm) {
            controller.search("alpha article", "END ARTICLE", algorithm);
            QVERIFY(controller.busy());
            QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 10000);
            finished.clear();
            QCOMPARE(controller.route().size(), 4);
            QCOMPARE(controller.route().first(), QString("Alpha Article"));
            QCOMPARE(controller.route().last(), QString("End Article"));
            QVERIFY(controller.elapsedMs() >= 0);

            controller.search("End Article", "Alpha Article", algorithm);
            QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 10000);
            finished.clear();
            QVERIFY(controller.route().isEmpty());
            QVERIFY(controller.status().startsWith("No directed path"));

            controller.search("End Article", "End Article", algorithm);
            QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 10000);
            finished.clear();
            QCOMPARE(controller.route(), QStringList{"End Article"});
        }
        controller.search("Not a title", "Alpha Article", 0);
        QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 10000);
        finished.clear();
        QVERIFY(controller.status().startsWith("Article not found"));
        QVERIFY(controller.route().isEmpty());
        controller.search("Outside Article", "Alpha Article", 0);
        QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 10000);
        finished.clear();
        QVERIFY(controller.status().contains("outside this graph"));
        controller.search("   ", "Alpha Article", 0);
        QVERIFY(!controller.busy());
        QVERIFY(controller.status().startsWith("Enter both"));
        controller.clearRoute();
        QCOMPARE(controller.elapsedMs(), -1.0);
    }

    void missingDataAndRetry() {
        QTemporaryDir emptyDirectory;
        qputenv("WIKIRUINER_DATA_DIR", emptyDirectory.path().toUtf8());
        SearchController controller;
        QTRY_VERIFY_WITH_TIMEOUT(controller.status().contains("Add graphdata_small.bin"), 10000);
        QVERIFY(!controller.ready());
        QVERIFY(!controller.busy());
        qputenv("WIKIRUINER_DATA_DIR", m_fixture.path().toUtf8());
        controller.loadData();
        QTRY_VERIFY_WITH_TIMEOUT(controller.ready(), 10000);
    }

    void windowAndButton() {
        QQmlApplicationEngine engine;
        QSignalSpy warnings(&engine, &QQmlEngine::warnings);
        engine.loadFromModule("untitled", "Main");
        QVERIFY2(!engine.rootObjects().isEmpty(), "Main.qml did not load");
        auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
        QVERIFY(window);
        auto* controller = window->findChild<SearchController*>("searchController");
        QVERIFY(controller);
        QTRY_VERIFY_WITH_TIMEOUT(controller->ready(), 10000);
        auto* from = window->findChild<QObject*>("startInput");
        auto* to = window->findChild<QObject*>("targetInput");
        auto* button = window->findChild<QQuickItem*>("findButton");
        QVERIFY(from && to && button);
        from->setProperty("text", "Alpha Article");
        to->setProperty("text", "End Article");
        QVERIFY(button->isEnabled());
        QVERIFY(QMetaObject::invokeMethod(button, "clicked"));
        QTRY_VERIFY_WITH_TIMEOUT(controller->route().size() == 4, 10000);
        const QString captures = qEnvironmentVariable("WIKIRUINER_TEST_CAPTURE_DIR");
        for (const QSize size : {QSize(1440, 980), QSize(1000, 760)}) {
            window->resize(size);
            QTest::qWait(150);
            const QImage image = window->grabWindow();
            QVERIFY(!image.isNull());
            if (!captures.isEmpty()) {
                QDir().mkpath(captures);
                QVERIFY(image.save(captures + QString("/gui-%1.png").arg(size.width())));
            }
        }
        QCOMPARE(warnings.count(), 0);
    }

    void realDataset() {
        const QString root = QStringLiteral(WIKIRUINER_DATA_ROOT);
        QFile file(root + "/dataset/graphdata_small.bin");
        if (!file.open(QIODevice::ReadOnly)) {
            QSKIP("The optional Wikipedia dataset is not installed.");
        }
        int edge[2];
        QCOMPARE(file.read(reinterpret_cast<char*>(edge), sizeof(edge)), qint64(sizeof(edge)));
        sqlite3* database = nullptr;
        QCOMPARE(sqlite3_open_v2((root + "/database/names.db").toUtf8().constData(),
                                &database, SQLITE_OPEN_READONLY, nullptr), SQLITE_OK);
        QStringList titles;
        for (int id : edge) {
            sqlite3_stmt* statement = nullptr;
            QCOMPARE(sqlite3_prepare_v2(database, "SELECT name FROM names WHERE id = ?", -1,
                                       &statement, nullptr), SQLITE_OK);
            sqlite3_bind_int(statement, 1, id);
            QCOMPARE(sqlite3_step(statement), SQLITE_ROW);
            titles.append(QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0))));
            sqlite3_finalize(statement);
        }
        sqlite3_close(database);
        qputenv("WIKIRUINER_DATA_DIR", root.toUtf8());
        QQmlApplicationEngine engine;
        engine.loadFromModule("untitled", "Main");
        QVERIFY(!engine.rootObjects().isEmpty());
        auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
        auto* controller = window->findChild<SearchController*>("searchController");
        QVERIFY(controller);
        QTRY_VERIFY_WITH_TIMEOUT(controller->ready(), 30000);
        window->findChild<QObject*>("startInput")->setProperty("text", titles[0]);
        window->findChild<QObject*>("targetInput")->setProperty("text", titles[1]);
        QVERIFY(QMetaObject::invokeMethod(window->findChild<QObject*>("findButton"), "clicked"));
        QTRY_VERIFY_WITH_TIMEOUT(!controller->busy(), 30000);
        QCOMPARE(controller->route(), titles);
        QVERIFY(controller->nodeCount() > 7);
        const QString captures = qEnvironmentVariable("WIKIRUINER_TEST_CAPTURE_DIR");
        if (!captures.isEmpty()) {
            QTest::qWait(150);
            QVERIFY(window->grabWindow().save(captures + "/gui-real.png"));
        }
        qputenv("WIKIRUINER_DATA_DIR", m_fixture.path().toUtf8());
    }
};

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setFont(QFont("Segoe UI", 10));
    QQuickStyle::setStyle("Basic");
    IntegrationTests tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "IntegrationTests.moc"
