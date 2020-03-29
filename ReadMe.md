# Взаимодействие с Git проектом 

Перед настройками нужно зарегистрироваться на GitHub

## Настройка GIT
Настройка Git’а:
Глобальные настройки гита хранятся в файле %user_home%\.gitconfig. Нужно настроить параметры 
git config --global user.name "%username%"
git config --global user.email "%email%"
Эти настройки будут отображаться в коммитах, для отображения автора

Для начала работы нужно клонировать репозиторий с сервера
git clone git@github.com:Al-xandr1/DTNResearch.git

В работе гит использует парольную аутентификацию и поэтому нужно чтобы гит "знал" пользователя.
Для этого нужно воспользоватся инструкцими https://help.github.com/articles/generating-an-ssh-key/ 
После чего сгенерированный ключ нужно будет добавить к проекту и github будет знать пользователя 
т.е. перед каждым коммитом нужно будет вводить ранее введённый пароль

git add %filename%   - добавляет файл в рабочую область гита
git commit -m "message"  - создание коммита в локальном репохитории
git push  - загрузка изменений в удалённый репозиторий
git pull  - обновление проекта из удалённого репозитория
git checkout - в зависимости от параметров используется для извлечения веток или оригинальных версий файлов

Полезные ссылки:
1)	https://git-scm.com/book/en/v2 
2)	http://www.tutorialspoint.com/git/git_online_repositories.htm



# Исходные данные

Исходные данные трасс можно взять тут: https://yadi.sk/d/qz5yuMo_vPsJM 
Это архивы с исходными трассами и результатами многоих экспериментов. Для распаковки может потребоваться несколько гигабайт на жёстком диске. Например, для KAIST:
* трассы лежат тут ./tracefiles (только файлы с расширением .txt)
* путевые точки, полученные WayPointFinder'ом - тут ./waypointfiles (KAIST Real Waypoints) (только файлы с расширением .wpt)
* файл bounds.bnd и прочие выходные файлы лежат в корне дистрибутива ./
* локации лежат, полученные через Hotspot - тут hotspotfiles
* и так далее. 



# Импортирование проекта

После выкачивания проекта нужно импортировать все проекты в рабочую область (если их нет). Сейчас это RoutingTest2, SelfSimLATP, LevyHotSpotsLATP, RealMobility, SimpleLevy и только (там был ещё LevyHotSpotsRandom, который не стоит импортировать, открывать и делать от него зависимости - в нём ошибки)
 
Зависимости такие:
* все проекты зависят от inet
* RoutingTest2 зависит от SelfSimLATP, LevyHotSpotsLATP, RealMobility, SimpleLevy
* SelfSimLATP зависит от SimpleLevy
* LevyHotSpotsLATP зависит от SimpleLevy
* RealMobility зависит от SimpleLevy
 
Также нужно RoutingTest2 собрать как exe файл, а SelfSimLATP, LevyHotSpotsLATP, RealMobility, SimpleLevy как dll библиотеки (это тоже делается в настройках проекта.)
 
Сначала нужно импортировать все проекты в Workspace Omnet++. Для этого нужно:
1. импортировать все проекты отдельно (File -> Import -> Genral -> Existing projects into Workspace -> Select root directory)
2. зайти в настройки проекта модулей в радел ProjectPreferences и проставить зависимости на:
	* для SimpleLevy на inet
	* для SelfSimLATP на inet и SimpleLevy
	* для LevyHotSpotsLATP на inet и SimpleLevy
	* для LevyHotSpotsRandom на inet и SimpleLevy
	* для RoutingTest2 на на inet и SimpleLevy, SelfSimLATP, LevyHotSpotsLATP, LevyHotSpotsRandom
3. и помнить, что все зависимости должны быть собраны НЕ в exe, а в dll. Т.е. если запускаем RoutingTest2, то нужно проекты SimpleLevy, SelfSimLATP, LevyHotSpotsLATP, LevyHotSpotsRando собрать как dll. Если запускаем SelfSimLATP, то нужно проект SimpleLevy собрать как dll. Делается это в Настройки проекта -> Omnet++ -> Makefile -> вкладка Target -> поставить галочку Shared Library. Тогда при компиляции родительские проекты будет видеть компилятор.
 
 

# Запуск OMNET++ проекта

Чтобы начать проводить эксперименты нужно взять ветку experiments. Сейчас там есть две ветки master и experiments. Ветка с профетом уже смёржена в ветку master, а оттуда в experiments.
 
Таким образом, в master у нас актуальный код со всеми включёнными проверками: if(...) exit(-...) или ASSERT. Это нужно для того, чтобы в мастере код как можно "быстрее и чувствительнее" реагировал на изменения и выкидывал ошибки, чтобы их можно было быстрее обнаруживать и исправлять. Однако это несёт дополнительную нагрузку из-за большого числа проверок. Поэтому для проведения экспериментов я сделал отдельную ветку expreriments, где всё тоже самое, только закомментированы все отладочные проверки (или большинство проверок - другие мог оставить, так как они не влияли на производительность).
Поэтому, на всякий случай, если Вы захотите что-нибудь доработать - то это, конечно, можно делать в periments (как говориться, не отходя от кассы), но коммитить нужно в мастер, а оттуда потом делать мёрж в experiments. Да и проверять код лучше в мастере - там много проверок.
 
Когда будете делать git pull в ветке experiments, могут быть конфликты обновления (если у вас такая ветка уже есть, она старая и там что-то менялось) - тогда нужно каждый файл отдельно поправить - но это лучше отдельно расскажу, если случится.
 
После того, как build проекта RoutingTest2 прошёл нормально, можно проводить запуск. Для этого нужно:
1. в папке проекта RoutingTest2 нужно создать папку Traces и поместить туда всё о трассе KAIST (или любой другой), а именно: hotspotfiles, rootfiles, tracefiles, waypointfiles, allroots.roo, bounds.bnd, herst.txt, length.hst, locations.loc, spotcount.cnt. Все эти данные есть тут: https://yadi.sk/d/qz5yuMo_vPsJM 
2. также нужно заранее создать в RoutingTest2 папку outTrace - туда будут генерироваться все выходные файлы.
3. Теперь можно запускать модель RoutingTest2 с конфигурацией RegularRootLATP для запуска маршрутизации. Параметры ciP и aliP меняются в omnetpp.ini файле. То, как нужно было проверить записал тут: https://github.com/Al-xandr1/DTNResearch/issues/57 
 
После окончания моделирования в папке out появляется файл routeHistory.xml - это файл с длительностью маршрутов узлов по дням (матрица, фактически). Чтобы посчитать среднее квадратическое отклонение этой матрицы от матрицы эталонных длительностей нужно:
1. раскомментировать строчку calculator: StatisticsCalculator; в файле StatisticsCollectorNetwork.ned - это калькулятор СКО, его код в StatisticsCalculator.h и StatisticsCalculator.cpp - там в комментариях описана как выглядит матрица.
2. в папку out положить файл routeHistory_KAIST_origin.xml, который нужно переименовать в routeHistory_another.xml - тут будет лежать длительность первых дней всех пользователей с территории KAIST - и тут это фактически будет вектор. (Возможность расчёта для матрицы заложил на всякий случай)
3. в итоге получаем в папке out два файла routeHistory.xml (от только что прошедшего моделирования) и routeHistory_another.xml
4. теперь запускаем модель с конфигурацией StatisticsCollector в консоли видим значение СКО. В общем случае там считается для матрици, где в столбцах лежат длительности всех узлов, соответственно получается что значения СКО считаются для всех столбцов. В случае сравнения в одним первым днём -> это один столбец -> одно значение.
Там есть проверка на то, что размеры матриц одной и другой должны быть равно. Если это не так - будет ошибка.
 
Так же не забудьте, что если меняете число пользователей, то оно должно быть одинаково в файлах routeHistory.xml и routeHistory_another.xml, т.е. в routeHistory_another.xml стоит предварительно от общего числа (92 пользователя) отрезать столько сколько нужно
 

# Запуск на выполнение моделей

Перед запуском моделей необходимо разместить входные данные в папке Traces:
* hotspotfiles - папка с файлами локаций: hotSpot1.hts и т.д.;
* rootfiles - папка с фалами маршрутов: KAIST_30sec_001.txt.wpt.rot и т.д.;
* tracefiles - папка с файлами трасс: KAIST_30sec_001.txt и т.д.;
* waypointfiles - папка с фалами путевых точек: KAIST_30sec_001.txt.wpt и т.д.;
* bounds.bnd - файл с границами, вычисленными на основе путевых точек;
* spotcount.cnt - файл с количеством посещений локаций;
* length.hst - ;
* locations.loc - общий список всех локаций; 
* roots_persistence_statistics.pst - данные о персиcтентности маршрутов; 
* allroots.roo - файл со всеми маршрутами;
* herst.txt - параметр Херста;
* variances.txt - набор дисперсий.

Выходные данные после моделирования формируются в папке outTrace.



# Запуск SLAW модели НА ОСНОВЕ ДАННЫХ Levy модели

1. Запустить Levy модель для получения папок с данными waypointfiles, hotspotfiles, tracefiles (для прогонки через WaypointFinder если нужно для получения файл statistics.stat), locations.loc;
2. Запустиь RootFinder на основе выше упомянутых папок waypointfiles и hotspotfiles. Получить папку rootfiles, length.hst, allroots.roo;
3. Чтобы получить файл bounds.bnd нужно запустить модуль WaypointFinder на основе полученных ранее waypointfiles. 
	Для этого предварительно в модуле WaypointFinder нужно отключить подпрограмму нахождения путевых точек, закомментировав строчку: argc = 2; argv = new char*[2] {"program", WPFIND}; mainMain(argc, argv);
	Также после вызова этого модуля получается файл statistics.stat - статистика мобильности на основе ПУТЕВЫХ точек без предварительного вызова аггрегирующей функции WPFIND. 
	Если нужно предварительно прогнать аггрегирующую процедуру - то нужно раскомментировать строчку: argc = 2; argv = new char*[2] {"program", WPFIND}; mainMain(argc, argv);
4. Далее нужно получить файл spotcount.cnt с помощью модуля RepeatCounter. Входные данные для него hotspotfiles и rootfiles;
5. Файлы herst.txt и variances.txt нужно взять на основе данных от реальной трассы
6. Полученный набор файлов положить в ..\DTNResearch\RoutingResearch\RoutingTest2\Traces\



# Работа с утилитами Utils проекта 

## WaypointFinder 
Запуск этой программы возможен в нескольких вариантах: 
* для определения путевых точек с последующим подсчётом статистики: 
	- для этого предварительно в модуле WaypointFinder нужно включить (раскомментировать) подпрограмму нахождения путевых точек:
		argc = 2; argv = new char*[2] {"program", WPFIND}; 
		mainMain(argc, argv);
	- положить известные файлы исходных трасс в папку tracefiles в директорию программы и запусить программу;
	- в результате в папке waypointfiles появятся путевые точки и файл статистики statistics.stat на основе путевых точек. 
* для подсчёта статистики на имеющихся путевых точках (или возможно сразу на точках трассы - если нужно, опишу позже): 
	- для этого предварительно в модуле WaypointFinder нужно выключить (закомментировать) подпрограмму нахождения путевых точек (см. строчки выше);
	- положить известные файлы путевых точек в папку waypointfiles в директорию программы и запусить программу. 
	- в результате в папке waypointfiles появится файл statistics.stat - статистика мобильности на основе путевых точек без предварительного вызова аггрегирующей функции WPFIND. 
* файл bound.bnd получается для трасс и путевых точек свои (в соответствующих папках)


## Hotspot
Для запуска процедуры определения локаций нужно:
* положить файлы с путевыми точками в папке waypointfiles в директорию программы; 
* положить файл bound.bnd (от путевых точек) в директорию программы; 
* запустить Hotspot
* в резульате получим в директории программы выходные файлы с локациями в папке hotspotfiles и файл locations.loc


## Процесс формирования полного набора данных после эксперимента
- Сбор данных о запуске ЛЮБОЙ модели:
	- настраиваем модель...
	- запускаем модель на выполнение 
	- сохраняем все выходные данные модели
	- копируем файлы: 
		- bounds.bnd - из входных данных
		- omnetpp.ini - из корня проекта
		- out.txt - сохраняем лог от запуска модели
		- roots_persistence_statistics.pst - из входных данных
		
	- Расчёт статистики StatisticsCollector:
		- запускаем расчёт
		- переименовать файл статистику в соответствие с моделью и ситуацией (задаётся наименованием общей папки с данными). Например: statistics_Hybird_n92_12d_aliP_05_uI01_PR
		
- Запуск Утилит: 
	- WaypointFinder: 
		- отключаем в программе нахождение WayPoint'ов (подготовка)
		- удаляем если есть папку waypointfiles\ из WaypointFinder\cmake-build-debug\ 
		- копируем папку waypointfiles\ из папки_эксперимента в WaypointFinder\cmake-build-debug\ 
		- запускаем утилиту
		- перемещаем папку waypointfiles\ из WaypointFinder\cmake-build-debug\ в папку_эксперимента 
		- переименовываем файл statistics.stat добавляя суффикс, описывающий модель и ситуацию (задаётся наименованием общей папки с данными) Например: "statistics_Hybird_n92_12d_aliP_05_uI01_PR.stat"
		- переименовываем файл statistics.stat в соответствие с моделью (пусть задаётся параметром в конфиге). Например: "гибридной модели  (Prophet)"
		
	- RootFinder: 
		- удаляем если есть папки hotspotfiles\ и waypointfiles\ из RootFinder\cmake-build-debug\
		- копируем папки hotspotfiles\ и waypointfiles\ из папки_эксперимента в RootFinder\cmake-build-debug\
		- запускаем утилиту
		- перемещаем папку и файлы rootfiles\, length.hst и allroots.roo из RootFinder\cmake-build-debug\ в папку_эксперимента
		- удаляем папки hotspotfiles\ и waypointfiles\ из RootFinder\cmake-build-debug\

	- RepeatCounter:
		- удаляем если есть папки hotspotfiles\ и rootfiles\ из RepeatCounter\cmake-build-debug\
		- копируем папки hotspotfiles\ и rootfiles\ из папки_эксперимента в RepeatCounter\cmake-build-debug\
		- запускаем утилиту
		- перемещаем файл spotcount.cnt из RepeatCounter\cmake-build-debug\ в папку_эксперимента
		- удаляем папки hotspotfiles\ и rootfiles\ из RepeatCounter\cmake-build-debug\
		
	- SelfSimCalculator:
		- удаляем если есть папку waypointfiles\ и файл bounds.bnd из SelfSimCalculator\cmake-build-debug\
		- копируем папку waypointfiles\ и файл bounds.bnd из папки_эксперимента в SelfSimCalculator\cmake-build-debug\
		- запускаем утилиту
		- перемещаем файлы herst.txt и variances.txt из SelfSimCalculator\cmake-build-debug\ в папку_эксперимента
		- копируем с переименованием файл variances.txt в соответствие с моделью (пусть задаётся параметром в конфиге). Например: "hybrid model (Variances)"
		