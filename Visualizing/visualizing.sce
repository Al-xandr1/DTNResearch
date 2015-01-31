//---------------------------- Параметры ---------------------------------------
// Директория, в которой лежат нужные файлы или папки
//PATH = '/Volumes/Macintosh/Users/Alexander/soft/omnetpp-4.5/GitHub/WaypointGenerator/';
PATH = 'C:\omnetpp-4.6\GitHub\DTNResearch\WaypointFinder\waypointfiles\';
SEPARATOR = '\';

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

//Рисование зависимости Dx от масштаба по имени файла
function drawStat(filename)
    doc = xmlRead(PATH + filename);
    
    base = getDoubleFromXml(doc, "//BASE/text()");
    level = getDoubleFromXml(doc, "//LEVELS/text()");
    levels = 1:1:level;
    areaCount = base^levels;
    DX = getVector(doc, "//DX/text()", level); 

    plot2d(areaCount, DX, GRAPH_COLOR);
    if (SHOW_LEGEND == 1) then
        hl=legend([ 'DX' ]);
    end

    xtitle("Dx of points from: " + filename);
    xgrid();
    
    xmlDelete(doc);
endfunction


//Рисование всех гистрограмм из одного файла статистики
function drawAllCCDF(filename)
    scf();  drawCCDF(filename, "FLIGHT-LENGTH");
    scf();  drawCCDF(filename, "VELOCITY");
    scf();  drawCCDF(filename, "PAUSE");
endfunction

//Рисование гистрограммы из одного файла статистики
function drawCCDF(filename, tag)
    doc = xmlRead(PATH + filename);
    
    cells = getDoubleFromXml(doc, "//" + tag + "/CELLS/text()");
    leftBound = getDoubleFromXml(doc, "//" + tag + "/LEFT-BOUND/text()");
    rightBound = getDoubleFromXml(doc, "//" + tag + "/RIGHT-BOUND/text()");
    cellWidth = getDoubleFromXml(doc, "//" + tag + "/CELL-WIDTH/text()");
    hist = getVector(doc, "//" + tag + "/HIST-VALS/text()", cells);

    //рисуем полигон частот
    len = (leftBound+cellWidth/2):cellWidth:rightBound;
    plot2d(len, hist, GRAPH_COLOR);
    if (SHOW_LEGEND == 1) then
        hl=legend([ 'Histogram of PDF' ]);
    end

    xtitle("PDF for "+ tag + " from: " + filename);
    xgrid();
    
    xmlDelete(doc);
endfunction



//Рисование зависимости log(Dx) от log(масштаба) по имени файла
function drawLogLogStat(filename)
    doc = xmlRead(PATH + filename);
    
    base = getDoubleFromXml(doc, "//BASE/text()");
    level = getDoubleFromXml(doc, "//LEVELS/text()");
    levels = 1:1:level;
    
    LOG_areaCount = log2(base^levels);
    LOG_DX = log2(getVector(doc, "//DX/text()", level)');  

    plot2d(LOG_areaCount, LOG_DX, -4);
    da=gda();
    da.x_label.text="log2( count_of_subareas_per_level )";
    da.y_label.text="log2( DX )"; 
    
    //Построение линии методом наименьших квадратов
    z = [LOG_areaCount; LOG_DX];
    c = [0; 0;];
    [a,S] = datafit(F,z,c);
    t = min(LOG_areaCount):0.01:max(LOG_areaCount);
    Yt = a(1)*t + a(2);

    plot2d(t, Yt, 5);  
    if (SHOW_LEGEND == 1) then
        b = atan(a(1));   
        H = 1-abs(b)/2;
        hl=legend([ "log2( DX )" ; "Least squares line, b = " + string(b) + ", H = " + string(H) ]);
    end

    xtitle("log-log Dx of points from: " + filename);
    xgrid();
    
    xmlDelete(doc);
endfunction

// функция для минимизации для построения линии МНК
function [zr]=F(c,z)
     zr=z(2)-c(1)*z(1)-c(2);
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


