package bgu.spl.net.impl.stomp;

import java.io.IOException;
import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.srv.BlockingConnectionHandler;
import bgu.spl.net.srv.Client;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.MessageFrame;
import bgu.spl.net.srv.ReceiptFrame;
import bgu.spl.net.srv.User;

public class ConnectionsImpl<T> implements Connections<T> {
    private ConcurrentHashMap<String, User> allUsers;
    private ConcurrentHashMap<Integer, Client<T>> activeClients;
    private ConcurrentHashMap<String, HashSet<Client<T>>> subscriptions;

    public ConnectionsImpl() {
        this.activeClients = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        Client<T> client = activeClients.get(connectionId);
        if (client == null) return false;
        client.getcHandler().send(msg);
        return true;
    }

    @SuppressWarnings("unchecked")
    @Override
    public void send(String channel, ConcurrentHashMap<String, String> msgFrame) {
        String destination = msgFrame.get("destination");
        for (Client<T> client : subscriptions.get(destination)) {
            String subscriptionId = "";
            for (Map.Entry<String, String> entry : client.getUser().getSubscriptions().entrySet()) {
                if (entry.getValue() == destination)
                    subscriptionId = entry.getKey();
            }
            Frame messageFrame = MessageFrame.getMessageFrame(msgFrame, subscriptionId);
            client.getcHandler().send((T) messageFrame.toString());
        }
                
    }

    @Override
    public void disconnect(int connectionId) {
        String username = activeClients.remove(connectionId).getUser().getUsername();

        for (HashSet<Client<T>> subscribedClients : subscriptions.values()) {
            Iterator<Client<T>> iterator = subscribedClients.iterator();
            while (iterator.hasNext()) {
                Client<T> client = iterator.next();
                if (client.getUser().getUsername().equals(username)) {
                    iterator.remove();
                }
            }
        }
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

    public Client<T> getActiveClient(int connectionId) {
        return activeClients.get(connectionId);
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

    public void subscribe(int connectionId, String destination, String id) {
        Client<T> client = getActiveClient(connectionId);
        User user = client.getUser();
        user.addSubscription(id, destination);
        HashSet<Client<T>> subscribesClients = subscriptions.get(destination);
        if (subscribesClients == null) {
            subscribesClients = new HashSet<>();
        }
        subscribesClients.add(client);
    }

    public void unsubscribe(int connectionId, String destination, String id) {
        Client<T> client = getActiveClient(connectionId);
        User user = client.getUser();
        user.removeSubscription(id);
        HashSet<Client<T>> subscribesClients = subscriptions.get(destination);
        // we can assume that a case in which a client unsubscribe a channel that he hasnt joined to, is being resolved in the client
        subscribesClients.remove(client);
    }

    public User getUserByName(String username) {
        return allUsers.get(username);
    }

    public void addUserIfAbsent(String username, User user) {
        allUsers.putIfAbsent(username, user);
    }
}
