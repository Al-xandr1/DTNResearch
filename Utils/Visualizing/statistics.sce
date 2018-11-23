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
    
    //privateDrawHistograms(statisticFiles, "FLIGHT-LENGTH-HISTOGRAM", "длина, [м]", 0, 0, 1);      // "Flight length [meters]"
    privateDrawHistograms(statisticFiles, "VELOCITY-HISTOGRAM", "скорость, [м/с]", 0, 0, 1);      // "Velocity magnitude [meters/sec]"
    //privateDrawHistograms(statisticFiles, "PAUSE-HISTOGRAM", "пауза, [с]", 0, 0, 1);              // "Pause time [sec]"
    
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
    
    privateDrawHistograms(fileNames, "FLIGHT-LENGTH-HISTOGRAM", "длина, [м]", 0, 0, 1);      // "Flight length [meters]"
    privateDrawHistograms(fileNames, "VELOCITY-HISTOGRAM", "скорость, [м/с]", 0, 0, 1);      // "Velocity magnitude [meters/sec]"
    privateDrawHistograms(fileNames, "PAUSE-HISTOGRAM", "пауза, [с]", 0, 0, 1);              // "Pause time [sec]"
endfunction


//-------------------- Функции для Статистики Маршрутизации --------------------

//Рисование всех гистрограмм из одного файла статистики маршрутизации, взятых из папки folder
function drawNodeHistogramsFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.xml");
    
    privateDrawHistograms(statisticFiles, "LIFE-TIME-HISTOGRAM", "время жизни, [с]", 0, 0, 1);    // "Life time [sec]"
    privateDrawHistograms(statisticFiles, "ICT-HISTOGRAM", "время контакта, [с]", 0, 0, 1);       // "ICT [sec]"
    
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
    
    privateDrawHistograms(fileNames, "LIFE-TIME-HISTOGRAM", "время жизни, [с]", 0, 0, 1);         // "Life time [sec]"
    privateDrawHistograms(fileNames, "ICT-HISTOGRAM", "время между контактами, [с]", 0, 0, 1);    // "ICT [sec]"
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
function privateDrawHistograms(filenames, tag, xlable, pdf, cdf, ccdf)
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
    
    
    if (pdf == 1) then
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
    end

    if (cdf == 1) then
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
    end

    if (ccdf == 1) then
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
    end
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

// Рисование зависимости Dx от масштаба и рисование зависимости log(Dx) от log(масштаба) по именам файлов СТАТИСТИКИ (*.STAT) 
function drawDXxml(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawDXxml: Ожидалось один или более параметров (имён файлов со статистикой)"));
    end

    bases = [];
    levels = [];
    DXs = [];
    filenames = [];
    for i = 1 : rhs
        doc = xmlRead(PATH + varargin(i));
        filenames = [filenames ; varargin(i)];
        bases     = [bases     ; getDoubleFromXml(doc, "//BASE/text()")];
        levels    = [levels    ; getDoubleFromXml(doc, "//LEVELS/text()")];
        DXs       = [DXs       , getVector(doc, "//DX/text()", levels(i))]; 
        xmlDelete(doc);
    end

    __drawDX__(filenames, bases, levels, DXs);
endfunction

//Рисование зависимости Dx от масштаба по имени файла variances.txt
function drawDXtxt(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawDXtxt: Ожидалось один или более параметров (имён файлов с дисперсиями)"));
    end

    bases = [];
    levels = [];
    DXs = [];
    filenames = [];
    for i = 1 : rhs
        dispertionPerLevel = read(PATH + varargin(i), 8, 2);
        filenames = [filenames ; varargin(i)];
        bases     = [bases     ; 4]; // число подобластей на каждом уровне
        levels    = [levels    ; 8]; // число уровней
        DXs       = [DXs       , dispertionPerLevel(1:8, 2)]; //levels(i)
    end
    
    __drawDX__(filenames, bases, levels, DXs);
endfunction

// Private. Рисование графиков дисперсии
function __drawDX__(filenames, bases, levels, DXs)
    count = size(filenames, 1);
    
    legenda = []; 
    legendaFit = [];
    colorLoc = GRAPH_COLOR;
    for i = 1 : count
        filename = filenames(i);
        base = bases(i);
        level = levels(i);
        DX = DXs(1:level, i);    

        lvls = 1:1:level;      // создаём массив из всех уровней для одного файла
        areaCount = base^lvls; // количесто прямоугольников на уровнях
        LOG_areaCount = log2(areaCount);
        LOG_DX = log2(DX');
        
        // рисование обычного графика
        scf(1); 
        plot2d(areaCount, DX, colorLoc);
        
        // рисование графика в логарифмических осях
        scf(2); 
        plot2d(LOG_areaCount, LOG_DX, colorLoc);
        
        // Построение линии методом наименьших квадратов
        scf(3);
        z = [LOG_areaCount; LOG_DX];
        c = [0; 0;];
        [a,S] = datafit(F,z,c);
        t = min(LOG_areaCount):0.01:max(LOG_areaCount);
        Yt = a(1)*t + a(2);
        plot2d(LOG_areaCount, LOG_DX, -colorLoc);  // рисуем точки дисперсий
        plot2d(t, Yt, colorLoc);                   // рисуем прямую МНК

        legenda = [ legenda ; ('D(X / EX) из  ' + filename) ];
        b = atan(a(1));   
        H = 1-abs(b)/2;
        legendaFit = [ legendaFit ; ('D(X / EX) из  ' + filename); "Линия по МНК для " + filename + ": H = " + string(H) ];
        
        colorLoc = colorLoc + COLOR_OFFSET;
        if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
    end
    
    if (SHOW_LEGEND == 1) then 
        whereLegenda = 2;
        scf(1); 
        hl=legend(legenda, whereLegenda); 
        scf(2); 
        hl=legend(legenda, whereLegenda);
        scf(3);
        hl=legend(legendaFit, whereLegenda);
    end
    
    scf(1); 
    prepareGraphic("График изменения дисперсий", "pow(N,l)", "D(X / EX)");
    scf(2);
    prepareGraphic("График изменения дисперсий (логарифмические оси)" , "log2( pow(N,l) )", "log2( D(X / EX) )");    
    scf(3);
    prepareGraphic("График изменения дисперсий (логарифмические оси) с аппроксимирующей прямой ", "log2( pow(N,l) )", "log2( D(X / EX) )");
endfunction

// функция для минимизации для построения линии МНК
function [zr]=F(c,z)
     zr=z(2)-c(1)*z(1)-c(2);
endfunction
