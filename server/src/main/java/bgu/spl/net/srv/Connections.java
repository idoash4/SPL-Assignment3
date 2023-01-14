package bgu.spl.net.srv;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    int connect(ConnectionHandler<T> connectionHandler);

    boolean isConnected(int connectionId);

}
