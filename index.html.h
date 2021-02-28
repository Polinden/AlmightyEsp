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
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.1/css/all.min.css" rel="stylesheet"/>
    <link href="https://fonts.googleapis.com/css?family=Roboto:300,400,500,700&display=swap" rel="stylesheet"/>
    <link href="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/3.3.0/mdb.min.css" rel="stylesheet"/>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-timepicker/0.5.2/css/bootstrap-timepicker.min.css" integrity="sha512-/Ae8qSd9X8ajHk6Zty0m8yfnKJPlelk42HTJjOHDWs1Tjr41RfsSkceZ/8yyJGLkxALGMIYd5L2oGemy/x1PLg==" crossorigin="anonymous" />
    <script src="https://code.jquery.com/jquery-3.5.1.min.js" integrity="sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-timepicker/0.5.2/js/bootstrap-timepicker.min.js" integrity="sha512-2xXe2z/uA+2SyT/sTSt9Uq4jDKsT0lV4evd3eoE/oxKih8DSAsOF6LUb+ncafMJPAimWAXdu9W+yMXGrCVOzQA==" crossorigin="anonymous"></script>
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
  .bootstrap-timepicker-widget table td input {
    font-size: 70%;
    width: 35px;
  }
  .fas {
    margin-right: 10px;
    margin-left: 2px;
    font-size: 110%;  
  }
  </style>
</head>
<body>
<h2>ESP Relay</h2>

<table class="table table-bordered">
        <tbody>
        <tr>
            <th scope="row" id="data-1">Relay 1</th>
            <td>
                <div class="btn-group-lg" role="group" aria-label="Relay 1">
                    <button type="button" class="btn btn-success" id="on-1" onclick="setRelay(1, true);">On</button>
                    <button type="button" class="btn btn-danger" id="off-1" onclick="setRelay(1, false);">Off
                    </button>
                </div>
            </td>
        </tr>

        <tr>
            <th scope="row">R1 Start</th>
            <td>
            <div class="input-group bootstrap-timepicker timepicker">
            <input id="timepicker1" type="text" class="form-control input-small">
            <span class="input-group-addon"><i class="fas fa-clock"></i></span>
                    <button type="button" class="btn btn-secondary" id="send-1" onclick="sendTime(1, 1, 'start')"">  OK </button>
                    <button type="button" class="btn btn-light" id="clear-1" onclick="sendClear(1,'start')"> No </button>
            </div>
            </td>
        </tr>
        <tr>
            <th scope="row">R1 Finish</th>
            <td>
            <div class="input-group bootstrap-timepicker timepicker">
            <input id="timepicker2" type="text" class="form-control input-small">
            <span class="input-group-addon"><i  class="fas fa-clock"></i></span>
                    <button type="button" class="btn btn-secondary" id="send-2" onclick="sendTime(2, 1, 'stop')""> OK </button>
                    <button type="button" class="btn btn-light" id="clear-2" onclick="sendClear(1,'stop')"> No </button>
            </div>
            </td>
        </tr>

        <tr>
            <th scope="row" id="data-2">Relay 2</th>
            <td>
                <div class="btn-group-lg" role="group" aria-label="Relay 2">
                    <button type="button" class="btn btn-success" id="on-2" onclick="setRelay(2, true);">On</button>
                    <button type="button" class="btn btn-danger" id="off-2" onclick="setRelay(2, false);">Off
                    </button>
                </div>
            </td>

        </tr>
        <tr>
            <th scope="row">R2 Start</th>
            <td>
            <div class="input-group bootstrap-timepicker timepicker">
            <input id="timepicker3" type="text" class="form-control input-small">
            <span class="input-group-addon"><i class="fas fa-clock"></i></span>
                    <button type="button" class="btn btn-secondary" id="send-3" onclick="sendTime(3, 2, 'start')"> OK </button>
                    <button type="button" class="btn btn-light" id="clear-3" onclick="sendClear(2,'start')"> No </button>
            </div>
            </td>
        </tr>
        <tr>
            <th scope="row">R2 Finish</th>
            <td>
            <div class="input-group bootstrap-timepicker timepicker">
            <input id="timepicker4" type="text" class="form-control input-small">
            <span class="input-group-addon"><i class="fas fa-clock"></i></span>
                    <button type="button" class="btn btn-secondary" id="send-4" onclick="sendTime(4, 2, 'stop')"> OK </button>
                    <button type="button" class="btn btn-light" id="clear-4" onclick="sendClear(2,'stop')"> No </button>
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
 
<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/3.3.0/mdb.min.js"></script>
<script type="text/javascript">
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

    function parseData(data){
    //case 1        
    if (data.startsWith("time")){
          i=data.indexOf(":");
          e=data.substring(i+3);
          document.getElementById("time").innerHTML=e;
        };
      //case 2  
     if (data.startsWith("status")){
          i=data.indexOf(":");
          e=data.substring(i+1);
          console.log(e);
          var y=JSON.parse(e)["relay"];
          for (var i=0; i<y.length; i++){
              s="data-"+(i+1).toString();
              if (y[i]) document.getElementById(s).innerHTML="Relay-"+(1+i).toString()+" On";
              else document.getElementById(s).innerHTML="Relay-"+(1+i).toString()+" Off";
           } 
           var q=JSON.parse(e)["timer"];
           for (var i=0; i<q.length; i++){
              if ((q[i].start_h)<100) {
                r3="AM";
                r1=q[i].start_h;
                if (r1>12) {r1-=12; r3="PM";}
                $('#timepicker'+(i*2+1).toString()).timepicker('setTime', r1+':'+q[i].start_m+r3);
                document.querySelector('#send-'+(i*2+1).toString()).disabled=true;
                document.querySelector('#send-'+(i*2+1).toString()).classList.remove('btn-secondary');
              }
              else{
                document.querySelector('#send-'+(i*2+1).toString()).disabled=false;
                document.querySelector('#send-'+(i*2+1).toString()).classList.add('btn-secondary');
              }
              if ((q[i].stop_h)<100) {
                r3="AM";
                r1=q[i].stop_h;
                if (r1>12) {r1-=12; r3="PM";}
                $('#timepicker'+(i*2+2).toString()).timepicker('setTime', r1+':'+q[i].stop_m+r3);
                document.querySelector('#send-'+(i*2+2).toString()).disabled=true;   
                document. querySelector('#send-'+(i*2+2).toString()).classList.remove('btn-secondary');   
              }
              else {
                document.querySelector('#send-'+(i*2+2).toString()).disabled=false;
                document.querySelector('#send-'+(i*2+2).toString()).classList.add('btn-secondary'); 
              }  
             }      
         };
     //case 3      
     if (data.startsWith("test")){
          i=data.indexOf(":");
          e=data.substring(i+1);
         console.log(e);
        };
    }   


    function setRelay(i, s)
    {
         var xmlHttp = new XMLHttpRequest();
         xmlHttp.open( 'GET', '/Relay?number='+i.toString()+'&on='+s.toString(), true );
         xmlHttp.send( null );
    }
    function sendTime(i, j, t)
    {    var s=document.getElementById("timepicker"+i.toString()).value;
         console.log(s);
         var xmlHttp = new XMLHttpRequest();
         xmlHttp.open( 'GET', '/Timer?number='+(j).toString()+'&time='+s+'&st='+t, true );
         xmlHttp.send( null );

    }
    function sendClear(i,t)
    {    var xmlHttp = new XMLHttpRequest();
         xmlHttp.open( 'GET', '/Timer?number='+(i).toString()+'&time=88:00%20PM&st='+t, true );
         xmlHttp.send( null );

    }

    for (var i=1; i<5; i++){        
    $('#timepicker'+i.toString()).timepicker({
                minuteStep: 5,
                showInputs: false,
          disableFocus: true, 
    icons:{
                   up: 'fas fa-angle-up',
                   down: 'fas fa-angle-down'
               }

             }); 
    };
    connect();
</script>

</body>
</html>



)===";
}
