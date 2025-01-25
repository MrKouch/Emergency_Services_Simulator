package bgu.spl.net.api;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;

public interface MessagingProtocol<T> {
    
    void start(int connectionId, Connections<T> connections, User user);
    /**
     * process the given message 
     * @param msg the received message
     * @return the response to send or null if no response is expected by the client
     */
    void process(T msg, Connections<T> connections, int connectionId);
 
    /**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();
 
}