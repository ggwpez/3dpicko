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
var colony_array = [];
var number_of_colonies = 0;

// React to clicks inside of drawArea

// fit size of canvas to window (max: image-size)
function resizeCanvas(img) {
    let width = $('#selection').width() - document.getElementById('detection-settings-div').offsetWidth - 20;
    let height =  window.innerHeight - document.getElementById('layer_parent').getBoundingClientRect().top - 55;
    if(width < img.width || height < img.height){
        down_scale = Math.min(width/img.width, height/img.height);
        width = down_scale*img.width;
        height = down_scale*img.height;
    }
    else{
        width = img.width;
        height = img.height;
        down_scale = 1;
    }

    // fix width
    // TODO enable resizing?
    $('#selection').css('min-width', width + document.getElementById('detection-settings-div').offsetWidth);

    console.log("Downscale factor: ", down_scale);

    layer0.canvas.width  = width; // in pixels
    layer0.canvas.height = height; // in pixels

    layer1.canvas.width  = width; // in pixels
    layer1.canvas.height = height; // in pixels

    layer2.canvas.width  = width; // in pixels
    layer2.canvas.height = height; // in pixels

    layer0.context.drawImage(img, 0, 0, img.width, img.height, 0, 0, width, height);
}

// colony detection
// function getPositions(){
//     var image_id = chosen_image.id;
//     console.log("colonies  from image " +image_id);
//     // lade Ergebnisse der Kolonieerkennung
//     api('getpositions', { id: image_id });
// }

let down_scale;
function selectionTabEnter()
{
    layer0.canvas  = document.getElementById('layer0');
    layer1.canvas  = document.getElementById('layer1');
    layer2.canvas  = document.getElementById('layer2');
    layer0.context = layer0.canvas.getContext('2d');
    layer1.context = layer1.canvas.getContext('2d');
    layer2.context = layer2.canvas.getContext('2d');

    let img = new Image();
    img.onload = function ()
    {
        resizeCanvas(img);
        // show settings and start detection
        document.getElementById('layer-control').style.display = 'flex';
        document.getElementById('select-algorithm').onchange();
    }
    img.src = chosen_image.path;

    layer2.canvas.onmousemove = (e) => {
        var rect = layer2.canvas.getBoundingClientRect();
        var x = e.clientX -rect.left,
        y = e.clientY -rect.top;

        for(let colony of included_colonies.values()){
            if(colony.isMouseOver(x,y))
            {
                if (!colony.mouseover)
                {
                    colony.mouseover = true;
                    drawTooltip(colony);
                }
            }
            else if (colony.mouseover)
            {
                colony.mouseover = false;
                layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
            }
        }
        for(let colony of excluded_colonies.values()){
            if(colony.isMouseOver(x,y))
            {
                if (!colony.mouseover)
                {
                    colony.mouseover = true;
                    drawTooltip(colony);
                }
            }
            else if (colony.mouseover)
            {
                colony.mouseover = false;
                layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
            }
        }
    };

    layer2.canvas.onmousedown = (e) =>
    {
        e.preventDefault();
        // TODO include/exclude colonies
        for(let colony of included_colonies.values()){
            if(colony.mouseover){
                colony.exclude();
                excluded_by_user.set(colony.get("hash"), colony);
                excluded_colonies.set(colony.get("hash"), colony);
                included_colonies.delete(colony.get("hash"));
                included_by_user.delete(colony.get("hash"));
                printPositions();
                drawTooltip(colony);
                return;
            }
        }
        for(let colony of excluded_colonies.values()){
            if(colony.mouseover){
                colony.include();
                included_by_user.set(colony.get("hash"), colony);
                included_colonies.set(colony.get("hash"), colony);
                excluded_colonies.delete(colony.get("hash"));
                excluded_by_user.delete(colony.get("hash"));
                printPositions();
                drawTooltip(colony);
                return;
            }
        }
    };

    layer2.canvas.onmouseleave = (e) =>
    {
        // colony.mouseover = false ignored for performance...
        layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
    }

    let hide_colonies_button = document.getElementById('hide-colonies');
    document.getElementById('show-selected').onchange = function(){
        show_selected = this.checked;
        printPositions();
        if(hide_colonies_button.checked){
            $('#hide-colonies').click();
            hide_colonies_button.checked = false;
        }
    };
    document.getElementById('show-excluded').onchange = function(){
        show_excluded = this.checked;
        printPositions();
        if(hide_colonies_button.checked){
            $('#hide-colonies').click();
            hide_colonies_button.checked = false;
        }
    };
    hide_colonies_button.onchange = function(){
        if(this.checked){
            $('#layer1').hide();
        } else $('#layer1').show();
    };
}

// TODO
function removeUnselectedColonies(indices)
{
    layer1.context.clearRect(0,0, layer1.canvas.width, layer1.canvas.height);
    included_colonies = new Map();
    indices.forEach(i => {
        console.log("#" , parseInt(i));
        let colony = colony_array[parseInt(i)];
        let hash = colony.x+','+colony.y;
        included_colonies.set(hash, new Circle({
                x: colony.x * layer1.canvas.width,
                y: colony.y * layer1.canvas.height,
                radius: colony.major_length * 1.25 * down_scale,
                linecolor: 'white',
                canvas: layer1.canvas,
                hash: hash
            }));
    });    
    show_selected = true;
    show_excluded = false;
    printPositions();
}

let excluded_by_user = new Map(), included_by_user = new Map();
let excluded_colonies = new Map(), included_colonies = new Map();

function updatePositions(coords)
{
    colony_array = coords.colonies;
    excluded_colonies = new Map();
    included_colonies = new Map();
    
    colony_array.forEach((colony) =>
    {
        let hash = colony.x+','+colony.y;
        
        let color = '#FFFFFF';
        if(colony.excluded_by != ""){
            // TODO build lookup table
            const found = algorithms["1"].settings.filter(s => (s.id == colony.excluded_by));
            if (found && found.length)
                color = found[0].color;
            else
                console.warn("Could not find setting ", colony.excluded_by);
            excluded_colonies.set(hash, new Circle({
                x: colony.x * layer1.canvas.width,
                y: colony.y * layer1.canvas.height,
                radius: colony.major_length * 1.25 * down_scale,
                linecolor: color,
                defaultLinecolor: color,
                canvas: layer1.canvas,
                excluded_by: colony.excluded_by,
                hash: hash
            }));
        }
        else{
            included_colonies.set(hash, new Circle({
                x: colony.x * layer1.canvas.width,
                y: colony.y * layer1.canvas.height,
                radius: colony.major_length * 1.25 * down_scale,
                linecolor: color,
                defaultLinecolor: color,
                canvas: layer1.canvas,
                excluded_by: "",
                hash: hash
            }));
        }
    });

    excluded_by_user.forEach((value, key) => {
        included_colonies.delete(key);
        excluded_colonies.set(key, value);
    });
    included_by_user.forEach((value, key) => {
        excluded_colonies.delete(key);
        included_colonies.set(key, value);
    });

    printPositions();
}
// Print colonies
var show_excluded = false;
var show_selected = true;
function printPositions(){
    layer1.context.clearRect(0,0, layer1.canvas.width, layer1.canvas.height);

    // TODO remove
    number_of_colonies = included_colonies.size;
    
    // Print new positions
    if(show_excluded){
        for(let colony of excluded_colonies.values()){
            colony.draw();
        }
    }
    if(show_selected){
        for(let colony of included_colonies.values()){
            colony.draw();
        }   
    }
}

function drawTooltip(colony){
    layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
    layer2.context.font = "12px sans-serif";
    colony.draw(layer2.context, 3);
    const colony_pos = colony.getPosition();
    let tooltip_pos = {x:colony_pos.x, y:colony_pos.y +colony.getRadius() *1.5, width: 10, height: 10};

    let text = "Radius " + (Math.round(colony.getRadius() * 1000) / 1000);
    if(colony.get('excluded_by') != ""){
        text += "\nExcluded by " + colony.get('excluded_by');
    } 
    else if(colony.get('included_by')){ 
        text += "\nIncluded by " + colony.get('included_by');
     }

    let lines = text.split("\n");
    lines.forEach(line =>{
        console.log(line);
        console.log(layer2.context.measureText(line));
        tooltip_pos.width = Math.max(tooltip_pos.width, layer2.context.measureText(line).width+20);
        tooltip_pos.height += 20;
    });
    if(tooltip_pos.x + tooltip_pos.width > layer2.canvas.width) tooltip_pos.x -= tooltip_pos.x + tooltip_pos.width - layer2.canvas.width;  
    if(tooltip_pos.y + tooltip_pos.height > layer2.canvas.height) tooltip_pos.y = colony_pos.y - colony.getRadius()*1.5 - tooltip_pos.height;
    layer2.context.fillStyle = "white";
    console.log("Width", tooltip_pos.width);
    console.log("height", tooltip_pos.height);

    layer2.context.fillRect(tooltip_pos.x, tooltip_pos.y, tooltip_pos.width, tooltip_pos.height);  

    layer2.context.fillStyle = "black";
    layer2.context.textBaseline = "top";
    
    let y_offset = 10;
    lines.forEach(line =>{
        console.log(line);
        layer2.context.fillText(line, tooltip_pos.x+10, tooltip_pos.y+y_offset);
        y_offset += 16; // line-height
    });
}