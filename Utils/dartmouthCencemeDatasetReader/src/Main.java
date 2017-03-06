import java.io.*;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.lang.System.exit;

/**
 * Created by Alexander on 01.03.17.
 */
public class Main {

    //todo бить каждого пользователя по дням!
    //todo временная шкала для каждого пользователя для каждого дня начинается с 00:00:00 этого дня


    //region Constants
    public static final boolean OUTPUT = false;
    public static final boolean DIVIDE_BY_PEACE = true;

    public static final String DB_FOLDER_NAME = "data";
    public static final String TRACES_FOLDER_NAME = "tracefiles";

    public static final Pattern DB_FILES_PATTERN = Pattern.compile("(?<CLEARNAME>CenceMeLiteLog\\d{1,2})\\.txt");
    public static final String CLEARNAME = "CLEARNAME";
    public static final String TRACE_FILE_SUFFIX = "_trace.txt";
    public static final String SEPARATOR = System.getProperty("file.separator");

    public static final String DLM = "\t";
    public static final String NEW_LINE = "\n";
    public static final Pattern PATTERN = Pattern.compile("(?<TIMESTAMP>\\d*) DATA \\((?<NUMBER>\\d){0,3}\\) - (?<TAG>GPS|ACT|GPS-Skipped|ACC): (?<DATA>.*)");
    public static final String TEST_GPS_LINE = "1216830985473 DATA (0) - GPS: 168.7,4342.38016,7217.17455,4.9,3.6*168.3,4342.38246,7217.16246,5.0,4.3*168.6,4342.3832,7217.16708,5.0,0.0*168.8,4342.38357,7217.17294,5.0,0.0*169.1,4342.38409,7217.17614,3.5,6.4*172.4,4342.38646,7217.17606,4.4,4.8*";
    public static final String TEST_ACT_LINE = "1216830974754 DATA (0) - ACT: 1216830959582,1216830973695,5";

    // blocks of a line
    public static final String TIMESTAMPE = "TIMESTAMP";
    public static final String NUMBER = "NUMBER";
    public static final String TAG = "TAG";
    public static final String DATA = "DATA";

    // tag's values
    public static final String ACT = "ACT"; // данные о текущем состоянии
    public static final String ACC = "ACC"; // данные акселерометра
    public static final String GPS_SKIPPED = "GPS-Skipped"; // GPS данные отсутствуют (пользователь бездействует более 15 минут)
    public static final String GPS = "GPS"; // gps данные

    // indexes of data in GPS samples
    public static final int ALTITUDE_INDEX = 0;
    public static final int LATITUDE_INDEX = 1;
    public static final int LONGITUDE_INDEX = 2;
    public static final int HDOP_INDEX = 3;
    public static final int SPEED_INDEX = 4;

    public static final String GPS_SKIPPED_VALUE = "user sitting";

    // indexes of activity data
    public static final int ACT_ACC_SAMPLING_START_INDEX = 0;
    public static final int ACT_ACC_SAMPLING_END_INDEX = 1;
    public static final int ACT_FACT_INDEX = 2;
    //endregion


    //region Fields
    private static Date startDateOfCurrentDay = null;
    private static Date firstTimeStampPerTrace = null;
    private static Date lastWroteTimestamp = null;
    private static String lastWroteLatitude = null;
    private static String lastWroteLongitude = null;
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
        }
    }

    private static void processFile(String fullInputFileName, String fullOutputFileName) throws Exception {
        String line = null;
        try (BufferedReader bufferedReader = new BufferedReader(new FileReader(fullInputFileName))) {

            int day = 1;
            BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(fullOutputFileName.replaceAll("\\.txt", "_day_" + day + ".txt")));
            boolean newDay;
            while ((line = bufferedReader.readLine()) != null) {
                newDay = processLine(bufferedWriter, line);
                if (newDay) {
                    // закрываем файл текущего дня и создаём новый
                    bufferedWriter.flush();
                    bufferedWriter.close();
                    day++;
                    bufferedWriter = new BufferedWriter(new FileWriter(fullOutputFileName.replaceAll("\\.txt", "_day_" + day + ".txt")));
                    //обрабатываем текущую запись ещё раз
                    newDay = processLine(bufferedWriter, line);
                    assert !newDay;
                }
            }
            bufferedWriter.flush();
            bufferedWriter.close();

        } catch (Exception ex) {
            ex.printStackTrace();
            System.err.println("DEBUG fullInputFileName: '" + fullInputFileName + "'");
            System.err.println("DEBUG fullOutputFileName: '" + fullOutputFileName + "'");
            System.err.println("DEBUG line: '" + line + "'");
            throw ex;
        }
    }

    private static boolean processLine(BufferedWriter bufferedWriter, String line) throws Exception {
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
                    if (OUTPUT) System.out.println(line);
                    processGPS(bufferedWriter, timestampDate, data);
                    break;
                }
                case ACC: {
                    processACC(data);
                    break;
                }
                case GPS_SKIPPED: {
                    if (OUTPUT) System.out.println(line);
                    processGPSSkipped(bufferedWriter, timestampDate, data);
                    break;
                }
                case ACT: {
                    if (OUTPUT) System.out.println(line);
                    processACT(data);
                    break;
                }
                default: {
                    throw new IllegalStateException("Unknown tag:" + tag);
                }
            }
        } else {
            //todo Сделать весь набор известных исключений! для проверки
            if (OUTPUT) System.out.println(line);
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
    private static boolean processGPS(BufferedWriter bufferedWriter, Date timestamp, String data) throws IOException {
        final String trimmedData = data.trim();
        if (trimmedData.isEmpty()) {
            // Точек в исходном файле для текущей строчки нет. Игнориуем и ищем следующие точки!
            return false; // ничего не записано в файл
        }

        //todo определить временные точки для каждой позиции? ИЛИ это несколько измерений одной точки?
        boolean anyWrote = false;
        String[] samples = data.split("\\*");
        for (String sample : samples) {
            String[] components = sample.split(",");
            try {
                String altitude = components[ALTITUDE_INDEX];
                String latitude = components[LATITUDE_INDEX];
                String longitude = components[LONGITUDE_INDEX];
                String hDop = components[HDOP_INDEX];
                String speed = components[SPEED_INDEX];
                assert !altitude.isEmpty() && !latitude.isEmpty() && !longitude.isEmpty() && !hDop.isEmpty() && !speed.isEmpty();

                write(bufferedWriter, timestamp, latitude, longitude);
                anyWrote = true;
            } catch (Exception ex) {
                // for debug
                ex.printStackTrace();
                System.err.println("DEBUG data: '" + data + "'");
                System.err.println("DEBUG samples: '" + Arrays.toString(samples) + "'");
                System.err.println("DEBUG samples: '" + Arrays.toString(samples) + "'");
                System.err.println("DEBUG sample: '" + sample + "'");
                System.err.println("DEBUG components: '" + Arrays.toString(components) + "'");
                throw ex;
            }
        }
        return anyWrote; // что-то записано в файл (true) или нет (false)
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
    private static boolean processGPSSkipped(BufferedWriter bufferedWriter, Date timestamp, String gpsSkippedData) throws IOException {
        assert gpsSkippedData.contains(GPS_SKIPPED_VALUE);
        // записать в файл последнюю актуальную позицию с новым временем
        if (lastWroteLatitude != null && lastWroteLongitude != null) {
            write(bufferedWriter, timestamp, lastWroteLatitude, lastWroteLongitude);
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

    /**
     * Формуруем итоговую строку в нужном формате
     *
     * @param bufferedWriter выходной файл
     * @param timestamp      время местоположения
     * @param latitude       широта
     * @param longitude      долгота
     * @throws IOException
     */
    private static void write(BufferedWriter bufferedWriter, Date timestamp, String latitude, String longitude) throws IOException {
        assert bufferedWriter != null;
        assert timestamp != null;
        assert latitude != null;
        assert !latitude.isEmpty();
        assert longitude != null;
        assert !longitude.isEmpty();

        lastWroteTimestamp = timestamp;
        lastWroteLatitude = latitude;
        lastWroteLongitude = longitude;

        // todo сделать перевод из ГЕОКООРДИНАТ в ДЕКАРТОВЫ координаты. Точка отсчёта должна быть одна!!!
        String str = toRelativeTime(lastWroteTimestamp) + DLM + lastWroteLatitude + DLM + lastWroteLongitude + NEW_LINE;
        bufferedWriter.write(str);
    }

    /**
     * Производим перевод из абсолютного времени в миллисекундах с 1970 года
     * в относительное время в секундах, относительно начала ТЕКУЩЕГО дня.
     *
     * @param timestamp абсолютное временя в миллисекундах с 1970 года
     * @return время в секундах относительно начала текущего дня
     */
    private static String toRelativeTime(Date timestamp) {
        if (firstTimeStampPerTrace == null) {
            firstTimeStampPerTrace = timestamp;
        }
        assert firstTimeStampPerTrace != null && firstTimeStampPerTrace.getTime() > 0;

        //todo сделать относительно текущго дня!!!
        final long relativeTimestamp = timestamp.getTime() - firstTimeStampPerTrace.getTime();
        assert relativeTimestamp >= 0 : "Some value less then firstTimeStampPerTrace: " + firstTimeStampPerTrace + ", " + timestamp.getTime();

        if (lastWroteTimestamp != null) {
            // проверка, что очередная временная отметка больше или равна предыдущей
            assert (lastWroteTimestamp.getTime() - firstTimeStampPerTrace.getTime()) <= relativeTimestamp;
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
}
