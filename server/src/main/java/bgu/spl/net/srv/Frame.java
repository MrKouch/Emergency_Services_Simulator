package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class Frame {
    private String command;
    private ConcurrentHashMap<String, String> headers;

    public Frame(String command, ConcurrentHashMap<String, String> headers) {
        this.command = command;
        this.headers = headers;
    }

    public String getCommand() {
        return command;
    }

    public ConcurrentHashMap<String, String> getHeaders() {
        return headers;
    }

    public String toString() {
        StringBuilder frame = new StringBuilder(command + "\n");
        for (ConcurrentHashMap.Entry<String, String> header : headers.entrySet()) {
            frame.append(header.getKey()).append(":").append(header.getValue()).append("\n");
        }
        return frame.toString();
    }

    

}