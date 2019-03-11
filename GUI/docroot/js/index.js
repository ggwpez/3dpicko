/* global WsSetup, api, AddProfileToList, DeleteProfile, drawPositions, SetDefaultProfile, $, Cropper, selectionTabEnter, drawWells, collided_row, collided_column, FormGroup, profile_templates, AddGeneralSetting, number_of_colonies */
//"id"=>"image_object"
var images_list = {};
var chosen_image = {};
// current job (local)
var current_job = {};
// all_jobs[job.id] = job;
var all_jobs = [];
var all_profiles = {};
var algorithms;
var algorithm_settings_values = {};
var default_profiles = {
    "printer-profile": "",
    "socket-profile": "",
    "plate-profile": ""
};
var profile_templates ={};
// TODO delete "new_job" if job saved or executed
var unsaved_elements = {};

var global_alert = $('#global-alert');
var alert_window = $('#alert-window');

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
                $('#overlay').hide();
                // TODO
                if(data.uploadimage=="") ShowAlert("Upload failed.<br>Maybe Image already exists.", "danger");
                else ShowAlert(JSON.stringify(data), "danger");
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
                AddImageToList(data);
                SetChosen(data.id);
                ShowAlert("Image "+data.original_name+" uploaded.", "success");
            }
            else if (type == "createjob")
            {
                AddJobToList(data);
                current_job = data;
                document.getElementById("headertag").innerHTML = "Entwurf #" +data.id;
                ShowAlert("Pickjob "+data.id+" saved.", "success");
            }
            else if (type == "getjoblist")
            {
                data.jobs.forEach(AddJobToList);
                console.log("Jobs:\n" +data +"\ncount: " +data.jobs.length);
            }
            else if (type == "getgeneralsettings"){
                // TODO
                data.settings.forEach(AddGeneralSetting);
            }
            else if (type == "getprofilelist")
            {
                console.log("Profiles:\n" +data +"\ncount: " +data.profiles.length);
                default_profiles["printer-profile"] = data.defaultPrinter;
                default_profiles["socket-profile"] = data.defaultSocket;
                default_profiles["plate-profile"] = data.defaultPlate;
                profile_templates["printer-profile"] = data["printerTemplate"];
                profile_templates["socket-profile"] = data["socketTemplate"];
                profile_templates["plate-profile"] = data["plateTemplate"];
                // TOOO add type in Backend?
                profile_templates["printer-profile"].type = "printer-profile";
                profile_templates["socket-profile"].type = "socket-profile";
                profile_templates["plate-profile"].type = "plate-profile";
                // empty "new-profiles"
                AddProfileToList(profile_templates["printer-profile"]);
                AddProfileToList(profile_templates["socket-profile"]);
                AddProfileToList(profile_templates["plate-profile"]);
                data.profiles.forEach(AddProfileToList);
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
                drawPositions(data);
                clearTimeout(loading_timeout_id);
                document.body.classList.remove('wait');
                let number = $('#detected-colonies-number');
                if(number.text() != number_of_colonies){
                    number.hide();
                    number.text(number_of_colonies);
                    number.fadeIn();
                }
                $('#strategy-tab-button').prop("disabled", number_of_colonies <= 0);
            }
            else if (type == "getdetectionalgorithms"){
                console.log("########## Algos", JSON.stringify(data));
                GetDetectionAlgorithms(data);
            }
            else if (type == "crop-image"){
                AddImageToList(data);
                chosen_image = data;
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
});

function GetDetectionAlgorithms(detection_algorithms){
    algorithms = detection_algorithms;
    // TODO Remove (only for debugging)
    /*algorithms["321"] = {
        name: "Fluro",
        description: "Good for detecting fluorescent colonies",
        settings:[{
            id: "slider_1",
            name: "Threshold 1",
            type: "rangeslider",
            min: 0,
            max: 1000,
            step: 0.1,
            defaultValue: {min: 1, max: 30},
            description: ""
        },
        {
            id: "checkbox_1",
            name: "Erode & Dilate",
            type: "checkbox",
            defaultValue: true,
            description: "",
            conditional_settings:[{
                id: "checkbox_1_1",
                name: "Erode & Dilate 2",
                type: "checkbox",
                defaultValue: false,
                description: "Only visible if Erode & Dilate is true"
            },
            {
                id: "slider_1_1",
                name: "Erode & Dilate 3",
                type: "rangeslider",
                min: 0,
                max: 1000,
                step: 0.1,
                defaultValue: {min: 1, max: 300},
                description: "Only visible if E&D is true"
            },
            {
                id: "checkbox_1_2",
                name: "Erode & Dilate",
                type: "checkbox",
                defaultValue: false,
                description: "",
                conditional_settings:[{
                    id: "checkbox_1_2_1",
                    name: "Erode & Dilate 2",
                    type: "checkbox",
                    defaultValue: false,
                    description: "Only visible if Erode & Dilate is true"
                },
                {
                    id: "slider_1_2_1",
                    name: "Erode & Dilate 3",
                    type: "rangeslider",
                    min: 0,
                    max: 1000,
                    step: 0.1,
                    defaultValue: {min: 1, max: 300},
                    description: "Only visible if E&D is true"
                }]
            }]
        }]
    };
    algorithms["423"] = {
        name: "Fluro 2",
        description: "Good for detecting more fluorescent colonies",
        settings:[{
            id: "ft",
            "defaultValue":"1",
            "description":"lel",
            "name":"Filter Type",
            "options":
            {
                "0":"Option A",
                "1":"Option B",
                "lol":"Option V"
            },
            "type":"dropdown"
        },
        {
            id: "1234",
            name: "Thres",
            type: "number",
            valueType: "float",
            min: -10,
            max: 10,
            step: 1,
            defaultValue: 1,
            description: "Hi this is very descriptive"
        },
        {
            id: "123",
            name: "Zahl",
            type: "slider",
            valueType: "float",
            min: -10,
            max: 10,
            step: 1,
            defaultValue: 1,
            description: ""
        }
        ]
    };*/

    const algorithm_selection = document.getElementById("select-algorithm");
    while (algorithm_selection.firstChild) algorithm_selection.removeChild(algorithm_selection.firstChild);

    for (let id in algorithms){
        let algorithm = algorithms[id];
        let algorithm_option = document.createElement('option');
        algorithm_option.value = id;
        algorithm_option.text = `${algorithm.name} ${(algorithm.description&&algorithm.description!="")?`(${algorithm.description})`:``}`;
        algorithm_option.title = algorithm.description;
        algorithm_selection.appendChild(algorithm_option);
        CreateDetectionAlgorithmSettings(id);
    }
    algorithm_selection.onchange = function(){$('#'+this.value+'-settings').collapse('show');UpdateDetectionSettings(this.value)};
}

function CreateDetectionAlgorithmSettings(id){
    const detection_settings = document.getElementById("detection-settings");
    // while (detection_settings.firstChild) detection_settings.removeChild(detection_settings.firstChild);

    let html = `<div id="${id}-settings" class="collapse" data-parent="#detection-settings">
    <form id="${id}-settings-form">
    <button type="reset" class="btn btn-outline-primary btn-sm w-100">Reset to Default Values</button>
    <button type="button" onclick="show_excluded = !show_excluded;printPositions();" class="btn btn-outline-primary btn-sm w-100 mt-1">Show/Hide Excluded Colonies</button>
    `;
    let settings = new FormGroup(algorithms[id], id+"-settings-form");
    detection_settings.insertAdjacentHTML("beforeend", html+settings.getHtml()+`</form></div>`);
    settings.AddInputEvents();
    FormGroup.AddFormEvents(id+"-settings-form");
    $('#'+id+'-settings-form').change(function(){
        UpdateDetectionSettings(id);
    });
}
var loading_timeout_id;
var UpdateDetectionSettings = function (id){
    // Send as:
    // {
    //  job_id: "222",
    //  algorithm: "321",
    //  settings:
    //  {
    //      "1234": 6,
    //      "123": true
    //  }
    // }
    document.body.classList.add('wait');
    let form = document.getElementById(id+'-settings-form');
    clearTimeout(loading_timeout_id);
    loading_timeout_id = setTimeout(function(){
        document.body.classList.remove('wait');
        ShowAlert("Colony Detection Timeout", "danger");
    }, 4000);
    const form_data = new FormData(form);
    const algorithm_id = document.getElementById('select-algorithm').value;
    algorithm_settings_values[algorithm_id] = {
        job_id : current_job.id,
        algorithm: algorithm_id,
        settings: FormGroup.ReadForm(algorithms[algorithm_id], form_data)
    };
    console.log("Update Settings:", algorithm_settings_values[algorithm_id]);
    api('updatedetectionsettings', algorithm_settings_values[algorithm_id]);
}

function AddJobToList(job)
{
    // Is the job not already in the list ?
    if (!(job.id in all_jobs))
    {
        console.log("Adding job: " +JSON.stringify(job));
        // ${job.created.formatted}
        var name = (job.step == 7) ? "Job" : "Entwurf";
        var html = `<div class="card m-1 job-card" id="job-${job.id}" >
        <button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="job" data-id="${job.id}">&times;</button>
        <div class="card-body p-1">
        <h5 class="card-title mr-2 p-1">${name} ${job.name}</h5>
        <img class="card-img" src="${images_list[job.img_id].path}" alt="${images_list[job.img_id].original_name}">
        <p class="card-text"><ul>
        <li>ID: ${job.id}</li>
        <li>Created: </li>
        <li>Step: ${job.step}</li>
        ${job.description}
        </p>
        </div>
        <div class="card-footer bg-white">
        <a href="#" class="btn btn-primary ">edit</a>
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
        <div class="card-body p-1" onclick="SetChosen('${image_object.id}');window.scrollTo(0,0);" style="cursor: pointer;">
        <h7 class="card-title mb-1 mr-3">${image_object.original_name}</h7>
        <div class="spinner-border m-5" id="loading-${image_object.id}"></div>
        <img class="card-img" src="${image_object.path}" alt="${image_object.original_name}" style="display: none;" onload="$(this).show();$('#loading-${image_object.id}').remove();">
        <p class="card-text">Date: ${image_object.uploaded.formatted}</li></p>
        </div>
        </div>`;

        let div_images = document.getElementById('all-images');
        images_list[image_object.id] = image_object;
        div_images.insertAdjacentHTML('afterbegin',html);
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
});

function SetChosen(image_id){
    const div_chosen = document.getElementById('chosen-image');
    const description = document.getElementById('selectedimage-description');
    const class_dropzone = document.getElementById('dropZone');
    const img_name = document.getElementById('img-name');

    if(image_id){
        chosen_image = images_list[image_id];
        console.log("Selecting image ", image_id);
        description.innerHTML = `Upload Date: ${chosen_image.uploaded.formatted}`;
        img_name.innerHTML = chosen_image.original_name;
        $(img_name).show();
        $(div_chosen).show();
        $(description).show();
        class_dropzone.innerHTML = `<img style="height: 100%; width: 100%; object-fit: contain" src="${chosen_image.path}" onload="clearTimeout(upload_timeout_id);$('#overlay').hide();"/>`;
        class_dropzone.removeClass('empty');
    }
    else{
        chosen_image = false;
        $(img_name).hide();
        $(div_chosen).hide();
        $(description).hide();
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

var cropper;
//Navigation
$('#steps').on('shown.bs.tab', function () {
    // TODO disable/enable button, only execute if disabled
    $('.next-step').html('Continue');
})
$('#cut-tab').on('shown.bs.tab', function () {
    let cutImg = document.getElementById('cutImg');
    cropper = new Cropper(cutImg, {
        aspectRatio: 1.5/1,
        movable: false,
        zoomable: false
    });
});
function cutTab(){
    if(chosen_image.path){
        const cutImg = document.getElementById('cutImg');
        cutImg.onload = function(){
            tabEnter(1);
        }
        cutImg.src = chosen_image.path;
    }
}

function attributesTab(){
    tabEnter(2);
    document.getElementById('staticImgName').innerHTML = chosen_image.original_name;
    document.getElementById('date-attribute').innerHTML = new Date().toLocaleDateString();
    // Do the cutting
    var rect = cropper.getData();
    api('crop-image', { id: chosen_image.id, x: rect.x, y: rect.y, width: rect.width, height: rect.height });
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
    console.log(all_profiles);
    for(let profile_id in all_profiles){
        let profile = all_profiles[profile_id];
        if(profile.type=="plate-profile"){
            if(profile.id == default_profiles[profile.type]) plate_selection.add(new Option(profile.profile_name,  profile.id, true, true), 0);
            else plate_selection.add(new Option(profile.profile_name, profile.id));
        }
    }
    if(plate_selection.length > 0){
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
            if(plate.settings.number_of_columns*plate.settings.number_of_rows < number_of_colonies) template.settings[0].description += "<br><span class='text-danger'>Please note that number of colonies is greater than plate size. Not all detected colonies fit on this plate.</span>";
            let form = new FormGroup(template, "strategy-form");
            document.getElementById("number-of-colonies-slider").innerHTML = form.getHtml();
            form.AddInputEvents();
            document.getElementById('number-max_number_of_coloniesstrategy-form').insertAdjacentHTML('afterend', "/"+number_of_colonies);
            document.getElementById('slider-max_number_of_coloniesstrategy-form').addEventListener('input', function(){
                updateWells(this.value);
            });
            updateWells(max_colonies);
        };
        plate_selection.onchange();
        tabEnter(4);
    }
}

function overviewTab(){
    removeUnselectedColonies();
    // TODO disable clicks
    $('.next-step').html(`<span class="spinner-border spinner-border-sm"></span>`);
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
        const html = `
        <ul class="mt-2">
        <li>Date: ${new Date().toLocaleDateString()}</li>
        <li>Job name: ${current_job.name}</li>
        <li>Printer: ${all_profiles[current_job.printer].profile_name}</li>
        <li>Socket: ${all_profiles[current_job.socket].profile_name}</li>
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
        api('startjob', {id: current_job.id});
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
