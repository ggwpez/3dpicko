/* global WsSetup, api, AddProfileToList, DeleteProfile, drawPositions, SetDefaultProfile, $, Cropper, selectionTabEnter, drawWells, collided_row, collided_column, FormGroup, profile_templates, AddGeneralSetting, number_of_colonies */
//"id"=>"image_object"
var images_list = {};
var chosen_image = {};
// current job (local)
var current_job = {};
// all_jobs[job.id] = job;
var all_jobs = [];
// TODO delete "new_job" if job saved or executed
var unsaved_elements = {};

var global_alert = $('#global-alert');
var alert_window = $('#alert-window');

var connected = false;

$(function Setup()
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
            connected = true;
            EnableDropzone();
        },
        (type, data) =>
        {
            console.log("Message type: '" +type +"'");
            console.log("Response: ", data);

            if (type == "error"){
                clearTimeout(loading_timeout_id);
                clearTimeout(upload_timeout_id);
                document.body.classList.remove('wait');
                $('#strategy-tab-button').prop("disabled", true);
                changed_settings = {id:'', value: false, processed: true};
                // $('#overlay').hide();
                EnableDropzone();
                // TODO
                if('uploadimage' == data.where) ShowAlert("Upload failed.<br>Maybe Plate can't be detected.", "danger");
                else if('deleteimage' == data.where) ShowAlert("Deletion failed.<br>Maybe Image is used in Pickjob", "danger");
                else ShowAlert("Error: "+data.what+" in "+data.where, "danger");
            }
            else if (type == "getimagelist")
            {
                console.log(data);
                let div_images = document.getElementById('all-images');
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
                    document.getElementById('img-' +data.id).style.display = "none";    // TODO remove deleted from selection
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
                    ShowAlert("Error: Non existing job. Reload.", "danger");
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
                clearTimeout(upload_timeout_id);
                let image = new Image();
                image.onload = function(){
                    AddImageToList(data);
                    SetChosen(data.id);
                    ShowAlert("Image "+data.original_name+" uploaded.", "success");
                    EnableDropzone();
                }
                image.src = data.path;
            }
            else if (type == "createjob")
            {
                AddJobToList(data);
                current_job = data;
                document.getElementById("headertag").innerHTML = "Job #" +data.id;
                ShowAlert("Pickjob "+data.id+" saved.", "success");
            }
            else if (type == "getjoblist")
            {
                // TODO review
                if(data.jobs){
                    data.jobs.forEach(AddJobToList);    
                } else AddJobToList(data);
                
                console.log("Jobs:\n" +data);
            }
            else if (type == "getprofilelist")
            {
                AddProfiles(data)
            }
            else if (type == "setdefaultsettingsprofile"){
                SetDefaultProfile(data.id);
            }
            else if (type == "createsettingsprofile"){
                AddProfileToList(data);
                ShowAlert(data.profile_name+" added.", "success");
            }
            else if (type == "updatesettingsprofile"){
                AddProfileToList(data);
                ShowAlert(data.profile_name+" updated.", "success");
            }
            else if (type == "deletesettingsprofile"){
                ShowAlert(all_profiles[data.id].profile_name+" deleted.", "success");
                DeleteProfile(data.id);
            }
            else if (type == "debug")
            {
                addDebugOutputLine(data.line);
            }
            else if (type == "getpositions")
            {
                if(current_job.id == data.job){
                    updatePositions(data);
                }
            }
            else if (type == "getdetectionalgorithms"){
                console.log("########## Algos", JSON.stringify(data));
                GetDetectionAlgorithms(data);
            }
            else if (type == "setcoloniestopick"){
                // job: id, ids: array 
                if(current_job.id == data.job){
                    overviewTab(data.ids);
                }
            }
            else
            {
                console.warn("Ignoring message of type '" +type +"'");
            }
        },
        () =>
        {
            DisableDropzone();
            global_alert.addClass('alert alert-danger');
            global_alert.css('display', 'block');
            document.title = "No connection - 3CPicko";
            connected = false;
        });
});

function AddJobToList(job)
{
    if (!(job.id in all_jobs))
    {
        console.log("Adding job: " +JSON.stringify(job));
        // TODO everything...
        var name = (job.step == 7) ? "Job" : "Entwurf";
        var html = `<div class="card m-1 job-card" id="job-${job.id}" >
        <button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="job" data-id="${job.id}">&times;</button>
        <div class="card-body p-1">
        <h5 class="card-title mr-2 p-1">${name} ${job.name}</h5>
        <img class="card-img" src="${images_list[job.img_id].path}" alt="${images_list[job.img_id].original_name}">
        <p class="card-text"><ul>
        <li>ID: ${job.id}</li>
        <li>Created: ${job.created.formatted} </li>
        ${job.description}
        </p>
        </div>
        <div class="card-footer bg-white">
        <a href="#" class="btn btn-primary ">Download Report</a>
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
        let html = `
        <div class="card m-1 image-card" id="img-${image_object.id}">
        <button type="button" class="close" style="line-height: 0.5;" data-toggle="modal" data-target="#delete-dialog" data-type="image" data-id="${image_object.id}">&times;</button>
        <div class="card-body p-1" style="cursor: pointer;">
        <h7 class="card-title mb-1 mr-3">${image_object.original_name}</h7>
        <div class="spinner-border" id="loading-${image_object.id}"></div>
        <img id="image-${image_object.id}" class="card-img" src="${image_object.path}" alt="${image_object.original_name}" style="display: none;">
        <p class="card-text">Date: ${image_object.uploaded.formatted}</li></p>
        </div>
        </div>`;

        images_list[image_object.id] = image_object;
        document.getElementById('all-images').insertAdjacentHTML('afterbegin',html);
        document.getElementById('image-'+image_object.id).onload = function(){
            $(this).show();
            $('#loading-'+image_object.id).remove();
            this.parentElement.onclick = function(){
                SetChosen(image_object.id);
                window.scrollTo(0,0);   
            };
        };
    }
}

$(function(){
    window.addEventListener("beforeunload", function (e) {
        if(Object.keys(unsaved_elements).length>0){
            let changes ="Unsaved Changes in:<br><ul class='mb-0'>";
            for(let id in unsaved_elements){
                changes += "<li>"+unsaved_elements[id]+'</li>';
            }
            changes += "</ul>";
            ShowAlert(changes, "danger", 6000);
            var confirmationMessage = "Unsaved Changes...";
            (e || window.event).returnValue = confirmationMessage;
            return confirmationMessage;
        }
        else{
            return false;
        }
    });

    //Navigation
    $('#steps').on('shown.bs.tab', function () {
        // TODO disable/enable button, only execute if disabled
        $('.next-step').html('Continue');
    });
    $('#nav-tab').on('shown.bs.tab', function (e) {
        window.location.hash = e.target.hash;
    })

    // handle back-button for main navigation
    function HashChanged(){
        let hash = window.location.hash;
        if(hash){
            $('.nav-tabs a[href="' + hash + '"]').tab('show');
            window.scrollTo(0, 0);
        }
    }
    window.onhashchange = HashChanged;
    HashChanged();

    $('#delete-dialog').on('show.bs.modal', function (e) {
        // use: <button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="image" data-id="${image_object.id}">&times;</button>
        const type = $(e.relatedTarget).data('type');
        const id = $(e.relatedTarget).data('id');
        const dialog_text = document.getElementById('delete-dialog-title');
        const dialog_button = document.getElementById('delete-button');

        switch (type) {
            case "image":
            dialog_text.innerHTML = `Delete Image ${images_list[id].original_name}?`;
            dialog_button.onclick = function(){
                console.log("Trying to delete image with id: " + id);
                api("deleteimage", {id: id.toString()});
            };
            break;
            case "job":
            dialog_text.innerHTML = `Delete Job ${all_jobs[id].name}?`;
            dialog_button.onclick = function(){
                console.log("Trying to delete job with id: " + id);
                api("deletejob", {id: id.toString()});
            };
            break;
            case "profile":
            dialog_text.innerHTML = `Delete Profile ${all_profiles[id].profile_name}?`;
            dialog_button.onclick = function(){
                console.log("Trying to delete profile with id: " + id);
                api("deletesettingsprofile", {id: id.toString()});
            };
            break;
            default:
            dialog_text.innerHTML = `Not Implemented`;
            dialog_button.onclick = function(){
                console.log("called non implemented delete");
            };
        }
    });
});

function SetChosen(image_id){
    const div_chosen = document.getElementById('chosen-image');
    const description = document.getElementById('selectedimage-description');
    const class_dropzone = document.getElementById('dropZone');
    const img_name = document.getElementById('img-name');

    if(image_id){
        console.log("Selecting image ", image_id);
        chosen_image = images_list[image_id];
        description.innerHTML = `Upload Date: ${chosen_image.uploaded.formatted}`;
        img_name.innerHTML = chosen_image.original_name;
        $(img_name).show();
        $(description).show();
        class_dropzone.classList.remove('empty');
        class_dropzone.innerHTML = `<img style="height: 100%; width: 100%; object-fit: contain" src="${chosen_image.path}"/>`;
        $(div_chosen).show();
    }
    else{
        chosen_image = false;
        $(img_name).hide();
        $(div_chosen).hide();
        $(description).hide();
        class_dropzone.innerHTML = 'Drop Image';
        class_dropzone.classList.add('empty');
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
    if(tabId > 0) unsaved_elements['new_job'] = "New Job: "+tabOrder[tabId];
    // TODO delete unsaved_elements['new_job']; if job saved or executed
    console.log("Enabling tab: " +tabOrder[tabId]);
    for(let id = 0; id < tabOrder.length; id++){
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

function attributesTab(){
    tabEnter(2);
    document.getElementById('staticImgName').innerHTML = chosen_image.original_name;
    document.getElementById('date-attribute').innerHTML = new Date().toLocaleDateString();
}
$('#selection-tab').on('shown.bs.tab', function () {
    selectionTabEnter();
});
function selectionTab(){
    if(chosen_image.path){
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
        api("getdetectionalgorithms");  // TODO should we put this in the init?
        tabEnter(3);
    }
}

function strategyTab(){
    let plate_selection = document.getElementById("select-plate-profile");
    if(plate_selection.length > 0){
        SetColoniesToPick();
        plate_selection.onchange = function(){
            let plate_id = this.options[this.selectedIndex].value;
            let plate = all_profiles[plate_id];
            drawWells(plate.settings.number_of_columns, plate.settings.number_of_rows, number_of_colonies);
            let max_colonies = Math.min(number_of_colonies, plate.settings.number_of_columns*plate.settings.number_of_rows);
            let template = {
                settings: [
                {
                    name: "Number of Colonies to be picked",
                    id: "max_number_of_colonies",
                    type: "slider",
                    min: 1,
                    max: max_colonies,
                    step: 1,
                    description: "Choose number of detected colonies that will be transferred to Goal Plate.",
                    defaultValue: max_colonies
                }
                ]
            }
            $('#number-of-colonies-warning').toggle(max_colonies < number_of_colonies);
            let form = new FormGroup(template, "strategy-form");
            document.getElementById("number-of-colonies-slider").innerHTML = form.getHtml();
            form.AddInputEvents();
            document.getElementById('number-max_number_of_coloniesstrategy-form').insertAdjacentHTML('afterend', "/"+number_of_colonies);
            document.getElementById('number-max_number_of_coloniesstrategy-form').addEventListener('change', function(){
                if(this.value < number_of_colonies) $('#number-of-colonies-warning').show();
                $('#number-of-colonies-warning').toggle(this.value < number_of_colonies);
                updateWells(this.value);
            });
            document.getElementById('slider-max_number_of_coloniesstrategy-form').addEventListener('input', function(){
                $('#number-of-colonies-warning').toggle(this.value < number_of_colonies);
                updateWells(this.value);
            });
            updateWells(max_colonies);
        };
        plate_selection.onchange();
        tabEnter(4);
    }
}
function overviewTab(selected_colonies){
    // TODO disable clicks
    $('.next-step').html(`<span class="spinner-border spinner-border-sm"></span>`);
    PrintSelectedColonies(selected_colonies);
    // TODO hack
    setTimeout(function(){
        let plate_selection = document.getElementById("select-plate-profile");
        let plate_id = plate_selection.options[plate_selection.selectedIndex].value;
        api("setstartingwell", {row: collided_row, column: collided_column, job_id: current_job.id, plate_id: plate_id});
        const canvas_layer0 = document.getElementById('layer0');
        const canvas_layer1 = document.getElementById('layer1');
        // TODO onclick resize
        document.getElementById('processed-layer0').src = canvas_layer0.toDataURL();
        document.getElementById('processed-layer1').src = canvas_layer1.toDataURL();
        // <li>Printer: ${all_profiles[current_job.printer].profile_name}</li>
        // <li>Socket: ${all_profiles[current_job.socket].profile_name}</li>
        const html = `
        <ul class="mt-2">
        <li>Date: ${new Date().toLocaleDateString()}</li>
        <li>Job name: ${current_job.name}</li>
        <li>Plate: ${all_profiles[plate_id].profile_name}</li>
        <li>Description: ${current_job.description}</li>
        <li>Number of detected colonies: ${number_of_colonies}</li>
        <li>Number of picked colonies: ${document.getElementById('number-max_number_of_coloniesstrategy-form').value}</li>
        <li>Pick strategy: Starting at ${String.fromCharCode(64 + collided_row)}${collided_column}</li>
        </ul>
        `;
        document.getElementById('overview-metadata').insertAdjacentHTML('afterbegin', html);
        tabEnter(5);
    }, 100);
}

function executeTab(){
    var form = document.getElementById('check-preconditions');
    if (form.checkValidity() === true){
        //tabEnter(6);
        $('#check-preconditions').hide();
        $('#execute-button').hide();
        $('#pickjob-running').show();
        api('startjob', {id: current_job.id, octoprint_profile: default_profiles['octoprint-profile']});
    }
    form.classList.add('was-validated');
}


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
