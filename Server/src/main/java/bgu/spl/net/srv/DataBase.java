package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class DataBase{
     private Map<String, List<String>> topicsSubsMap;
     private DataBase instance;

     public DataBase(){
          topicsSubsMap= new HashMap<>();
          instance=this;
     }

     public DataBase getInstance(){
          return instance;
     }


}




