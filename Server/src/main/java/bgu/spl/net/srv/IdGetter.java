package bgu.spl.net.srv;

import java.util.List;

public class IdGetter {

    private List<Integer> connectionsId;
    private List<Integer> messagesId;
    private int messageIdCounter;
    private int connectionIdCounter;
    private static IdGetter single_instance = null;


    public static IdGetter getInstance(){
        if (single_instance==null){
            single_instance = new IdGetter();
        }
        return single_instance;

    }


    private IdGetter(){
        messageIdCounter=0;
        connectionIdCounter=0;
    }


    public int getMsgId(){
        int tmp  = messageIdCounter;
        messageIdCounter++;
        return tmp;
    }

    public int getconnectionId(){
        int tmp  = connectionIdCounter;
        connectionIdCounter++;
        return tmp;
    }

}
