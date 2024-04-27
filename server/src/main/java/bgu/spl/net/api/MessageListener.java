package bgu.spl.net.api;

import bgu.spl.net.impl.stomp.ProtocolException;
import bgu.spl.net.srv.Connections;

public interface MessageListener<T> {

    void start(Connections<T> connections);

    boolean isForListener(T message);

    void process(int connectionId, T message) throws ProtocolException;

    void clear(int connectionId);
}
