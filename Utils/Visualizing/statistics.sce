IS_COMMON_SCE_LOADED = COMMON_SCE_LOADED;//чтобы убедиться, что файл common.sce загружен

//------------------- Функции для Статистики маршрутов *.pst -------------------
function printPstFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    pstFiles = getFiles(PATH, "*.pst");

    tags = ["ROOTS-DIMENSION-HISTOGRAM/MIN" ; "ROOTS-DIMENSION-HISTOGRAM/MAX" ; "ROOTS-DIMENSION-HISTOGRAM/AVERAGE"]
    __privatePrintTable__(pstFiles, tags);
    
    tags = ["ROOT-LENGTH-HISTOGRAM/MIN" ; "ROOT-LENGTH-HISTOGRAM/MAX" ; "ROOT-LENGTH-HISTOGRAM/AVERAGE"]
    __privatePrintTable__(pstFiles, tags);

    tags = ["NEW-PERSISTENCE/ETHALON-ROOT/ROOT-NUM" ; "NEW-PERSISTENCE/ETHALON-ROOT/COEF"]
    __privatePrintTable__(pstFiles, tags);

    PATH = SAVE_PATH;
endfunction


//--------------------- Функции для Статистики Мобильности ---------------------

//Рисование всех гистрограмм из одного файла статистики мобильности, взятых из папки folder
function drawWPHistogramsFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.stat");
    
    __privateDrawHistograms__(statisticFiles, "FLIGHT-LENGTH-HISTOGRAM", "длины перемещений", "длина, [м]", 0, 0, 1);      // "Flight length [meters]"
    __privateDrawHistograms__(statisticFiles, "VELOCITY-HISTOGRAM", "скорости перемещений", "скорость, [м/с]", 0, 0, 1);      // "Velocity magnitude [meters/sec]"
    __privateDrawHistograms__(statisticFiles, "PAUSE-HISTOGRAM", "паузы в путевых точках", "пауза, [с]", 0, 0, 1);              // "Pause time [sec]"
    
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
    
    __privateDrawHistograms__(fileNames, "FLIGHT-LENGTH-HISTOGRAM", "длины перемещений", "длина, [м]", 0, 0, 1);      // "Flight length [meters]"
    __privateDrawHistograms__(fileNames, "VELOCITY-HISTOGRAM", "скорости перемещений", "скорость, [м/с]", 0, 0, 1);      // "Velocity magnitude [meters/sec]"
    __privateDrawHistograms__(fileNames, "PAUSE-HISTOGRAM", "паузы в путевых точках", "пауза, [с]", 0, 0, 1);              // "Pause time [sec]"
endfunction


//-------------------- Функции для Статистики Маршрутизации --------------------

//Рисование всех гистрограмм из одного файла статистики маршрутизации, взятых из папки folder
function drawNodeHistogramsFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.xml");
    
    __privateDrawHistograms__(statisticFiles, "LIFE-TIME-HISTOGRAM", "времени жизни пакета", "время жизни, [с]", 0, 0, 1);    // "Life time [sec]"
    __privateDrawHistograms__(statisticFiles, "ICT-HISTOGRAM", "времени взаимодействия узлов", "время контакта, [с]", 0, 0, 1);       // "ICT [sec]"
    
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
    
    __privateDrawHistograms__(fileNames, "LIFE-TIME-HISTOGRAM", "времени жизни пакета", "время жизни, [с]", 0, 0, 1);         // "Life time [sec]"
    __privateDrawHistograms__(fileNames, "ICT-HISTOGRAM", "времени взаимодействия узлов", "время между контактами, [с]", 0, 0, 1);    // "ICT [sec]"
endfunction


//Вывод в таблицу значений из файлов статистики маршрутизации, взятых из папки folder
function printNodeValuesFolder(folder)
    SAVE_PATH = PATH;
    
    if (folder<>"") then PATH = PATH + folder + SEPARATOR; end
    statisticFiles = getFiles(PATH, "*.xml");
    
    tags = ["DELIVERED-PACKETS" ; "LIFE-TIME-HISTOGRAM/MEAN"]
    __privatePrintTable__(statisticFiles, tags);
    
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
    __privatePrintTable__(fileNames, tags);
endfunction


//-------------------- Private функции для статистики --------------------------

//Рисование гистрограммы из вектора файлов статистики
function __privateDrawHistograms__(filenames, tag, graphicName, xlable, isPdf, isCdf, isCcdf)
    fileCount = size(filenames, 1);
    
    cells = []; cellWidth = []; leftBound = []; rightBound = [];
    for i=1:fileCount
        doc = xmlRead(PATH + filenames(i));
        cells      = [cells      ; getDoubleFromXml(doc, "//" + tag + "/CELLS/text()")]; 
        cellWidth  = [cellWidth  ; getDoubleFromXml(doc, "//" + tag + "/CELL-WIDTH/text()")];
        leftBound  = [leftBound  ; getDoubleFromXml(doc, "//" + tag + "/LEFT-BOUND/text()")];
        rightBound = [rightBound ; getDoubleFromXml(doc, "//" + tag + "/RIGHT-BOUND/text()")];
        xmlDelete(doc);
    end
    
    if (isPdf == 1) then
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
            legenda = [ legenda ; ('PDF ' + filenames(i)) ];
            xmlDelete(doc);
        end
        if (SHOW_LEGEND == 1) then hl=legend(legenda); end
        prepareGraphic("PDF для " + graphicName + " (логарифмические оси)", xlable, "PDF");
    end

    if (isCdf == 1) then
        scf();
        legenda = [];  colorLoc = GRAPH_COLOR;
        for i=1:fileCount
            doc = xmlRead(PATH + filenames(i));
            len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
            cdf = getVector(doc, "//" + tag + "/CDF-VALS/text()", cells(i));

            cdf_cutted = [];
            for k=1:cells(i) if (cdf(k) < 1) then cdf_cutted(k) = cdf(k); else break; end; end
            secs = [];
            for k=1:size(cdf_cutted, 1) secs(k) = len(k); end

            plot2d(log2(secs), log2(cdf_cutted), colorLoc);
            colorLoc = colorLoc + COLOR_OFFSET;
            if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
            legenda = [ legenda ; ('CDF ' + filenames(i)) ];
            xmlDelete(doc);
        end
        if (SHOW_LEGEND == 1) then h1=legend(legenda, 4); end
        prepareGraphic("CDF для " + graphicName + " (логарифмические оси)", "log2( " + xlable + " )", "log2( CDF : P(X < x) )");
    end

    table_err = ["TRACE", "d2", "d3", "d4"]
    if (isCcdf == 1) then
        scf();    
        legenda = [];  colorLoc = GRAPH_COLOR; ethalonLen = []; ethalonCcdf = [];
        for i=1:fileCount
            doc = xmlRead(PATH + filenames(i));
            len = (leftBound(i)+cellWidth(i)/2):cellWidth(i):rightBound(i);
            ccdf = getVector(doc, "//" + tag + "/CCDF-VALS/text()", cells(i));

            if (i == 1) then ethalonLen = len; ethalonCcdf = ccdf; end
            [d2, d3, d4] = __computeDiff__(ethalonLen, ethalonCcdf, len, ccdf);

            ccdf_cutted = [];
            for k=1:cells(i) if (ccdf(k) > 0) then ccdf_cutted(k) = ccdf(k); else break; end; end
            secs = [];
            for k=1:size(ccdf_cutted, 1) secs(k) = len(k); end

            plot2d(log2(secs), log2(ccdf_cutted), colorLoc);
            colorLoc = colorLoc + COLOR_OFFSET;
            if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
            legenda = [ legenda ; ('CCDF ' + filenames(i))];
            table_err = [table_err; filenames(i), string(d2), string(d3), string(d4)]
            xmlDelete(doc);
        end
        if (SHOW_LEGEND == 1) then hl=legend(legenda, 3); end
        prepareGraphic("CCDF для " + graphicName + " (логарифмические оси)", "log2( " + xlable + " )", "log2( CCDF : P(X > x) )");
        disp("Errors table for " + tag)
        disp(table_err)
    end
endfunction


//Функция расчёта отклонения графиков друг от друга
//В общем виде могут быть следующие случаи:
// 1) сетки гист. с одинаковыми разбиением и границами - DONE
// 2) сетки гист. с одинаковым разбиением и разными границами - DONE
// 3) сетки гист с разными разбиениями и границами - NOT SUPPORTED
function [d2, d3, d4] = __computeDiff__(ethalonLen, ethalonCcdf, len, ccdf)
    if (size(ethalonLen, "c") <> size(len, "c") || size(ethalonCcdf, "r") <> size(ccdf, "r") || size(ethalonLen, "c") <> size(ethalonCcdf, "r")) then
        //disp(size(ethalonLen, "c")); disp(size(len, "c")); disp(size(ethalonCcdf, "r")); disp(size(ccdf, "r")); 
        //disp(msprintf("__computeDiff__: разные длины векторов. Обрезаем."));
        if (size(ethalonLen, "c") <> size(ethalonCcdf, "r") || size(len, "c") <> size(ccdf, "r")) then
            error(msprintf("__computeDiff__: разные длины ethalonLen и ethalonCcdf или len и ccdf"))
        end
        minSize = min(size(ethalonLen, "c"), size(len, "c"))
        ethalonLen = ethalonLen(1, 1:minSize); 
        len = len(1, 1:minSize)
        ethalonCcdf = ethalonCcdf(1:minSize, 1)
        ccdf = ccdf(1:minSize, 1)
        //disp(msprintf("__computeDiff__: Обрезали:"));
        //disp(size(ethalonLen, "c")); disp(size(len, "c")); disp(size(ethalonCcdf, "r")); disp(size(ccdf, "r")); 
    end
    
    for (i=1:size(ethalonLen, 2))
        if (ethalonLen(i) <> len(i)) then
            error(msprintf("__computeDiff__: значени компонент в векторах ethalonLen и len: i=" + string(i) + ", ethalonLen(i)=" + string(ethalonLen(i)) + ", len(i)=" + string(len(i))));
        end
    end

    d2 = 0; actualCount = 0
    d3 = norm(ethalonCcdf - ccdf) / norm(ethalonCcdf) // относительная ошибка вектора    
    d4 = 0; b1 = 1; bn = b1; q = 1.125; count = size(ethalonLen, 2)
    for (i=1:count)
        if (ethalonCcdf(i) <> 0) then d2 = d2 + ((ethalonCcdf(i)-ccdf(i))/ethalonCcdf(i))^2; actualCount = actualCount + 1; end
        d4 = d4 + bn * (ethalonCcdf(i)-ccdf(i))^2
        bn = bn * q; //член геометрической прогр
    end
    d2 = sqrt(d2 / actualCount) // СКО из относительных велечин
    coefSum = (b1 * (1 - q^count)) / (1 - q) //сумма геометрической прогр.
    d4 = sqrt(d4 / coefSum) // СКО взвешенное
endfunction


//Вывод в таблицу указанного вектора значений из вектора файлов статистики
function __privatePrintTable__(filenames, tags)
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

    filenames = [];
    bases = [];
    levels = [];
    DXs = [];
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

    filenames = [];
    bases = [];
    levels = [];
    DXs = [];
    for i = 1 : rhs
        dispertionAndSizePerLevel = read(PATH + varargin(i), 10, 4);
        filenames = [filenames ; varargin(i)];
        bases     = [bases     ; 4]; // число подобластей на каждом уровне
        levels    = [levels    ; 9]; // число уровней
        DXs       = [DXs       , dispertionAndSizePerLevel(2:10, 2)];
    end
    
    __drawDX__(filenames, bases, levels, DXs);
endfunction

// Private. Рисование графиков дисперсии
function __drawDX__(filenames, bases, levels, DXs)
    count = size(filenames, 1);
    
    format('v',5); // выставление нужного формата
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
        mnkLine = a(1) * LOG_areaCount + a(2);
        b = atan(a(1));
        H = 1-abs(b)/2;
        plot2d(LOG_areaCount, LOG_DX, -colorLoc);  // рисуем точки дисперсий
        plot2d(LOG_areaCount, mnkLine, colorLoc);  // рисуем прямую МНК
        
        legenda = [ legenda ; ('Нормированная дисперсия ' + filename) ];
        legendaFit = [ legendaFit ; ('Нормированная дисперсия ' + filename); "МНК для " + filename + ": H = " + string(H) ];
        
        colorLoc = colorLoc + COLOR_OFFSET;
        if (colorLoc == 8) then colorLoc = colorLoc + COLOR_OFFSET; end // перешагиваем белый цвет
    end
    
    if (SHOW_LEGEND == 1) then 
        whereLegenda = 4;
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
    prepareGraphic("График изменения дисперсий (логарифмические оси)", "log2( pow(N,l) )", "log2( D(X / EX) )");    
    scf(3);
    prepareGraphic("График изменения дисперсий (логарифмические оси)", "log2( pow(N,l) )", "log2( D(X / EX) )");
    format('v',10); // возвращение стандартного формата
endfunction


// функция для минимизации для построения линии МНК
function [zr]=F(c,z)
     zr=z(2)-c(1)*z(1)-c(2);
endfunction
