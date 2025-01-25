package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;

public class StompMessagingProtocolImpl implements MessagingProtocol<String> {
    private boolean shouldTerminate = false;

    @Override
    public void start(int connectionId, Connections<String> connections, User user) {
        connections.attachUserToClient(connectionId, user);
    }

    @Override
    public void process(String msg, Connections<String> connections, int connectionId) {
        String[] lines = msg.split(System.lineSeparator());
        String command = lines[0];
        if (command.equals("CONNECT")) {
            String username = lines[3];
            String password = lines[4];
            if (connections.isUserAlreadyActive(username)) {
                System.out.println("user already logged in");
                // returns ERROR frame "User alreadylogged in"
                shouldTerminate = true;
            }
            else {
                User user = new User(username, password);
                start(connectionId, connections, user);
                System.out.println("connecting user");
                // returns CONNECTED frame
            }
        }
        System.out.println("sec line: " + lines[1]);
        System.out.println("third line: " + lines[2]);
        System.out.println("forth line: " + lines[3]);
        System.out.println("fifth line: " + lines[4]);
        System.out.println("Active Clients: " + connections.getActiveClients().get(1));
        
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
    
}
