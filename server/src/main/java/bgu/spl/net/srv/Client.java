package bgu.spl.net.srv;

public class Client<T> {
    private ConnectionHandler<T> cHandler;
    private User user;

    public Client(ConnectionHandler<T> cHandler) {
        this.cHandler = cHandler;
        this.user = null;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        System.out.println("[DEBUG]: in setUser");
        System.out.println("[DEBUG]: user is " + user);
        this.user = user;
    }

    public ConnectionHandler<T> getcHandler() {
        return cHandler;
    }

}
