/**
 * User: sbt-tcarev-aa 
 * Date: 04.06.2017
 * Time: 13:12
 */
class FileParserStart {

    static void main(String[] args) {
        println "Hello World from groovy"

        def maxLines = 0L
        String filename = null
        def size = args.size()
        String outFileName = null
        switch (size) {
            case 0:
                println "Filename does not specified"
                break

            case 1:
                filename = args[0]
                break

            case 2:
                filename = args[0]
                maxLines = Long.valueOf(args[1])
                break

            case 3:
            default:
                filename = args[0]
                maxLines = Long.valueOf(args[1])
                outFileName = args[2]
                break

        }

        final def file = new BufferedReader(new FileReader(filename))
        final def outFile = new BufferedWriter(new FileWriter(outFileName))
        for (int i = 0; i < maxLines; i++) {
            def line = file.readLine()
            assert i <= maxLines: "wrong logic"
            if (i < maxLines)
                if (line != null) {
                    if (!outFileName) {
                        println line
                    } else {
                        outFile.writeLine(line)
                    }
                } else {
                    break
                }
        }
        outFile.flush()
        outFile.close()
    }
}
