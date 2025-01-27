package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class ConnectedFrame {

    public static Frame getConnectedFrame() {
        ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();
        headers.put("version", "1.2");
        return new Frame("CONNECTED", headers);
    }
}
