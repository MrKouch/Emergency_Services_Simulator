package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class User {
    private String username;
    private String password;
    private ConcurrentHashMap<String, String> subscriptions;

    public User(String username, String password) {
        this.username = username;
        this.password = password;
        this.subscriptions = new ConcurrentHashMap<>();
    }
    
    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public ConcurrentHashMap<String, String> getSubscriptions() {
        return subscriptions;
    }

    public void addSubscription(String id, String subscription) {
        this.subscriptions.put(id, subscription);
    }

    public void removeSubscription(String id) {
        this.subscriptions.remove(id);
    }

}
