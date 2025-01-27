package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.impl.stomp.ConnectionsImpl;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final MessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    private int clientId;

    private ConnectionsImpl<T> connections;

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoder<T> reader, MessagingProtocol<T> protocol, ConnectionsImpl<T> connections) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.connections = connections;
        this.out = null;
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;
            
            this.in = new BufferedInputStream(sock.getInputStream());
            this.out = new BufferedOutputStream(sock.getOutputStream());

            this.clientId = IdGenerator.generateNextId();
            
            this.connections.connect(clientId, true, this);
            
            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    protocol.process(nextMessage, connections, clientId);
                }
            }
            System.out.println("[DEBUG]: after while");
                
        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        try {
            System.out.println("[DEBUG]: in send in cnnectionHandler");
            System.out.println("[DEBUG]: msg is: \n" + msg);
            out.write(encdec.encode(msg));
            out.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
