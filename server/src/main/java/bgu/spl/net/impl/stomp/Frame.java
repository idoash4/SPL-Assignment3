package bgu.spl.net.impl.stomp;

import java.util.HashMap;

public class Frame {
    private final FrameType type;
    private final HashMap<String, String> headers;
    private String body;
    public static final String HEADER_DELIMITER = String.valueOf(':');

    public Frame(FrameType type, HashMap<String, String> headers, String body) {
        this.type = type;
        this.headers = headers;
        this.body = body;
    }

    public Frame(FrameType type) {
        this.type = type;
        headers = new HashMap<>();
    }

    public FrameType getType() {
        return type;
    }

    public String getHeader(String key) throws ProtocolException{
        String header = headers.get(key);
        if (header == null) {
            throw new ProtocolException("Missing header: " + key, this);
        }
        return header;
    }

    public String getHeader(String key, String defaultValue) {
        String header = headers.get(key);
        if (header == null) {
            return defaultValue;
        }
        return header;
    }

    public void setHeader(String key, String value) {
        headers.put(key, value);
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append(type.toString()).append('\n');
        headers.forEach((k, v) -> builder.append(k).append(HEADER_DELIMITER).append(v).append("\n"));
        builder.append("\n");
        if (body != null)
            builder.append(body);
        return builder.toString();
    }
}
