package bgu.spl.net.impl.stomp.listeners;

import bgu.spl.net.api.MessageListener;
import bgu.spl.net.impl.stomp.Frame;
import bgu.spl.net.impl.stomp.FrameMessagingProtocol;
import bgu.spl.net.impl.stomp.FrameType;
import bgu.spl.net.impl.stomp.ProtocolException;
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
                login(connectionId, message);
                break;
            case DISCONNECT:
                logout(connectionId, message);
                break;
            default:
                if (!loggedInUsers.containsKey(connectionId))
                    throw new ProtocolException("User not logged in", message);
        }
    }

    public void login(int connectionId, Frame message) throws ProtocolException {
        String username = message.getHeader("login");
        String password = message.getHeader("passcode");

        // password and username can't be null so we use putIfAbsent for thread safety
        if (users.putIfAbsent(username, password) != null && !users.get(username).equals(password)) {
            throw new ProtocolException("Wrong password", message);
        }

        loginLock.lock();
        try {
            if (loggedInUsers.containsValue(username)) {
                throw new ProtocolException("User already logged in", message);
            }
            loggedInUsers.put(connectionId, username);
        } finally {
            loginLock.unlock();
        }

        Frame connectedFrame = new Frame(FrameType.CONNECTED);
        connectedFrame.setHeader("version", FrameMessagingProtocol.VERSION);
        connections.send(connectionId, connectedFrame);
    }

    public void logout(int connectionId, Frame message) throws ProtocolException {
        loggedInUsers.remove(connectionId);

        // We have to send the receipt frame here because the client will be removed from the connections map
        Frame receiptFrame = new Frame(FrameType.RECEIPT);
        receiptFrame.setHeader("receipt-id", message.getHeader("receipt"));
        connections.send(connectionId, receiptFrame);

        // Remove the client from the server
        connections.disconnect(connectionId);
    }

    @Override
    public void clear(int connectionId) {
        loggedInUsers.remove(connectionId);
    }


}
