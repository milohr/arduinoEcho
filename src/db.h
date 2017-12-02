#ifndef DB_H
#define DB_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QDebug>


class DB
{

public:
    DB();
    void setUpCollection(const QString &path);
    void prepareCollectionDB();
    bool insert(const QString &tableName, const QVariantMap &insertData);
private:
    QSqlDatabase m_db;

    bool openDB();
    void closeDB();

    bool execQuery(const QString &query);
    bool execQuery(QSqlQuery &query) const;

};

#endif // DB_H
