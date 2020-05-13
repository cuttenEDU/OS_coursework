import javafx.embed.swing.JFXPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

import java.io.IOException;

public class FXPanel {


    JFXPanel createPanel() throws IOException {
        JFXPanel mainPanel = new JFXPanel();
        Parent parent = FXMLLoader.load(Main.class.getResource("mainScene.fxml"));
        InfoUpdater.init();
        TextField time = (TextField) parent.lookup("#time");
        TextField session = (TextField) parent.lookup("#session");
        TextField cp = (TextField) parent.lookup("#cp");
        TextField s1p= (TextField) parent.lookup("#s1p");
        TextField s2p = (TextField) parent.lookup("#s2p");
        TextArea tpta = (TextArea) parent.lookup("#tpta");
        Button reload = (Button) parent.lookup("#reload");
        time.setText(InfoUpdater.getSystemTime());
        session.setText(InfoUpdater.getSessionTime());
        String[] priors = InfoUpdater.getPriorities().split("\n");
        cp.setText(priors[0]);
        s1p.setText(priors[1]);
        s2p.setText(priors[2]);
        tpta.setText(InfoUpdater.getTPriorities());

        reload.setOnAction(event -> {
            time.setText(InfoUpdater.getSystemTime());
            session.setText(InfoUpdater.getSessionTime());
            String [] priors1 = InfoUpdater.getPriorities().split("\n");
            cp.setText(priors1[0]);
            s1p.setText(priors1[1]);
            s2p.setText(priors1[2]);
            tpta.setText(InfoUpdater.getTPriorities());
        });

        mainPanel.setScene(new Scene(parent));
        return mainPanel;
    }



}
