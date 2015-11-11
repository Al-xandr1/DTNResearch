//---------------------------- Параметры ---------------------------------------
// Директория, в которой лежат нужные файлы или папки
//PATH = '/Volumes/Macintosh/Users/Alexander/soft/omnetpp-4.5/GitHub/WaypointGenerator/';
PATH = '/Users/Alexander/soft/omnetpp-4.5/GitHub/DTNResearch/WaypointFinder/waypointfiles/';
SEPARATOR = '/';

GRAPH_COLOR = 2;    // Цвет первого графика
COLOR_OFFSET = 1;   // дробление цветового диапазона (для большого числа трасс ставить меньше значение)

SHOW_LEGEND = 1;    // 1 - показывать легенду, 0 - НЕ показывать легенду
//---------------------------- Параметры ---------------------------------------


//------------------ Функции для рисования ПАПОК WAYPOINT'ов -------------------

//Рисование вектора WayPoint'ов
function drawWPTs(x, y, GRAPH_COLOR)
    plot2d(x, y, [GRAPH_COLOR]);
    a=gca(); 
    poly1= a.children(1).children(1);
    poly1.line_mode = 'off';
    poly1.mark_style = 9;
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
        disp(x);
        disp(y);
    end
endfunction



//Рисование WayPoint'ов из входного вектора с именами файлов (ФУНКЦИЯ НЕ ДЛЯ ПРЯМОГО ИСПОЛЬЗОВАНИЯ)
function drawWPTsFromFiles(fileNamesVector)
    [x, y] = gatherWPTs(fileNamesVector);
    drawWPTs(x, y, GRAPH_COLOR);
endfunction



//Рисование файлов WayPoint'ов, взятых из папки folder
function drawWPTsFolder(folder)
    SAVE_PATH = PATH;
    
    PATH = PATH + folder + SEPARATOR;
    wpFiles = getFiles(PATH, "*.wpt");

    drawWPTsFromFiles(wpFiles);
    xtitle("WayPoints from " + folder);
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
        drawWPTsFolder(folders(i));
        legenda = [ legenda ; (' _ ' + folders(i)) ];
        
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
