#pragma once
#include "pagehelper.h"    
namespace index_html
{
    CreatePageName; 
    HTMLPAGE = R"===(
<!DOCTYPE HTML>
<html lang = "en">
  <head>
    <title>Pattern</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> 
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css" integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2" crossorigin="anonymous">
<style>
body { background: gray;}
h2 {
  color: white; 
  padding: 1% 1%;
}  
@media (max-width : 480px) {
    mobile_fix {
        padding:0;
    }
}

#time
{
  margin-top:5px;
  font-size:25px;
  color:silver;
  border:2px dashed #2E9AFE;
  padding:5px;
  width:160px;
  margin-left:5px;
}

</style>
  </head>
  <body>
    <h2>ESP Relay</h2>

<table class="table table-bordered">
        <tbody>
        <tr>
            <th scope="row">Relay 1</th>
            <td>
                <div class="btn-group-lg" role="group" aria-label="Relay 1">
                    <button type="button" class="btn btn-success" id="on-1" onclick="setRelay(1, true);">On</button>
                    <button type="button" class="btn btn-danger" id="off-1" onclick="setRelay(1, false);">Off
                    </button>
                    <button type="button" class="btn btn-secondary" id="data-1">
                        Is Off
                    </button>
                </div>
            </td>
        </tr>
        <tr>
            <th scope="row">Relay 2</th>
            <td>
                <div class="btn-group-lg" role="group" aria-label="Relay 2">
                    <button type="button" class="btn btn-success" id="on-2" onclick="setRelay(2, true);">On</button>
                    <button type="button" class="btn btn-danger" id="off-2" onclick="setRelay(2, false);">Off
                    </button>
                    <button type="button" class="btn btn-secondary" id="data-2">
                        Is Off
                    </button>
                </div>
            </td>

        </tr>
        <tr style="visibility:collapse">
            <th scope="row">Relay 3</th>
            <td>
                <div class="btn-group-lg" role="group" aria-label="Relay 3">
                    <button type="button" class="btn btn-success" id="on-3" onclick="setRelay(3, true);">On</button>
                    <button type="button" class="btn btn-danger" id="off-3" onclick="setRelay(3, false);">Off
                    </button>
                    <button type="button" class="btn btn-secondary" id="data-3">
                        Is Off
                    </button>
                </div>
            </td>
        </tr>
        <tr style="visibility:collapse">
            <th scope="row">Relay 4</th>
            <td>
                <div class="btn-group-lg" role="group" aria-label="Relay 4">
                    <button type="button" class="btn btn-success" id="on-4" onclick="setRelay(4, true);">On</button>
                    <button type="button" class="btn btn-danger" id="off-4" onclick="setRelay(4, false);">Off
                    </button>
                    <button type="button" class="btn btn-secondary" id="data-4">
                        Is Off
                    </button>
                </div>
            </td>
        </tr>
        <tr>
            <th scope="row">Time</th>
            <td>
                <p id="time"></p>
            </td>
        </tr>
      </tbody>
    </table>
    </div>

  <script>
  function connect() {
    let socket = new WebSocket('ws://' + window.location.hostname + '/ws');
    socket.onopen = function(e) {
   console.log("[open] Connection established");
         //socket.send("My name is John");
    };

    socket.onmessage = function(event) {
         parseData(event.data);
    };
     socket.onclose = function(event) {
    if (event.wasClean) {
      console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
    } else {
      console.log('[close] Connection died');
    }
    setTimeout(function() {
            connect();
          }, 1000);
        };

     socket.onerror = function(error) {
    console.log(`[error] ${error.message}`);
       };
  }   
  connect();

  
    function parseData(data){
    //case 1        
    if (data.startsWith("time")){
          i=data.indexOf(":");
          e=data.substring(i+3);
          document.getElementById("time").innerHTML=e;
        };
    //case 2      
    if (data.startsWith("data")){
          i=data.indexOf(":");
          e=data.substring(i+1);
         console.log(e);
        };
     //case 2     
     if (data.startsWith("status")){
          i=data.indexOf(":");
          e=data.substring(i+1);
          console.log(e);
         var y=JSON.parse(e)["relay"];
         for (var i=0; i<4; i++){
              s="data-"+(i+1).toString();
              if (y[i]) document.getElementById(s).innerHTML="Is On";
              else document.getElementById(s).innerHTML="Is Off";
         } 
     };
    }   


    //window.onload = setInterval(clock,1000);

    function clock()
    {
    var d = new Date();
    var hour =d.getHours();
          var min = d.getMinutes();
    var sec = d.getSeconds();
    document.getElementById("time").innerHTML=hour+":"+min+":"+sec;
    }

    function setRelay(i, s)
    {
         var xmlHttp = new XMLHttpRequest();
         xmlHttp.open( 'GET', '/Relay?number='+i.toString()+'&on='+s.toString(), true );
         xmlHttp.send( null );
    }


  </script>
  
  </body>
</html>

    )===";
}
