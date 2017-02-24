import java.sql.SQLException;

public class Main {
    static LifeMapDatabase db;

    /**
     * @param args
     * @throws SQLException
     */
    public static void main(String[] args) throws SQLException {
        db = new LifeMapDatabase();
        db.showMobilityTrace();
    }

}