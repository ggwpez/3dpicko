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
//  Image        Colonies     Tooltips
var layer0 = {}, layer1 = {}, layer2 = { };
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
var down_scale;
function selectionTabEnter()
{
    layer0.canvas  = document.getElementById('layer0');
    layer1.canvas  = document.getElementById('layer1');
    layer2.canvas  = document.getElementById('layer2');
    layer0.context = layer0.canvas.getContext('2d');
    layer1.context = layer1.canvas.getContext('2d');
    layer2.context = layer2.canvas.getContext('2d');

    img = new Image();
    img.onload = function ()
    {
        // TODO only downscale, would upscale on 4k screens

        var height, width;
        if ($(window).width() < img.width)
        {
            width = $(window).width() *.75;
            down_scale = width /img.width;
            height = down_scale *img.height;
        }
        else
        {
            width = img.width;
            height = img.height;
            down_scale = 1;
        }

        console.log("Downscale factor: ", down_scale);

        layer0.canvas.width  = width; // in pixels
        layer0.canvas.height = height; // in pixels

        layer1.canvas.width  = width; // in pixels
        layer1.canvas.height = height; // in pixels

        layer2.canvas.width  = width; // in pixels
        layer2.canvas.height = height; // in pixels

        layer0.context.drawImage(img, 0, 0, img.width, img.height, 0, 0, width, height);
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
            radius: colony[2] *1.25 *down_scale,            // Make them a bit better to see
            linecolor: 'white',
            background: 'transparent',
            canvas: layer1.canvas
        });

        /*ball.addEvent('mouseenter', function () {
            this.set('linecolor', 'red');
        });

        ball.addEvent('mouseleave', function () {
            this.set('linecolor', 'white');
        });*/

        balls.push(ball);
    });

    layer2.canvas.onmousemove =
        function (e)
        {
            var rect = layer2.canvas.getBoundingClientRect();
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

                        drawTooltip(i);
                    }
                }
                else if (ball.mouseover)
                {
                    ball.mouseover = false;
                    ball.fireEvent('mouseleave');

                    layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
                }
            }
        };

    layer2.canvas.onmousedown = (e) =>
    {
        for (var i = 0; i < balls.length; ++i)
        {
            var ball = balls[i];

            if (ball.mouseover && !ball.mousedown)  // superfluous
            {
                ball.fireEvent('toggle-select');
                ball.mousedown = true;  // TODO issue #4
                break;
            }
            else if (ball.mouseover && ball.mousedown)   // a bit superflous
            {
                const pos = ball.getPosition();
                const rad = ball.options.radius *Math.sqrt(2);

                layer1.context.clearRect(pos.x -rad/2, pos.y -rad/2, rad,rad);
                ball.fireEvent('toggle-select');
                ball.mousedown = false;
                // break; // TODO issue #4
            }
        }
    };
}

function drawTooltip(circle_id)
{
    var ball = balls[circle_id];
    const ball_pos = ball.getPosition();
    const tooltip_pos = {x:ball_pos.x, y:ball_pos.y +ball.getRadius() *1.5};


    layer2.context.fillStyle = "black";
    // FIXME relative tooltip size
    layer2.context.fillRect(tooltip_pos.x, tooltip_pos.y, 100, 60);  // TODO check that it works for colonies on the edge of the image

    layer2.context.fillStyle = "white";
    layer2.context.font = "12px sans-serif";
    layer2.context.textBaseline = "top";
    layer2.context.wrapText("Colony #" +circle_id +"\nRadius " +(Math.round(ball.getRadius() *100) /100), tooltip_pos.x +10, tooltip_pos.y +10, 80, 16);
    console.log("Drawing tooltip #", circle_id, " at ", tooltip_pos.x +20, "/", tooltip_pos.y +10);
}