public class InfoUpdater {

    static{
        System.loadLibrary("Project1");
    }

    public native static String getSystemTime();

    public native static String getSessionTime();

    public native static String getPriorities();

    public native static String getTPriorities();

    public native static void init();
}
