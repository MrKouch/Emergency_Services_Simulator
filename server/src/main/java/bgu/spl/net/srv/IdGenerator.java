package bgu.spl.net.srv;

public class IdGenerator {
    private static int ID = 0;
    
    public synchronized static int generateNextId() {
        ID += 1;
        return ID;
    }

}
