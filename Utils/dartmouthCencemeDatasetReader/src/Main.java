import java.io.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Alexander on 01.03.17.
 */
public class Main {

    // The name of the file to open.
    public static final String dbFileName = "data/CenceMeLiteLog19.txt";
    public static final String traceFileName = "data/CenceMeLiteLog19_trace.txt";
    static private FileReader inputFile;
    static private FileReader outputFile;

    public static final String TEST_GPS_LINE = "1216830985473 DATA (0) - GPS: 168.7,4342.38016,7217.17455,4.9,3.6*168.3,4342.38246,7217.16246,5.0,4.3*168.6,4342.3832,7217.16708,5.0,0.0*168.8,4342.38357,7217.17294,5.0,0.0*169.1,4342.38409,7217.17614,3.5,6.4*172.4,4342.38646,7217.17606,4.4,4.8*";
    public static final String TEST_ACT_LINE = "1216830974754 DATA (0) - ACT: 1216830959582,1216830973695,5";

    public static void main(String[] args) {
//        processFile();
        Pattern pattern = Pattern.compile("(?<TIMESTAMP>\\d*) DATA \\((?<NUMBER>\\d){0,3}\\) - (?<TAG>GPS|ACT|GPS-Skipped): (?<DATA>.*)");
        Matcher matcher = pattern.matcher(TEST_GPS_LINE);

        System.out.println(matcher.find());
        System.out.println(matcher.group());
        System.out.println(matcher.groupCount());
        System.out.println();
        for (int i = 0; i < matcher.groupCount(); i++) {
            System.out.println(matcher.group(i));
        }
        System.out.println("\nby name\n");
        System.out.println(matcher.group("TIMESTAMP"));
        System.out.println(matcher.group("NUMBER"));
        System.out.println(matcher.group("TAG"));
        System.out.println(matcher.group("DATA"));
    }

    private static void processFile() {
        // This will reference one line at a time

        try {
            // FileReader reads text files in the default encoding.
            FileReader fileReader = new FileReader(dbFileName);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            FileWriter fileWriter = new FileWriter(traceFileName);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            String line;
            while ((line = bufferedReader.readLine()) != null) {
                processLine(bufferedWriter, line);
            }

            // Always close files.
            bufferedReader.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    private static void processLine(BufferedWriter bufferedWriter, String line) {
        //System.out.println(line);
        Pattern pattern = Pattern.compile("");


    }
}
