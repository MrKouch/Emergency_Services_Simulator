package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Not enough arguments...");
            System.exit(1);
        }
        else {
            System.out.println("args: " + args[0] + " " + args[1]);
        }

        int port = Integer.parseInt(args[0].substring(1, args[0].length() - 1));
        System.out.println("port: " + port);
        String serverType = args[1];

        if (serverType.equals("tpc")) {
            Server.threadPerClient(
                    port, //port
                    () -> new StompMessagingProtocolImpl(), //protocol factory
                    StompEncDec::new //message encoder decoder factory
            ).serve();
        }
        else if (serverType.equals("reactor")) {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port, //port
                    () -> new StompMessagingProtocolImpl(), //protocol factory
                    StompEncDec::new //message encoder decoder factory
            ).serve();
        }

    }
}
