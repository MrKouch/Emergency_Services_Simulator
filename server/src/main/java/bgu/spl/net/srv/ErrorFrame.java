package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class ErrorFrame {

    public static Frame getErrorFrame(String type) {
        ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();
        if (type.equals("USER ALREADY LOGGED IN")) {
            headers.put("message", "User already logged in");
            return new Frame("ERROR", headers);
        } else if (type.equals("WRONG PASSWORD")) {
            headers.put("message", "Wrong Password");
            return new Frame("ERROR", headers);
        }
        return null;
    }
}
