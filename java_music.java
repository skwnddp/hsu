import javazoom.jl.decoder.JavaLayerException;
import javazoom.jl.player.Player;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.URL;

public class SimpleMusicPlayer {

    private Player player;
    private String musicUrl;

    public SimpleMusicPlayer(String url) {
        this.musicUrl = url;
    }

    public void play() {
        try (BufferedInputStream bis = new BufferedInputStream(new URL(musicUrl).openStream())) {
            player = new Player(bis);
            System.out.println("Playing music from URL...");
            player.play();
        } catch (JavaLayerException | IOException e) {
            System.err.println("Problem playing file from URL: " + musicUrl);
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("Please provide the URL to the music file.");
            return;
        }
        SimpleMusicPlayer musicPlayer = new SimpleMusicPlayer(args[0]);
        musicPlayer.play();
    }
}
