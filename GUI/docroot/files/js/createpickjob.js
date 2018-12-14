// name of chosen image (with extension) 
var chosenImageName;
// valid steps
//var steps = {choose: false, cut: false, selection: false};


var processed = false;
function startImageProcessing(){
	$("#settings-tab").removeClass("disabled");
	//if(!processed) alert('Request wird an Server gesendet... (nicht implementiert)'); processed=true;
	if(chosenImageName) document.getElementById('settingsImg').src = "images/"+chosenImageName;
	$('#steps a[href="#settings"]').tab('show');
}

function cutTab(){
	if(chosenImageName){
		document.getElementById('cutImg').src = "images/"+chosenImageName;
		$('#steps a[href=\'#cut\']').tab('show');
	}
}

function selectionTab(){
	if(chosenImageName){
		$("#selection-tab").removeClass("disabled");
		document.getElementById('photograph').src = "images/"+chosenImageName;
		$('#steps a[href=\'#selection\']').tab('show')
	}
}

function strategyTab(){
	$("#strategy-tab").removeClass("disabled");
	$('#steps a[href=\'#strategy\']').tab('show');
}

function overviewTab(){
	$("#overview-tab").removeClass("disabled");
	$('#steps a[href=\'#overview\']').tab('show')
}

function attributesTab(){
	$("#attributes-tab").removeClass("disabled");
	$('#steps a[href=\'#attributes\']').tab('show')
}

function settingsTab(){
	$("#settings-tab").removeClass("disabled");
	$('#steps a[href=\'#settings\']').tab('show')
}


(function loadImages(){
	$.getJSON( "/getImageList", function(data) {
		var items = [];
		$.each( data, function( key, val ) {
			addImageToList(val[0], val[1])
		});
	});
})();

function addImageToList(filename, date){
	imagesDiv = document.getElementById('images');
	html = '\
	<div class="card m-1 float-left" id="'+filename+'" style="max-width: 200px;">\
	<button type="button" class="close" data-toggle="modal" data-target="#deleteDialog" onclick="select(\''+filename+'\')">&times;</button>\
	<div href="link.html" class="card-body p-1" onclick="setChosen(\''+filename+'\',\''+date+'\')" style="cursor: pointer;">\
	<h5 class="card-title mr-3">'+filename+'</h5>\
	<img class="card-img" src="images/'+filename+'" alt="Bild der Agarplatte">\
	<p class="card-text">Upload-Datum: '+date+'</li></p>\
	</div>\
	</div>';
	imagesDiv.insertAdjacentHTML('afterbegin',html);
}

var selectedCard;
function select(id) {
	selectedCard = document.getElementById(id)
}
function del(){
	selectedCard.style.display = "none";
}

function setChosen(filename, date){
	$("#cut-tab").removeClass("disabled");
	chosenImageName = filename;
	chosenDiv = document.getElementById('chosen');
	//<button type="button" class="close" data-toggle="modal" data-target="#deleteDialog" onclick="select(\'chosen\')">&times;</button>\
	chosenDiv.innerHTML = '\
	<ul><li>Upload-Datum: '+date+'</li></ul>\
	<button class="btn btn-dark" onclick="cutTab()">Pickjob erstellen</button>\
	';
	chosenDiv.style.display = "block";
	$(".dropzone").css('background-image', 'url(images/' + filename + ')');
	//window.scrollTo(0,0);
	$("html, body").animate({ scrollTop: 0 }, "slow");
}

(function() {
	$(".dropzone").dropzone({
		url: '/imageUpload',
		margin: 20,
		allowedFileTypes: 'jpg, png',
		params:{
			'action': 'save'
		},
		dataType: 'json',
		success: function(data, index){
			var array = JSON.parse(data.responseText);
			addImageToList(array[0], array[1]);
			setChosen(array[0], array[1]);
		}
	});
}());

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