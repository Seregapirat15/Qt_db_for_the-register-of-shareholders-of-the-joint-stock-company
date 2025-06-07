// Модифицированная версия метода createReportsTab с отладочной информацией
QWidget* MainWindow::createReportsTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Создаем представления для отображения отчетов
    QTableView *shareholdersReportView = new QTableView();
    QTableView *attendanceReportView = new QTableView();
    
    // Создаем модели для отчетов
    QSqlQueryModel *shareholdersReportModel = new QSqlQueryModel(this);
    shareholdersReportModel->setQuery("SELECT * FROM Акционеры_и_Акции");
    
    // Проверка ошибок для первой модели
    if (shareholdersReportModel->lastError().isValid()) {
        qDebug() << "Ошибка запроса к Акционеры_и_Акции:" << shareholdersReportModel->lastError().text();
    } else {
        qDebug() << "Запрос к Акционеры_и_Акции успешен. Строк:" << shareholdersReportModel->rowCount();
    }
    
    QSqlQueryModel *attendanceReportModel = new QSqlQueryModel(this);
    
    // Выполняем прямой SQL-запрос вместо использования представления
    QString directQuery = "SELECT "
                          "s.ID_собрания, "
                          "s.Повестка_дня, "
                          "s.Дата, "
                          "COUNT(CASE WHEN p.Присутствие = 'Да' THEN 1 END) AS Количество_присутствующих, "
                          "COUNT(p.ID_записи) AS Общее_количество_акционеров, "
                          "ROUND((COUNT(CASE WHEN p.Присутствие = 'Да' THEN 1 END)::numeric / COUNT(p.ID_записи)) * 100, 2) AS Процент_посещаемости "
                          "FROM Собрание_акционеров s "
                          "LEFT JOIN Присутствие p ON s.ID_собрания = p.причина_собрания "
                          "GROUP BY s.ID_собрания, s.Повестка_дня, s.Дата";
    
    attendanceReportModel->setQuery(directQuery);
    
    // Проверка ошибок для второй модели
    if (attendanceReportModel->lastError().isValid()) {
        qDebug() << "Ошибка запроса к Посещаемость_собраний:" << attendanceReportModel->lastError().text();
    } else {
        qDebug() << "Запрос к Посещаемость_собраний успешен. Строк:" << attendanceReportModel->rowCount();
    }
    
    // Настройка представлений
    shareholdersReportView->setModel(shareholdersReportModel);
    shareholdersReportView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    attendanceReportView->setModel(attendanceReportModel);
    attendanceReportView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Создаем вкладки для отчетов
    QTabWidget *reportsTabWidget = new QTabWidget();
    reportsTabWidget->addTab(shareholdersReportView, "Акционеры и акции");
    reportsTabWidget->addTab(attendanceReportView, "Посещаемость собраний");
    
    layout->addWidget(reportsTabWidget);
    
    return widget;
} 