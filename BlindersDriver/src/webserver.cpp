#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "webServer.h"

#include "blindersAPI.h"


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>Blinders x 2</title>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <script type="text/javascript"  src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js" ></script>

        <style>
html, body {
  height: 99%;
  overflow: hidden; }

body {
  background-color: #3a3d55;
  display: flex;
  align-items: center;
  justify-content: center; }

.button-background {
  position: relative;
  background-color: rgba(255, 255, 255, 0.3);
  width: 80px;
  height: 400px;
  border: white;
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center; }

.swipe {
  margin-left: 2em;
  margin-right: 2em; }

.swipe-guide {
  width: 32px;
  height: 100%;
  border-radius: 16px;
  position: absolute;
  left: 24px;
  background-color: #00A030; }

.slider {
  transition: width 0.3s, border-radius 0.3s, height 0.3s;
  position: absolute;
  top: -10px;
  background-color: white;
  width: 100px;
  height: 100px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center; }
  .slider.unlocked {
    transition: all 0.3s;
    width: inherit;
    top: 0 !important;
    height: inherit;
    border-radius: inherit; }

.blinderslider {
  display: flex;
  transition: width 0.3s, border-radius 0.3s, height 0.3s;
  position: absolute;
  background-color: white;
  width: 100%;
  height: 32px;
  border-radius: 8px;
  align-items: center;
  justify-content: center; }
  .blinderslider.unlocked {
    transition: all 0.3s;
    width: inherit;
    top: 0 !important;
    height: inherit;
    border-radius: inherit; }

.material-icons {
  color: black;
  font-size: 50px;
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
  cursor: default; }

.slide-text {
  color: #3a3d55;
  font-size: 24px;
  text-transform: uppercase;
  font-family: 'Roboto', sans-serif;
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
  cursor: default; }

.bottom {
  position: fixed;
  bottom: 0;
  font-size: 14px;
  color: white; }
  .bottom a {
    color: white; }
            
        </style>
        
        <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Roboto">        
        <link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">        

        <script type="text/javascript" >
function initBlinderSlider(sliderRoot, sliderUrl) {
    
    var slider = $( " div[data-id='slider']" , sliderRoot);

    var height = sliderRoot.height();
    
    var initialMouse = 0;
    var slideMovementTotal = 0;
    var mouseIsDown = false;

    var handlers = {
        horizontal: {
            total : function(event, src)  {
                slideMovementTotal = $('div[data-id="button-background"]', sliderRoot).width() - src.width() + 10;
                initialMouse = event.clientX || event.originalEvent.touches[0].pageX;
            },
            currentMouse : function(event) { return event.clientX || event.changedTouches[0].pageX; },
            animateSliderReturn : function(slider) {  slider.animate({left: (height/2-10)+"px"}, 300);},
            positionSlider : function(slider, pos) { slider.css({ 'left': pos + 'px' });}
        },
        vertical : {
            total: function(event , src) {
            slideMovementTotal = $('div[data-id="button-background"]', sliderRoot).height() - src.height() + 10;
            initialMouse = event.clientY || event.originalEvent.touches[0].pageY;
            },
            currentMouse : function(event) { return event.clientY || event.changedTouches[0].pageY; },
            animateSliderReturn : function(slider) {  slider.animate({top: (height/2-10)+"px"}, 300);},
            positionSlider : function(slider, pos) { slider.css({ 'top': (pos+height/2) + 'px' });}
        }
        
    };

    var statusFireInterval = null;
    var status;
    var currentStatusAjax = null;
    
    
    function fireStatus()
    {
        if ( currentStatusAjax )
            return;
        
        currentStatusAjax = $.ajax( sliderUrl + status, {
            accepts: "text/plain",
            method: "PUT"
        }).done( function(data, textStatus, jqXHR) {
            console.log("DONE: " + sliderUrl + status);
        }).fail(function( jqXHR, textStatus, errorThrown) {
            console.log("FAIL: " + sliderUrl + status);
        }).always( function( data_jqXHR, textStatus, jqXHR_error) {
            currentStatusAjax = null;
        });
    }
    
    var handler = handlers.vertical;
    
    slider.on('mousedown touchstart', function(event){
            mouseIsDown = true;
            handler.total(event, $(this) );
            
            status = "/stop";
            if ( statusFireInterval)
                clearInterval(statusFireInterval);
            statusFireInterval = setInterval( fireStatus, 50);
    });

    $(document.body).on('mouseup touchend', function (event) {
            if (!mouseIsDown)
                    return;
                
            mouseIsDown = false;
            
            clearInterval(statusFireInterval);
            
            status = "/stop";
            fireStatus();
            
            var currentMouse = handler.currentMouse(event); // event.clientX || event.changedTouches[0].pageX;
            var relativeMouse = currentMouse - initialMouse;

            if (relativeMouse < slideMovementTotal) {
                    handler.animateSliderReturn(slider);
                    return;
            }
            slider.addClass('unlocked');
            $('i[data-id="locker"]', sliderRoot).text('lock_outline');
            setTimeout(function(){
                    slider.on('click tap', function(event){
                            if (!slider.hasClass('unlocked'))
                                    return;
                            slider.removeClass('unlocked');
                            $('i[data-id="locker"]', sliderRoot).text('lock_open');
                            slider.off('click tap');
                    });
            }, 0);
    });

    $(document.body).on('mousemove touchmove', function(event){
            if (!mouseIsDown)
                    return;
            var currentMouse = handler.currentMouse(event) ;     // )event.clientX || event.originalEvent.touches[0].pageX;
            var relativeMouse = currentMouse - initialMouse;
            var slidePercent = 1 - (relativeMouse / slideMovementTotal);

            if (relativeMouse > height/2-16) {
                status = "/down";
                handler.positionSlider(slider, height/2-32);
            } else if (relativeMouse < 16-height/2 ) {
                status = "/up";
                handler.positionSlider(slider, -height/2);
            } else {
                status = relativeMouse > 0 ? "/down" : (relativeMouse < 0 ? "/up" : "/stop");
                handler.positionSlider(slider, relativeMouse-16);            
            }
    });
}
        </script>
    </head>
    <body onload="initBlinderSlider($('#blinderA'), '/blinders/A');initBlinderSlider($('#blinderB'), '/blinders/B')">
        <div id="blinderA"  class="swipe">
            <div class="button-background" data-id="button-background">
                    <div data-id="slider" class="blinderslider">
                    </div>
            </div>            
        </div>
        <div id="blinderB"  class="swipe">
            <div class="button-background" data-id="button-background">
                    <div data-id="slider" class="blinderslider">
                    </div>
            </div>            
        </div>
    </body>
</html>
)rawliteral";


String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  Serial.print("Resolving variable : ");
  Serial.println(var.c_str());
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Blinder UP</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"blinders('up')\" id=\"5\" " + outputState(5) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Blinder DOWN</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"blinders('down')\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Blinder STOP</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"blinders('stop')\" id=\"3\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}


void setupRelay(AsyncWebServer& server, sk::softar::iot::Blinders* blinders, std::string path)
{
  server.on( (path + "/up").c_str(), HTTP_PUT, [blinders] (AsyncWebServerRequest *request) {
    Serial.println(request->url().c_str());
    blinders->blindersUp();
    request->send(200, "text/plain", "OK");
  });

  server.on((path + "/down").c_str(), HTTP_PUT, [blinders] (AsyncWebServerRequest *request) {
    Serial.println(request->url().c_str());
    blinders->blindersDown();
    request->send(200, "text/plain", "OK");
  });

  server.on((path + "/stop").c_str(), HTTP_PUT, [blinders] (AsyncWebServerRequest *request) {
    Serial.println(request->url().c_str());
    blinders->blindersStop();
    request->send(200, "text/plain", "OK");
  });
}


void setupWebServer(AsyncWebServer& server, std::map<const char*, sk::softar::iot::Blinders*, sk::softar::iot::cmp_str>& blinders) {
// Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  for ( auto bp = blinders.begin(); bp != blinders.end(); bp++ )
    setupRelay(server, bp->second, bp->first);

  // Start server
  server.begin();    
}
