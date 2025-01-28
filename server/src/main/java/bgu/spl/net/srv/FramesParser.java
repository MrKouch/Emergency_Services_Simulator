package bgu.spl.net.srv;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class FramesParser {
    private static final String[] connectFrameHeaders = {"accept-version", "host", "login", "passcode"};
    private static final String[] disconnectFrameHeaders = {"receipt"};
    private static final String[] subscribeFrameHeaders = {"receipt", "destination", "id"};
    private static final String[] unsubscribeFrameHeaders = {"receipt"};
    private static final String[] sendFrameHeaders = {"destination", "event_name", "city", "date_time", "description",
                                                        "general_information", "active", "forces_arrival_at_scene"};
    private static final ConcurrentHashMap<String, String[]> typesMap;

    static {
        typesMap = new ConcurrentHashMap<>();
        typesMap.put("connect", connectFrameHeaders);
        typesMap.put("disconnect", disconnectFrameHeaders);
        typesMap.put("subscribe", subscribeFrameHeaders);
        typesMap.put("unsubscribe", unsubscribeFrameHeaders);
        typesMap.put("send", sendFrameHeaders);
    }

    public static ConcurrentHashMap<String, String> parse(String type, String[] lines) {
        System.out.println("[DEBUG]: persing type: " + type);
        ConcurrentHashMap<String, String> frameData = new ConcurrentHashMap<String, String>();
        List<String> frameHeadersList = Arrays.asList(typesMap.get(type));
        for (String line : lines) {
            String[] parts = line.split(":", 2);
            String key = parts[0];
            String value = parts.length > 1 ? parts[1] : "";
            if (frameHeadersList.contains(key)) {
                frameData.put(key, value);
            }
            else {
                System.out.println("[DEBUG]: got key: " + key + " but it's not in the list of headers for type: " + type);
                frameData.put("missing_key", key);
            }
        }
        return frameData;
    }

    public static String toStringMalformedError(String command, ConcurrentHashMap<String, String> headers) {
        StringBuilder frame = new StringBuilder("ERROR" + "\n");
        if (headers.get("receipt-id") != null) {
            frame.append("receipt-id:").append(headers.get("receipt-id")).append("\n");
        }
        frame.append("message:malformed frame received").append("\n").append("\n");
        frame.append("The message:").append("\n");
        frame.append("-----").append("\n");
        frame.append(command).append("\n");
        for (ConcurrentHashMap.Entry<String, String> header : headers.entrySet()) {
            if (!header.getKey().equals("missing_key"))
                frame.append(header.getKey()).append(":").append(header.getValue()).append("\n");
        }
        frame.append("-----").append("\n");
        frame.append("Did not contain a ").append(headers.get("missing_key")).append(" header, which is REQUIED for message propagation.");
        return frame.toString();
    }
    
}
