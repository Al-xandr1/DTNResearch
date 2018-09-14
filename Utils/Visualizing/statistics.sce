IS_COMMON_SCE_LOADED = COMMON_SCE_LOADED;//чтобы убедиться, что файл common.sce загружен

//------------------- Функции для Статистики маршрутов *.pst -------------------
function printPstFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    pstFiles = getFiles(PATH, "*.pst");

    tags = ["ROOTS-DIMENSION-HISTOGRAM/MIN" ; "ROOTS-DIMENSION-HISTOGRAM/MAX" ; "ROOTS-DIMENSION-HISTOGRAM/AVERAGE"]
    privatePrintTable(pstFiles, tags);
    
    tags = ["ROOT-LENGTH-HISTOGRAM/MIN" ; "ROOT-LENGTH-HISTOGRAM/MAX" ; "ROOT-LENGTH-HISTOGRAM/AVERAGE"]
    privatePrintTable(pstFiles, tags);

    tags = ["NEW-PERSISTENCE/ETHALON-ROOT/ROOT-NUM" ; "NEW-PERSISTENCE/ETHALON-ROOT/COEF"]
    privatePrintTable(pstFiles, tags);

    PATH = SAVE_PATH;
endfunction


//--------------------- Функции для Статистики Мобильности ---------------------

//Рисование всех гистрограмм из одного файла статистики мобильности, взятых из папки folder
function drawWPHistogramsFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.stat");
    
    //privateDrawHistograms(statisticFiles, "FLIGHT-LENGTH-HISTOGRAM", "длина, [м]");      // "Flight length [meters]"
    privateDrawHistograms(statisticFiles, "VELOCITY-HISTOGRAM", "скорость, [м/с]");      // "Velocity magnitude [meters/sec]"
    //privateDrawHistograms(statisticFiles, "PAUSE-HISTOGRAM", "пауза, [с]");              // "Pause time [sec]"
    
    PATH = SAVE_PATH;
endfunction


//Рисование всех гистрограмм из файлов статистики мобильности
function drawWPHistograms(varargin)
    fileNames = [];
    for i = 1 : argn(2)// rhs - количество входных параметров
        fileNames = [fileNames ; varargin(i)];
    end
    if (size(fileNames, 1) < 1) then
        error(msprintf("drawWPHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    privateDrawHistograms(fileNames, "FLIGHT-LENGTH-HISTOGRAM", "длина, [м]");      // "Flight length [meters]"
    privateDrawHistograms(fileNames, "VELOCITY-HISTOGRAM", "скорость, [м/с]");      // "Velocity magnitude [meters/sec]"
    privateDrawHistograms(fileNames, "PAUSE-HISTOGRAM", "пауза, [с]");              // "Pause time [sec]"
endfunction


//-------------------- Функции для Статистики Маршрутизации --------------------

//Рисование всех гистрограмм из одного файла статистики маршрутизации, взятых из папки folder
function drawNodeHistogramsFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.xml");
    
    privateDrawHistograms(statisticFiles, "LIFE-TIME-HISTOGRAM", "время жизни, [с]");    // "Life time [sec]"
    privateDrawHistograms(statisticFiles, "ICT-HISTOGRAM", "время контакта, [с]");       // "ICT [sec]"
    
    PATH = SAVE_PATH;
endfunction


//Рисование всех гистрограмм из одного файла статистики маршрутизации
function drawNodeHistograms(varargin)
    fileNames = [];
    for i = 1 : argn(2)// rhs - количество входных параметров
        fileNames = [fileNames ; varargin(i)];
    end
    if (size(fileNames, 1) < 1) then
        error(msprintf("drawWPHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    privateDrawHistograms(fileNames, "LIFE-TIME-HISTOGRAM", "время жизни, [с]");         // "Life time [sec]"
    privateDrawHistograms(fileNames, "ICT-HISTOGRAM", "время между контактами, [с]");    // "ICT [sec]"
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
    fileNames = [];
    for i = 1 : argn(2)// rhs - количество входных параметров
        fileNames = [fileNames ; varargin(i)];
    end
    if (size(fileNames, 1) < 1) then
        error(msprintf("drawWPHistograms: Ожидалось один или более параметров (имён файлов)"));
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
    
    
    scf();
    //рисуем полигон частот
    legenda = [];  colorLoc = GRAPH_COLOR;
    for i=1:fileCount
        doc = xmlRead(PATH + filenames(i));
        len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
        pdf = getVector(doc, "//" + tag + "/PDF-VALS/text()", cells(i));
        plot2d(len, pdf, colorLoc);
        colorLoc = colorLoc + COLOR_OFFSET;
        if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
        legenda = [ legenda ; ('PDF из  ' + filenames(i)) ];
        xmlDelete(doc);
    end
    if (SHOW_LEGEND == 1) then hl=legend(legenda); end
    prepareGraphic("PDF для "+ tag, xlable, "PDF");


    scf();
    legenda = [];  colorLoc = GRAPH_COLOR;
    for i=1:fileCount
        doc = xmlRead(PATH + filenames(i));
        len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
        cdf = getVector(doc, "//" + tag + "/CDF-VALS/text()", cells(i));
        cdf1 = [];
        for k=1:cells(i)
            if (cdf(k) < 1) then cdf1(k) = cdf(k); else break; end;
        end
        secs = [];
        for k=1:size(cdf1, 1) secs(k) = len(k); end
        plot2d(log2(secs), log2(cdf1), colorLoc);
        colorLoc = colorLoc + COLOR_OFFSET;
        if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
        legenda = [ legenda ; ('CDF из  ' + filenames(i)) ];
        xmlDelete(doc);
    end
    if (SHOW_LEGEND == 1) then h1=legend(legenda, 4); end
    prepareGraphic("CDF для " + tag, "log2( " + xlable + " )", "log2( CDF : P(X < x) )");
    

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
        if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
        legenda = [ legenda ; ('CCDF из  ' + filenames(i)) ];
        xmlDelete(doc);
    end
    if (SHOW_LEGEND == 1) then hl=legend(legenda, 3); end
    prepareGraphic("CCDF для "+ tag, "log2( " + xlable + " )", "log2( CCDF : P(X > x) )");
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

//Рисование зависимости Dx от масштаба по имени файла СТАТИСТИКИ (*.STAT)
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


//Рисование зависимости log(Dx) от log(масштаба) по имени файла СТАТИСТИКИ (*.STAT)
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


//Рисование зависимости Dx от масштаба по имени файла variances.txt
function drawDXtxt(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawDXtxt: Ожидалось один или более параметров (имён файлов с дисперсиями)"));
    end

    base = 4;   // число подобластей на каждом уровне    
    legenda = [];  colorLoc = GRAPH_COLOR;
    for i = 1 : rhs
        dispertionPerLevel = read(PATH + varargin(i), 8, 2);
        levels = dispertionPerLevel(1:8, 1);
        DX = dispertionPerLevel(1:8, 2); 

        scf(1); 
        plot2d(base^levels, DX, colorLoc);
        
        scf(2); 
        LOG_areaCount = log2(base^levels);
        LOG_DX = log2(DX);  
        plot2d(LOG_areaCount, LOG_DX, colorLoc);
        
        colorLoc = colorLoc + COLOR_OFFSET;
        if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
        legenda = [ legenda ; ('DX из  ' + varargin(i)) ];
    end

    if (SHOW_LEGEND == 1) then 
        scf(1); 
        hl=legend(legenda, 3); 
        scf(2); 
        hl=legend(legenda, 3);
    end
    
    scf(1); 
    prepareGraphic("График изменения дисперсий", "pow(N,l)", "DX");
    scf(2);
    prepareGraphic("График изменения дисперсий (логарифмические оси)" , "log2( pow(N,l) )", "log2( DX )");    
endfunction


// функция для минимизации для построения линии МНК
function [zr]=F(c,z)
     zr=z(2)-c(1)*z(1)-c(2);
endfunction
