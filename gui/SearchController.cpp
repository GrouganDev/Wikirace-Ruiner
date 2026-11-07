#include "SearchController.h"
#include "../src/Graph.h"
#include "../lib/sqlite3.h"

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QtConcurrent/QtConcurrentRun>
#include <climits>
#include <stdexcept>

namespace {
using Connection = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

Connection openNames(const QString& path) {
    sqlite3* raw = nullptr;
    const int code = sqlite3_open_v2(path.toUtf8().constData(), &raw, SQLITE_OPEN_READONLY, nullptr);
    Connection database(raw, sqlite3_close);
    if (code != SQLITE_OK) {
        throw std::runtime_error("Cannot open names.db. Check that it is a valid SQLite database.");
    }
    return database;
}

Statement prepare(sqlite3* database, const char* sql) {
    sqlite3_stmt* raw = nullptr;
    const int code = sqlite3_prepare_v2(database, sql, -1, &raw, nullptr);
    Statement statement(raw, sqlite3_finalize);
    if (code != SQLITE_OK) {
        throw std::runtime_error("The database needs a names table with id and name columns.");
    }
    return statement;
}

int articleId(sqlite3* database, const QString& title) {
    auto statement = prepare(database, "SELECT id FROM names WHERE name = ? COLLATE NOCASE LIMIT 1");
    const QByteArray utf8 = title.toUtf8();
    sqlite3_bind_text(statement.get(), 1, utf8.constData(), utf8.size(), SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement.get());
    if (code == SQLITE_ROW) {
        return sqlite3_column_int(statement.get(), 0);
    }
    if (code != SQLITE_DONE) {
        throw std::runtime_error("Could not read the names database.");
    }
    throw std::runtime_error(QString("Article not found: %1. Try its full Wikipedia title.")
                                 .arg(title).toStdString());
}

QString articleTitle(sqlite3* database, int id) {
    auto statement = prepare(database, "SELECT name FROM names WHERE id = ? LIMIT 1");
    sqlite3_bind_int(statement.get(), 1, id);
    const int code = sqlite3_step(statement.get());
    if (code == SQLITE_ROW) {
        return QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(statement.get(), 0)));
    }
    if (code != SQLITE_DONE) {
        throw std::runtime_error("Could not read an article title from the database.");
    }
    return QString("Article #%1").arg(id);
}

bool containsArticle(const Graph& graph, int id) {
    if (graph.map.count(id)) {
        return true;
    }
    for (const auto& entry : graph.map) {
        if (entry.second.count(id)) {
            return true;
        }
    }
    return false;
}

QString dataRoot() {
    const QString overrideRoot = qEnvironmentVariable("WIKIRUINER_DATA_DIR");
    if (!overrideRoot.isEmpty()) {
        return overrideRoot;
    }
    QStringList roots = {QStringLiteral(WIKIRUINER_DATA_ROOT), QDir::currentPath()};
    QDir executableDirectory(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 6; ++i) {
        roots.append(executableDirectory.absolutePath());
        if (!executableDirectory.cdUp()) {
            break;
        }
    }
    for (const auto& root : roots) {
        if (QFileInfo::exists(root + "/dataset/graphdata_small.bin") &&
            QFileInfo::exists(root + "/database/names.db")) {
            return root;
        }
    }
    return QStringLiteral(WIKIRUINER_DATA_ROOT);
}
} // namespace

SearchController::SearchController(QObject* parent) : QObject(parent) {
    connect(&m_watcher, &QFutureWatcher<Result>::finished, this, [this]() {
        const Result result = m_watcher.result();
        m_busy = false;
        m_status = result.status;
        if (result.loading) {
            m_graph = result.graph;
            m_databasePath = result.databasePath;
            m_nodeCount = result.nodes;
            m_edgeCount = result.edges;
        } else {
            m_route = result.route;
            m_elapsedMs = result.elapsedMs;
        }
        emit changed();
        if (!result.loading) {
            emit searchCompleted();
        }
    });
    QTimer::singleShot(0, this, &SearchController::loadData);
}

SearchController::~SearchController() {
    // A worker holds a shared graph reference and never touches the QML screen.
    m_watcher.waitForFinished();
}

void SearchController::loadData() {
    if (m_busy) {
        return;
    }
    m_busy = true;
    m_graph.reset();
    m_nodeCount = 0;
    m_edgeCount = 0;
    m_route.clear();
    m_elapsedMs = -1;
    m_status = "Loading the Wikipedia graph...";
    emit changed();

    const QString root = dataRoot();
    m_watcher.setFuture(QtConcurrent::run([root]() {
        Result result;
        result.loading = true;
        try {
            const QString graphPath = QDir(root).filePath("dataset/graphdata_small.bin");
            result.databasePath = QDir(root).filePath("database/names.db");
            QFile file(graphPath);
            if (!file.open(QIODevice::ReadOnly)) {
                throw std::runtime_error(QString("Add graphdata_small.bin to %1/dataset, then click Retry.")
                                             .arg(QDir::cleanPath(root)).toStdString());
            }
            const qint64 rowBytes = 2 * sizeof(int);
            const qint64 bytes = file.size();
            if (bytes <= 0 || bytes % rowBytes || bytes > INT_MAX) {
                throw std::runtime_error("The graph file is empty, incomplete, or too large for this graph loader.");
            }
            file.close();
            auto database = openNames(result.databasePath);
            auto schema = prepare(database.get(), "SELECT id, name FROM names LIMIT 1");
            auto graph = std::make_shared<Graph>(graphPath.toStdString(), int(bytes / rowBytes), 2);
            if (graph->map.empty()) {
                throw std::runtime_error("The graph did not load. Check graphdata_small.bin and retry.");
            }
            std::unordered_set<int> articles;
            for (const auto& entry : graph->map) {
                articles.insert(entry.first);
                articles.insert(entry.second.begin(), entry.second.end());
                result.edges += double(entry.second.size());
            }
            result.nodes = double(articles.size());
            result.graph = std::move(graph);
            result.status = "Graph ready. Choose two articles to find a route.";
        } catch (const std::exception& error) {
            result.status = QString::fromUtf8(error.what());
        }
        return result;
    }));
}

void SearchController::search(const QString& start, const QString& target, int algorithmIndex) {
    if (m_busy || !m_graph) {
        return;
    }
    const QString from = start.trimmed();
    const QString to = target.trimmed();
    m_route.clear();
    m_elapsedMs = -1;
    if (from.isEmpty() || to.isEmpty()) {
        m_status = "Enter both a starting article and a destination article.";
        emit changed();
        return;
    }
    m_busy = true;
    m_algorithm = algorithmIndex == 1 ? "Dijkstra" : "Breadth-First Search";
    m_status = "Searching for the shortest connection...";
    emit changed();
    const auto graph = m_graph;
    const QString databasePath = m_databasePath;
    m_watcher.setFuture(QtConcurrent::run([graph, databasePath, from, to, algorithmIndex]() {
        Result result;
        try {
            auto database = openNames(databasePath);
            const int startId = articleId(database.get(), from);
            const int targetId = articleId(database.get(), to);
            QStringList missingArticles;
            if (!containsArticle(*graph, startId)) {
                missingArticles.append(QString("Starting article \"%1\" is outside the loaded graph.").arg(from));
            }
            if (!containsArticle(*graph, targetId)) {
                missingArticles.append(QString("Destination article \"%1\" is outside the loaded graph.").arg(to));
            }
            if (!missingArticles.isEmpty()) {
                throw std::runtime_error((missingArticles.join('\n') +
                    "\nTry titles in the smaller dataset.").toStdString());
            }
            QElapsedTimer timer;
            timer.start();
            const auto path = algorithmIndex == 1 ? graph->dij(startId, targetId)
                                                 : graph->BFS(startId, targetId);
            result.elapsedMs = timer.nsecsElapsed() / 1000000.0;
            for (int id : path) {
                result.route.append(articleTitle(database.get(), id));
            }
            result.status = path.empty() ? "No directed path connects these articles in the loaded graph."
                                        : "Route found. Follow the links below.";
        } catch (const std::exception& error) {
            result.status = QString::fromUtf8(error.what());
        }
        return result;
    }));
}

void SearchController::clearRoute() {
    if (m_busy) {
        return;
    }
    m_route.clear();
    m_elapsedMs = -1;
    m_status = ready() ? "Graph ready. Choose two articles to find a route." : m_status;
    emit changed();
}
