//---------------------------- Параметры ---------------------------------------
// Директория, в которой лежат нужные файлы или папки
//PATH = '/Volumes/Macintosh/Users/Alexander/soft/omnetpp-4.5/GitHub/WaypointGenerator/';
PATH = 'C:\omnetpp-4.6\GitHub\DTNResearch\WaypointFinder\waypointfiles\';
SEPARATOR = '\';

GRAPH_COLOR = 2;    // Цвет первого графика
COLOR_OFFSET = 1;   // дробление цветового диапазона (для большого числа трасс ставить меньше значение)
CHANGE_COLOR = 1;

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
        
        if (CHANGE_COLOR == 1) then
            GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET;
        end
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

//Рисование всех гистрограмм из одного файла статистики
function drawNodeHistograms(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawNodeHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    fileNames = [];
    for i = 1 : rhs
        fileNames = [fileNames ; varargin(i)];
    end
    drawHistograms(fileNames, "LIFE-TIME-HISTOGRAM", "Life time, simsecs");
    drawHistograms(fileNames, "ICT-PDF-HISTOGRAM", "ICT, simsecs");
endfunction

//Рисование всех гистрограмм из одного файла статистики
function drawWPHistograms(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawWPHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    fileNames = [];
    for i = 1 : rhs
        fileNames = [fileNames ; varargin(i)];
    end
    drawHistograms(fileNames, "FLIGHT-LENGTH-HISTOGRAM", "Flight length, meters");
    //drawHistograms(fileNames, "VELOCITY-HISTOGRAM", "Velocity magnitude, meters/sec");
    //drawHistograms(fileNames, "PAUSE-HISTOGRAM", "Pause time, sec");
endfunction

//Рисование гистрограммы из одного файла статистики
function drawHistograms(filenames, tag, xlable)
    fileCount = size(filenames, 1);
    
    cells = []; cellWidth = []; leftBound = []; rightBound = [];
    for i=1:fileCount
        doc = xmlRead(PATH + filenames(i));
        cellsLoc = getDoubleFromXml(doc, "//" + tag + "/CELLS/text()");
        cellWidthLoc = getDoubleFromXml(doc, "//" + tag + "/CELL-WIDTH/text()");
        leftBoundLoc = getDoubleFromXml(doc, "//" + tag + "/LEFT-BOUND/text()");
        rightBoundLoc = getDoubleFromXml(doc, "//" + tag + "/RIGHT-BOUND/text()");
        
        
        cells      = [cells ; cellsLoc]; 
        cellWidth  = [cellWidth ; cellWidthLoc];
        leftBound  = [leftBound ; leftBoundLoc];
        rightBound = [rightBound ; rightBoundLoc];
        xmlDelete(doc);
    end
    
    
//    scf();
//    //рисуем полигон частот
//    legenda = [];  colorLoc = GRAPH_COLOR;
//    for i=1:fileCount
//        doc = xmlRead(PATH + filenames(i));
//        len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
//        pdf = getVector(doc, "//" + tag + "/PDF-VALS/text()", cells(i));
//        plot2d(len, pdf, colorLoc);
//        colorLoc = colorLoc + COLOR_OFFSET;
//        legenda = [ legenda ; ('PDF from  ' + filenames(i)) ];
//        xmlDelete(doc);
//    end
//    if (SHOW_LEGEND == 1) then hl=legend(legenda); end
//    prepareGraphic("PDF for "+ tag, xlable, "PDF");


//    scf();
//    legenda = [];  colorLoc = GRAPH_COLOR;
//    for i=1:fileCount
//        doc = xmlRead(PATH + filenames(i));
//        len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
//        cdf = getVector(doc, "//" + tag + "/CDF-VALS/text()", cells(i));
//        cdf1 = [];
//        for k=1:cells(i)
//            if (cdf(k) < 1) then cdf1(k) = cdf(k); else break; end;
//        end
//        meters = [];
//        for k=1:size(cdf1, 1) meters(k) = len(k); end
//        plot2d(meters, cdf1, colorLoc);
//        colorLoc = colorLoc + COLOR_OFFSET;
//        legenda = [ legenda ; ('CDF from  ' + filenames(i)) ];
//        xmlDelete(doc);
//    end
//    if (SHOW_LEGEND == 1) then h1=legend(legenda); end
//    prepareGraphic("CDF for "+ tag, xlable, "CDF : P(X < x))");


    scf();    
    legenda = [];  colorLoc = GRAPH_COLOR;
    for i=1:fileCount
        doc = xmlRead(PATH + filenames(i));
        len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
        ccdf = getVector(doc, "//" + tag + "/CCDF-VALS/text()", cells(i));
        ccdf1 = [];
        for k=1:cells(i)
            if (ccdf(k) > 0) then ccdf1(k) = ccdf(k); else break; end;
        end
        secs = [];
        for k=1:size(ccdf1, 1) secs(k) = len(k); end
        plot2d(log2(secs), log2(ccdf1), colorLoc);
        colorLoc = colorLoc + COLOR_OFFSET;
        legenda = [ legenda ; ('CCDF from  ' + filenames(i)) ];
        xmlDelete(doc);
    end
    if (SHOW_LEGEND == 1) then hl=legend(legenda); end
    prepareGraphic("CCDF for "+ tag, "LOG( " + xlable + " )", "LOG( CCDF : P(X > x) )");
endfunction



//Рисование зависимости Dx от масштаба по имени файла
function drawDX(filename)
    doc = xmlRead(PATH + filename);
    
    base = getDoubleFromXml(doc, "//BASE/text()");
    level = getDoubleFromXml(doc, "//LEVELS/text()");
    levels = 1:1:level;
    DX = getVector(doc, "//DX/text()", level); 

    plot2d(base^levels, DX, GRAPH_COLOR);
    if (SHOW_LEGEND == 1) then
        hl=legend([ 'DX' ]);
    end
    prepareGraphic("Dx of points from: " + filename, "count_of_subareas_per_level", "DX");
    
    xmlDelete(doc);
endfunction



//Рисование зависимости log(Dx) от log(масштаба) по имени файла
function drawLogLogDX(filename)
    doc = xmlRead(PATH + filename);
    
    base = getDoubleFromXml(doc, "//BASE/text()");
    level = getDoubleFromXml(doc, "//LEVELS/text()");
    levels = 1:1:level;
    LOG_areaCount = log2(base^levels);
    LOG_DX = log2(getVector(doc, "//DX/text()", level)');  

    plot2d(LOG_areaCount, LOG_DX, -4);
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
    prepareGraphic("log-log Dx of points from: " + filename, "log2( count_of_subareas_per_level )", "log2( DX )");
    
    xmlDelete(doc);
endfunction

// функция для минимизации для построения линии МНК
function [zr]=F(c,z)
     zr=z(2)-c(1)*z(1)-c(2);
endfunction



//-------------------------- Вспомогательные функции ---------------------------
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


