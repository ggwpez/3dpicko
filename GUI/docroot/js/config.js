/* global unsaved_elements, $, api, all_profiles, default_profiles, ShowAlert, FormGroup */
// id => profile-object
var all_profiles = {};
// *-profile => *-profile-template
var profile_templates = {};
var default_profiles = {
    "printer-profile": "",
    "socket-profile": "",
    "plate-profile": "",
    "octoprint-profile": ""
};

var UpdateSettingsProfile = function (e){
    // sends:
    // json_object = {
    //   id: "XYZ",
    //   type: "printer-profile",
    //   profile_name: "Example Printer",
    //   settings: {
    //     "cut_filament_position_above_trigger": {x: 1,y: 2, z: 3},
    //     "z_coordinate_pushing_the_trigger": 4,
    //     ...
    //   }
    // };
    e.preventDefault();
    const form_data = new FormData(this);
    let id = form_data.get('id');
    if("form-"+id in unsaved_elements){
        let type = form_data.get('type');
        var json_object = {
            id: id,
            type: form_data.get('type'),
            profile_name: form_data.get('profile_name'),
        };
        json_object.settings = FormGroup.ReadForm(profile_templates[type], form_data);
        console.log({json_object});
        if(form_data.get('new_profile')=="true"){
            $('#form-new-'+type)[0].reset();
            api("createsettingsprofile", json_object);
        }
        else api("updatesettingsprofile", json_object);
    }
    $('#collapse-'+id).collapse('hide');
}

// add profil-list sent by server
function AddProfiles(data){
    default_profiles["printer-profile"] = data.defaultPrinter;
    default_profiles["socket-profile"] = data.defaultSocket;
    default_profiles["plate-profile"] = data.defaultPlate;
    default_profiles["octoprint-profile"] = data.defaultOctoprint;
    profile_templates["printer-profile"] = data.printerTemplate;
    profile_templates["socket-profile"] = data.socketTemplate;
    profile_templates["plate-profile"] = data.plateTemplate;
    profile_templates["octoprint-profile"] = data.octoprintTemplate;
    profile_templates["printer-profile"].type = "printer-profile";
    profile_templates["socket-profile"].type = "socket-profile";
    profile_templates["plate-profile"].type = "plate-profile";
    profile_templates["octoprint-profile"].type = "octoprint-profile";
    // add empty "new-profiles"
    AddProfileToList(profile_templates["printer-profile"]);
    AddProfileToList(profile_templates["socket-profile"]);
    AddProfileToList(profile_templates["plate-profile"]);
    if(!data.profiles.find(function(element){
        return element.type == "octoprint-profile";
    })) AddProfileToList(profile_templates["octoprint-profile"]);
    // add Profiles
    data.profiles.forEach(AddProfileToList);
}

function AddProfileToList(profile){
    if(profile.id in all_profiles){
        // update profile (delete old profile)
        // TODO check if new values
        DeleteProfile(profile.id);
    }

    let new_profile = false;
    if(!profile.id) {
        if(document.getElementById('card-new-'+profile.type)) return;
        new_profile = true;
        profile.id = "new-"+profile.type;
    }
    else all_profiles[profile.id] = profile;

    let html = `
    <div class="card" id="card-${profile.id}">
    <div class="card-header" data-toggle="collapse" data-target="#collapse-${profile.id}">
    <span id="default-label-${profile.id}" class="badge badge-primary" ${(default_profiles[profile.type]==profile.id)?``:`style="display: none;"`}>Default</span>
    <h4 class="btn btn-link">
    ${(new_profile)?``:`<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="profile" data-id="${profile.id}">&times;</button>`}
    ${(new_profile)?`Create New Profile &gt;`:`${profile.profile_name}`}
    </h4>
    </div>
    <div id="collapse-${profile.id}" class="collapse" data-parent="#group">
    <div class="card-body">
    <form id="form-${profile.id}" name="form-${profile.id}" data-description="${(new_profile)?profile.id:profile.profile_name}" style="overflow-x: auto;">
    <input type="hidden" id="new_profile" name="new_profile" value=${(new_profile)?true:false}>
    <input type="hidden" id="id" name="id" value="${profile.id}">
    <input type="hidden" id="type" name="type" value="${profile.type}">
    <div class="form-group">
    <label for="profile_name"><a class="textLink" data-toggle="modal" data-target="#colony-detection-help" onclick="loadHelp('${profile.type}s_identifier-h')">Profile Identifier:</a></label>
    <input required="required" id="profile_name" class="form-control" name="profile_name" type="text" placeholder="choose identifier" value="${(new_profile)?``:profile.profile_name}">
    </div>
    `;
    let profile_form;
    if(Array.isArray(profile.settings)){
        profile_form = new FormGroup(profile, 'form-'+profile.id);
    }
    else{
        // only data -> use template
        // TODO backend send text field for profile name? (in template)
        // profile.settings.profile_name = profile.profile_name;
        profile_form = new FormGroup(profile_templates[profile.type], 'form-'+profile.id, profile);
    }
    html += profile_form.getHtml();

    if(new_profile) html += `<button type="submit" class="btn btn-primary">Create profile</button>`;
    else html += `<button type="submit" class="btn btn-primary mr-2">Save changes</button><button type="button" class="btn btn-outline-primary" onclick="api('setdefaultsettingsprofile', {'id':'${profile.id}'});">Set as Default</button>`;
    html += `</form></div></div></div>`;
    document.getElementById(profile.type+'s').insertAdjacentHTML('beforeend',html);
    profile_form.AddInputEvents();
    FormGroup.AddFormEvents('form-'+profile.id, UpdateSettingsProfile, unsaved_elements);

    // add profiles as options
    let select = document.getElementById("select-"+profile.type);
    if(!new_profile && select){
        if(profile.id == default_profiles[profile.type]){
            document.getElementById("select-"+profile.type).add(new Option(profile.profile_name,  profile.id, true, true), 0);
        }
        else document.getElementById("select-"+profile.type).add(new Option(profile.profile_name, profile.id));
    }
}

function SetDefaultProfile(id){
    if(id in all_profiles){
        let profile = all_profiles[id];
        default_profiles[profile.type] = id;
        $(`#${profile.type}s .card-header .badge`).hide();
        $(`#${profile.type}s #card-${id} .badge`).show();
        document.getElementById("select-"+profile.type).value = id;
        ShowAlert("Set "+profile.profile_name+" as Default");
    }
}

function DeleteProfile(id){
    if(id in all_profiles){
        $('#card-'+id).remove();
        const profile = all_profiles[id];
        const type = profile.type;
        // delete profile in options
        let option = $('#select-'+profile.type+' option[value='+id+']');
        if(option.length > 0) option.remove();
        delete all_profiles[id];
        delete unsaved_elements["form-"+id];
    }
}