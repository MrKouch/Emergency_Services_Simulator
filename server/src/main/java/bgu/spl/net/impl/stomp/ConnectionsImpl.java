package bgu.spl.net.impl.stomp;

import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.srv.BlockingConnectionHandler;
import bgu.spl.net.srv.Client;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;

public class ConnectionsImpl<T> implements Connections {
    private ConcurrentHashMap<Integer, Client<T>> activeClients;
    private ConcurrentHashMap<String, HashSet<T>> subscriptions;

    public ConnectionsImpl() {
        this.activeClients = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, Object msg) {
        return true;
    }

    @Override
    public void send(String channel, Object msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'send'");
    }

    @Override
    public void disconnect(int connectionId) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'disconnect'");
    }
    
    
    public void connect(int connectionId, boolean blocking, ConnectionHandler<T> cHandler) {
        Client<T> client = new Client<T>(cHandler);
        if (blocking)
        this.activeClients.putIfAbsent(connectionId, client);
        else {
            
        }
    }

    public ConcurrentHashMap<Integer, Client<T>> getActiveClients() {
        return activeClients;
    }

    public boolean isUserAlreadyActive(String username) {
        for (Client<T> client : activeClients.values()) {
            if (client.getUser() != null) {
                if (client.getUser().getUsername().equals(username))
                    return true;
            }
        }
        return false;
    }

    public void attachUserToClient(int connectionId, User user) {
        activeClients.get(connectionId).setUser(user);
    }

}
