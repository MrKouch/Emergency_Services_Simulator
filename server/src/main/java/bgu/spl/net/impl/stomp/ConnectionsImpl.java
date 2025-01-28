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
    private ConcurrentHashMap<Integer, Client<T>> activeClients;
    private ConcurrentHashMap<String, HashSet<Client<T>>> subscriptions;
    private ConcurrentHashMap<String, User> allUsers;

    public ConnectionsImpl() {
        this.activeClients = new ConcurrentHashMap<>();
        this.subscriptions = new ConcurrentHashMap<>();
        this.allUsers = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        Client<T> client = activeClients.get(connectionId);
        if (client == null) {
            return false;
        }
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
                if (entry.getValue().equals(destination)) {
                    subscriptionId = entry.getKey();
                }
            }
            Frame messageFrame = MessageFrame.getMessageFrame(msgFrame, subscriptionId);
            client.getcHandler().send((T) messageFrame.toString());
        }
                
    }

    @Override
    public void disconnect(int connectionId) {
        System.out.println("Client disconnected");
        Client<T> theClient = activeClients.get(connectionId);
        System.out.println("after Client disconnected");
        if (theClient.getUser() == null)
            System.out.println("Client user is null");
        System.out.println(theClient.getUser().getUsername() + " disconnected");
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

    public void closeConnection(int connectionId) {
        try {
            activeClients.get(connectionId).getcHandler().close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
  
    public void connect(int connectionId, ConnectionHandler<T> cHandler) {
        Client<T> client = new Client<T>(cHandler);
        this.activeClients.putIfAbsent(connectionId, client);
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

    public boolean isUserSubscribedToChannel(int connectionId, String channel) {
        HashSet<Client<T>> subscribedClients = subscriptions.get(channel);
        if (subscribedClients == null) {
            System.out.println("[DEBUG]: subscribedClients is null");
            return false;
        }
        System.out.println("[DEBUG]: username: " + activeClients.get(connectionId).getUser().getUsername());
        System.out.println("[DEBUG]: usernames: ");
        for (Client<T> client : subscribedClients) {
            System.out.println("[DEBUG]: " + client.getUser().getUsername());
        }
        return subscribedClients.contains(activeClients.get(connectionId));
    }

    public void attachUserToClient(int connectionId, User user) {
        activeClients.get(connectionId).setUser(user);
    }

    public void subscribe(int connectionId, String destination, String id) {
        Client<T> client = getActiveClient(connectionId);
        User user = client.getUser();
        System.out.println("[DEBUG]: user descriptions before user.addSubscription: " + user.getSubscriptions());
        user.addSubscription(id, destination);
        System.out.println("[DEBUG]: user descriptions after user.addSubscription: " + user.getSubscriptions());
        HashSet<Client<T>> subscribesClients = subscriptions.computeIfAbsent(destination, k -> new HashSet<>());
        System.out.println("[DEBUG]: subscriptions before subscribesClients.add: " + subscribesClients);
        subscribesClients.add(client);
        System.out.println("[DEBUG]: subscriptions after subscribesClients.add: " + subscribesClients);
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

    public void addUser(String username, User user) {
        allUsers.put(username, user);
    }
}
