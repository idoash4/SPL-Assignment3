package bgu.spl.net.srv;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsManager<T> implements Connections<T> {
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectedClients;
    private ConcurrentHashMap<String, List<Integer>> channels;
    private AtomicInteger clientsCounter = new AtomicInteger(0);


    public ConnectionsManager() {
        connectedClients = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> clientHandler = connectedClients.get(connectionId);

        if (clientHandler == null) {
            return false;
        }

        clientHandler.send(msg);
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        for (int connectionId : channels.get(channel)) {
            send(connectionId, msg);
        }
    }

    @Override
    public int connect(ConnectionHandler<T> connectionHandler) {
        int connectionId = clientsCounter.getAndIncrement();
        connectedClients.put(connectionId, connectionHandler);
        return connectionId;
    }

    @Override
    public void disconnect(int connectionId) {
        try {
            ConnectionHandler<T> clientHandler = connectedClients.remove(connectionId);
            if (clientHandler != null)
                clientHandler.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean isConnected(int connectionId) {
        return connectedClients.containsKey(connectionId);
    }
}
