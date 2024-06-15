var  linear_speed = 0;
     angular_speed = 0,
     diffty_IP = '',
     lightsOn = false,
     scanCycle = 100;
createJoystick = function () {
    var options = {
      zone: document.getElementById('zone_joystick'),
      threshold: 0.1,
      position: {left: '50%', top: '70%'},
      mode: 'static',
      size: 130,
      color: '#3452eb',
    };
    manager = nipplejs.create(options);

     linear_speed = 0;
     angular_speed = 0;

    self.manager.on('start', function (event, nipple) {
      console.log("Movement start");
    });

    manager.on('move', function (event, nipple) {
        max_linear = 4.0; // m/s
        max_angular = 2; // rad/s
        max_distance = 80.0; // pixels;
        linear_speed = Math.floor( Math.sin(nipple.angle.radian) * max_linear * nipple.distance/max_distance);
        angular_speed = Math.floor(-Math.cos(nipple.angle.radian) * max_angular * nipple.distance/max_distance);
        console.log("linear = ", linear_speed);
        console.log("angular = ",angular_speed);
      });

    self.manager.on('end', function () {
      console.log("Movement end");
      linear_speed=0.0 ;
      angular_speed=0.0;
        });
  }

  window.onload = function () {
    createJoystick();
  }
  function printLog (log){
      log = '<li class="list-group-item">'+log+'</li>' ;
      $('#logList').append(log);
  }

  /*handing the ros connection*/
  /********************************************************** */
  var ros = new ROSLIB.Ros({
  });

 var move_pup = new ROSLIB.Topic({
     ros:ros,
     name:'/diffty/cmd_vel',
     messageType :'geometry_msgs/Twist',
 });
 var light_pup = new ROSLIB.Topic({
    ros:ros,
    name:'/diffty/light_cmd',
    messageType :'std_msgs/Bool',
});

 
 function rosSend (){
    var twist = new ROSLIB.Message({
        linear : {
            x : linear_speed 
        },
        angular: {
            z : angular_speed
        }
     });
     var light_cmd = new ROSLIB.Message({
         data : lightsOn
     });

     move_pup.publish(twist);
     light_pup.publish(light_cmd);
    
 }


  ros.on('connection', function() {
    console.log('Connected to websocket server.');
    printLog("connected to Diffty :D");
    move_pup.advertise();
    light_pup.advertise();
    var  scanCycle_id = setInterval(rosSend,scanCycle);
    //changing the src image to stream 
    var imgURL = 'http://'+diffty_IP+':8080/stream?topic=/diffty_front_camera/image_raw' ;
    $("#streamBox").attr('src',imgURL );
  });

  ros.on('error', function(error) {
    console.log('Error connecting to websocket server: ', error);
    printLog('Connection failed ..open console fore more');
  });

  ros.on('close', function() {
    console.log('Connection to websocket server closed.');
    printLog('Diffty says bye :(');
    clearInterval(scanCycle_id);
    $("#streamBox").attr('src','/science_Spot.jpg');
  });


  //setting up evet listerers to key presses
  // W -- A --S -- D 
  $('body').on('keydown keyup', function (event) {
      if(event.type === 'keydown'){
        var key = event.which ;
        if (key === 87){ // w was pressed 
            $('#w').addClass("btn-success");
            linear_speed = 1.0 ;
        } 
         if (key === 65){ // A was pressed 
            $('#a').addClass("btn-success");
            angular_speed = 1.0 ;
        }
         if (key === 83){ // s was pressed 
            $('#s').addClass("btn-success");
            linear_speed = -1.0 ;
        }
         if (key === 68){ // D was pressed 
            $('#d').addClass("btn-success");
            angular_speed = -1.0 ;
        }
      }
      else{ //when key is realsed or the keyup event happens
          $('.btn').removeClass("btn-success");
          linear_speed = 0.0 ;
          angular_speed = 0.0 ;
      }
  });
  //connect and disconnect orders
 $('#connect').on('click', function (eve) { 
     diffty_IP = $('#ipAdd').val();
     var webSocketURL = 'ws://'+diffty_IP+':9090' ;
     console.log('web socket url :' , webSocketURL);
     ros.connect(webSocketURL);
     
  })
  $('#disconnect').on('click', function () {
      ros.close();
  });
  $('#light_btn').on('click', function () {
    if(lightsOn) lightsOn = false ;
    else lightsOn = true ;
});
  
