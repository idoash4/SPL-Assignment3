package bgu.spl.net.impl.stomp.listeners;

import bgu.spl.net.api.MessageListener;
import bgu.spl.net.impl.stomp.Frame;
import bgu.spl.net.impl.stomp.FrameType;
import bgu.spl.net.impl.stomp.ProtocolException;
import bgu.spl.net.srv.Connections;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class SubscriptionMessageListener implements MessageListener<Frame> {
    private Connections<Frame> connections;
    private ConcurrentHashMap<String, Topic> topics = new ConcurrentHashMap<>();

    @Override
    public void start(Connections<Frame> connections) {
        this.connections = connections;
    }

    @Override
    public boolean isForListener(Frame message) {
        FrameType type = message.getType();
        switch (type) {
            case SUBSCRIBE:
            case UNSUBSCRIBE:
            case SEND:
            case DISCONNECT:
                return true;
            default:
                return false;
        }
    }

    @Override
    public void process(int connectionId, Frame message) throws ProtocolException {
        FrameType type = message.getType();
        switch (type) {
            case SUBSCRIBE:
                subscribe(connectionId, message);
                break;
            case UNSUBSCRIBE:
                unsubscribe(connectionId, message);
                break;
            case DISCONNECT:
                clear(connectionId);
                break;
            case SEND:
                send(connectionId, message);
                break;
        }
    }

    @Override
    public void clear(int connectionId) {
        for (Topic topic : topics.values()) {
            topic.removeSubscriber(connectionId);
        }
    }

    public void subscribe(int connectionId, Frame message) throws ProtocolException {
        String topicName = message.getHeader("destination");
        String subscriptionId = message.getHeader("id");
        // We create an object and use put if absent for thread safety
        topics.putIfAbsent(topicName, new Topic(topicName));
        Topic topic = topics.get(topicName);
        if (topic != null) {
            topic.addSubscriber(connectionId, subscriptionId);
        }

    }

    public void unsubscribe(int connectionId, Frame message) throws ProtocolException {
        String topicName = message.getHeader("destination");
        Topic topic = topics.get(topicName);
        if (topic == null) {
            throw new ProtocolException("Destination does not exist", message);
        }
        topic.removeSubscriber(connectionId);
    }

    public void send(int connectionId, Frame message) throws ProtocolException {
        String topicName = message.getHeader("destination");
        Topic topic = topics.get(topicName);
        if (topic == null) {
            throw new ProtocolException("Destination does not exist", message);
        }
        if (!topic.hasSubscriber(connectionId)) {
            throw new ProtocolException("User is not subscribed to this topic", message);
        }
        // Add the message to the topic
        Message topicMessage = topic.addMessage(message.getBody());
        HashMap<Integer, String> subscribers = topic.getSubscribers();
        // Send the message to all subscribers
        for (int subscriberConnectionId : subscribers.keySet()) {
            Frame broadcastFrame = new Frame(FrameType.MESSAGE);
            broadcastFrame.setHeader("subscription", subscribers.get(subscriberConnectionId));
            broadcastFrame.setHeader("message-id", String.valueOf(topicMessage.getId()));
            broadcastFrame.setHeader("destination", topicName);
            broadcastFrame.setBody(topicMessage.getContent());
            connections.send(subscriberConnectionId, broadcastFrame);
        }
    }

}

class Topic {
    private String name;
    private List<Message> messages;
    private AtomicInteger messagesCounter;
    // connection id to subscription id
    private ConcurrentHashMap<Integer, String> subscribers;

    public Topic(String name) {
        this.name = name;
        this.messages = Collections.synchronizedList(new ArrayList<>());
        this.messagesCounter = new AtomicInteger(0);
        this.subscribers = new ConcurrentHashMap<>();
    }

    public String getName() {
        return name;
    }

    public boolean hasSubscriber(int connectionId) {
        return subscribers.containsKey(connectionId);
    }

    public void addSubscriber(int connectionId, String subscriptionId) {
        subscribers.put(connectionId, subscriptionId);
    }

    public void removeSubscriber(int connectionId) {
        subscribers.remove(connectionId);
    }

    public HashMap<Integer, String> getSubscribers() {
        return new HashMap<>(subscribers);
    }

    public Message addMessage(String message) {
        Message msg = new Message(messagesCounter.getAndIncrement(), message);
        messages.add(msg);
        return msg;
    }

}

class Message {
    private int id;
    private String content;

    public Message(int id, String content) {
        this.id = id;
        this.content = content;
    }

    public int getId() {
        return id;
    }

    public String getContent() {
        return content;
    }
}