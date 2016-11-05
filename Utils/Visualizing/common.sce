//---------------------------- Параметры ---------------------------------------

// Директория, в которой лежат нужные файлы или папки: DTNResearch\Utils\Visualizing\input\
PATH = 'C:\Users\sbt-tsarev-aa\Soft\omnetpp-4.6\GitHub\DTNResearch\Utils\Visualizing\input\';
SEPARATOR = '\';

GRAPH_COLOR = 2;    // Цвет первого графика
COLOR_OFFSET = 1;   // дробление цветового диапазона (для большого числа трасс ставить меньше значение)
CHANGE_COLOR = 1;

SHOW_LEGEND = 1;    // 1 - показывать легенду, 0 - НЕ показывать легенду

//---------------------------- Параметры ---------------------------------------



//---------------------------- Вспомогательные функции -------------------------

// Получаем файлы по указанному пути
function [files] = getFiles(path, pattern)
    cd(path);
    files = ls(pattern);
    files = invert(files);
    printf("Список фалов: "); 
    disp(files);
    printf("\n");
    filesCount = size(files, 1);
    if (filesCount < 1) then
        error(msprintf("getFiles: Нет файлов для обработки в директории " + path));
    end
endfunction


//Инвертируем массив-столбец
function [invX] = invert(x)
    n = size(x, 'r');
    invX = [];
    for (i = 1 : n )
        invX = [invX ; x(n - i + 1)];
    end
endfunction


//Общие настройки текущего графика
function prepareGraphic(title1, xlable, ylable) 
    xtitle(title1);
    xgrid();
    a=gca();
    a.x_label.text=xlable;
    a.x_label.font_size=3;
    a.y_label.text=ylable;
    a.Y_label.font_size=3;
endfunction


// Чтение вещественного числа из xml тега
function [result] = getDoubleFromXml(doc, xmlPath)
    xmlList = xmlXPath(doc, xmlPath);//take element from xmlPath
    result = strtod(xmlList(1).content);
endfunction


// Чтение большой строки чисел как вектор маленьких строк
function [result] = getVector(doc, xmlPath, limit)
    xmlList = xmlXPath(doc, xmlPath);//take element from xmlPath
    bigString = xmlList(1).content;
    strs = strsplit(bigString(1), "  ", limit);
    for i = 1 : limit
       vec(i) = strtod(strs(i));
    end
    result = vec;
endfunction


COMMON_SCE_LOADED = "common.sce loaded";//для контроля загрузки из других скрипт фалов
disp(COMMON_SCE_LOADED);
