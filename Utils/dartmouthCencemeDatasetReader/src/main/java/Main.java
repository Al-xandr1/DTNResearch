import org.apache.commons.lang.StringUtils;
import uk.me.jstott.jcoord.LatLng;
import uk.me.jstott.jcoord.UTMRef;

import java.io.*;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.lang.System.exit;

/**
 * Created by Alexander on 01.03.17.
 */
public class Main {

    //region Constants
    static final boolean DIVIDE_BY_DAYS = true;

    static final String DB_FOLDER_NAME = "data";
    static final String TRACES_FOLDER_NAME = "tracefiles";

    static final Pattern DB_FILES_PATTERN = Pattern.compile("(?<CLEARNAME>CenceMeLiteLog_id=\\d{1,3})_\\.txt");
    static final String CLEARNAME = "CLEARNAME";
    static final String TRACE_FILE_SUFFIX = "_trace.txt";
    static final String SEPARATOR = System.getProperty("file.separator");

    static final String DLM = "\t ";
    static final Pattern PATTERN = Pattern.compile("(?<TIMESTAMP>\\d*) DATA \\((?<NUMBER>\\d){0,3}\\) - (?<TAG>GPS|ACT|GPS-Skipped|ACC): (?<DATA>.*)");
    static final String TEST_GPS_LINE = "1216830985473 DATA (0) - GPS: 168.7,4342.38016,7217.17455,4.9,3.6*168.3,4342.38246,7217.16246,5.0,4.3*168.6,4342.3832,7217.16708,5.0,0.0*168.8,4342.38357,7217.17294,5.0,0.0*169.1,4342.38409,7217.17614,3.5,6.4*172.4,4342.38646,7217.17606,4.4,4.8*";
    static final String TEST_ACT_LINE = "1216830974754 DATA (0) - ACT: 1216830959582,1216830973695,5";

    static final List<Pattern> LEGAL_EXCLUDES = new ArrayList<Pattern>() {
        {
            add(Pattern.compile("-*"));
            add(Pattern.compile("-* (NEXT LOG) -*"));
            add(Pattern.compile("\\d* INFO \\(\\d\\) - CenceMeLite: (Configuration|CONFIG):( \\d{1,3} \\*)*.*"));
            add(Pattern.compile("\\d* WARNING \\(\\d\\) - LocalAccSens: Error while reading input stream \\(read -1 bytes\\): null"));
            add(Pattern.compile("\\d* WARNING \\(\\d\\) - LocalAccSens: Couldn't connect to local accelerometer sensor socket:\\/\\/(\\d{1,3}.){3}\\d:\\d{4}: .*"));
            add(Pattern.compile("\\d* ERROR \\(\\d\\) - (CenceMeLite|ConfigurationReader|CenceMe):.*"));
            add(Pattern.compile("System error"));
            add(Pattern.compile("Error del sistema"));
        }
    };


    // blocks of a line
    static final String TIMESTAMPE = "TIMESTAMP";
    static final String NUMBER = "NUMBER";
    static final String TAG = "TAG";
    static final String DATA = "DATA";

    // tag's values
    static final String ACT = "ACT"; // данные о текущем состоянии
    static final String ACC = "ACC"; // данные акселерометра
    static final String GPS_SKIPPED = "GPS-Skipped"; // GPS данные отсутствуют (пользователь бездействует более 15 минут)
    static final String GPS = "GPS"; // gps данные

    // indexes of data in GPS samples
    static final int ALTITUDE_INDEX = 0;
    static final int LATITUDE_INDEX = 1;
    static final int LONGITUDE_INDEX = 2;
    static final int HDOP_INDEX = 3;
    static final int SPEED_INDEX = 4;

    static final String GPS_SKIPPED_VALUE = "user sitting";

    // indexes of activity data
    static final int ACT_ACC_SAMPLING_START_INDEX = 0;
    static final int ACT_ACC_SAMPLING_END_INDEX = 1;
    static final int ACT_FACT_INDEX = 2;
    //endregion


    //region Fields
    static Date startDateOfCurrentDay = null;
    static Date firstTimeStampPerTrace = null;
    static Date lastWroteTimestamp = null;
    static String lastWroteLatitude = null;
    static String lastWroteLongitude = null;
    //endregion


    public static void main(String[] args) throws Exception {
        final File dataDir = new File(DB_FOLDER_NAME);

        if (!dataDir.exists()) {
            System.out.println("Directory " + DB_FOLDER_NAME + " does not exist!");
            exit(-1);
        }

        if (!dataDir.isDirectory()) {
            System.out.println(dataDir.toString() + " is not a directory");
            exit(-2);
        }

        final String[] list = dataDir.list((dir, name) -> DB_FILES_PATTERN.matcher(name).matches());
        if (list == null || list.length == 0) {
            System.out.println("Noting to process");
            exit(-3);
        }

        File traceFolder = new File(TRACES_FOLDER_NAME);
        if (!traceFolder.exists()) {
            final boolean mkdir = traceFolder.mkdir();
            assert mkdir;
        }

        for (String name : list) {
            final Matcher matcher = DB_FILES_PATTERN.matcher(name);
            final boolean found = matcher.find();
            assert found;

            final String clearName = matcher.group(CLEARNAME);
            final String traceFileName = clearName + TRACE_FILE_SUFFIX;

            System.out.println("Processing file " + name + " into " + traceFileName + " ...");
            processFile(DB_FOLDER_NAME + SEPARATOR + name, TRACES_FOLDER_NAME + SEPARATOR + traceFileName);
            System.out.println("File " + name + " into " + traceFileName + " has been processed! \n");

            // сбрасываем начальное абсолютное время для новой трассы
            firstTimeStampPerTrace = null;
            startDateOfCurrentDay = null;
        }

        System.out.println("maxX = " + maxX);
        System.out.println("minX = " + minX);
        System.out.println("maxY = " + maxY);
        System.out.println("minY = " + minY);

        System.out.println("countX = " + countX);
        System.out.println("countY = " + countY);
        System.out.println("sumX = " + avX);
        System.out.println("sumY = " + avY);
        System.out.println("averageX = " + avX / countX);
        System.out.println("averageY = " + avY / countY);
        System.out.println("middleX = " + (maxX + minX) / 2);
        System.out.println("middleY = " + (maxY + minY) / 2);
    }

    private static void processFile(String fullInputFileName, String fullOutputFileName) throws Exception {
        String line = null;
        try (BufferedReader bufferedReader = new BufferedReader(new FileReader(fullInputFileName))) {

            int day = 1;
            String fileName;
            //если "делитель" включён, то нужно имя заменить на другок
            if (DIVIDE_BY_DAYS)
                fileName = fullOutputFileName.replaceAll("\\.txt", "_day=" + toPaddedString(day) + "_.txt");
            else fileName = fullOutputFileName;
            PrintWriter printWriter = new PrintWriter(new FileWriter(fileName));
            boolean newDay;
            while ((line = bufferedReader.readLine()) != null) {
                newDay = processLine(printWriter, line);
                if (DIVIDE_BY_DAYS && newDay) {
                    // закрываем файл текущего дня и создаём новый
                    printWriter.flush();
                    printWriter.close();
                    day++;
                    printWriter = new PrintWriter(new FileWriter(fullOutputFileName.replaceAll("\\.txt",
                            "_day=" + toPaddedString(day) + "_.txt")));
                    //обрабатываем текущую запись ещё раз
                    newDay = processLine(printWriter, line);
                    assert !newDay;

                } else if (!DIVIDE_BY_DAYS && newDay) {
                    //если "делитель" отключён, то нужно текущую строчку обрпботать ещё раз
                    newDay = processLine(printWriter, line);
                    assert !newDay;
                }
            }
            printWriter.flush();
            printWriter.close();

        } catch (Exception ex) {
            ex.printStackTrace();
            System.err.println("DEBUG fullInputFileName: '" + fullInputFileName + "'");
            System.err.println("DEBUG fullOutputFileName: '" + fullOutputFileName + "'");
            System.err.println("DEBUG line: '" + line + "'");
            throw ex;
        }
    }

    private static boolean processLine(PrintWriter printWriter, String line) throws Exception {
        Matcher matcher = PATTERN.matcher(line);

        if (matcher.find()) {
            final String timestamp = matcher.group("TIMESTAMP");
            final String number = matcher.group("NUMBER");
            final String tag = matcher.group("TAG");
            final String data = matcher.group("DATA");

            final Date timestampDate = new Date(parseTimestamp(timestamp));
            if (captureAndCheckStartDateOfCurrentDay(timestampDate)) {
                return true; // обнаружен новый день
            }

            switch (tag) {
                case GPS: {
                    processGPS(printWriter, timestampDate, data);
                    break;
                }
                case ACC: {
                    processACC(data);
                    break;
                }
                case GPS_SKIPPED: {
                    processGPSSkipped(printWriter, timestampDate, data);
                    break;
                }
                case ACT: {
                    processACT(data);
                    break;
                }
                default: {
                    throw new IllegalStateException("Unknown tag:" + tag);
                }
            }
        } else {
            // это нужно для контроля того, что все данные в наборе известного формата
            if (!LEGAL_EXCLUDES.stream().anyMatch(pattern -> pattern.matcher(line).matches())) {
                System.out.println(">>> " + line);
            }
        }

        return false; // обработан старый день
    }

    /**
     * Проверяет наступил ли новый день по timestampDate
     *
     * @return true - если новый день наступил, false - в противном случае
     */
    private static boolean captureAndCheckStartDateOfCurrentDay(Date timestampDate) {
        final Calendar timestampDateCalendar = Calendar.getInstance();
        timestampDateCalendar.setTime(timestampDate);

        final int year = timestampDateCalendar.get(Calendar.YEAR);
        final int month = timestampDateCalendar.get(Calendar.MONTH);
        final int day = timestampDateCalendar.get(Calendar.DATE);

        final Calendar startDateOfTimestampDateCalendar = Calendar.getInstance();
        startDateOfTimestampDateCalendar.setTimeInMillis(0);
        startDateOfTimestampDateCalendar.set(year, month, day, 0, 0, 0);
        final Date startDateOfTimestampDate = startDateOfTimestampDateCalendar.getTime();

        if (startDateOfCurrentDay != null) {
            if (startDateOfTimestampDate.after(startDateOfCurrentDay)) {
                //фиксируем новый день
                startDateOfCurrentDay = startDateOfTimestampDate;
                return true; // говорим, что новый день начат
            }

        } else {
            //записываем первый день
            startDateOfCurrentDay = startDateOfTimestampDate;
        }

        return false; // дата timestampDate не не начинает новый день
    }

    /**
     * @return признак того, произошла ли запись в файл или нет
     */
    private static boolean processGPS(PrintWriter printWriter, Date timestamp, String data) throws IOException {
        final String trimmedData = data.trim();
        if (trimmedData.isEmpty()) {
            // Точек в исходном файле для текущей строчки нет. Игнориуем и ищем следующие точки!
            return false; // ничего не записано в файл
        }

        String[] samples = data.split("\\*");
        String sample = average(samples);

        String[] components = sample.split(",");
        String altitude = components[ALTITUDE_INDEX];
        String latitude = components[LATITUDE_INDEX];
        String longitude = components[LONGITUDE_INDEX];
        String hDop = components[HDOP_INDEX];
        String speed = components[SPEED_INDEX];
        assert !altitude.isEmpty() && !latitude.isEmpty() && !longitude.isEmpty() && !hDop.isEmpty() &&
                !speed.isEmpty();

        write(printWriter, timestamp, latitude, longitude);

        return true; // что-то записано в файл (true) или нет (false)
    }

    private static String average(String[] samples) {
        double avAlt = 0;
        double avLat = 0;
        double avLong = 0;
        double avHdop = 0;
        double avSpeed = 0;
        final double n = samples.length;
        for (String sample : samples) {
            String[] components = sample.split(",");
            try {
                avAlt += Double.parseDouble(components[ALTITUDE_INDEX]);
                avLat += Double.parseDouble(components[LATITUDE_INDEX]);
                avLong += Double.parseDouble(components[LONGITUDE_INDEX]);
                avHdop += Double.parseDouble(components[HDOP_INDEX]);
                avSpeed += Double.parseDouble(components[SPEED_INDEX]);
            } catch (Exception ex) {
                ex.printStackTrace();
                System.err.println("DEBUG samples: '" + Arrays.toString(samples) + "'");
                System.err.println("DEBUG sample: '" + sample + "'");
                System.err.println("DEBUG components: '" + Arrays.toString(components) + "'");
                throw ex;
            }
        }
        return (avAlt / n) + "," + (avLat / n) + "," + (avLong / n) + "," + (avHdop / n) + "," + (avSpeed / n);
    }

    /**
     * @return признак того, произошла ли запись в файл или нет
     */
    private static boolean processACC(String accData) {
        String[] samples = accData.split("\\*");
        assert samples.length != 0;

        return false; // ничего не записано в файл
    }

    /**
     * @return признак того, произошла ли запись в файл или нет
     */
    private static boolean processGPSSkipped(PrintWriter printWriter, Date timestamp, String gpsSkippedData) throws IOException {
        assert gpsSkippedData.contains(GPS_SKIPPED_VALUE);
        // записать в файл последнюю актуальную позицию с новым временем
        if (lastWroteLatitude != null && lastWroteLongitude != null) {
            write(printWriter, timestamp, lastWroteLatitude, lastWroteLongitude);
            return true; // запись в файл произошла
        }

        assert lastWroteLatitude == null && lastWroteLongitude == null;
        return false; // ничего не записано в файл
    }

    /**
     * @return признак того, произошла ли запись в файл или нет
     */
    private static boolean processACT(String actData) {
        String[] samples = actData.split(",");
        String accSamplingStart = samples[ACT_ACC_SAMPLING_START_INDEX];
        String accSamplingEnd = samples[ACT_ACC_SAMPLING_END_INDEX];
        String fact = samples[ACT_FACT_INDEX];
        assert !accSamplingStart.isEmpty() && !accSamplingEnd.isEmpty() && !fact.isEmpty();

        return false; // ничего не записано в файл
    }

    private static final int EARTH_RADIUS = 6371;

    /**
     * Формуруем итоговую строку в нужном формате
     *
     * @param printWriter выходной файл
     * @param timestamp   время местоположения
     * @param latitude    широта
     * @param longitude   долгота
     * @throws IOException
     */
    private static void write(PrintWriter printWriter, Date timestamp, String latitude, String longitude) throws IOException {
        assert printWriter != null;
        assert timestamp != null;
        assert latitude != null;
        assert !latitude.isEmpty();
        assert longitude != null;
        assert !longitude.isEmpty();

        lastWroteTimestamp = timestamp;
        lastWroteLatitude = latitude;
        lastWroteLongitude = longitude;

        // division by 100.0 for casting to decimal format of coordinate
        final double lat = Double.parseDouble(lastWroteLatitude) / 100.0;
        final double lon = Double.parseDouble(lastWroteLongitude) / 100.0;
        assert lat >= 0;
        assert lon >= 0;

        final UTMRef coords = new LatLng(lat, lon).toUTMRef();

        final double x = coords.getEasting();
        final double y = coords.getNorthing();

        maxX = Math.max(maxX, x);
        minX = Math.min(minX, x);
        maxY = Math.max(maxY, y);
        minY = Math.min(minY, y);
        avX += x;
        countX++;
        avY += y;
        countY++;

        String relativeTime;
        if (DIVIDE_BY_DAYS) {
            // если "делитель" включён, то используем startDateOfCurrentDay как точку отсчёта
            relativeTime = toRelativeTime(startDateOfCurrentDay, lastWroteTimestamp);
        } else {
            if (firstTimeStampPerTrace == null) firstTimeStampPerTrace = timestamp;
            // если "делитель" отключён, то используем firstTimeStampPerTrace как точку отсчёта
            relativeTime = toRelativeTime(firstTimeStampPerTrace, lastWroteTimestamp);
        }
        final double fixedX = x - MIDDLE_X;
        final double fixedY = y - MIDDLE_Y;
        String str = "  " + relativeTime + DLM + fixedX + DLM + fixedY + DLM;
        printWriter.println(str);
    }

    /**
     * MIDDLE_X & MIDDLE_Y взяты из данных в качестве точки отсчёта
     */
    public static final double MIDDLE_X = 483786;
    public static final double MIDDLE_Y = 4463456;

    private static double maxX = Double.MIN_VALUE;
    private static double minX = Double.MAX_VALUE;
    private static double maxY = Double.MIN_VALUE;
    private static double minY = Double.MAX_VALUE;
    private static double avX = 0;
    private static long countX = 0;
    private static double avY = 0;
    private static long countY = 0;

    /**
     * Производим перевод из абсолютного времени в миллисекундах с 1970 года
     * в относительное время в секундах
     *
     * @param startingPoint точка отсчёта
     * @param timestamp     абсолютное временя в миллисекундах с 1970 года
     * @return время в секундах относительно начала текущего дня
     */
    private static String toRelativeTime(Date startingPoint, Date timestamp) {
        assert startingPoint != null && startingPoint.getTime() > 0;

        final long relativeTimestamp = timestamp.getTime() - startingPoint.getTime();
        assert relativeTimestamp >= 0
                : "Some value less then firstTimeStampPerTrace: " + startingPoint + ", " + timestamp.getTime();

        if (lastWroteTimestamp != null) {
            // проверка, что очередная временная отметка больше или равна предыдущей
            assert (lastWroteTimestamp.getTime() - startingPoint.getTime()) <= relativeTimestamp;
        }

        final double seconds = (1.0 * relativeTimestamp) / 1000.0;
        assert seconds >= 0;

        return Double.toString(seconds);
    }

    private static long parseTimestamp(String timestamp) {
        final long millis = Long.parseLong(timestamp);
        assert millis > 0;
        return millis;
    }

    private static String toPaddedString(int day) {
        return StringUtils.leftPad(String.valueOf(day), 3, "0");
    }
}
