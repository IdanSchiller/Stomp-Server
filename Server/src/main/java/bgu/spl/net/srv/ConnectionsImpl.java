package bgu.spl.net.srv;

import java.io.IOException;
import java.security.KeyPair;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl <T>implements Connections<T> {

    private Map<Integer, ConnectionHandler> clients_ConsHandMap;
    private Map<String,Client> clientsMap;
    private Map<String, List<Integer>> topics_subsMap;
    private Map<Integer,Map<String,Integer>> connId_topic_subId_map;
    private static ConnectionsImpl single_instance=null;

    private ConnectionsImpl(){
        clients_ConsHandMap=new ConcurrentHashMap<>();
        clientsMap = new ConcurrentHashMap<>();
        topics_subsMap = new ConcurrentHashMap<>();
        connId_topic_subId_map = new ConcurrentHashMap<>();
    }

    public static ConnectionsImpl getInstance(){
        if (single_instance==null)
            single_instance = new ConnectionsImpl();
        return single_instance;
    }




    @Override
    /** uses the ConnectionHandler's send methods of this {@param} connectoinId to flush() the {@param} msg  */
    public boolean send(int connectionId, T msg) throws IOException {
        clients_ConsHandMap.get(connectionId).send(msg);
        return false;
    }

    @Override
    /**
     * sends the {@param} msg to every subId that is subscribes to the {@param} channel. * */
    public void send(String channel, T msg) throws IOException {
        if(topics_subsMap.get(channel)!=null) {
            for (int connId : topics_subsMap.get(channel)) {
                String[] stringMsg = ((String) msg).split("\n");
                if (stringMsg[0].equals("MESSAGE")) {
                    String msg_with_subsId = "";
                    String subsId = connId_topic_subId_map.get(connId).get(channel).toString(); // gets the subscripId of this connId for this topic/channel
                    stringMsg[1] = "subscription:" + subsId; // replaces it
                    for (String s : stringMsg)
                        msg_with_subsId += s+"\n"; //create the new message with the subscripId;
                    msg_with_subsId=msg_with_subsId.substring(0,msg_with_subsId.length()-1)+"\u0000";
                    send(connId, (T) msg_with_subsId);
                } else
                    send(connId, msg);
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {

    }

    public Map<Integer,ConnectionHandler> getClients_ConsHandMap() {
        return clients_ConsHandMap;
    }




    public Map<String, Client> getClientsMap() {
        return clientsMap;
    }

//    public void setClientsMap(Map<Integer, Client> clientsMap) {
//        this.clientsMap = clientsMap;
//    }


    public Map<String, List<Integer>> getTopics_subsMap() {
        return topics_subsMap;
    }

    public Map<Integer, Map<String, Integer>> getConnId_topic_subId_map() {
        return connId_topic_subId_map;
    }

}

