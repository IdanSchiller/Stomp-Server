package bgu.spl.net.srv;

public class ReplyMessage {

    private String topic;
    private String msg;

    public ReplyMessage(){}

    public ReplyMessage(String topic, String msg) {
        this.topic = topic;
        this.msg = msg;
    }

    public void setTopic(String topic) {
        this.topic = topic;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }


    public String getTopic() {
        return topic;
    }

    public String getMsg() {
        return msg;
    }






}
