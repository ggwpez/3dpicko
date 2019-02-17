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
var layer0 = {}, layer1 = {};
// Variable containing the list of colonies
var colony_coords = [];


// Variable containing the list of colonies in mm
var coordinatesMm = {"colonies":[]};


// Correct canvas size
//resizeCanvas(drawArea);

// React to clicks insiede of drawArea

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

var img;
function selectionTabEnter()
{
    layer0.canvas  = document.getElementById('layer0'),
    layer1.canvas  = document.getElementById('layer1'),
    layer0.context = layer0.canvas.getContext('2d');
    layer1.context = layer1.canvas.getContext('2d');

    img = new Image();
    img.onload = function ()
    {
        layer0.canvas.width  = img.width; // in pixels
        layer0.canvas.height = img.height; // in pixels

        layer1.canvas.width  = img.width; // in pixels
        layer1.canvas.height = img.height; // in pixels

        layer0.context.drawImage(img, 0, 0);
        // TODO only make the detection button ready when we are here
    }
    img.src = chosen_image.path;
}

function drawPositions(coords)
{
    colony_coords = coords.coords;

    printPositions();
}

var balls = [];
// Print colonies
function printPositions(){
    // show coordinatesPx
    for (ball in balls)
        delete ball;
    balls = [];

    layer1.context.clearRect(0,0, layer1.canvas.width, layer1.canvas.height);
    layer1.context.font = "30px Arial";
    layer1.context.fillStyle = "red";
    layer1.context.textAlign = "left";
    layer1.context.fillText(colony_coords.length,50,50);

    // Print new positions
    colony_coords.forEach((colony) =>
    {
        var ball = new Circle({
            x: colony[0] *layer1.canvas.width,
            y: colony[1] *layer1.canvas.height,
            radius: colony[2],
            linecolor: 'green',
            background: 'transparent',
            canvas: layer1.canvas
        });

        ball.addEvent('mouseenter', function () {
            this.set('linecolor', 'red');
        });

        ball.addEvent('mouseleave', function () {
            this.set('linecolor', 'green');
        });
        balls.push(ball);
    });

    layer1.canvas.onmousemove =
        function (e)
        {
            var rect = layer1.canvas.getBoundingClientRect();
            var x = e.clientX -rect.left,
            y = e.clientY -rect.top;

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
        };
}