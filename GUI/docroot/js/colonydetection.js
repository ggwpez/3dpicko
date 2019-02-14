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
    pickstrategy_context = layer1.context;
    
    coords.coords.forEach((value) => {
        colony_coords.push({
            x: value[0] * img.width,
            y: value[1] * img.height,
            r: value[2]
        });
    });

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
    }

    layer1.canvas.addEvent('mousemove', function (e)
    {
        var rect = layer1.canvas.getBoundingClientRect();
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