package bgu.spl.net.impl.stomp;

import java.io.IOException;
import java.util.Arrays;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Client;
import bgu.spl.net.srv.ConnectedFrame;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.FramesParser;
import bgu.spl.net.srv.ReceiptFrame;
import bgu.spl.net.srv.ErrorFrame;
import bgu.spl.net.srv.User;

public class StompMessagingProtocolImpl implements MessagingProtocol<String> {
    private boolean shouldTerminate = false;

    @Override
    public void start(int connectionId, Connections<String> connections, User user) {
        connections.attachUserToClient(connectionId, user);
    }

    @Override
    public void process(String msg, Connections<String> connections, int connectionId) throws IOException {
        String[] lines = msg.split(System.lineSeparator());
        String command = lines[0];
        if (command.equals("CONNECT")) {
            ConcurrentHashMap<String, String> connectFrame = FramesParser.parse("connect", Arrays.copyOfRange(lines, 1, lines.length));
            if (connectFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("CONNECT", connectFrame));
            }
            else {
                String username = connectFrame.get("login");
                String password = connectFrame.get("passcode");
                if (connections.isUserAlreadyActive(username)) {
                    Frame userAlreadyLoggedInFrame = ErrorFrame.getErrorFrame("USER ALREADY LOGGED IN");
                    connections.send(connectionId, userAlreadyLoggedInFrame.toString());
                }
                else {
                    User user = connections.getUserByName(username);
                    if (user == null || (user != null && user.getPassword() == password)) {
                        start(connectionId, connections, user);
                        Frame connectedFrame = ConnectedFrame.getConnectedFrame();
                        connections.send(connectionId, connectedFrame.toString());
                        connections.addUserIfAbsent(username, user);
                    }
                    else {
                        Frame wrongPasswordFrame = ErrorFrame.getErrorFrame("WRONG PASSWORD");
                        connections.send(connectionId, wrongPasswordFrame.toString());
                        
                    }
                }
            }
        } else if (command == "DISCONNECT") {
            ConcurrentHashMap<String, String> disconnectFrame = FramesParser.parse("disconnect", Arrays.copyOfRange(lines, 1, lines.length));
            if (disconnectFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("DISCONNECT", disconnectFrame));
            }            
        }
        else if (command == "SUBSCRIBE") {
            ConcurrentHashMap<String, String> subscribeFrame = FramesParser.parse("subscribe", Arrays.copyOfRange(lines, 1, lines.length));
            if (subscribeFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("SUBSCRIBE", subscribeFrame));
            }
            String destination = subscribeFrame.get("destination");
            String id = subscribeFrame.get("id");
            connections.subscribe(connectionId, destination, id);
            Frame receiptFrame = ReceiptFrame.getReceiptFrame(id);
            connections.send(connectionId, receiptFrame.toString());
        } else if (command == "UNSUBSCRIBE") {
            ConcurrentHashMap<String, String> unsubscribeFrame = FramesParser.parse("unsubscribe", Arrays.copyOfRange(lines, 1, lines.length));
            if (unsubscribeFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("UNSUBSCRIBE", unsubscribeFrame));
            }
            String destination = unsubscribeFrame.get("destination");
            String id = unsubscribeFrame.get("id");
            connections.unsubscribe(connectionId, destination, id);
            String receiptId = unsubscribeFrame.get("receipt");
            Frame receiptFrame = ReceiptFrame.getReceiptFrame(receiptId);
            connections.send(connectionId, receiptFrame.toString());
        } else if (command == "SEND") {
            ConcurrentHashMap<String, String> sendFrame = FramesParser.parse("unsubscribe", Arrays.copyOfRange(lines, 1, lines.length));
            if (sendFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("SEND", sendFrame));
            }
            String destination = sendFrame.get("destination");
            connections.send(destination, sendFrame);
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
    
}
