package bgu.spl.net.srv;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, ConcurrentHashMap<String, String> msg);

    void disconnect(int connectionId);

    // void connect(int connectionId, boolean blocking, ConnectionHandler<T> cHandler);

    ConcurrentHashMap<Integer, Client<T>> getActiveClients();

    boolean isUserAlreadyActive(String username);

    boolean isUserSubscribedToChannel(int connectionId, String channel);

    void attachUserToClient (int connectionId, User user);

    Client<T> getActiveClient(int connectionId);

    void subscribe(int connectionId, String destination, String id);

    void unsubscribe(int connectionId, String id);

    User getUserByName(String username);

    void addUser(String username, User user);

}
