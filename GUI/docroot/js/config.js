/* global unsaved_elements, $, api, all_profiles, default_profiles, ShowAlert, FormGroup */

var UpdateSettingsProfile = function (e){
    // var send = {
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
        if (id == "new-printer-profile" || id == "new-socket-profile" || id == "new-plate-profile"){
            $('#form-new-'+type)[0].reset();
            api("createsettingsprofile", json_object);
        }
        else api("updatesettingsprofile", json_object);
    }
    $('#collapse-'+id).collapse('hide');
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
    ${(new_profile)?``:`<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="printer-profile" data-id="${profile.id}">&times;</button>`}
    ${(new_profile)?`Create New Profile &gt;`:`${profile.profile_name}`}
    </h4>
    </div>
    <div id="collapse-${profile.id}" class="collapse" data-parent="#group">
    <div class="card-body">
    <form id="form-${profile.id}" name="form-${profile.id}" data-description="${profile.profile_name}">
    <input type="hidden" id="id" name="id" value="${profile.id}">
    <input type="hidden" id="type" name="type" value="${profile.type}">
    <div class="form-group">
    <label for="profile_name">Profile Identifier:</label>
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

    if(!(new_profile) && (profile.type == "printer-profile" || profile.type == "socket-profile")){
        if(profile.id == default_profiles[profile.type]){
            document.getElementById("select-"+profile.type).add(new Option(profile.profile_name,  profile.id, true, true), 0);
        }
        else document.getElementById("select-"+profile.type).add(new Option(profile.profile_name, profile.id));
    }
}

function AddGeneralSetting(template){
    let html = `
    <div class="card" id="card-${template.id}">
    <div class="card-header" data-toggle="collapse" data-target="#collapse-${template.id}">
    <h4 class="btn btn-link">
    ${template.name}
    </h4>
    </div>
    <div id="collapse-${template.id}" class="collapse" data-parent="#group">
    <div class="card-body">
    <form id="form-${template.id}" name="form-${template.id}" data-description="${template.name}">
    `;
    let form;
    if(Array.isArray(template.settings)){
        form = new FormGroup(template, 'form-'+template.id);
    }
    html += form.getHtml();

    html += `<button type="submit" class="btn btn-primary mr-2">Save changes</button>`;
    html += `</form></div></div></div>`;
    document.getElementById('general-settings').insertAdjacentHTML('beforeend',html);
    form.AddInputEvents();
    // TODO FormGroup.AddFormEvents('form-'+template.id, /*TODO*/, unsaved_elements);
}

function SetDefaultProfile(id){
    // TODO Default Plate Profile
    if(id in all_profiles){
        let profile = all_profiles[id];
        default_profiles[profile.type] = id;
        console.log(default_profiles);
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
        switch(profile.type){
            case "printer-profile":
            $('#select-printer-profile option[value='+id+']').remove();
            break;
            case "socket-profile":
            $('#select-socket-profile option[value='+id+']').remove();
            break;
            case "plate-profile":
            // TODO update options list
            break;
        }
        delete all_profiles[id];
        delete unsaved_elements["form-"+id];
    }
}