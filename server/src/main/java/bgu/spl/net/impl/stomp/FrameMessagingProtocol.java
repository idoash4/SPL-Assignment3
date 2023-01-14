package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageListener;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

import java.util.List;

public class FrameMessagingProtocol implements StompMessagingProtocol<Frame> {
    private int connectionId;
    private Connections<Frame> connections;
    private final List<MessageListener<Frame>> listeners;
    public static final String VERSION = "1.2";

    public FrameMessagingProtocol(List<MessageListener<Frame>> listeners) {
        this.listeners = listeners;
    }

    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(Frame message) {
        if (shouldTerminate()) {
            return;
        }

        try {
            if (message.getType() == FrameType.INVALID) {
                throw new ProtocolException("Malformed frame received");
            }

            for (MessageListener<Frame> listener : listeners) {
                if (listener.isForListener(message)) {
                    listener.process(connectionId, message);
                }
            }
        } catch (ProtocolException ex) {
            connections.send(connectionId, ex.toErrorFrame());
            listeners.forEach(listener -> listener.clear(connectionId));
            connections.disconnect(connectionId);
        }
    }

    @Override
    public boolean shouldTerminate() {
        return !connections.isConnected(connectionId);
    }
}
