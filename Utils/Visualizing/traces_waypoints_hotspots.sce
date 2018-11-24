IS_COMMON_SCE_LOADED = COMMON_SCE_LOADED;//чтобы убедиться, что файл common.sce загружен

function drawSpeed(filename)
    fd = mopen(PATH + filename, 'rt');
    mtr = mfscanf(-1, fd, '%lg %lg');      
    
    n = size(mtr, 1); 
    l = mtr(1:n, 1)';
    t = mtr(1:n, 2)'; 
    
    disp(size(l));
    disp(size(t));
    
    lSize = size(l, 2);
    disp(lSize);
    v = [];
    for i=1 : lSize;
        v = [v; (l(1, i) / t(1, i))];
    end
    
    plot2d(l, v, [GRAPH_COLOR]);
    a=gca(); 
    poly1= a.children(1).children(1);
    poly1.line_mode = 'off';
    poly1.mark_style = 4;
    poly1.mark_size_unit = 'point'
    poly1.mark_size = 3
    poly1.mark_foreground = GRAPH_COLOR;
    poly1.mark_background = GRAPH_COLOR;
    a.x_label.text="length, [m]";
    a.x_label.font_size=3;
    a.y_label.text="velocity, [m/s]";
    a.Y_label.font_size=3;

    xtitle("Speed.dat");
    xgrid();
    
    mclose(fd);
endfunction

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
        if (GRAPH_COLOR == 8) then GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET; end // перешагиваем белый цвет
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
    
    __privateAllWPFiles__(wpFiles);
endfunction


//Рисование файлов WayPoint'ов, взятых из папки folder
function drawWPFilesFolder(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    wpFiles = getFiles(PATH, "*.wpt");

    __privateAllWPFiles__(wpFiles);
    xtitle("WayPoints from " + folder);
    
    PATH = SAVE_PATH;
endfunction


//Рисование WayPoint'ов из входного вектора с именами файлов (ФУНКЦИЯ НЕ ДЛЯ ПРЯМОГО ИСПОЛЬЗОВАНИЯ)
function __privateAllWPFiles__(fileNamesVector)
    fileCount = size(fileNamesVector, 1);
    if (fileCount < 1) then
        error(msprintf("privateAllWPFiles: Нет файлов для обработки"));
    end
    save_color = GRAPH_COLOR;
    
    //Рисование графиков и формирование легенды
    legenda = [];
    for i = 1 : fileCount
        drawWPFile(fileNamesVector(i));
        legenda = [ legenda ; ('WayPoints  ' + fileNamesVector(i)) ];
        
        if (CHANGE_COLOR == 1) then
            GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET;
            if (GRAPH_COLOR == 8) then GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET; end // перешагиваем белый цвет
        end
    end

    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    xtitle("WayPoints");
    GRAPH_COLOR = save_color;
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
        if (GRAPH_COLOR == 8) then GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET; end // перешагиваем белый цвет
    end
    
    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    GRAPH_COLOR = save_color;
    xtitle("Traces from "+ traceFolder + "and corresponding WayPoints from " + wpFolder);
endfunction


//------------------ Функции для рисования ПАПОК WAYPOINT'ов -------------------

//Рисование вектора WayPoint'ов
function drawWPTs(x, y, GRAPH_COLOR)
    plot2d(x, y, [GRAPH_COLOR]);
    a=gca(); 
    poly1= a.children(1).children(1);
    poly1.line_mode = 'off';
    poly1.mark_style = 9;
    poly1.mark_size_unit = 'point'
    poly1.mark_size = 1
    poly1.mark_foreground = GRAPH_COLOR;
    poly1.mark_background = GRAPH_COLOR;
    a.x_label.text="x";
    a.x_label.font_size=3;
    a.y_label.text="y";
    a.Y_label.font_size=3;
endfunction


function [x, y] = gatherWPTs(fileNamesVector)
    fileCount = size(fileNamesVector, 1);

    x = [];
    y = [];
    for i = 1 : fileCount
        fd = mopen(PATH + fileNamesVector(i), 'rt');
        l = mfscanf(-1, fd, '%lg %lg %lg %lg');  
        
        n = size(l, 1); 
        x = [x, l(1:n, 1)'];
        y = [y, l(1:n, 2)']; 
        
        mclose(fd);
    end
endfunction


//Рисование WayPoint'ов из входного вектора с именами файлов (ФУНКЦИЯ НЕ ДЛЯ ПРЯМОГО ИСПОЛЬЗОВАНИЯ)
function drawWPTsFromFiles(fileNamesVector)
    [x, y] = gatherWPTs(fileNamesVector);
    drawWPTs(x, y, GRAPH_COLOR);
endfunction


//Рисование файлов WayPoint'ов, взятых из папки folder
function [fileCount] = drawWPTsFolder(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    wpFiles = getFiles(PATH, "*.wpt");
    fileCount = size(wpFiles, 1);
    
    drawWPTsFromFiles(wpFiles);
    xtitle("WayPoints from " + folder + ': ' + string(fileCount) + ' files');
    xgrid();
    
    PATH = SAVE_PATH;
endfunction


function drawAllWPTFolders(folders)
    folderCount = size(folders, 1);
    if (folderCount < 1) then
        error(msprintf("drawAllWPTFolders: Нет папок для обработки"));
    end
    
    SAVE_COLOR = GRAPH_COLOR;

    //Рисование графиков и формирование легенды
    legenda = [];
    for i = 1 : folderCount
        fileCount = drawWPTsFolder(folders(i));
        legenda = [ legenda ; (' _ ' + folders(i)) + ': ' + string(fileCount) + ' files'];
        
        GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET;
        if (GRAPH_COLOR == 8) then GRAPH_COLOR = GRAPH_COLOR + COLOR_OFFSET; end // перешагиваем белый цвет
    end
    
    GRAPH_COLOR = SAVE_COLOR;

    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    xtitle("Путевые точки");
endfunction


function drawWPTFolders(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawWPFolders: Ожидалось один или более параметров (имён папок)"));
    end
    
    wpFolders = [];
    for i = 1 : rhs
        wpFolders = [wpFolders ; varargin(i)];
    end
    
    drawAllWPTFolders(wpFolders);
endfunction


//---------------------- Функции для рисования HotSpot -------------------------

function drawHS(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    hsFiles = getFiles(PATH, "*.hts");
    fileCount = size(hsFiles, 1);
    
    for i = 1 : fileCount
        drawHotSpot(hsFiles(i), 5)
    end
    
    xtitle("HotSpots from " + folder + ': ' + string(fileCount) + ' files');
    xgrid();
    
    PATH = SAVE_PATH;
    cd(PATH);
endfunction


//Рисование HotSpot'а из файла
function drawHotSpot(filename, GRAPH_COLOR)
    HS = read(filename, 2, 2);  
    X_min=HS(1,1);    X_max=HS(1,2);
    Y_min=HS(2,1);    Y_max=HS(2,2);
    
    x = [X_min, X_min, X_max, X_max, X_min];
    y = [Y_min, Y_max, Y_max, Y_min, Y_min];
    
    plot2d(x, y, [GRAPH_COLOR]);
    a=gca(); 
    poly1= a.children(1).children(1);
    poly1.line_mode = 'on';
    poly1.fill_mode = 'off';
    poly1.mark_style = 9;
    poly1.thickness = 1;
    poly1.mark_size_unit = 'point';
    poly1.mark_size = 4;
    poly1.mark_foreground = GRAPH_COLOR;
    poly1.mark_background = GRAPH_COLOR;
endfunction



//Отображение таблицы для сравнения данных о локациях по маршрутам
//На вход передаётся папка с локациями, реальные и сгенерированные маршруты
function printCompareTableHtsRot(htsFolder, realRtsFolder, genRtsFolder)
    htsTable = getTableHts(htsFolder);
    htsTable = htsTable(:,1:4); // оставляем только первые 4 столбца
    htsTableN = size(htsTable, 1);
    
    [realRtsNames, realRtsTableList] = getTableRots(realRtsFolder);
    [genRtsNames, genRtsTableList] = getTableRots(genRtsFolder);
    if(DEBUG_ECHO == 1) then
        printf("Список фалов: "); 
        disp(realRtsNames);
        disp(genRtsNames);
        printf("\n");
    end;
    
    realRtsNamesN = size(realRtsNames, 1);
    genRtsNamesN = size(genRtsNames, 1);
    if (realRtsNamesN <> genRtsNamesN) then  
        error(msprintf("printCompareTableHtsRot: Количество маршрутов в папке " + realRtsFolder + " = " + string(realRtsNamesN) + ", а папке " + genRtsFolder + " = " + string(genRtsNamesN)));
    end
    
    // бежим по всем файлам маршрутов
    for i = 1 : realRtsNamesN
        realRtsTable = realRtsTableList(i);
        genRtsTable = genRtsTableList(i);

        table  = [];
        //header = [realRtsFolder + "(" + string(realRtsNames(i)) + "):", genRtsFolder + "(" + string(genRtsNames(i)) + "):"];
        header = [realRtsFolder + "(" + string(i) + "):", genRtsFolder + "(" + string(i) + "):"];
        table = [table ; header];
        
        // бежим по всем локациям в наборе
        for j = 2 : htsTableN
            htsName = htsTable(j, 1);
            row = [0 , 0]; // (1,1) - для реальных точек, (1,2) - для сгенерированных
            
            for k = 2 : size(realRtsTable, 1) // начинаем с 2, чтобы пропустить заголовок
                htsRealInRootName = realRtsTable(k, 1);
                if (strcmp(htsName, htsRealInRootName) == 0) then
                    row(1, 1) = row(1, 1) + strtod(realRtsTable(k, 7)); // суммируем путевые точки
                end
            end
            
            for k = 2 : size(genRtsTable, 1) // начинаем с 2, чтобы пропустить заголовок
                htsGenInRootName = genRtsTable(k, 1);
                if (strcmp(htsName, htsGenInRootName) == 0) then
                    row(1, 2) = row(1, 2) + strtod(genRtsTable(k, 7)); // суммируем путевые точки
                end
            end
            
            table = [table ; string(row)];
        end
        
        htsTable = [htsTable , table]
    end

    disp(htsTable);
endfunction

//Функция извлечения данных из маршрутов в таблицу со стоблацми:
// ["File:", "Xmin:", "Xmax:", "Ymin:", "Ymax:", "T_Sum:", "WP_Count:"]
function [rotFileNames, rotTableList] = getTableRots(rotFolder)
    SAVE_PATH = PATH;
    
    PATH = PATH + rotFolder + SEPARATOR;
    rotFiles = getFiles(PATH, "*.rot");
    fileCount = size(rotFiles, 1);
    
    rotFileNames = [];
    rotTableList = list();
    for i = 1 : fileCount
        rotTable  = [];
        header = ["File:", "Xmin:", "Xmax:", "Ymin:", "Ymax:", "T_Sum:", "WP_Count:"];
        rotTable = [rotTable ; header];
    
        ROT_STRs = mgetl(rotFiles(i)); //считали вектор строк
        for j = 1 : size(ROT_STRs, 1)
            p = strsplit(stripblanks(ROT_STRs(j), %t), '/	/');
            HS_filename=p(1); X_min=p(2); X_max=p(3); Y_min=p(4); Y_max=p(5); T_Sum=p(6); WP_Count=p(7);
            row = [HS_filename, X_min, X_max, Y_min, Y_max, T_Sum, WP_Count];
            rotTable = [rotTable ; row];
        end
        
        rotFileNames = [rotFileNames ; rotFiles(i)];
        rotTableList($+1) = rotTable;
    end
    
    PATH = SAVE_PATH;
    cd(PATH);   
endfunction


//Отображение таблицы для сравнения данных о локациях
//На вход передаётся список папок с локациями и строится таблица для сравнения
function printCompareTableHts(varargin)
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("printCompareTableHts: Ожидалось один или более параметров (имён папок)"));
    end
    
    table  = [];
    header = ["File:", "Length:", "Width:", "Square:"];
    for i = 1 : rhs
        header = [header, "T_Sum(" + varargin(i) + "):"];   // СНАЧАЛА 5 столбец
    end
    for i = 1 : rhs
        header = [header, "WP_Count(" + varargin(i) + "):"];// ПОТОМ 6 столбец
    end
    table = [table ; header];
    
    firstHtsTable = getTableHts(varargin(1));
    HS_Count = size(firstHtsTable, 1);
    for i = 2 : HS_Count
        row = [];
        //вырезаем первые 4 стобца i-ой строчки
        row = [row, firstHtsTable(i, 1:4)];

        //вырезаем СНАЧАЛА 5 столбец каждой строчки
        for j = 1 : rhs
            htsTable = getTableHts(varargin(j));
            if (HS_Count <> size(htsTable, 1)) then  
                error(msprintf("printCompareTableHts: Количество локаций в первой папке =" + string(HS_Count) + " и в папке " + string(j) + " = " + string(size(htsTable, 1))));
            end
            row = [row, htsTable(i, 5)];
        end
        
        //вырезаем ПОТОМ 6 столбец каждой строчки
        for j = 1 : rhs
            htsTable = getTableHts(varargin(j));
            row = [row, htsTable(i, 6)];
        end
        
        table = [table ; row];
    end
    
    // теперь добавим строчку с суммой всех столбцов
    total = zeros(1, size(table, 2));
    for j = 2 : size(table, 2) // со второго столбца, чтобы пропустить имена локаций
        for i = 2 : size(table, 1) // со второй строки, чтобы пропустить заголовок
            total(1,j) = total(1,j) + strtod(table(i,j));
        end
    end
    
    total = string(total);
    total(1,1) = "TOTAL:";
    
    table = [table ; total];
    
    disp(table);
endfunction

//Функция извлечения данных из списка локаций в таблицу со стоблацми:
// ["File:", "Length:", "Width:", "Square:", "T_Sum:", "WP_Count:"]
function htsTable = getTableHts(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    hsFiles = getFiles(PATH, "*.hts");
    fileCount = size(hsFiles, 1);
    
    htsTable  = [];
    header = ["File:", "Length:", "Width:", "Square:", "T_Sum:", "WP_Count:"];
    htsTable = [htsTable ; header];
    for i = 1 : fileCount
        HS = read(hsFiles(i), 3, 2);  
        X_min=HS(1,1);    X_max=HS(1,2);
        Y_min=HS(2,1);    Y_max=HS(2,2);
        T_Sum=HS(3,1);    WP_Count=HS(3,2);
        
        HS_length = X_max - X_min;
        HS_width = Y_max - Y_min;
        HS_Square = HS_length * HS_width; 
        
        row = [string(hsFiles(i)), string(HS_length), string(HS_width), string(HS_Square), string(T_Sum), string(WP_Count)];
        htsTable = [htsTable ; row];
    end
    
    PATH = SAVE_PATH;
    cd(PATH);   
endfunction
