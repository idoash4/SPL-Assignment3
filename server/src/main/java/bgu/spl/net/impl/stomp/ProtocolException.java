package bgu.spl.net.impl.stomp;

public class ProtocolException extends Exception {
    private final Frame processedFrame;
    private final String description;

    public ProtocolException(String message) {
        super(message);
        processedFrame = null;
        description = null;
    }

    public ProtocolException(String message, Frame processedFrame) {
        super(message);
        this.processedFrame = processedFrame;
        description = null;
    }

    public ProtocolException(String message, Frame processedFrame, String description) {
        super(message);
        this.processedFrame = processedFrame;
        this.description = description;
    }

    public Frame toErrorFrame() {
        Frame errorFrame = new Frame(FrameType.ERROR);
        errorFrame.setHeader("message", this.getMessage());
        StringBuilder body = new StringBuilder();
        if (processedFrame != null) {
            String receiptId = processedFrame.getHeader("receipt", null);
            if (receiptId != null) {
                errorFrame.setHeader("receipt-id", receiptId);
            }
            body.append("-----");
            body.append(processedFrame);
            body.append("-----");
        }
        if (description != null && !description.isEmpty()) {
            body.append(description);
        }
        errorFrame.setBody(body.toString());
        return errorFrame;
    }
}
