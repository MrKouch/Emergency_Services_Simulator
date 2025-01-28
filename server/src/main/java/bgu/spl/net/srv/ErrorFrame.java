package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class ErrorFrame {

    public static Frame getErrorFrame(String type) {
        ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();
        if (type.equals("USER ALREADY LOGGED IN")) {
            headers.put("message", "User already logged in");
            return new Frame("ERROR", headers);
        } else if (type.equals("WRONG PASSWORD")) {
            headers.put("message", "User password is different than what you inserted");
            return new Frame("ERROR", headers);
        } else if (type.equals("USER IS NOT SUBSCRIBED")) {
            headers.put("message", "You are not subscribed to this channel");
            return new Frame("ERROR", headers);
        }
        return null;
    }
}
