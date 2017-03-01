import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

/**
 * Created by Alexander on 01.03.17.
 */
public class Main {

    // The name of the file to open.
    public static final String dbFileName = "data/CenceMeLiteLog20.txt";
    static private FileReader inputFile;

    public static void main(String[] args) throws FileNotFoundException {
        // This will reference one line at a time
        String line = null;

        try {
            // FileReader reads text files in the default encoding.
            FileReader fileReader = new FileReader(dbFileName);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = new BufferedReader(fileReader);

            while ((line = bufferedReader.readLine()) != null) {
                System.out.println(line);
            }

            // Always close files.
            bufferedReader.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
}
