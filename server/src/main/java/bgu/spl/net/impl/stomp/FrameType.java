package bgu.spl.net.impl.stomp;

public enum FrameType {
    CONNECT,
    CONNECTED,
    MESSAGE,
    RECEIPT,
    ERROR,
    SEND,
    SUBSCRIBE,
    UNSUBSCRIBE,
    DISCONNECT,
    INVALID
}
