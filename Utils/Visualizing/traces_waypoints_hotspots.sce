IS_COMMON_SCE_LOADED = COMMON_SCE_LOADED;//чтобы убедиться, что файл common.sce загружен

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
    
    privateAllWPFiles(wpFiles);
endfunction


//Рисование файлов WayPoint'ов, взятых из папки folder
function drawWPFilesFolder(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    wpFiles = getFiles(PATH, "*.wpt");

    privateAllWPFiles(wpFiles);
    xtitle("WayPoints from " + folder);
    
    PATH = SAVE_PATH;
endfunction


//Рисование WayPoint'ов из входного вектора с именами файлов (ФУНКЦИЯ НЕ ДЛЯ ПРЯМОГО ИСПОЛЬЗОВАНИЯ)
function privateAllWPFiles(fileNamesVector)
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
    end
    
    GRAPH_COLOR = SAVE_COLOR;

    if (SHOW_LEGEND == 1) then
        hl=legend(legenda);
    end
    
    xtitle("WayPoints");
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
