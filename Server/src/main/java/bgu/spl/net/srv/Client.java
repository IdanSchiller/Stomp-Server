package bgu.spl.net.srv;

public class Client {

    private String name;
    private int connectionId;
    private boolean loggedIn;
    private String password;

    public Client(String name, String password, int connectionId) {
        this.name = name;
        this.password = password;
        this.connectionId = connectionId;
        this.loggedIn = false;
    }

    public boolean isLoggedIn(){
        return loggedIn;
    }

    public void setLoggedIn(boolean loggedIn){
        this.loggedIn=loggedIn;
    }

    public String getName() {
        return name;
    }

    public String getPassword() {
        return password;
    }

    public int getConnectionId() {
        return connectionId;
    }


}
