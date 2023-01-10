package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageListener;
import bgu.spl.net.srv.Server;
import bgu.spl.net.srv.ConnectionsManager;

import java.util.ArrayList;
import java.util.Collections;

public class StompServer {

    public static void main(String[] args) {
        ConnectionsManager<Frame> connections = new ConnectionsManager<>();
        ArrayList<MessageListener<Frame>> listeners = new ArrayList<>();
        ConnectMessageListener connectListener = new ConnectMessageListener();
        connectListener.start(connections);
        SubscriptionMessageListener subscriptionMessageListener = new SubscriptionMessageListener();
        subscriptionMessageListener.start(connections);
        ReceiptMessageListener receiptListener = new ReceiptMessageListener();
        receiptListener.start(connections);

        listeners.add(connectListener);
        listeners.add(subscriptionMessageListener);
        listeners.add(receiptListener);

        if (args.length < 2) {
            System.out.println("Usage: StompServer <type> <port>");
            return;
        }

        String serverType = args[0];
        int port = Integer.parseInt(args[1]);

        if (serverType.equals("reactor")) {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port, //port
                    () -> new FrameMessagingProtocol(Collections.unmodifiableList(listeners)), //protocol factory
                    FrameMessageEncoderDecoder::new,
                    connections//message encoder decoder factory
            ).serve();
        } else if (serverType.equals("tpc")) {
            Server.threadPerClient(
                    port, //port
                    () -> new FrameMessagingProtocol(Collections.unmodifiableList(listeners)), //protocol factory
                    FrameMessageEncoderDecoder::new,
                    connections
            ).serve();
        } else {
            System.out.println("Unknown server type: " + serverType);
        }
    }
}
