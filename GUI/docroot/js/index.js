//"id"=>"image_object"
var images_list = {};
var chosen_image = {};
// current job (local) 
var current_job = {};
// all_jobs[job.id] = job;
var all_jobs = [];
var all_profiles = {};
// ?
var cards = 1;

(function Setup()
{
	WsSetup(
		() =>
		{
			document.title = "Connected - 3CPicko";
			global_alert.css('display', 'none');
			ShowAlert("Connected to server", "success", 500);
			api("getimagelist");
			api("getjoblist");
			api("getprofilelist");
		},
		(type, data) =>
		{
			console.log("Message type: '" +type +"'");
			console.log("Response: ", data);

			if ("error" in data)				// TODO unclean
				ShowAlert(data.error, "danger");
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
					ShowAlert("Error: Non existing image.", "danger");
				}
				else
				{
					console.log("Deleting image: " +data.id);
					document.getElementById('img-' +data.id).style.display = "none";	// TODO remove deleted from selection
					ShowAlert(images_list[data.id].original_name+" deleted.", "success");
					if(chosen_image.id == data.id) SetChosen(false);
					delete images_list[data.id];
				}
			}
			else if (type == "deletejob")
			{
				if (!(data.id in all_jobs))
				{
					console.warn("Server trying to delete non existing job: " + JSON.stringify(data));
					ShowAlert("Error: Non existing job.", "danger");
				}
				else
				{
					const job_name = (all_jobs[data.id].name=="")?data.id:all_jobs[data.id].name;
					console.log("Deleting job: " +data.id);
					document.getElementById('job-' +data.id).style.display = "none";
					ShowAlert("Job "+job_name+" deleted.", "success");
					delete all_jobs[data.id];
				}
			}
			else if (type == "uploadimage")
			{
				AddImageToList(data);
				SetChosen(data.id);
				ShowAlert("Image "+data.original_name+" uploaded.", "success");
			}
			else if (type == "createjob")
			{
				AddJobToList(data);
				document.getElementById("headertag").innerHTML = "Entwurf #" +data.id;
				// current_job = data;
			}
			else if (type == "getjoblist")
			{
				data.jobs.forEach(AddJobToList);
				console.log("Jobs:\n" +data +"\ncount: " +data.jobs.length);
			}
			else if (type == "getprofilelist")
			{
				data.profiles.forEach(AddProfileToList);
				console.log("Profiles:\n" +data +"\ncount: " +data.profiles.length);
			}
			else if (type == "createsettingsprofile"){
				AddProfileToList(data);
				ShowAlert(data.profile_name+" added.", "success");
			}
			else if(type == "updatesettingsprofile"){
				AddProfileToList(data);
				ShowAlert(data.profile_name+" updated.", "success");
			}
			else if(type == "deletesettingsprofile"){
				ShowAlert(all_profiles[data.id].profile_name+" deleted.", "success");
				DeleteProfile(data.id);
			}
			else if (type == "debug")
			{
				addDebugOutputLine(data.line);
				// TODO...
				if(data.line == "Ignoring doubled image") ShowAlert("Image already exists.", "danger");
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
			global_alert.addClass('alert alert-danger');
			global_alert.css('display', 'block');
			document.title = "No connection - 3CPicko";
		});
})();

function AddJobToList(job)
{
	// Is the job not already in the list ?
	if (!(job.id in all_jobs))
	{
		console.log("Adding job: " +JSON.stringify(job));

		var name = (job.step == 7) ? "Job" : "Entwurf";
		var html = `<div class="card m-1 job-card" id="job-${job.id}" >
		<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="job" data-id="${job.id}">&times;</button>
		<div class="card-body p-1">
		<h5 class="card-title mr-2 p-1">${name} ${job.name}</h5>
		<img class="card-img" src="${images_list[job.img_id].path}" alt="${images_list[job.img_id].original_name}">
		<p class="card-text"><ul>
		<li>ID: ${job.id}</li>
		<li>Created: ${DateToString(job.job_created)}</li>
		<li>Step: ${job.step}</li>
		${job.description}
		</p>
		</div>
		<div class="card-footer bg-white">
		<a href="#" class="btn btn-dark ">edit</a>
		</div>
		</div>`;

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
		html = `
		<div class="card m-1 image-card" id="img-${image_object.id}">
		<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="image" data-id="${image_object.id}">&times;</button>
		<div class="card-body p-1" onclick="SetChosen('${image_object.id}');" $(&quot;html, body&quot;).animate({ scrollTop: 0 }, &quot;slow&quot;);" style="cursor: pointer;">
		<h5 class="card-title mr-2 p-1">${image_object.original_name}</h5>
		<div class="spinner-border m-5" id="loading-${image_object.id}"></div>
		<img class="card-img" src="${image_object.path}" alt="${image_object.original_name}" style="display: none;" onload="$(this).show();$('#loading-${image_object.id}').remove();">
		<p class="card-text">Date: ${DateToString(image_object.uploaded)}</li></p>
		</div>
		</div>`;

		div_images = document.getElementById('all-images');
		images_list[image_object.id] = image_object;
		div_images.insertAdjacentHTML('afterbegin',html);
	}
}

$('#delete-dialog').on('show.bs.modal', function (e) {
  // use: <button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="image" data-id="${image_object.id}">&times;</button>
  const type = $(e.relatedTarget).data('type');
  const id = $(e.relatedTarget).data('id');
  const dialog_text = document.getElementById('delete-dialog-title');
  const dialog_button = document.getElementById('delete-button');
  
  switch (type) {
  	case "image":
  	dialog_text.innerHTML = `Delete image ${images_list[id].original_name}?`;
  	dialog_button.onclick = function(){
  		console.log("Trying to delete image with id: " + id);
  		api("deleteimage", {id: id.toString()});
  	};
  	break;
  	case "job":
  	dialog_text.innerHTML = `Delete job ${all_jobs[id].name}?`;
  	dialog_button.onclick = function(){
  		console.log("Trying to delete job with id: " + id);
  		api("deletejob", {id: id.toString()});
  	};
  	break;
  	case "printer-profile":
  	dialog_text.innerHTML = `Delete printer profile ${all_profiles[id].profile_name}?`;
  	dialog_button.onclick = function(){
  		console.log("Trying to delete printer profile with id: " + id);
  		api("deletesettingsprofile", {id: id.toString()});
  	};
  	break;
  	case "socket-profile":
  	dialog_text.innerHTML = `Delete socket profile ${all_profiles[id].profile_name}?`;
    	dialog_button.onclick = function(){
  		console.log("Trying to delete socket profile with id: " + id);
  		api("deletesettingsprofile", {id: id.toString()});
  	};
  	break;
  	case "plate-profile":
  	dialog_text.innerHTML = `Delete plate profile ${all_profiles[id].profile_name}?`;
  	dialog_button.onclick = function(){
  		console.log("Trying to delete plate profile with id: " + id);
  		api("deletesettingsprofile", {id: id.toString()});
  	};
  	break;
  	default:
  	dialog_text.innerHTML = `Not implemented`;
  	dialog_button.onclick = function(){
  		console.log("called non implemented delete");
  	};
  }
});

function SetChosen(image_id){
	div_chosen = document.getElementById('chosen-image');
	class_dropzone = document.getElementById('dropZone');

	if(image_id){
		chosen_image = images_list[image_id];
		console.log("Selecting image ", image_id);
		div_chosen.innerHTML = `
		<ul><li>Dateiname: ${chosen_image.original_name}</li><li>Upload Date: ${DateToString(chosen_image.uploaded)}</li></ul>\
		<button class="btn btn-dark" onclick="cutTab()">Choose image ></button>\
		`;
		class_dropzone.innerHTML = `<img style="height: 100%; width: 100%; object-fit: contain" src="${chosen_image.path}"/>`;
		class_dropzone.removeClass('empty');
	}
	else{
		chosen_image = false;
		div_chosen.innerHTML = "";
		class_dropzone.innerHTML = 'Drop Image';
		class_dropzone.addClass('empty');
	}
}

function addDebugOutputLine(line)
{
	//console.log("Adding: " +line);
	document.getElementById('debug-output').insertAdjacentHTML('beforeend', line +'<br>');
}

const tabOrder = [ "browse", "cut", "attributes", "selection", "strategy", "overview", "execute" ];

function tabEnter(tabId)
{
	console.log("Enabling tab: " +tabOrder[tabId]);
	for(id = 0; id < tabOrder.length; id++){
		if(id < tabId){
			console.log("Disabling tab: " +tabOrder[id]);
			$("#" +tabOrder[id] +"-tab").addClass("disabled");
		}
		else{
			$("#" +tabOrder[id] +"-tab").removeClass("disabled");
		}
	}
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
			});
		}, 1000);
		//setTimeout(function() {cropper.replace(chosen_image.path);}, 1000);
	}
}

function attributesTab(){
	tabEnter(2);
	document.getElementById('staticImgName').innerHTML = chosen_image.original_name;

	// Do the cutting
	var rect = cropper.getData();
	api('crop-image', { id: chosen_image.id, x: rect.x, y: rect.y, width: rect.width, height: rect.height });
}
function selectionTab(){
	if(chosen_image){
		tabEnter(3);
		document.getElementById('photograph').src = chosen_image.path;
		const printer_selection = document.getElementById('select-printer-profile');
		const socket_selection = document.getElementById('select-socket-profile');
		const printer_id = printer_selection.options[printer_selection.selectedIndex].value;
		const socket_id =  socket_selection.options[socket_selection.selectedIndex].value;
		const description = document.getElementById('description-input').value;

		current_job = {
			img_id: chosen_image.id,
			name: document.getElementById('inputJobName').value,
			printer: printer_id,
			socket: socket_id,
			description: description
		}
		api("createjob", current_job);
	} 
}

function strategyTab(){
	let plate_selection = document.getElementById("select-plate-profile");
	for(let profile_id in all_profiles){
		let profile = all_profiles[profile_id];
		if(profile.type=="plate-profile"){
			//TODO read number of colonys
			if(profile.number_of_rows*profile.number_of_columns >= 96){
				let plate_profile_option = document.createElement('option');
				plate_profile_option.value = profile.id;
				plate_profile_option.text = profile.profile_name;
				plate_selection.appendChild(plate_profile_option);
			}
		}
	}
	let plate_id = plate_selection.options[plate_selection.selectedIndex].value;
	drawWells(all_profiles[plate_id].number_of_columns, all_profiles[plate_id].number_of_rows);
	plate_selection.addEventListener("change", function(){
		const plate_id = plate_selection.options[plate_selection.selectedIndex].value;
		drawWells(all_profiles[plate_id].number_of_columns, all_profiles[plate_id].number_of_rows);
	});

	tabEnter(4);
}

function overviewTab(){
	tabEnter(5);
	console.log(all_jobs);
	console.log(current_job);
	// TODO get processed images
	const html = `
	<img id="processed-image" src="${images_list[current_job.img_id].path}" width="50%">
	<ul class="mt-2">
	<li>Job name: ${current_job.name}</li>
	<li>Printer: ${all_profiles[current_job.printer].profile_name}</li>
	<li>Socket: ${all_profiles[current_job.socket].profile_name}</li>
	<li>Description: ${current_job.description}</li>
	<li>Pick strategy:</li>
	</ul>
	`;

	document.getElementById('overview-content').insertAdjacentHTML('afterbegin', html);
}
function executeTab(){
	var form = document.getElementById('check-preconditions');
	if (form.checkValidity() === true) tabEnter(6);
	form.classList.add('was-validated');
}

var global_alert = $('#global-alert');
var alert_window = $('#alert-window');

// bootstrap alert-type: success/warning/danger/primary/dark... 
function ShowAlert(message, type = "success", delay=3000){
	let new_alert = global_alert.clone();
	new_alert[0].innerHTML = message;
	new_alert[0].className = "alert alert-" + type;
	new_alert.fadeIn("slow", function(){new_alert.delay(delay).fadeOut("slow")});
	alert_window.append(new_alert);
}

function DateToString(milisec){
	const date = new Date(milisec);
	return date.toLocaleDateString();
}