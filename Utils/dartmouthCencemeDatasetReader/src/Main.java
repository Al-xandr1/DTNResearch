import java.io.*;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.lang.System.exit;

/**
 * Created by Alexander on 01.03.17.
 */
public class Main {

    public static final String DB_FOLDER_NAME = "data";
    public static final String TRACES_FOLDER_NAME = "tracefiles";

    public static final Pattern DB_FILES_PATTERN = Pattern.compile("(?<CLEARNAME>CenceMeLiteLog\\d{1,2})\\.txt");
    public static final String CLEARNAME = "CLEARNAME";
    public static final String TRACE_FILE_SUFFIX = "_trace.txt";
    public static final String SEPARATOR = System.getProperty("file.separator");

    static private FileReader inputFile;
    static private FileReader outputFile;

    public static final String DLM = "\t";
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


    public static void main(String[] args) {
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
        if (list == null) {
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
            firstTimeStampPerTrace = -1;
        }
    }

    private static void processFile(String fullInputFileName, String fullOutputFileName) {
        try (BufferedReader bufferedReader = new BufferedReader(new FileReader(fullInputFileName));
             BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(fullOutputFileName))) {

            String line;
            boolean processed = false;
            while ((line = bufferedReader.readLine()) != null) {
                processed |= processLine(bufferedWriter, line);
            }
            assert processed : "File incorrect!";

            bufferedWriter.flush();
        } catch (Exception ex) {
            ex.printStackTrace();
            System.err.println("DEBUG fullInputFileName: '" + fullInputFileName + "'");
        }
    }

    private static boolean processLine(BufferedWriter bufferedWriter, String line) throws Exception {
        Matcher matcher = PATTERN.matcher(line);
        boolean anyProcessed = false;

        if (matcher.find()) {
            final String timestamp = matcher.group("TIMESTAMP");
            final String number = matcher.group("NUMBER");
            final String tag = matcher.group("TAG");
            final String data = matcher.group("DATA");

            switch (tag) {
                case GPS: {
                    //todo определить временные точки для каждой позиции

                    final String trimedData = data.trim();
                    if (trimedData.isEmpty()) {
                        // Точек в исходном файле для текущей строчки нет. Игнориуем и ищё следующие точки
                        break;
                    }

                    String[] samples = data.split("\\*");
                    for (String sample : samples) {
                        String[] components = sample.split(",");
                        try {
                            String altitude = components[ALTITUDE_INDEX];
                            String latitude = components[LATITUDE_INDEX];
                            String longitude = components[LONGITUDE_INDEX];
                            String Hdop = components[HDOP_INDEX];
                            String speed = components[SPEED_INDEX];
                            assert !altitude.isEmpty() && !latitude.isEmpty() && !longitude.isEmpty() && !Hdop.isEmpty() && !speed.isEmpty();

                            write(bufferedWriter, timestamp, latitude, longitude);
                            anyProcessed = true;

                        } catch (Exception ex) {
                            // for debug
                            ex.printStackTrace();
                            System.err.println("DEBUG line: '" + line + "'");
                            System.err.println("DEBUG data: '" + data + "'");
                            System.err.println("DEBUG samples: '" + Arrays.toString(samples) + "'");
                            System.err.println("DEBUG samples: '" + Arrays.toString(samples) + "'");
                            System.err.println("DEBUG sample: '" + sample + "'");
                            System.err.println("DEBUG components: '" + Arrays.toString(components) + "'");
                            throw ex;
                        }
                    }
                    break;
                }
                case ACC: {
                    String[] samples = data.split("\\*");
                    assert samples.length != 0;
                    // ignore
                    anyProcessed = true;
                    break;
                }
                case GPS_SKIPPED: {
                    assert data.contains(GPS_SKIPPED_VALUE);
                    // записать в файл последнюю актуальную позицию с новым временем
                    write(bufferedWriter, timestamp, null, null);
                    anyProcessed = true;
                    break;
                }
                case ACT: {
                    String[] samples = data.split(",");
                    String accSamplingStart = samples[ACT_ACC_SAMPLING_START_INDEX];
                    String accSamplingEnd = samples[ACT_ACC_SAMPLING_END_INDEX];
                    String fact = samples[ACT_FACT_INDEX];
                    assert !accSamplingStart.isEmpty() && !accSamplingEnd.isEmpty() && !fact.isEmpty();
                    // ignore
                    anyProcessed = true;
                    break;
                }
                default: {
                    throw new IllegalStateException("Unknown tag:" + tag);
                }
            }
        } else {
            //todo Сделать весь набор известных исключений! для проверки
            //System.out.println(line);
        }

        return anyProcessed;
    }


    private static long firstTimeStampPerTrace = -1;
    private static String lastWroteTimestamp = null;
    private static String lastWroteLatitude = null;
    private static String lastWroteLongitude = null;

    /**
     * Формуруем итоговую строку в нужном формате
     *
     * @param bufferedWriter выходной файл
     * @param timestamp      время местоположения
     * @param latitude       широта
     * @param longitude      долгота
     * @throws IOException
     */
    private static void write(BufferedWriter bufferedWriter, String timestamp, String latitude, String longitude) throws IOException {
        assert bufferedWriter != null && (timestamp == null || !timestamp.isEmpty())
                && (latitude == null || !latitude.isEmpty()) && (longitude == null || !longitude.isEmpty());

        lastWroteTimestamp = timestamp != null ? timestamp : lastWroteTimestamp;
        lastWroteLatitude = latitude != null ? latitude : lastWroteLatitude;
        lastWroteLongitude = longitude != null ? longitude : lastWroteLongitude;

        // todo сделать перевод из ГЕОКООРДИНАТ в ДЕКАРТОВЫ координаты
        String str = toValidTime(lastWroteTimestamp) + DLM + lastWroteLatitude + DLM + lastWroteLongitude + "\n";
        bufferedWriter.write(str);
    }

    /**
     * Производим перевод из абсолютного времени в миллисекундах с 1970 года
     * в относительное время (первая запись берётся за точку отсчёта) в секундах
     *
     * @param timestamp абсолютное временя в миллисекундах с 1970 года
     * @return относительное время в секундах
     */
    private static String toValidTime(String timestamp) {
        final long millis = Long.parseLong(timestamp);
        assert millis > 0;

        if (firstTimeStampPerTrace == -1) {
            firstTimeStampPerTrace = millis;
        }
        assert firstTimeStampPerTrace > 0 : "firstTimeStampPerTrace is negative! " + firstTimeStampPerTrace;

        final long relativeTimestamp = millis - firstTimeStampPerTrace;
        assert relativeTimestamp >= 0 : "Some value less then firstTimeStampPerTrace: " + firstTimeStampPerTrace + ", " + millis;

        if (lastWroteTimestamp != null) {
            // проверка, что очередная временная отметка больше или равна предыдущей
            final long lastWroteAbsoluteTime = Long.parseLong(lastWroteTimestamp);
            assert lastWroteAbsoluteTime > 0;
            assert (lastWroteAbsoluteTime - firstTimeStampPerTrace) <= relativeTimestamp;
        }

        final double seconds = (1.0 * millis) / 1000.0;
        assert seconds >= 0;

        //todo WTF???!!! что то со временем...

        return Double.toString(seconds);
    }
}
