/*
Zu tun:
hohe Prio:
	Bild verschieben
	Bild drehen
	Bild zuschneiden
niedrige Prio:
*/

/*
Infos:
Größe Petrischale:
127,5 x 85
1,5:1
*/

// Position left top and width and hight
var x;
var y;
var w;
var h;
var angle;


var jcrop_api;
jQuery(function($){
    $('#photograph').Jcrop({
        aspectRatio: 1.5 / 1,
        onSelect: saveCoords
        
    },function(){
        jcrop_api = this;
    });
});

function saveCoords(c){
    x = c.x;
    y = c.y;
    w = c.w;
    h = c.h;
    showCoords2();
}

function showCoords2() {
    $('#x').val(x);
    $('#y').val(y);
    $('#w').val(w);
    $('#h').val(h);
}

var angle = 0;
//function test(){
jQuery(function($){
    $('#left').on('click', function () {
    angle -= 90;
    $("test").rotate(angle);
});
});
