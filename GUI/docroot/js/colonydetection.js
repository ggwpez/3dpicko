/*
Zu tun:
hohe Prio:
	starte Kolonieerkennung.
	gebe Daten weiter.
niedrige Prio:
	automatisch die größe Anpassen
	passe Kreisgröße auf Durchschnitt von erkannten Kolonien an.
	ziehe umrandung für die echte clickArea und füge Punkte hinzu.
	erkenne Farbe und invertiere sie für die Kreise.
*/

/*
Infos:
Größe Petrischale:
127,5 x 85
1,5:1
*/

// Variable if colonies should be added or delted
var mode ='add';

var context, canvas;
// Variable containing the list of colonies
var colony_coords = [];

// Size of the petri dish
var widthMm = 127.5;
var heightMm = 85;

// Variable containing the list of colonies in mm
var coordinatesMm = {"colonies":[]};

// Make clickArea interactive
var container = document.querySelector("#clickArea");

// Correct canvas size
//resizeCanvas(drawArea);

// React to clicks insiede of drawArea
container.addEventListener("click", changePosition, false);

// Select size of the canvas
function resizeCanvas(canvas) {
   const width = canvas.clientWidth;
   const height = canvas.clientHeight;

   if (canvas.width !== width || canvas.height !== height) {
     canvas.width = width;
     canvas.height = height;
   }
}

// colony detection
function getPositions(){
    var image_id = chosen_image.id;
    console.log("colonies  from image " +image_id);
// lade Ergebnisse der Kolonieerkennung

    api('getpositions', { id: image_id });
}

function drawPositions(coords)
{
    canvas  = document.getElementById('drawArea'),
    context = canvas.getContext('2d');
    img = document.getElementById('photograph');

    canvas.style.position = "absolute";
    canvas.style.left = img.offsetLeft + "px";
    canvas.style.top = img.offsetTop + "px";
    canvas.style.width = img.width +"px";
    canvas.style.height = img.height +"px";

    img.width = img.width;
    img.height = img.height;

    // Import colonies from colony detection
    coords.coords.forEach((value) => {
        colony_coords.push({
            x: value[0] * img.width  /100,
            y: value[1] * img.height /100,
            r: value[2]
        });
    });
    printPositions();
}

// find exact position
function getPosition(event) {
    var xPosition = 0;
    var yPosition = 0;
    while (event) {
        if (event.tagName == "BODY") {
            var xScroll = event.scrollLeft || document.documentElement.scrollLeft;
            var yScroll = event.scrollTop || document.documentElement.scrollTop;

            xPosition += (event.offsetLeft - xScroll + event.clientLeft);
            yPosition += (event.offsetTop - yScroll + event.clientTop);
        } else {
            xPosition += (event.offsetLeft - event.scrollLeft + event.clientLeft);
            yPosition += (event.offsetTop - event.scrollTop + event.clientTop);
        }
        event = event.offsetParent;
    }
    return {
        x: xPosition,
        y: yPosition
    };
}

// add or delete colonies
function changePosition(event){
    var parentPosition = getPosition(event.currentTarget);
    var xPosition = event.clientX - parentPosition.x;
    var yPosition = event.clientY - parentPosition.y;		

    // Add colonies
    if (mode == 'add'){
        var colony = {"xCoordinate":xPosition,"yCoordinate":yPosition, "diameter":"10"};
        coordinatesPx.colonies.push(colony);		
    } 

    // Delete colonies
    if (mode == 'del'){
        for (i in coordinatesPx.colonies) {
            if (Math.abs(coordinatesPx.colonies[i].xCoordinate - xPosition) < coordinatesPx.colonies[i].diameter &&
                Math.abs(coordinatesPx.colonies[i].yCoordinate - yPosition) < coordinatesPx.colonies[i].diameter){
                delete coordinatesPx.colonies[i];
            }
        }
    }
    printPositions();
}

var balls = [];
// Print colonies
function printPositions(){
    // show coordinatesPx
    showCoordinates.innerHTML = JSON.stringify(colony_coords, null, 4);

    // Print new positions
    for (i in colony_coords)
    {
        var ball = new Circle({
            x: colony_coords[i].x,
            y: colony_coords[i].y,
            radius: colony_coords[i].r,
            linecolor: 'green',
            background: 'transparent'
        });

        ball.addEvent('mouseenter', function () {
            this.set('linecolor', 'red');
        });

        ball.addEvent('mouseleave', function () {
            this.set('linecolor', 'green');
        });
        balls.push(ball);
    }

    canvas.addEvent('mousemove', function (e)
    {
        var rect = canvas.getBoundingClientRect();
        var x = e.client.x -rect.left,
        y = e.client.y -rect.top;

        for (var i = 0; i < balls.length; ++i)
        {
            var ball = balls[i];

            if (ball.isMouseOver(x, y))
            {
                if (! ball.mouseover)
                {
                    ball.fireEvent('mouseenter');
                    ball.mouseover = true;
                }
            }
            else if (ball.mouseover)
            {
                ball.mouseover = false;
                ball.fireEvent('mouseleave');
            }
        }
    });
}