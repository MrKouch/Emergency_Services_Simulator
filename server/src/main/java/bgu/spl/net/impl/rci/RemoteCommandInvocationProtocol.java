package bgu.spl.net.impl.rci;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;

import java.io.Serializable;

public class RemoteCommandInvocationProtocol<T> implements MessagingProtocol<Serializable> {

    private T arg;

    @Override
    public void start(int connectionId, Connections<Serializable> connections, User user) {

    }
    public RemoteCommandInvocationProtocol(T arg) {
        this.arg = arg;
    }

    @Override
    public void process(Serializable msg, Connections<Serializable> connections, int connectionId) {
        // return ((Command) msg).execute(arg);
    }

    @Override
    public boolean shouldTerminate() {
        return false;
    }

}
