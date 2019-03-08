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

// Array of colonies. See ImageRecognition/include/Colony.hpp
var colonies = [];
var number_of_colonies = 0;
// Correct canvas size
//resizeCanvas(drawArea);

// React to clicks inside of drawArea

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
        if ($('#colony-layers').width() < img.width)
        {
            width = $('#colony-layers').width();
            down_scale = width /img.width;
            height = down_scale *img.height;
        }
        else
        {
            width = img.width;
            height = img.height;
            down_scale = 1;
        }

        // fix width
        // TODO enable resizing?
        $('#selection').css('min-width', width+$('#detection-settings-div').innerWidth());

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
    colonies = coords.colonies;

    printPositions();
}

var circles = [];
function removeUnselectedColonies()
{
    var j = 0;
    var good = 0;
    const max_good = parseInt(document.getElementById('number-max_number_of_coloniesstrategy-form').value);
    layer1.context.clearRect(0,0, layer1.canvas.width, layer1.canvas.height);
    console.log("Max good colonies: ", max_good);

    for (var i = 0; i < circles.length && good < max_good; ++i)
    {
        var ball = circles[i];

        if (ball.options.linecolor == 'white')
        {
            ball.draw();
            good++;
        }
    }
}

// Print colonies
var show_excluded = false;
function printPositions(){
    // show coordinatesPx
    for (ball in circles)
        delete ball;
    circles = [];

    layer1.context.clearRect(0,0, layer1.canvas.width, layer1.canvas.height);

    number_of_colonies = 0;
    // Print new positions
    colonies.forEach((colony) =>
    {
        var color = 'white';
        // TODO build color lookuptabl before for speedup
        if (colony.excluded_by != "")
        {
            if(!show_excluded) return;
            const found = algorithms["1"].settings.filter(s => (s.id == colony.excluded_by));

            if (found && found.length)
                color = found[0].color;
            else
                console.warn("Could not find setting ", colony.excluded_by);
        } else number_of_colonies++;

        var ball = new Circle({
            x: colony.x *layer1.canvas.width,
            y: colony.y *layer1.canvas.height,
            radius: colony.major_length *1.25 *down_scale,            // Make them a bit better to see
            linecolor: color,
            defaultLinecolor: color,
            background: 'transparent',
            canvas: layer1.canvas
        });

        /*ball.addEvent('mouseenter', function () {
            this.set('linecolor', 'red');
        });

        ball.addEvent('mouseleave', function () {
            this.set('linecolor', 'white');
        });*/

        circles.push(ball);
    });

    layer2.canvas.onmousemove =
    function (e)
    {
        var rect = layer2.canvas.getBoundingClientRect();
        var x = e.clientX -rect.left,
        y = e.clientY -rect.top;

        for (var i = 0; i < circles.length; ++i)
        {
            var ball = circles[i];

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
        for (var i = 0; i < circles.length; ++i)
        {
            var ball = circles[i];

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
    var ball = circles[circle_id];
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