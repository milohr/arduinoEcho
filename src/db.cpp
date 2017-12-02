#include "db.h"

DB::DB()
{

}

void DB::closeDB()
{
    qDebug()<<"CLOSING COLLECTIONDB";
    m_db.close();
}

void DB::setUpCollection(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    openDB();
}

bool DB::execQuery(const QString &queryTxt)
{
    QSqlQuery query(queryTxt);
    return this->execQuery(query);
}

bool DB::execQuery(QSqlQuery &query) const
{
    if(query.exec()) return true;
    qDebug()<<"ERROR ON EXEC QUERY";
    qDebug()<<query.lastError()<<query.lastQuery();
    return false;
}

bool DB::openDB()
{
    if (m_db.open()) return true;

    qDebug() << "Error: connection with database fail" <<m_db.lastError().text();
    return false;
}


void DB::prepareCollectionDB()
{
    QSqlQuery query;

    QFile file(":/db/script.sql");

    if (!file.exists())
    {
        QString log = QStringLiteral("Fatal error on build database. The file '");
        log.append(file.fileName() + QStringLiteral("' for database and tables creation query cannot be not found!"));
        qDebug()<<log;
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<QStringLiteral("Fatal error on try to create database! The file with sql queries for database creation cannot be opened!");
        return;
    }

    bool hasText;
    QString line;
    QByteArray readLine;
    QString cleanedLine;
    QStringList strings;

    while (!file.atEnd())
    {
        hasText     = false;
        line        = "";
        readLine    = "";
        cleanedLine = "";
        strings.clear();
        while (!hasText)
        {
            readLine    = file.readLine();
            cleanedLine = readLine.trimmed();
            strings     = cleanedLine.split("--");
            cleanedLine = strings.at(0);
            if (!cleanedLine.startsWith("--") && !cleanedLine.startsWith("DROP") && !cleanedLine.isEmpty())
                line += cleanedLine;
            if (cleanedLine.endsWith(";"))
                break;
            if (cleanedLine.startsWith("COMMIT"))
                hasText = true;
        }

        if (!line.isEmpty())
        {
            if (!query.exec(line))
                qDebug()<<"exec failed"<<query.lastQuery()<<query.lastError();

        } else qDebug()<<"exec wrong"<<query.lastError();
    }
    file.close();
}

bool DB::insert(const QString &tableName, const QVariantMap &insertData)
{
    if (tableName.isEmpty())
    {
        qDebug()<<QStringLiteral("Fatal error on insert! The table name is empty!");
        return false;
    } else if (insertData.isEmpty())
    {
        qDebug()<<QStringLiteral("Fatal error on insert! The insertData is empty!");
        return false;
    }

    QStringList strValues;
    QStringList fields = insertData.keys();
    QVariantList values = insertData.values();
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        strValues.append("?");


    QString sqlQueryString = "INSERT INTO " + tableName + "(" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")";
    QSqlQuery query;
    query.prepare(sqlQueryString);

    int k = 0;

    foreach (const QVariant &value, values)
        query.bindValue(k++, value);


    return this->execQuery(query);
}
