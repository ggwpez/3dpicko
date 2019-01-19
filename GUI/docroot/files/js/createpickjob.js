//"id"=>"image_object"
var images_list = {}, user_images = {};
var chosen_image = {};
var user_id;
var ws;

/*(function LoadAllImages(){
	$.getJSON( "/api/getimagelist", function(response) {
		div_images = document.getElementById('all-images');
		$.each( response, function( key, image_object ) {
			AddImageToList(image_object);
		});
		div_images.style.display = "block";
	});
})();*/

(function WsSetup() {
	ws = new WebSocket("ws://" +window.location.hostname +":8888/");

	ws.onopen = function()
	{
    	api("getimagelist");
	};
	ws.onmessage = function(evt)
	{
		var msg = JSON.parse(evt.data);
		console.log(msg);

		if (msg.type == "getimagelist")
		{
			div_images = document.getElementById('all-images');
			console.log("adding image list");
			msg.data.images.forEach(AddImageToList);
			div_images.style.display = "block";
		}
		else if (msg.type == "deleteimage")
		{
			msg.data.deleted.forEach(function (id)
			{
				if(id == chosen_image.id) SetChosen(false);
				delete user_images[id];
				delete images_list[id];
			});
		}
		else if (msg.type == "uploadimage")
		{
			AddImageToList(msg.data);
			SetChosen(msg.data.id);
		}
		else if (msg.type == "createjob")
		{
			alert(msg.data.id);
		}
		else
		{
			alert("Unknown message type " +msg.type);
		}
	};
	ws.onerror = function (errorEvent)
	{
    	alert("Error! Die Verbindung wurde unerwartet geschlossen");
	};
	ws.onclose = function()
	{

	};
})();

function api(query, data)
{
	ws.send(JSON.stringify({request: /*"/apiv2/" +*/query, data: data}));
}

function AddImageToList(image_object){
	console.log("Adding image " +image_object.path);
	html = '\
	<div class="card m-1" id="'+image_object.id+'" style="display: inline-block; vertical-align:top; max-width: 200px;" min-height: 250px;"">\
	<button type="button" class="close" data-toggle="modal" data-target="#deleteDialog" onclick="MarkForDeletion(\''+image_object.id+'\')">&times;</button>\
	<div href="link.html" class="card-body p-1" onclick="SetChosen(\''+image_object.id+'\'); $(&quot;html, body&quot;).animate({ scrollTop: 0 }, &quot;slow&quot;);" style="cursor: pointer;">\
	<h5 class="card-title mr-3" style="overflow: hidden">'+image_object.filename+'</h5>\
	<img class="card-img" src="'+image_object.path+'" alt="'+image_object.filename+'"">\
	<p class="card-text">Datum: '+image_object.upload_date+'</li></p>\
	</div>\
	</div>';
	var div_images;
	if(image_object.user_id == user_id){
		div_images = document.getElementById('user-images');
		images_list[image_object.id] = image_object;
		user_images[image_object.id] = image_object;
		div_images.insertAdjacentHTML('afterbegin',html);
	}
	else{
		div_images = document.getElementById('all-images');
		images_list[image_object.id] = image_object;
		div_images.insertAdjacentHTML('beforeend',html);
	}
}

// extra lang für Flo (aka Vater der Bezeichner die länger sind als mein Bildschirm) 
var image_id_marked_for_deletion;
function MarkForDeletion(image_id) {
	image_id_marked_for_deletion = image_id;
}
function DeleteImage(){
	document.getElementById(image_id_marked_for_deletion).style.display = "none";

	api("deleteimage", images_list[image_id_marked_for_deletion]);
}

function SetChosen(image_id){
	div_chosen = document.getElementById('chosen-image');
	class_dropzone = document.getElementById('dropZone');

	if(image_id){
		chosen_image = images_list[image_id];
		div_chosen.innerHTML = '\
		<ul><li>Dateiname: '+chosen_image.filename+'</li><li>Upload-Datum: '+chosen_image.upload_date+'</li></ul>\
		<button class="btn btn-dark" onclick="cutTab()">Bild auswählen ></button>\
		';
		class_dropzone.innerHTML = '<img style="height: 100%; width: 100%; object-fit: contain" src="' + chosen_image.path +'"/>';
		class_dropzone.style.backgroundColor = 'transparent';
	}
	else{
		chosen_image = false;
		class_dropzone.innerHTML = '';
		class_dropzone.style.backgroundColor = 'grey';
	}
}

//Navigation
function cutTab(){
	if(chosen_image){
		$("#cut-tab").removeClass("disabled");
		document.getElementById('cutImg').src = chosen_image.path;
		$('#steps a[href=\'#cut\']').tab('show');
	}
}
function selectionTab(){
	if(chosen_image){
		$("#selection-tab").removeClass("disabled");
		document.getElementById('photograph').src = chosen_image.path;
		$('#steps a[href=\'#selection\']').tab('show');

		api("createjob", {name: document.getElementById('inputJobName').value});
	}
}
function strategyTab(){
	$("#strategy-tab").removeClass("disabled");
	$('#steps a[href=\'#strategy\']').tab('show');
}
function overviewTab(){
	$("#overview-tab").removeClass("disabled");
	$('#steps a[href=\'#overview\']').tab('show');
}
function attributesTab(){
	$("#attributes-tab").removeClass("disabled");
	document.getElementById('staticImgName').value = chosen_image.filename;
	$('#steps a[href=\'#attributes\']').tab('show');
}
function settingsTab(){
	$("#settings-tab").removeClass("disabled");
	$('#steps a[href=\'#settings\']').tab('show');
}
