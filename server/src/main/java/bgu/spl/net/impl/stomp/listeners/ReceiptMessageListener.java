package bgu.spl.net.impl.stomp.listeners;

import bgu.spl.net.api.MessageListener;
import bgu.spl.net.impl.stomp.Frame;
import bgu.spl.net.impl.stomp.FrameType;
import bgu.spl.net.srv.Connections;


public class ReceiptMessageListener implements MessageListener<Frame> {
    private Connections<Frame> connections;

    @Override
    public void start(Connections<Frame> connections) {
        this.connections = connections;
    }

    @Override
    public boolean isForListener(Frame message) {
        return true;
    }

    @Override
    public void process(int connectionId, Frame message) {
        String receiptId = message.getHeader("receipt", null);
        if (receiptId != null) {
            Frame receiptFrame = new Frame(FrameType.RECEIPT);
            receiptFrame.setHeader("receipt-id", receiptId);
            connections.send(connectionId, receiptFrame);
        }
    }

    @Override
    public void clear(int connectionId) {}
}
