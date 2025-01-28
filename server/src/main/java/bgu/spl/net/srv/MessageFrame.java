package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class MessageFrame {
    public static Frame getMessageFrame(ConcurrentHashMap<String, String> msgFrame, String subscriptionId) {
        ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();
        headers.put("subscription", subscriptionId);
        headers.put("user", msgFrame.get("user"));
        headers.put("message-id", String.valueOf(IdGenerator.generateNextId()));
        headers.put("destination", msgFrame.get("destination"));
        headers.put("event_name", msgFrame.get("event_name"));
        headers.put("city", msgFrame.get("city"));
        headers.put("date_time", msgFrame.get("date_time"));
        headers.put("description", msgFrame.get("description"));
        headers.put("general_information", msgFrame.get("general_information"));
        headers.put("active", msgFrame.get("active"));
        headers.put("forces_arrival_at_scene", msgFrame.get("forces_arrival_at_scene"));
        return new Frame("MESSAGE", headers);
    }
}
