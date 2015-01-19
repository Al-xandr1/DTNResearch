//---------------------------- Параметры ---------------------------------------
// Директория, в которой лежат нужные файлы или папки
//PATH = '/Volumes/Macintosh/Users/Alexander/soft/omnetpp-4.5/GitHub/WaypointGenerator/';
PATH = '/Volumes/Macintosh/Users/Alexander/Dropbox/Postgraduate/Dissertation/';
SEPARATOR = '/';

GRAPH_COLOR = 2;    // Цвет первого графика
COLOR_OFFSET = 0.25;   // дробление цветового диапазона (для большого числа трасс ставить меньше значение)

SHOW_LEGEND = 1;    // 1 - показывать легенду, 0 - НЕ показывать легенду
//---------------------------- Параметры ---------------------------------------



//------------------ Функции для рисования ТРАСС -------------------------------

//Рисование трассы по имени файла
function drawTrace(filename)
    fd = mopen(PATH + filename, 'rt');
    l = mfscanf(-1, fd, '%lg %lg %lg');  
    
    n = size(l, 1); 
    x = l(1:n, 2)';
    y = l(1:n, 3)'; 

    plot2d(x, y, [GRAPH_COLOR]);
//    a=gca(); //Выделение начала и конца трассы
//    poly1= a.children(1).children(1);
//    poly1.mark_style = 14;
//    poly1.mark_size = 4;
//    poly1.mark_offset = 0;
//    poly1.mark_stride = n-1;
//    poly1.mark_foreground = GRAPH_COLOR;
//    poly1.mark_background = GRAPH_COLOR;

    xtitle("Trace: " + filename);
    xgrid();
    
    mclose(fd);
endfunction


//Рисование трасс из входного вектора с именами трасс (ФУНКЦИЯ НЕ ДЛЯ ПРЯМОГО ИСПОЛЬЗОВАНИЯ)
function drawAllTraces(fileNamesVector)
    fileCount = size(fileNamesVector, 1);
    if (fileCount < 1) then
        error(msprintf("drawAllTraces: Нет файлов для обработки"));
    end
    save_color = GRAPH_COLOR;
    
    //Рисование графиков и формирование легенды
    legenda = [];
    for i = 1 : fileCount
        drawTrace(fileNamesVector(i));
        legenda = [ legenda ; ('Trace  ' + fileNamesVector(i)) ];
        
        GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET;
    end

    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    xtitle("Traces");
    GRAPH_COLOR = save_color;
endfunction


//Рисование трасс, перечисленных в параметрах данной функции
function drawTraces(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawTraces: Ожидалось один или более параметров (имён файлов)"));
    end
    
    traceFiles = [];
    for i = 1 : rhs
        traceFiles = [traceFiles ; varargin(i)];
    end
    
    drawAllTraces(traceFiles);
endfunction


//Рисование трасс, взятых из папки folder
function drawTracesFolder(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    traceFiles = getFiles(PATH, "*.txt");
    
    drawAllTraces(traceFiles);
    xtitle("Traces from " + folder);
    
    PATH = SAVE_PATH;
endfunction




//------------------ Функции для рисования WAYPOINT'ов -------------------------

//Рисование WayPoint'ов по имени файла
function drawWPFile(filename)
    fd = mopen(PATH + filename, 'rt');
    l = mfscanf(-1, fd, '%lg %lg %lg %lg');  
    
    n = size(l, 1); 
    x = l(1:n, 1)';
    y = l(1:n, 2)'; 

    plot2d(x, y, [GRAPH_COLOR]);
    a=gca(); 
    poly1= a.children(1).children(1);
    poly1.line_mode = 'off';
    poly1.mark_style = 9;
    poly1.mark_foreground = GRAPH_COLOR;
    poly1.mark_background = GRAPH_COLOR;

    xtitle("WayPoints: " + filename);
    xgrid();
    
    mclose(fd);
endfunction


//Рисование WayPoint'ов из входного вектора с именами файлов (ФУНКЦИЯ НЕ ДЛЯ ПРЯМОГО ИСПОЛЬЗОВАНИЯ)
function drawAllWPFiles(fileNamesVector)
    fileCount = size(fileNamesVector, 1);
    if (fileCount < 1) then
        error(msprintf("drawAllWPFiles: Нет файлов для обработки"));
    end
    save_color = GRAPH_COLOR;
    
    //Рисование графиков и формирование легенды
    legenda = [];
    for i = 1 : fileCount
        drawWPFile(fileNamesVector(i));
        legenda = [ legenda ; ('WayPoints  ' + fileNamesVector(i)) ];
        
        GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET;
    end

    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    xtitle("WayPoints");
    GRAPH_COLOR = save_color;
endfunction


//Рисование файлов WayPoint'ов, перечисленных в параметрах данной функции
function drawWPFiles(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawWPFiles: Ожидалось один или более параметров (имён файлов)"));
    end
    
    wpFiles = [];
    for i = 1 : rhs
        wpFiles = [wpFiles ; varargin(i)];
    end
    
    drawAllWPFiles(wpFiles);
endfunction


//Рисование файлов WayPoint'ов, взятых из папки folder
function drawWPFilesFolder(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    wpFiles = getFiles(PATH, "*.wpt");

    drawAllWPFiles(wpFiles);
    xtitle("WayPoints from " + folder);
    
    PATH = SAVE_PATH;
endfunction




//------------------ Функции для рисования ТРАСС и WAYPOINT'ов ------------------

//Рисование трассы и соответствующих WayPoint'ов по именам файлов
function drawTraceAndWP(traceFilename, wpFilename)
    drawTrace(traceFilename);
    drawWPFile(wpFilename); 

    if (SHOW_LEGEND == 1) then
        legenda = [ 'Trace  ' + traceFilename ; ('WayPoints  ' + wpFilename) ];  
        hl=legend(legenda);
    end
    
    xtitle("Trace and WayPoints");
endfunction


//Рисование трасс и соответствующих им WayPoint'ов из указанных папок (порядок файлов в этих папка должен совпадать)
function drawTraceAndWPFolder(traceFolder, wpFolder)
    SAVE_PATH = PATH;
    
    traces = getFiles(PATH + traceFolder + SEPARATOR, "*.txt");
    traceCount = size(traces, 1);

    wps = getFiles(PATH + wpFolder + SEPARATOR, "*.wpt");
    wpCount = size(wps, 1);

    if (traceCount <> wpCount) then
        error(msprintf("drawTraceAndWPFolder: Число трасс в папке " + traceFolder + " не совпадает с числом файлов WayPointов в папке " + wpFolder));
    end
    
    save_color = GRAPH_COLOR;
    
    //Рисование графиков и формирование легенды
    legenda = [];
    for i = 1 : traceCount
        PATH = PATH + traceFolder + SEPARATOR;
        drawTrace(traces(i));
        PATH = SAVE_PATH;
        
        PATH = PATH + wpFolder + SEPARATOR;
        drawWPFile(wps(i));        
        PATH = SAVE_PATH;
        
        legenda = [ legenda ; ('Trace  ' + traces(i)) ];
        legenda = [ legenda ; ('WayPoints  ' + wps(i)) ];
            
        GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET;
    end
    
    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    GRAPH_COLOR = save_color;
    xtitle("Traces from "+ traceFolder + "and corresponding WayPoints from " + wpFolder);
endfunction



//------------------ Функции для рисования Статистики -------------------------

//Рисование зависимости Dx от мастаба по имени файла
function drawStat(filename)
    fd = mopen(PATH + filename, 'rt');
    l = mfscanf(-1, fd, '%lg %lg %lg');  
    
    n = size(l, 1); 
    areaCount = l(1:n, 1)';
    DX = l(1:n, 3)';  

    plot2d(areaCount, DX, GRAPH_COLOR);

    if (SHOW_LEGEND == 1) then
        hl=legend([ 'DX' ]);
    end

    xtitle("Dx of points from: " + filename);
    xgrid();
    
    mclose(fd);
endfunction


//Рисование зависимости log(Dx) от log(мастаба) по имени файла
function drawLogLogStat(filename)
    fd = mopen(PATH + filename, 'rt');
    l = mfscanf(-1, fd, '%lg %lg %lg');  
    
    n = size(l, 1); 
    areaCount = l(1:n, 1)';
    DX = l(1:n, 3)';  

    plot2d(log2(areaCount), log2(DX), GRAPH_COLOR);
    da=gda();
    da.x_label.text="log(tree_level)";
    da.y_label.text="log(DX)";  
    a.labels_font_size=5;

    if (SHOW_LEGEND == 1) then
        hl=legend([ 'log2(DX)' ]);
    end

    xtitle("log-log Dx of points from: " + filename);
    xgrid();
    
    mclose(fd);
endfunction


//-------------------------- Вспомогательные функции ---------------------------
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
