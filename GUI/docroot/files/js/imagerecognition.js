/*
	Platzhalter-Datei für Bilderkennung/Bildverarbeitung
*/

// Variabel Modi hinzufügen oder enfernen von Kolonien
var mode ='add';
// Variabel für Liste der Kolinien
var coordinates = {"colonies":[
{"xCoordinate":"20","yCoordinate":"20", "diameter":"10" },
{"xCoordinate":"20","yCoordinate":"580", "diameter":"10" },
{"xCoordinate":"880","yCoordinate":"20", "diameter":"10" },
{"xCoordinate":"880","yCoordinate":"580", "diameter":"10" }]};		

// Mache clock Area interaktiv
var container = document.querySelector("#clickArea");
// reagiere auf Mausklicks in click Area
container.addEventListener("click", changePosition, false);

// Starte Kolonieerkennung
function getPositions(){
	printPositions();
}

//	Modus ob Kolonien hinzugefügt oder gelöscht werden
function editColonyMode(m){
	mode = m;
}

// Finde genaue Position
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

// lösche oder füge Kolonien hinzu
function changePosition(event){

	var parentPosition = getPosition(event.currentTarget);
	var xPosition = event.clientX - parentPosition.x;
	var yPosition = event.clientY - parentPosition.y;		

	// füge Position hinzu
	if (mode == 'add'){
		var colony = {"xCoordinate":xPosition,"yCoordinate":yPosition, "diameter":"10"};
		coordinates.colonies.push(colony);		
	} 

	// lösche Position
	if (mode == 'del'){
		for (i in coordinates.colonies) {
			if (Math.abs(coordinates.colonies[i].xCoordinate - xPosition) < coordinates.colonies[i].diameter &&
				Math.abs(coordinates.colonies[i].yCoordinate - yPosition) < coordinates.colonies[i].diameter){
				delete coordinates.colonies[i];
		}
	}
}

	// Zeige Positionen an
	printPositions();
}

// zeige Kolonien an
function printPositions(){

	// Zeige Coordinaten
	//showCoordinates.innerHTML = JSON.stringify(coordinates.colonies, null, 4);

	// Lösche alte Kreise
	const context = drawArea.getContext('2d');
	context.clearRect(0, 0, drawArea.width, drawArea.height);

	// Zeichne neue Kreise
	for (i in coordinates.colonies){
		var canvas = document.getElementById("drawArea");
		var circle = canvas.getContext("2d");
		circle.beginPath();
		circle.arc(coordinates.colonies[i].xCoordinate, coordinates.colonies[i].yCoordinate, coordinates.colonies[i].diameter, 0, 2*Math.PI);
		circle.lineWidth = 4;
		circle.strokeStyle = 'blue';
		circle.stroke();
	}
}

// Erstelle pickjob
function recPositions(){
	$("#strategy-tab").removeClass("disabled");
	$('#steps a[href=\'#strategy\']').tab('show');
}

// Wandelt Koordinaten um
function convertCoordinates(){

}