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
        System.out.println("[DEBUG]: in start");
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
                handleError(connectionId, connections);
            }
            else {
                String username = connectFrame.get("login");
                String password = connectFrame.get("passcode");
                if (connections.isUserAlreadyActive(username)) {
                    Frame userAlreadyLoggedInFrame = ErrorFrame.getErrorFrame("USER ALREADY LOGGED IN");
                    connections.send(connectionId, userAlreadyLoggedInFrame.toString());
                    handleError(connectionId, connections);
                }
                else {
                    User user = connections.getUserByName(username);
                    if (user == null || (user != null && user.getPassword().equals(password))) {
                        if (user == null) {
                            user = new User(username, password);
                            connections.addUser(username, user);
                        }
                        System.out.println("[DEBUG]: in connect");
                        start(connectionId, connections, user);
                        Frame connectedFrame = ConnectedFrame.getConnectedFrame();
                        connections.send(connectionId, connectedFrame.toString());
                    }
                    else {
                        Frame wrongPasswordFrame = ErrorFrame.getErrorFrame("WRONG PASSWORD");
                        connections.send(connectionId, wrongPasswordFrame.toString());
                        handleError(connectionId, connections);
                    }
                }
            }
        } else if (command.equals("DISCONNECT")) {
            System.out.println("[DEBUG]: in disconnect");
            ConcurrentHashMap<String, String> disconnectFrame = FramesParser.parse("disconnect", Arrays.copyOfRange(lines, 1, lines.length));
            if (disconnectFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("DISCONNECT", disconnectFrame));
                handleError(connectionId, connections);
            }
            else {
                ConnectionHandler<String> connectionHandler = connections.getActiveClient(connectionId).getcHandler();
                connections.disconnect(connectionId);
                // shouldTerminate = true;

                Frame receiptFrame = ReceiptFrame.getReceiptFrame(disconnectFrame.get("receipt"));
                System.out.println("[DEBUG]: sending receipt frame");
                System.out.println(receiptFrame.toString());
                connectionHandler.send(receiptFrame.toString());
            }   
        }
        else if (command.equals("SUBSCRIBE")) {
            ConcurrentHashMap<String, String> subscribeFrame = FramesParser.parse("subscribe", Arrays.copyOfRange(lines, 1, lines.length));
            if (subscribeFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("SUBSCRIBE", subscribeFrame));
                handleError(connectionId, connections);
            }
            String destination = subscribeFrame.get("destination");
            String id = subscribeFrame.get("id");
            String receiptId = subscribeFrame.get("receipt");
            connections.subscribe(connectionId, destination, id);
            Frame receiptFrame = ReceiptFrame.getReceiptFrame(receiptId);
            connections.send(connectionId, receiptFrame.toString());
        } else if (command.equals("UNSUBSCRIBE")) {
            ConcurrentHashMap<String, String> unsubscribeFrame = FramesParser.parse("unsubscribe", Arrays.copyOfRange(lines, 1, lines.length));
            if (unsubscribeFrame.get("missing_key") != null) {
                connections.send(connectionId, FramesParser.toStringMalformedError("UNSUBSCRIBE", unsubscribeFrame));
                handleError(connectionId, connections);
            }
            String destination = unsubscribeFrame.get("destination");
            String id = unsubscribeFrame.get("id");
            connections.unsubscribe(connectionId, destination, id);
            String receiptId = unsubscribeFrame.get("receipt");
            Frame receiptFrame = ReceiptFrame.getReceiptFrame(receiptId);
            connections.send(connectionId, receiptFrame.toString());
        } else if (command.equals("SEND")) {
            System.out.println("[DEBUG]: in send");
            for (String line : lines) {
                System.out.println(line);
            }
            ConcurrentHashMap<String, String> sendFrame = FramesParser.parse("send", Arrays.copyOfRange(lines, 1, lines.length));
            if (sendFrame.get("missing_key") != null) {
                System.out.println("[DEBUG]: missing key in send frame");
                connections.send(connectionId, FramesParser.toStringMalformedError("SEND", sendFrame));
                handleError(connectionId, connections);
            }
            System.out.println("[DEBUG]: before send channel");
            String destination = sendFrame.get("destination");
            System.out.println("[DEBUG]: before send channel, destination: " + destination);
            connections.send(destination, sendFrame);
        }
    }

    public void handleError(int connectionId, Connections<String> connections) {
        connections.closeConnection(connectionId);
        connections.disconnect(connectionId);
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
    
}
