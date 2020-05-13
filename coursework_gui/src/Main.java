import javafx.embed.swing.JFXPanel;

        import javax.swing.*;
        import java.io.IOException;
        import java.util.Arrays;

public class Main extends JFrame {

    Main() throws IOException {
        super("Клиент");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        FXPanel panel = new FXPanel();
        setContentPane(panel.createPanel());
        setLocationRelativeTo(null);
        setVisible(true);
        setResizable(false);
        pack();
    }

    public static void main(String[] args) throws IOException {
        Main main = new Main();
        //test();
    }

    static void test(){
        InfoUpdater.init();
        System.out.println(InfoUpdater.getSystemTime());
        System.out.println(InfoUpdater.getSessionTime());
        System.out.println(InfoUpdater.getPriorities());
        System.out.println(Arrays.toString(InfoUpdater.getPriorities().split("\n")));
        System.out.println(InfoUpdater.getTPriorities());
    }
}
