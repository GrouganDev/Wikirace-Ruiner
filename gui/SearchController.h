#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QStringList>
#include <QtQml/qqmlregistration.h>
#include <memory>

struct Graph;

// The QML screen talks to this adapter; the original graph classes stay unchanged.
class SearchController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool busy READ busy NOTIFY changed)
    Q_PROPERTY(bool ready READ ready NOTIFY changed)
    Q_PROPERTY(QString status READ status NOTIFY changed)
    Q_PROPERTY(QStringList route READ route NOTIFY changed)
    Q_PROPERTY(QString algorithm READ algorithm NOTIFY changed)
    Q_PROPERTY(double elapsedMs READ elapsedMs NOTIFY changed)
    Q_PROPERTY(double nodeCount READ nodeCount NOTIFY changed)
    Q_PROPERTY(double edgeCount READ edgeCount NOTIFY changed)

public:
    explicit SearchController(QObject* parent = nullptr);
    ~SearchController() override;

    bool busy() const { return m_busy; }
    bool ready() const { return bool(m_graph); }
    QString status() const { return m_status; }
    QStringList route() const { return m_route; }
    QString algorithm() const { return m_algorithm; }
    double elapsedMs() const { return m_elapsedMs; }
    double nodeCount() const { return m_nodeCount; }
    double edgeCount() const { return m_edgeCount; }

    Q_INVOKABLE void loadData();
    Q_INVOKABLE void search(const QString& start, const QString& target, int algorithmIndex);
    Q_INVOKABLE void clearRoute();

signals:
    void changed();
    void searchCompleted();

private:
    struct Result {
        bool loading = false;
        std::shared_ptr<Graph> graph;
        QString databasePath;
        QString status;
        QStringList route;
        double elapsedMs = -1;
        double nodes = 0;
        double edges = 0;
    };

    QFutureWatcher<Result> m_watcher;
    std::shared_ptr<Graph> m_graph;
    QString m_databasePath;
    QString m_status = "Preparing graph...";
    QStringList m_route;
    QString m_algorithm = "Breadth-First Search";
    double m_elapsedMs = -1;
    double m_nodeCount = 0;
    double m_edgeCount = 0;
    bool m_busy = false;
};
