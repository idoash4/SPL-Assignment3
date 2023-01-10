package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageListener;
import bgu.spl.net.srv.Connections;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.ReentrantLock;

public class ConnectMessageListener implements MessageListener<Frame> {
    private Connections<Frame> connections;
    private ConcurrentHashMap<String, String> users = new ConcurrentHashMap<>();
    private ConcurrentHashMap<Integer, String> loggedInUsers = new ConcurrentHashMap<>();
    private ReentrantLock loginLock = new ReentrantLock();

    @Override
    public void start(Connections<Frame> connections) {
        this.connections = connections;
    }

    @Override
    public boolean isForListener(Frame message) {
        return true;
    }

    @Override
    public void process(int connectionId, Frame message) throws ProtocolException {
        switch (message.getType()) {
            case CONNECT:
                String username = message.getHeader("login");
                String password = message.getHeader("passcode");

                // password and username can't be null so we use putIfAbsent for thread safety
                if (users.putIfAbsent(username, password) != null && !users.get(username).equals(password)) {
                    throw new ProtocolException("Wrong password");
                }

                loginLock.lock();
                if (loggedInUsers.containsKey(connectionId)) {
                    throw new ProtocolException("Already logged in");
                }
                loggedInUsers.put(connectionId, username);
                loginLock.unlock();

                Frame connectedFrame = new Frame(FrameType.CONNECTED);
                connectedFrame.setHeader("version", FrameMessagingProtocol.VERSION);
                connections.send(connectionId, connectedFrame);
                break;
            case DISCONNECT:
                loggedInUsers.remove(connectionId);

                // We have to send the receipt frame here because the client will be removed from the connections map
                Frame receiptFrame = new Frame(FrameType.RECEIPT);
                receiptFrame.setHeader("receipt-id", message.getHeader("receipt"));
                connections.send(connectionId, receiptFrame);

                // Remove the client from the server
                connections.disconnect(connectionId);
                break;
            default:
                if (!loggedInUsers.containsKey(connectionId))
                    throw new ProtocolException("User not logged in", message);
        }
    }

    @Override
    public void clear(int connectionId) {
        loggedInUsers.remove(connectionId);
    }


}
