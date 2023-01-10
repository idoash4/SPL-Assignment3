package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.HashMap;

public class FrameMessageEncoderDecoder implements MessageEncoderDecoder<Frame> {

    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    public static final char END_OF_MESSAGE = '\0';

    @Override
    public Frame decodeNextByte(byte nextByte) {
        if (nextByte == END_OF_MESSAGE) {
            return popFrame();
        }

        pushByte(nextByte);
        return null; //not a frame yet
    }

    @Override
    public byte[] encode(Frame message) {
        return (message.toString() + END_OF_MESSAGE).getBytes(); //uses utf8 by default
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

    private Frame popFrame() {
        try {
            String message = new String(bytes, 0, len, StandardCharsets.UTF_8);
            return parseFrame(message);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            len = 0;
        }
        return null;
    }

    private Frame parseFrame(String message) {
        try (BufferedReader reader = new BufferedReader(new StringReader(message))) {
            FrameType type = FrameType.valueOf(reader.readLine());

            HashMap<String, String> headers = new HashMap<>();
            String line = reader.readLine();

            // Read lines into headers until we reach an empty line
            while (!line.isEmpty()) {
                String[] splintedLine = line.split(Frame.HEADER_DELIMITER, 2);
                headers.put(splintedLine[0], splintedLine[1]);
                line = reader.readLine();
            }

            // Read the rest of the message as the body of the frame
            StringBuilder body = new StringBuilder();
            while ((line = reader.readLine()) != null) {
                body.append(line);
            }

            return new Frame(type, headers, body.toString());

        } catch (Exception ex) {
            ex.printStackTrace();
            return new Frame(FrameType.INVALID);
        }
    }
}
