package bgu.spl.net.srv;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    // void connect(int connectionId, boolean blocking, ConnectionHandler<T> cHandler);

    ConcurrentHashMap<Integer, Client<T>> getActiveClients();

    boolean isUserAlreadyActive(String username);

    void attachUserToClient (int connectionId, User user);
}
