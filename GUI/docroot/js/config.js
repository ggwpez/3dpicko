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

$(function LoadProfiles(){
    // empty "new-profiles"
    // AddProfileToList(profile_templates['printer-profile']);
    // AddProfileToList(profile_templates['socket-profile']);
    // AddProfileToList(profile_templates['plate-profile']);
    // debugging profiles
    // AddProfileToList(example_printer);
    // AddProfileToList(makergear);
    // AddProfileToList(creality);
    // AddProfileToList(prototyp1);
    // AddProfileToList(default_plate);
    // AddProfileToList(large_plate);
});

function AddProfileToList(profile){
    if(profile.id in all_profiles){
        // update profile (delete old profile)
        // TODO check if new values
        DeleteProfile(profile.id);
    }

    let new_profile = false;
    if(profile.id == "new-"+profile.type){
        new_profile = true;
    }
    else all_profiles[profile.id] = profile;

    let html = `
    <div class="card" id="card-${profile.id}">
    <div class="card-header" data-toggle="collapse" data-target="#collapse-${profile.id}">
    <span id="default-label-${profile.id}" class="badge badge-primary" ${(default_profiles[profile.type]==profile.id)?``:`style="display: none;"`}>Default</span>
    <h4 class="btn btn-link">
    ${(new_profile)?``:`<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="printer-profile" data-id="${profile.id}">&times;</button>`}
    ${profile.profile_name}
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
        profile.settings.profile_name = profile.profile_name;
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

/*
 = {
    "printer-profile": {
        id : "new-printer-profile",
        type : "printer-profile",
        profile_name : "Create New Printer Profile &gt;",
        settings: [{
            id: "profile_name",
            name: "Profile Identifier",
            type: "text",
            placeholder: "choose identifier",
            defaultValue: ""
        },
        {
            id: "cut_filament_position_above_trigger",
            name: "Cut position",
            type: "vector3",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The (x,y,z) position, in the coordinate system of the printer, the nozzle needs to move to in order to be above the trigger of the scissor and directly above the center of the space between the scissors blades."
        },
        {
            id: "z_coordinate_pushing_the_trigger",
            name: "Push trigger at",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The global z coordinate (at x and y of cut-position) the trigger of the scissor is definitely pushed."
        },
        {
            id: "distance_between_pushed_trigger_and_gap_between_scissors_blade",
            name: "Cut distance",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The distance between the nozzle when the trigger is pushed and the gap between the scissors blades, where the filament will be cut."
        },
        {
            id: "movement_speed",
            name: "Movement speed",
            type: "number",
            min: 0,
            step: 1,
            defaultValue: "",
            unit: "mm/min",
            description: "The speed the nozzle is moved with."
        },
        {
            id: "filament_extrusion_length_on_move_offset",
            name: "Filament offset (move)",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "Offset to the length up to which the filament will be extruded while moving the nozzle above the plates."
        },
        {
            id: "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset",
            name: "Filament offset (pick)",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "Offset to the length up to which the filament will be extruded when picking from source- and placing on masterplate."
        }]
    },
    "socket-profile" : {
        id : "new-socket-profile",
        type : "socket-profile",
        profile_name : "Create New Socket Profile &gt;",
        settings: [{
            id: "profile_name",
            name: "Profile Identifier",
            type: "text",
            placeholder: "choose identifier",
            defaultValue: ""
        },
        {
            id: "socket_origin_offset",
            name: "Socket origin offset",
            type: "vector3",
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The (x,y,z) offset of the socket origin (with z-origin on socket surface) to the origin of the printers coordinate system (home)."
        },
        {
            id: "global_origin_of_source_plate",
            name: "Source plate cutout origin",
            type: "vector2",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The (x,y) origin of the slot/cut-out of the source plate given as a point of the coordinate system of the socket."
        },
        {
            id: "depth_of_cutout_the_source_plate_lies_in",
            name: "Source plate cutout depth",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The source plate cutout depth."
        },
        {
            id: "global_origin_of_master_plate",
            name: "Master plate cutout origin",
            type: "vector2",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The (x,y) origin of the slot/cut-out of the master plate given as a point of the coordinate system of the socket."
        },
        {
            id:  "depth_of_cutout_the_master_plate_lies_in",
            name: "Master plate cutout depth",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The master plate cutout depth."
        },
        {
            id: "global_origin_of_goal_plate",
            name: "Goal plate cutout origin",
            type: "vector2",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The (x,y) origin of the slot/cut-out of the goal plate given as a point of the coordinate system of the socket."
        },
        {
            id: "depth_of_cutout_the_goal_plate_lies_in",
            name: "Goal plate cutout depth",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The goal plate cutout depth."
        },
        {
            id: "orientation_of_goal_plate",
            name: "Goal and master plate orientation",
            type: "radio",
            options: {
                kFirstRowFirstColumnAtCutoutOrigin : "Well 'A1' at cutout origin.",
                kLastRowFirstColumnAtCutoutOrigin : "Well 'm1' at cutout origin. (m=last row)"
            },
            defaultValue: "kFirstRowFirstColumnAtCutoutOrigin",
            description: "Orientation of goal- and masterplate compared to the cutout it is lying in."
        }]
    },
    "plate-profile" : {
        id : "new-plate-profile",
        type : "plate-profile",
        profile_name : "Create New Plate Profile &gt;",
        settings: [{
            id: "profile_name",
            name: "Profile Identifier",
            type: "text",
            placeholder: "choose identifier",
            defaultValue: "Create New Plate Profile &gt;"
        },
        {
            id:  "number_of_rows",
            name: "Number of rows",
            type: "number",
            step: 1,
            defaultValue: "",
            unit: "",
            description: "Number of rows the goal plate has."
        },
        {
            id: "number_of_columns",
            name: "Number of columns",
            type: "number",
            step: 1,
            defaultValue: "",
            unit: "",
            description: "Number of columns the goal plate has."
        },
        {
            id: "a1_row_offset",
            name: "A1 row offset",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The offset of the center of the first well A1 to the upper edge of the goal plate."
        },
        {
            id: "a1_column_offset",
            name: "A1 column offset",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The offset of the center of the first well A1 to the left edge of the goal plate."
        },
        {
            id: "well_spacing_center_to_center",
            name: "Well spacing center to center",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The distance between the center of a well to the center of any directly adjacent well."
        },
        {
            id: "height_source_plate",
            name: "Source plate height",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The height of the source plate."
        },
        {
            id: "height_master_plate",
            name: "Master plate height",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The height of the master plate."
        },
        {
            id: "height_goal_plate",
            name: "Goal plate height",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The height of the goal plate."
        },
        {
            id: "well_depth",
            name: "Well depth",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The depth of every well."
        },
        {
            id : "culture_medium_thickness",
            name: "Culture medium thickness",
            type: "number",
            min: 0,
            step: 0.01,
            defaultValue: "",
            unit: "mm",
            description: "The thickness of the used culture medium inside the source and master plate, for instance agars."
        }]
    }
};
*/

/*
Local Profiles for debugging
*/

let example_printer = {
    id : "PrinterXYZ",
    type : "printer-profile",
    profile_name: "Example Printer",
    settings: [{
        id: "profile_name",
        name: "Profile Identifier",
        type: "text",
        placeholder: "choose identifier",
        defaultValue: "",
        value: "Example Printer"
    },
    {
        id: "cut_filament_position_above_trigger",
        name: "Cut position",
        type: "vector3",
        min: 0,
        step: 0.01,
        value: {x:11,y:12,z:13},
        unit: "mm",
        description: "The (x,y,z) position, in the coordinate system of the printer, the nozzle needs to move to in order to be above the trigger of the scissor and directly above the center of the space between the scissors blades."
    },
    {
        id: "z_coordinate_pushing_the_trigger",
        name: "Push trigger at",
        type: "number",
        min: 0,
        step: 0.01,
        value: 14,
        unit: "mm",
        description: "The global z coordinate (at x and y of cut-position) the trigger of the scissor is definitely pushed."
    },
    {
        id: "distance_between_pushed_trigger_and_gap_between_scissors_blade",
        name: "Cut distance",
        type: "number",
        min: 0,
        step: 0.01,
        value: 15,
        unit: "mm",
        description: "The distance between the nozzle when the trigger is pushed and the gap between the scissors blades, where the filament will be cut."
    },
    {
        id: "movement_speed",
        name: "Movement speed",
        type: "number",
        min: 0,
        step: 1,
        value: 16,
        unit: "mm/min",
        description: "The speed the nozzle is moved with."
    },
    {
        id: "filament_extrusion_length_on_move_offset",
        name: "Filament offset (move)",
        type: "number",
        min: 0,
        step: 0.01,
        value: 17,
        unit: "mm",
        description: "Offset to the length up to which the filament will be extruded while moving the nozzle above the plates."
    },
    {
        id: "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset",
        name: "Filament offset (pick)",
        type: "number",
        min: 0,
        step: 0.01,
        value: 18,
        unit: "mm",
        description: "Offset to the length up to which the filament will be extruded when picking from source- and placing on masterplate."
    },
    {
        id: "ttrtwtr",
        name: "Zahl",
        type: "slider",
        valueType: "float",
        min: -10,
        max: 10,
        step: 1,
        value: 1,
        description: ""
    }]
};

let prototyp1 = {
    "id" : "DEFAULTBLUE",
    "type" : "socket-profile",
    "profile_name" : "Default Socket Blue",
    settings: {
        "socket_origin_offset" : {x: 1, y: 2, z: 3},
        "global_origin_of_source_plate" : {x:4, y:5},
        "depth_of_cutout_the_source_plate_lies_in" : 6,
        "global_origin_of_master_plate" : {x:7,y:8},
        "depth_of_cutout_the_master_plate_lies_in" : 9,
        "global_origin_of_goal_plate" : {x:10,y:11},
        "depth_of_cutout_the_goal_plate_lies_in" : 12,
        "orientation_of_goal_plate" : "kLastRowFirstColumnAtCutoutOrigin"
    }
};
let default_plate = {
    "id" : "DEFALUTPLATE",
    "type" : "plate-profile",
    "profile_name" : "Default Plates - 96 Wells",
    settings:{
        "number_of_rows" : 8,
        "number_of_columns" : 12,
        "a1_row_offset" : 1,
        "a1_column_offset" : 2,
        "well_spacing_center_to_center" : 3,
        "height_source_plate" : 4,
        "height_master_plate" : 5,
        "height_goal_plate" : 6,
        "well_depth" : 7,
        "culture_medium_thickness" : 8
    }
};
let large_plate = {
    "id" : "LARGEPLATE",
    "type" : "plate-profile",
    "profile_name" : "LargePlate - 16x24=384 Wells",
    settings:{
        "number_of_rows" : 16,
        "number_of_columns" : 24,
        "a1_row_offset" : 1,
        "a1_column_offset" : 2,
        "well_spacing_center_to_center" : 3.875876587658765487648,
        "height_source_plate" : 4,
        "height_master_plate" : 5,
        "height_goal_plate" : 6,
        "well_depth" : 7,
        "culture_medium_thickness" : 8
    }
};
let makergear = {
    "id" : "XYZ",
    "type" : "printer-profile",
    "profile_name" : "Makergear M3",
    settings:{
        "cut_filament_position_above_trigger" : {x:1,y:2,z:3},
        "z_coordinate_pushing_the_trigger" : 4,
        "distance_between_pushed_trigger_and_gap_between_scissors_blade" : 5,
        "movement_speed" : 6,
        "filament_extrusion_length_on_move_offset": 7,
        "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : 8
    }
};
let creality = {
    "id" : "ABC",
    "type" : "printer-profile",
    "profile_name" : "Creality Ender-3",
    settings:{
        "cut_filament_position_above_trigger" : {x:1,y:2,z:3},
        "z_coordinate_pushing_the_trigger" : 4,
        "distance_between_pushed_trigger_and_gap_between_scissors_blade" : 5,
        "movement_speed" : 6,
        "filament_extrusion_length_on_move_offset": 7,
        "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : 8
    }
};
let new_printer_profile = {
    "id" : "new-printer-profile",
    "type" : "printer-profile",
    "profile_name" : "Create New Printer Profile &gt;",
    settings:{
        "cut_filament_position_above_trigger" : "",
        "z_coordinate_pushing_the_trigger" : "",
        "distance_between_pushed_trigger_and_gap_between_scissors_blade" : "",
        "movement_speed" : "",
        "filament_extrusion_length_on_move_offset": "",
        "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : ""
    }
};
let new_plate_profile = {
    "id" : "new-plate-profile",
    "type" : "plate-profile",
    "profile_name" : "Create New Plate Profile &gt;",
    settings:{
        "number_of_rows" : "",
        "number_of_columns" : "",
        "a1_row_offset" : "",
        "a1_column_offset" : "",
        "well_spacing_center_to_center" : "",
        "height_source_plate" : "",
        "height_master_plate" : "",
        "height_goal_plate" : "",
        "well_depth" : "",
        "culture_medium_thickness" : ""
    }
};
let new_socket_profile = {
    "id" : "new-socket-profile",
    "type" : "socket-profile",
    "profile_name" : "Create New Socket Profile &gt;",
    settings: {
        "socket_origin_offset" : "",
        "global_origin_of_source_plate" : "",
        "depth_of_cutout_the_source_plate_lies_in" : "",
        "global_origin_of_master_plate" : "",
        "depth_of_cutout_the_master_plate_lies_in" : "",
        "global_origin_of_goal_plate" : "",
        "depth_of_cutout_the_goal_plate_lies_in" : "",
        "orientation_of_goal_plate" : "kFirstRowFirstColumnAtCutoutOrigin"
    }
};
