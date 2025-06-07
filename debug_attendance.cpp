#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

void debugAttendanceReport() {
    QSqlQueryModel model;
    
    // Выполняем запрос к представлению
    model.setQuery("SELECT * FROM Посещаемость_собраний");
    
    // Проверяем наличие ошибок
    if (model.lastError().isValid()) {
        qDebug() << "Ошибка запроса к представлению:" << model.lastError().text();
        return;
    }
    
    // Проверяем количество строк и столбцов
    int rows = model.rowCount();
    int cols = model.columnCount();
    qDebug() << "Количество строк в представлении:" << rows;
    qDebug() << "Количество столбцов в представлении:" << cols;
    
    // Выводим названия столбцов
    QStringList headers;
    for (int i = 0; i < cols; ++i) {
        headers << model.headerData(i, Qt::Horizontal).toString();
    }
    qDebug() << "Столбцы:" << headers.join(", ");
    
    // Выводим первую строку данных (если есть)
    if (rows > 0) {
        QStringList firstRow;
        for (int i = 0; i < cols; ++i) {
            firstRow << model.data(model.index(0, i)).toString();
        }
        qDebug() << "Первая строка:" << firstRow.join(", ");
    }
} 