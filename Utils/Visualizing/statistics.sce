IS_COMMON_SCE_LOADED = COMMON_SCE_LOADED;//чтобы убедиться, что файл common.sce загружен

//--------------------- Функции для Статистики Мобильности ---------------------

//Рисование всех гистрограмм из файлов статистики мобильности
function drawWPHistograms(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawWPHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    fileNames = [];
    for i = 1 : rhs
        fileNames = [fileNames ; varargin(i)];
    end
    privateDrawHistograms(fileNames, "FLIGHT-LENGTH-HISTOGRAM", "Flight length, meters");
    //privateDrawHistograms(fileNames, "VELOCITY-HISTOGRAM", "Velocity magnitude, meters/sec");
    //privateDrawHistograms(fileNames, "PAUSE-HISTOGRAM", "Pause time, sec");
endfunction


//-------------------- Функции для Статистики Маршрутизации --------------------

//Рисование всех гистрограмм из одного файла статистики маршрутизации, взятых из папки folder
function drawNodeHistogramsFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.xml");
    
    privateDrawHistograms(statisticFiles, "LIFE-TIME-HISTOGRAM", "Life time, simsecs");
    privateDrawHistograms(statisticFiles, "ICT-HISTOGRAM", "ICT, simsecs");
    
    PATH = SAVE_PATH;
endfunction


//Рисование всех гистрограмм из одного файла статистики маршрутизации
function drawNodeHistograms(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawNodeHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    fileNames = [];
    for i = 1 : rhs
        fileNames = [fileNames ; varargin(i)];
    end
    privateDrawHistograms(fileNames, "LIFE-TIME-HISTOGRAM", "Life time, simsecs");
    privateDrawHistograms(fileNames, "ICT-HISTOGRAM", "ICT, simsecs");
endfunction


//Вывод в таблицу значений из файлов статистики маршрутизации, взятых из папки folder
function printNodeValuesFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.xml");
    
    tags = ["DELIVERED-PACKETS" ; "LIFE-TIME-HISTOGRAM/MEAN"]
    privatePrintTable(statisticFiles, tags);
    
    PATH = SAVE_PATH;
endfunction


//Вывод в таблицу значений из файлов статистики маршрутизации
function printNodeValues(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawNodeValues: Ожидалось один или более параметров (имён файлов)"));
    end
    
    fileNames = [];
    for i = 1 : rhs
        fileNames = [fileNames ; varargin(i)];
    end
    
    tags = ["DELIVERED-PACKETS" ; "LIFE-TIME-HISTOGRAM/MEAN"]
    privatePrintTable(fileNames, tags);
endfunction


//-------------------- Private функции для статистики --------------------------

//Рисование гистрограммы из вектора файлов статистики
function privateDrawHistograms(filenames, tag, xlable)
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


//Вывод в таблицу указанного вектора значений из вектора файлов статистики
function privatePrintTable(filenames, tags)
    fileCount = size(filenames, 1);
    tagCount = size(tags, 1);
    
    table  = [];
    header = []; header = [header , "Files:   \   Tags:"];
    for j=1:tagCount
        header = [header , tags(j)];
    end
    table = [table ; header];
    
    for i=1:fileCount
        doc = xmlRead(PATH + filenames(i));
        row = []; row = [row , filenames(i)];
        for j=1:tagCount
            val = getDoubleFromXml(doc, "//" + tags(j) + "/text()");
            row = [row , string(val)];
        end
        table = [table ; row];
        xmlDelete(doc);
    end
    
    disp(table);
endfunction


//----------------- Функции для работы с файлами дисперсий ---------------------

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
