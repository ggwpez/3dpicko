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

// Variable containing the list of colonies
var coordinatesPx = {"colonies":[]};

// Size of the petri dish
var widthMm = 127.5;
var heightMm = 85;

// Variable containing the list of colonies in mm
var coordinatesMm = {"colonies":[]};

// Make clickArea interactive
var container = document.querySelector("#clickArea");

// Correct canvas size
resizeCanvas(drawArea);

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

// lade Ergebnisse der Kolonieerkennung

	// Import colonies from colony detection
	for (i in input.colonies) {
		xPosition = (input.colonies[i].xCoordinate * 9);
		yPosition = (input.colonies[i].yCoordinate * 6);
		diameter = (input.colonies[i].diameter * 8);
		var colony = {"xCoordinate":xPosition,"yCoordinate":yPosition, "diameter":diameter};
		coordinatesPx.colonies.push(colony);
	}
	printPositions();
}

//	change mode if colonies should be added or deleted
function editColonyMode(m){
	mode = m;
    var element = document.getElementById("drawArea");
    if (m == "add"){
        element.classList.remove("del");
        element.classList.add("add");
    } else {
        element.classList.remove("add");
        element.classList.add("del");
    }
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

// Print colonies
function printPositions(){

    // show coordinatesPx
    showCoordinates.innerHTML = JSON.stringify(coordinatesPx.colonies, null, 4);

    // Delete positions
    const context = drawArea.getContext('2d');
    context.clearRect(0, 0, drawArea.width, drawArea.height);

    // Print new positions
    for (i in coordinatesPx.colonies){
        var canvas = document.getElementById("drawArea");
        var circle = canvas.getContext("2d");
        circle.beginPath();
        circle.arc(coordinatesPx.colonies[i].xCoordinate, coordinatesPx.colonies[i].yCoordinate, coordinatesPx.colonies[i].diameter, 0, 2*Math.PI);
        circle.lineWidth = 4;
        circle.strokeStyle = 'blue';
        circle.stroke();
    }
}

// Delete null elements from array
function removeNull(){
	var coordinatesPxHelper = {"colonies":[]}; 
	for (i in coordinatesPx.colonies){
		var colony = coordinatesPx.colonies[i];
		if (colony) {
			coordinatesPxHelper.colonies.push(colony);
		}
	}
	coordinatesPx = coordinatesPxHelper;
	//showCoordinates.innerHTML = JSON.stringify(coordinatesPx.colonies, null, 4);
}

// Convert coordinates to mm format
function convertCoordinates(){
	removeNull();
	
	for (i in coordinatesPx.colonies){
		var colonyPx = coordinatesPx.colonies[i];
		
		var xPositionPx = colonyPx.xCoordinate;
		var yPositionPx = colonyPx.yCoordinate;
		
		var xPositionMm = Math.round(widthMm * (xPositionPx / drawArea.clientWidth));
		var yPositionMm = Math.round(heightMm * (yPositionPx / drawArea.clientHeight));
		
        var colonyMm = {"xCoordinate":xPositionMm,"yCoordinate":yPositionMm};
        coordinatesMm.colonies.push(colonyMm);
	}
	showCoordinates.innerHTML = JSON.stringify(coordinatesMm.colonies, null, 4);
}

// Create pickjob
function recPositions(){
	convertCoordinates();
	
}
