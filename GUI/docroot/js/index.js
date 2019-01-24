//"id"=>"image_object"
var images_list = {}, user_images = {};
var chosen_image = {};
var user_id;

/*(function LoadAllImages(){
	$.getJSON( "/api/getimagelist", function(response) {
		div_images = document.getElementById('all-images');
		$.each( response, function( key, image_object ) {
			AddImageToList(image_object);
		});
		div_images.style.display = "block";
	});
})();*/

(function Setup()
{
	WsSetup(
		() =>
		{
			document.title = "Verbunden - 3CPicko";
			api("getimagelist");
			api("getjoblist");
		},
		(type, data) =>
		{
			console.log("Message type: '" +type +"'");

			if ("error" in data)				// TODO unclean
				alert(data.error);
			else if (type == "getimagelist")
			{
				console.log(data);
				div_images = document.getElementById('all-images');
				data.images.forEach(AddImageToList);
				div_images.style.display = "block";
			}
			else if (type == "deleteimage")
			{
				if (!(data.id in images_list))
				{
					console.warn("Server trying to delete non existing image: " + JSON.stringify(data));
				}
				else
				{
					console.log("Deleting image: " +data.id);
					document.getElementById('img-' +data.id).style.display = "none";	// TODO remove deleted from selection
					if(chosen_image.id == data.id) SetChosen(false);
					delete user_images[data.id];
					delete images_list[data.id];
				}
			}
			else if (type == "deletejob")
			{
				if (!(data.id in all_jobs))
				{
					console.warn("Server trying to delete non existing job: " + JSON.stringify(data));
				}
				else
				{
					console.log("Deleting job: " +data.id);
					document.getElementById('job-' +data.id).style.display = "none";

					delete all_jobs[data.id];
				}
			}
			else if (type == "uploadimage")
			{
				AddImageToList(data);
				SetChosen(data.id);
			}
			else if (type == "createjob")
			{
				AddJobToList(data);
				document.getElementById("headertag").innerHTML = "Entwurf #" +data.id;
			}
			else if (type == "getjoblist")
			{
				data.jobs.forEach(AddJobToList);
				console.log("Jobs:\n" +data +"\ncount: " +data.jobs.length);
			}
			else if (type == "createsettingsprofile"){
				const profile_object = data; 
				switch(profile_object.type){
					case "printer-profile": 
					AddPrinterProfile(profile_object);
					printer_profiles[profile_object.id] = profile_object;
					break;
				}
				$('#form-new'+profile_object.type)[0].reset();
				$('#collapse-new'+profile_object.type).collapse('hide');
				$('#info-dialog-title').text(profile_object.profile_name+" erfolgreich hinzugefügt.");
				$('#info-dialog').modal('show');
			}
			else if(type == "updatesettingsprofile"){
				const profile_object = data; 
				switch(profile_object.type){
					case "printer-profile": 
					printer_profiles[profile_object.id] = profile_object;
					break;
				}
				$('#link-'+profile_object.id).text(profile_object.profile_name);
				$('#collapse-'+profile_object.id).collapse('hide');
				$('#info-dialog-title').text(profile_object.profile_name+" erfolgreich aktualisiert.");
				$('#info-dialog').modal('show');
			}
			else if (type == "debug")
			{
				addDebugOutputLine(data.line);
			}
			else if (type == "getpositions")
			{
				console.log("Positions\n" +JSON.stringify(data));
				drawPositions(data);
			}
			else
			{
				console.warn("Ignoring message of type '" +type +"'");
			}
		},
		() =>
		{
			document.title = "Nicht verbunden - 3CPicko";
		});
})();

// all_jobs[job.id] = job;
var all_jobs = [];
var cards = 1;

function AddJobToList(job)
{
	// Is the job not already in the list ?
	if (!(job.id in all_jobs))
	{
		console.log("Adding job: " +JSON.stringify(job));

		var name = (job.step == 7) ? "Job" : "Entwurf";
		var html = '<div class="card m-1" id="job-' +job.id +'">\
		<button type="button" class="close" data-toggle="modal" data-target="#deleteJobDialog" onclick="MarkJobForDeletion(\''+job.id+'\')">&times;</button>\
		<div class="card-body pb-0">\
		<h5 class="card-title">' +name +' ' +job.name +'</h5>\
		<img class="card-img" src="' +images_list[job.img_id].path +'" alt="Bild der Agarplatte">\
		<p class="card-text"><ul>\
		<li>ID: ' +job.id +'</li>\
		<li>Erstellt: ' +job.job_created +'</li>\
		<li>Schritt: ' +job.step +'</li>\
		' +job.description +'\
		</p>\
		</div>\
		<div class="card-footer bg-white">\
		<a href="#" class="btn btn-dark ">bearbeiten</a>\
		</div>\
		</div>';

		document.getElementById('allJobList').insertAdjacentHTML('beforeend', html);
		all_jobs[job.id] = job;
	}
}

function AddJobToHistoryList(job)
{
	
}

function AddImageToList(image_object){

	if (!(image_object.id in images_list))
	{
		console.log("Adding image " +JSON.stringify(image_object));
		html = '\
		<div class="card m-1" id="img-'+image_object.id+'" style="display: inline-block; vertical-align:top; max-width: 200px;" min-height: 250px;"">\
		<button type="button" class="close" data-toggle="modal" data-target="#deleteDialog" onclick="MarkForDeletion(\''+image_object.id+'\')">&times;</button>\
		<div href="link.html" class="card-body p-1" onclick="SetChosen(\''+image_object.id+'\'); $(&quot;html, body&quot;).animate({ scrollTop: 0 }, &quot;slow&quot;);" style="cursor: pointer;">\
		<h5 class="card-title mr-3" style="overflow: hidden">'+image_object.original_name+'</h5>\
		<img class="card-img" src="'+image_object.path+'" alt="'+image_object.original_name+'"">\
		<p class="card-text">Datum: '+image_object.uploaded +'</li></p>\
		</div>\
		</div>';

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

var job_id_marked_for_deletion;
function MarkJobForDeletion(job_id) {
	job_id_marked_for_deletion = job_id;
}

function DeleteImage(){
	console.log("Trying to delete image with id: " +image_id_marked_for_deletion);
	api("deleteimage", {id: image_id_marked_for_deletion});
}

function DeleteJob(){
	console.log("Trying to delete job with id: " +job_id_marked_for_deletion);
	api("deletejob", {id: job_id_marked_for_deletion});
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

function addDebugOutputLine(line)
{
	console.log("Adding: " +line);
	document.getElementById('debug-output').insertAdjacentHTML('beforeend', line +'<br>');
}

const tabOrder = [ "browse", "cut", "attributes", "selection", "strategy", "overview", "execute" ];

function tabEnter(tabId)
{
	console.log("Enabling tab: " +tabOrder[tabId]);
	if (tabId > 0)
	{
		console.log("Disabling tab: " +tabOrder[tabId -1]);
		$("#" +tabOrder[tabId -1] +"-tab").addClass("disabled");
		$("#" +tabOrder[tabId -1] +"-tab").css("background-color", '#11d100');
	}

	$("#" +tabOrder[tabId] +"-tab").removeClass("disabled");
	$("#" +tabOrder[tabId] +"-tab").tab('show');
}

var cropper;
//Navigation
function cutTab(){
	if(chosen_image){
		tabEnter(1);

		cutImg = document.querySelector('#cutImg');
		cutImg.src = chosen_image.path;

		setTimeout(() => {
			console.log("Image loaded ###########");
			cropper = new Cropper(cutImg, {
			        aspectRatio: 1.5/1
		});}, 1000);


	    
    //setTimeout(function() {cropper.replace(chosen_image.path);}, 1000);
	}
}

function attributesTab(){
	tabEnter(2);
	document.getElementById('staticImgName').value = chosen_image.original_name;
}

// colony
function selectionTab(){
	if(chosen_image){
		tabEnter(3);
		document.getElementById('photograph').src = chosen_image.path;

		api("createjob", { img_id: chosen_image.id, name: document.getElementById('inputJobName').value});
	}
}
function strategyTab(){
	tabEnter(4);
}
function overviewTab(){
	tabEnter(5);
}