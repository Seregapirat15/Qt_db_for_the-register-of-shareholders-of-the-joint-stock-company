-- Проверка существования представления
SELECT EXISTS (
    SELECT FROM information_schema.views 
    WHERE table_schema = 'public' 
    AND table_name = 'посещаемость_собраний'
) AS view_exists;

-- Проверка содержимого представления
SELECT * FROM Посещаемость_собраний; 