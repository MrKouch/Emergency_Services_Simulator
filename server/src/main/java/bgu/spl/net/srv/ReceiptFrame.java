package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class ReceiptFrame {

    public static Frame getReceiptFrame(String receiptId) {
        ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();
        headers.put("receipt-id", receiptId);
        return new Frame("RECEIPT", headers);
    }
}
